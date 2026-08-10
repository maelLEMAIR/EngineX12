#include "PhysicsWorld.h"

#include <chrono>

#include "../Math/MathUtils.hpp"
#include "../Math/Geometry/AABB.h"
#include "../Math/Geometry/OBB.h"
#include "../Math/Geometry/Sphere.h"

PhysicsWorld::~PhysicsWorld()
{
    Stop();
}

void PhysicsWorld::Start(float _tickRate)
{
    if (m_running)
        return;

    m_tickRate = _tickRate;
    m_running  = true;
    m_thread   = std::thread(&PhysicsWorld::PhysicsLoop, this);
}

void PhysicsWorld::Stop()
{
    if (!m_running)
        return;

    m_running = false;

    if (m_thread.joinable())
        m_thread.join();
}

void PhysicsWorld::PhysicsLoop()
{
    Chrono chrono;
    chrono.Start();

    float accumulator    = 0.0f;
    float logAccumulator = 0.0f;

    uint64 stepTimeAccumUs = 0;
    uint64 maxStepTimeUs   = 0;
    uint64 stepCount       = 0;

    while (m_running)
    {
        float tickDelay = 1.0f / m_tickRate;
        float frameTime = chrono.Reset();
        accumulator += frameTime;

        while (accumulator >= tickDelay)
        {
            auto stepStart = std::chrono::high_resolution_clock::now();
            Step(tickDelay);
            auto stepEnd = std::chrono::high_resolution_clock::now();

            uint64 stepUs = (uint64)std::chrono::duration_cast<std::chrono::microseconds>(stepEnd - stepStart).count();
            stepTimeAccumUs += stepUs;
            maxStepTimeUs = MathUtils::Max(maxStepTimeUs, stepUs);
            stepCount++;

            accumulator -= tickDelay;
        }

        // Statistiques du thread physique : coût réel de Step() (broadphase +
        // narrowphase + résolution), indépendant du framerate de rendu.
        logAccumulator += frameTime;
        if (logAccumulator >= 1.0f && stepCount > 0)
        {
            printf("[PhysicsWorld] actifs=%u endormis=%u statiques=%u paires_candidates=%u | Step: %.3f ms moy, %.3f ms max (%llu steps/s)\n",
                m_lastDynamicAwakeCount,
                m_lastDynamicSleepingCount,
                m_lastStaticCount,
                m_lastCandidatePairCount,
                (stepTimeAccumUs / 1000.0) / (double)stepCount,
                maxStepTimeUs / 1000.0,
                (unsigned long long)stepCount);

            logAccumulator  = 0.0f;
            stepTimeAccumUs = 0;
            maxStepTimeUs   = 0;
            stepCount       = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void PhysicsWorld::Step(float _dt)
{
    LockGuard lock(m_bodiesMutex);

    DrainCommands();

    size_t count = m_bodies.size();

    m_staticBodyIndices.clear();
    m_activeDynamicIndices.clear();
    m_gridCells.clear();

    uint32 aliveDynamicCount = 0;

    // Intégration, puis insertion en grille. Les corps statiques ne sont
    // jamais insérés (AABB potentiellement bien plus grande qu'une cellule,
    // cf. le sol) : ils sont mémorisés à part. Le suivi du sommeil est fait
    // plus bas, après résolution des contacts (cf. commentaire associé).
    for (size_t i = 0; i < count; i++)
    {
        BodySlot& slot = m_bodies[i];
        if (!slot.alive)
            continue;

        RigidBody& body = slot.body;

        if (!body.IsDynamic())
        {
            m_staticBodyIndices.push_back((uint32)i);
            continue;
        }

        if (!body.isSleeping)
        {
            Vect3f32 acceleration = m_gravity * body.gravityScale + body.forceAccum * body.invMass;
            body.linearVelocity += acceleration * _dt;

            if (body.maxFallSpeed > 0.0f && !m_gravity.IsNull())
            {
                Vect3f32 gravityDir = m_gravity.Normalized();
                float fallSpeed = body.linearVelocity.Dot(gravityDir);
                if (fallSpeed > body.maxFallSpeed)
                    body.linearVelocity -= gravityDir * (fallSpeed - body.maxFallSpeed);
            }

            body.position += body.linearVelocity * _dt;
        }

        body.forceAccum = Vect3f32(0.0f, 0.0f, 0.0f);
        aliveDynamicCount++;

        if (!body.isSleeping)
            m_activeDynamicIndices.push_back((uint32)i);

        InsertIntoGrid((uint32)i, MakeBroadphaseAABB(body));
    }

    // Paires dynamique-dynamique : uniquement entre corps partageant au
    // moins une cellule de la grille.
    m_testedPairs.clear();

    for (auto const& cell : m_gridCells)
    {
        Vector<uint32> const& indices = cell.second;

        for (size_t a = 0; a < indices.size(); a++)
        {
            for (size_t b = a + 1; b < indices.size(); b++)
            {
                uint32 i = indices[a];
                uint32 j = indices[b];

                if (i > j)
                {
                    uint32 tmp = i;
                    i = j;
                    j = tmp;
                }

                uint64 pairKey = (uint64(i) << 32) | uint64(j);
                if (!m_testedPairs.insert(pairKey).second)
                    continue; // déjà testée via une autre cellule partagée

                TestBodyPair(i, j);
            }
        }
    }

    // Paires dynamique-statique : peu de corps statiques, testés contre les
    // seuls corps dynamiques actifs (un corps endormi repose déjà en
    // équilibre sur son support, inutile de le retester chaque tick).
    for (uint32 staticIdx : m_staticBodyIndices)
    {
        for (uint32 dynIdx : m_activeDynamicIndices)
            TestBodyPair(staticIdx, dynIdx);
    }

    // Suivi du sommeil, évalué APRES résolution des contacts : la vitesse
    // juste après l'intégration inclut encore l'accélération de gravité de
    // ce tick (qui sera annulée par la résolution d'un contact au repos) et
    // franchirait donc systématiquement le seuil, empêchant tout corps posé
    // de jamais s'endormir.
    uint32 sleepingCount = 0;

    for (uint32 idx : m_activeDynamicIndices)
    {
        RigidBody& body = m_bodies[idx].body;

        if (body.linearVelocity.LengthSquared() < k_sleepLinearVelocitySqThreshold)
        {
            body.sleepTimer += _dt;
            if (body.sleepTimer >= k_sleepTimeThreshold)
            {
                body.isSleeping     = true;
                body.linearVelocity = Vect3f32(0.0f, 0.0f, 0.0f);
                sleepingCount++;
            }
        }
        else
        {
            body.sleepTimer = 0.0f;
        }
    }

    m_lastDynamicAwakeCount    = (uint32)m_activeDynamicIndices.size() - sleepingCount;
    m_lastDynamicSleepingCount = aliveDynamicCount - m_lastDynamicAwakeCount;
    m_lastStaticCount          = (uint32)m_staticBodyIndices.size();
    m_lastCandidatePairCount   = (uint32)m_testedPairs.size();
}

int64 PhysicsWorld::PackCellCoord(int32 _x, int32 _y, int32 _z)
{
    // Décalage pour supporter des coordonnées de cellule négatives ;
    // 21 bits par axe (~ +/-1M cellules), largement suffisant pour une scène.
    const int64 offset = 1 << 20;
    int64 x = (int64)_x + offset;
    int64 y = (int64)_y + offset;
    int64 z = (int64)_z + offset;
    return (x << 42) | (y << 21) | z;
}

void PhysicsWorld::InsertIntoGrid(uint32 _bodyIndex, AABB const& _aabb)
{
    int32 minX = MathUtils::Floor(_aabb.min.x / m_broadphaseCellSize);
    int32 minY = MathUtils::Floor(_aabb.min.y / m_broadphaseCellSize);
    int32 minZ = MathUtils::Floor(_aabb.min.z / m_broadphaseCellSize);
    int32 maxX = MathUtils::Floor(_aabb.max.x / m_broadphaseCellSize);
    int32 maxY = MathUtils::Floor(_aabb.max.y / m_broadphaseCellSize);
    int32 maxZ = MathUtils::Floor(_aabb.max.z / m_broadphaseCellSize);

    for (int32 x = minX; x <= maxX; x++)
        for (int32 y = minY; y <= maxY; y++)
            for (int32 z = minZ; z <= maxZ; z++)
                m_gridCells[PackCellCoord(x, y, z)].push_back(_bodyIndex);
}

void PhysicsWorld::TestBodyPair(uint32 _i, uint32 _j)
{
    RigidBody& a = m_bodies[_i].body;
    RigidBody& b = m_bodies[_j].body;

    if (!a.IsDynamic() && !b.IsDynamic())
        return;

    // Deux corps endormis sont en équilibre stable : rien de nouveau à résoudre.
    if (a.isSleeping && b.isSleeping)
        return;

    AABB aabbA = MakeBroadphaseAABB(a);
    AABB aabbB = MakeBroadphaseAABB(b);

    if (!aabbA.Intersects(aabbB))
        return;

    Manifold manifold;
    if (!NarrowPhase(a, b, manifold))
        return;

    // Un contact persistant (résultant du repos) ne doit pas empêcher un tas
    // de s'endormir : on ne réveille un corps endormi que si son partenaire
    // porte une vitesse réellement significative (impact), pas simplement
    // parce qu'il chevauche encore géométriquement son voisin au repos.
    bool aMoving = a.IsDynamic() && a.linearVelocity.LengthSquared() >= k_sleepLinearVelocitySqThreshold;
    bool bMoving = b.IsDynamic() && b.linearVelocity.LengthSquared() >= k_sleepLinearVelocitySqThreshold;

    if (a.isSleeping && !bMoving)
        return;
    if (b.isSleeping && !aMoving)
        return;

    if (a.isSleeping) { a.isSleeping = false; a.sleepTimer = 0.0f; }
    if (b.isSleeping) { b.isSleeping = false; b.sleepTimer = 0.0f; }

    ResolveContact(a, b, manifold);
}

void PhysicsWorld::DrainCommands()
{
    Queue<Command> commands;
    {
        LockGuard lock(m_commandMutex);
        std::swap(commands, m_commands);
    }

    while (!commands.empty())
    {
        Command const& cmd = commands.front();
        RigidBody* body = GetBodyUnlocked(cmd.handle);

        if (body != nullptr)
        {
            // Toute commande explicite réveille le corps : un ApplyImpulse/
            // SetVelocity sur un corps endormi doit avoir un effet immédiat,
            // pas être écrasé par le maintien à zéro d'un corps qui dort.
            body->isSleeping = false;
            body->sleepTimer = 0.0f;

            switch (cmd.type)
            {
            case CommandType::ApplyForce:
                body->forceAccum += cmd.value;
                break;

            case CommandType::ApplyImpulse:
                if (body->IsDynamic())
                    body->linearVelocity += cmd.value * body->invMass;
                break;

            case CommandType::SetVelocity:
                body->linearVelocity = cmd.value;
                break;

            case CommandType::SetPosition:
                body->position = cmd.value;
                break;
            }
        }

        commands.pop();
    }
}

void PhysicsWorld::PushCommand(CommandType _type, BodyHandle _handle, Vect3f32 const& _value)
{
    if (!_handle.IsValid())
        return;

    Command cmd{ _type, _handle, _value };

    LockGuard lock(m_commandMutex);
    m_commands.push(cmd);
}

RigidBody* PhysicsWorld::GetBodyUnlocked(BodyHandle _handle)
{
    if (!_handle.IsValid())
        return nullptr;

    if (_handle.index >= m_bodies.size())
        return nullptr;

    BodySlot& slot = m_bodies[_handle.index];
    if (!slot.alive || slot.generation != _handle.generation)
        return nullptr;

    return &slot.body;
}

PhysicsWorld::BodyHandle PhysicsWorld::CreateBody(RigidBody const& _desc)
{
    LockGuard lock(m_bodiesMutex);

    uint32 index;

    if (!m_freeList.empty())
    {
        index = m_freeList.back();
        m_freeList.pop_back();
    }
    else
    {
        index = (uint32)m_bodies.size();
        m_bodies.push_back(BodySlot());
    }

    BodySlot& slot = m_bodies[index];
    slot.body  = _desc;
    slot.alive = true;

    return BodyHandle{ index, slot.generation };
}

void PhysicsWorld::DestroyBody(BodyHandle _handle)
{
    LockGuard lock(m_bodiesMutex);

    if (!_handle.IsValid() || _handle.index >= m_bodies.size())
        return;

    BodySlot& slot = m_bodies[_handle.index];
    if (!slot.alive || slot.generation != _handle.generation)
        return;

    slot.alive = false;
    slot.generation++;
    m_freeList.push_back(_handle.index);
}

void PhysicsWorld::ApplyForce(BodyHandle _handle, Vect3f32 const& _force)
{
    PushCommand(CommandType::ApplyForce, _handle, _force);
}

void PhysicsWorld::ApplyImpulse(BodyHandle _handle, Vect3f32 const& _impulse)
{
    PushCommand(CommandType::ApplyImpulse, _handle, _impulse);
}

void PhysicsWorld::SetLinearVelocity(BodyHandle _handle, Vect3f32 const& _velocity)
{
    PushCommand(CommandType::SetVelocity, _handle, _velocity);
}

void PhysicsWorld::SetPosition(BodyHandle _handle, Vect3f32 const& _position)
{
    PushCommand(CommandType::SetPosition, _handle, _position);
}

void PhysicsWorld::SetGravity(Vect3f32 const& _gravity)
{
    LockGuard lock(m_bodiesMutex);
    m_gravity = _gravity;
}

void PhysicsWorld::SetBroadphaseCellSize(float _cellSize)
{
    LockGuard lock(m_bodiesMutex);
    m_broadphaseCellSize = MathUtils::Max(_cellSize, 0.01f);
}

void PhysicsWorld::GetSnapshot(Vector<BodySnapshot>& _out)
{
    LockGuard lock(m_bodiesMutex);

    _out.clear();
    _out.reserve(m_bodies.size());

    for (uint32 i = 0; i < (uint32)m_bodies.size(); i++)
    {
        BodySlot const& slot = m_bodies[i];
        if (!slot.alive)
            continue;

        BodySnapshot snap;
        snap.handle         = BodyHandle{ i, slot.generation };
        snap.position       = slot.body.position;
        snap.linearVelocity = slot.body.linearVelocity;

        _out.push_back(snap);
    }
}

AABB PhysicsWorld::MakeBroadphaseAABB(RigidBody const& _body) const
{
    Vect3f32 center = _body.position + _body.collider.offset;

    switch (_body.collider.shape)
    {
    case ColliderShape::Sphere:
        return AABB::FromCenterExtent(center, Vect3f32(_body.collider.radius));

    case ColliderShape::OBB:
    {
        Mat3f32 const& o = _body.collider.orientation;
        Vect3f32 const& e = _body.collider.extent;

        Vect3f32 worldExtent =
            Vect3f32::Abs(o.rows[0]) * e.x +
            Vect3f32::Abs(o.rows[1]) * e.y +
            Vect3f32::Abs(o.rows[2]) * e.z;

        return AABB::FromCenterExtent(center, worldExtent);
    }

    case ColliderShape::AABB:
    default:
        return AABB::FromCenterExtent(center, _body.collider.extent);
    }
}

bool PhysicsWorld::NarrowPhase(RigidBody const& _a, RigidBody const& _b, Manifold& _outManifold) const
{
    Vect3f32 posA = _a.position + _a.collider.offset;
    Vect3f32 posB = _b.position + _b.collider.offset;

    switch (_a.collider.shape)
    {
    case ColliderShape::Sphere:
    {
        Sphere sa(posA, _a.collider.radius);

        switch (_b.collider.shape)
        {
        case ColliderShape::Sphere: return sa.Intersects(Sphere(posB, _b.collider.radius), &_outManifold);
        case ColliderShape::AABB:   return sa.Intersects(AABB::FromCenterExtent(posB, _b.collider.extent), &_outManifold);
        case ColliderShape::OBB:    return sa.Intersects(OBB(posB, _b.collider.extent, _b.collider.orientation), &_outManifold);
        }
        break;
    }

    case ColliderShape::AABB:
    {
        AABB aa = AABB::FromCenterExtent(posA, _a.collider.extent);

        switch (_b.collider.shape)
        {
        case ColliderShape::Sphere: return aa.Intersects(Sphere(posB, _b.collider.radius), &_outManifold);
        case ColliderShape::AABB:   return aa.Intersects(AABB::FromCenterExtent(posB, _b.collider.extent), &_outManifold);
        case ColliderShape::OBB:    return aa.Intersects(OBB(posB, _b.collider.extent, _b.collider.orientation), &_outManifold);
        }
        break;
    }

    case ColliderShape::OBB:
    {
        OBB oa(posA, _a.collider.extent, _a.collider.orientation);

        switch (_b.collider.shape)
        {
        case ColliderShape::Sphere: return oa.Intersects(Sphere(posB, _b.collider.radius), &_outManifold);
        case ColliderShape::AABB:   return oa.Intersects(AABB::FromCenterExtent(posB, _b.collider.extent), &_outManifold);
        case ColliderShape::OBB:    return oa.Intersects(OBB(posB, _b.collider.extent, _b.collider.orientation), &_outManifold);
        }
        break;
    }
    }

    return false;
}

void PhysicsWorld::ResolveContact(RigidBody& _a, RigidBody& _b, Manifold const& _manifold) const
{
    float invMassA = _a.IsDynamic() ? _a.invMass : 0.0f;
    float invMassB = _b.IsDynamic() ? _b.invMass : 0.0f;
    float invMassSum = invMassA + invMassB;

    if (invMassSum <= 0.0f)
        return;

    Vect3f32 rv = _b.linearVelocity - _a.linearVelocity;
    float velAlongNormal = Vect3f32::Dot(rv, _manifold.normal);

    if (velAlongNormal <= 0.0f)
    {
        // En dessous de ce seuil, on désactive la restitution : sans ça, un
        // contact au repos (vitesse de rapprochement quasi nulle réinjectée
        // par la gravité à chaque tick) rebondit indéfiniment au lieu de
        // converger vers zéro, ce qui empêche tout corps de s'endormir.
        const float restitutionVelocityThreshold = 1.0f;
        float e = (-velAlongNormal < restitutionVelocityThreshold) ? 0.0f : MathUtils::Min(_a.restitution, _b.restitution);
        float j = -(1.0f + e) * velAlongNormal / invMassSum;

        Vect3f32 impulse = _manifold.normal * j;
        _a.linearVelocity -= impulse * invMassA;
        _b.linearVelocity += impulse * invMassB;

        Vect3f32 tangent = rv - _manifold.normal * velAlongNormal;
        float tangentLenSq = tangent.LengthSquared();

        if (tangentLenSq > MathUtils::EPSILON)
        {
            tangent = tangent / MathUtils::Sqrt(tangentLenSq);

            float velAlongTangent = Vect3f32::Dot(rv, tangent);
            float jt = -velAlongTangent / invMassSum;

            float mu = MathUtils::Sqrt(_a.friction * _b.friction);
            jt = MathUtils::Clamp(jt, -j * mu, j * mu);

            Vect3f32 frictionImpulse = tangent * jt;
            _a.linearVelocity -= frictionImpulse * invMassA;
            _b.linearVelocity += frictionImpulse * invMassB;
        }
    }

    const float percent = 0.2f;
    const float slop    = 0.01f;

    float corrMag = MathUtils::Max(_manifold.penetration - slop, 0.0f) / invMassSum * percent;
    Vect3f32 correction = _manifold.normal * corrMag;

    _a.position -= correction * invMassA;
    _b.position += correction * invMassB;
}
