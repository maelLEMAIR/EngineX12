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

    float accumulator = 0.0f;

    while (m_running)
    {
        float tickDelay = 1.0f / m_tickRate;
        float frameTime = chrono.Reset();
        accumulator += frameTime;

        while (accumulator >= tickDelay)
        {
            Step(tickDelay);
            accumulator -= tickDelay;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void PhysicsWorld::Step(float _dt)
{
    LockGuard lock(m_bodiesMutex);

    DrainCommands();

    for (BodySlot& slot : m_bodies)
    {
        if (!slot.alive)
            continue;

        RigidBody& body = slot.body;
        if (!body.IsDynamic())
            continue;

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
        body.forceAccum = Vect3f32(0.0f, 0.0f, 0.0f);
    }

    size_t count = m_bodies.size();
    for (size_t i = 0; i < count; i++)
    {
        if (!m_bodies[i].alive)
            continue;

        for (size_t j = i + 1; j < count; j++)
        {
            if (!m_bodies[j].alive)
                continue;

            RigidBody& a = m_bodies[i].body;
            RigidBody& b = m_bodies[j].body;

            if (!a.IsDynamic() && !b.IsDynamic())
                continue;

            AABB aabbA = MakeBroadphaseAABB(a);
            AABB aabbB = MakeBroadphaseAABB(b);

            if (!aabbA.Intersects(aabbB))
                continue;

            Manifold manifold;
            if (NarrowPhase(a, b, manifold))
                ResolveContact(a, b, manifold);
        }
    }
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
        float e = MathUtils::Min(_a.restitution, _b.restitution);
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
