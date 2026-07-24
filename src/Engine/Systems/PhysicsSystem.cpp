#include "PhysicsSystem.h"
#include "../ECS/World.h"

namespace
{
    Vect3f32 ToVect3(XMFLOAT3 const& _v) { return Vect3f32(_v.x, _v.y, _v.z); }
    XMFLOAT3 ToXM(Vect3f32 const& _v)    { return XMFLOAT3(_v.x, _v.y, _v.z); }
}

void PhysicsSystem::OnRegister(World& world)
{
    System::OnRegister(world);

    m_physicsWorld.Start();

    m_onRigidBodyRemoved = world.OnComponentRemoved<RigidBodyComponent>(
        [this](World&, EntityId id) { DestroyBodyFor(id); }
    );

    m_onEntityDestroyed = world.OnEntityDestroyed(
        [this](World&, EntityId id) { DestroyBodyFor(id); }
    );
}

void PhysicsSystem::OnUnregister(World& world)
{
    world.RemoveObserver(m_onRigidBodyRemoved);
    world.RemoveObserver(m_onEntityDestroyed);

    m_physicsWorld.Stop();
}

void PhysicsSystem::Update(World& world, float deltaTime)
{
    world.QueryWithEntity<RigidBodyComponent, ColliderComponent, TransformComponent>(
        [&](EntityId id, RigidBodyComponent& rb, ColliderComponent& collider, TransformComponent& transform)
        {
            CreateBodyIfNeeded(id, rb, collider, transform);
        }
    );

    Vector<PhysicsWorld::BodySnapshot> snapshot;
    m_physicsWorld.GetSnapshot(snapshot);

    for (PhysicsWorld::BodySnapshot const& body : snapshot)
    {
        auto it = m_bodyToEntity.find(body.handle.index);
        if (it == m_bodyToEntity.end())
            continue;

        TransformComponent* transform = world.GetComponent<TransformComponent>(it->second);
        if (transform == nullptr)
            continue;

        transform->local.SetPosition(ToXM(body.position));
    }
}

void PhysicsSystem::SetGravity(Vect3f32 const& _gravity)
{
    m_physicsWorld.SetGravity(_gravity);
}

void PhysicsSystem::ApplyForce(EntityId _entity, Vect3f32 const& _force)
{
    auto it = m_entityToBody.find(_entity);
    if (it != m_entityToBody.end())
        m_physicsWorld.ApplyForce(it->second, _force);
}

void PhysicsSystem::ApplyImpulse(EntityId _entity, Vect3f32 const& _impulse)
{
    auto it = m_entityToBody.find(_entity);
    if (it != m_entityToBody.end())
        m_physicsWorld.ApplyImpulse(it->second, _impulse);
}

void PhysicsSystem::SetLinearVelocity(EntityId _entity, Vect3f32 const& _velocity)
{
    auto it = m_entityToBody.find(_entity);
    if (it != m_entityToBody.end())
        m_physicsWorld.SetLinearVelocity(it->second, _velocity);
}

void PhysicsSystem::CreateBodyIfNeeded(EntityId _id, RigidBodyComponent& _rb, ColliderComponent& _collider, TransformComponent& _transform)
{
    if (_rb.handle.IsValid())
        return;

    RigidBody desc;
    desc.position        = ToVect3(_transform.world.pos);
    desc.linearVelocity  = _rb.initialVelocity;
    desc.invMass         = _rb.isStatic ? 0.0f : _rb.invMass;
    desc.restitution     = _rb.restitution;
    desc.friction        = _rb.friction;
    desc.gravityScale    = _rb.gravityScale;
    desc.isStatic        = _rb.isStatic;
    desc.collider        = MakeCollider(_collider);

    PhysicsWorld::BodyHandle handle = m_physicsWorld.CreateBody(desc);

    _rb.handle = handle;
    m_entityToBody[_id] = handle;
    m_bodyToEntity[handle.index] = _id;
}

void PhysicsSystem::DestroyBodyFor(EntityId _id)
{
    auto it = m_entityToBody.find(_id);
    if (it == m_entityToBody.end())
        return;

    m_physicsWorld.DestroyBody(it->second);
    m_bodyToEntity.erase(it->second.index);
    m_entityToBody.erase(it);
}

Collider PhysicsSystem::MakeCollider(ColliderComponent const& _c) const
{
    Collider collider;
    collider.shape       = _c.shape;
    collider.offset      = _c.offset;
    collider.extent      = _c.extent;
    collider.radius      = _c.radius;
    collider.orientation = _c.orientation;
    return collider;
}
