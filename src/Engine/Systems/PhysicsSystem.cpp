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

bool PhysicsSystem::AddToPhysicWorld(World& world, EntityId _entity)
{
    RigidBodyComponent* rb        = world.GetComponent<RigidBodyComponent>(_entity);
    ColliderComponent*  collider  = world.GetComponent<ColliderComponent>(_entity);
    TransformComponent* transform = world.GetComponent<TransformComponent>(_entity);

    if (rb == nullptr || collider == nullptr || transform == nullptr)
        return false;

    if (rb->handle.IsValid())
        return true;

    world.GetSystem<TransformSystem>()->Sync(world, _entity);

    RigidBody desc;
    desc.position = ToVect3(transform->world.pos);
    desc.linearVelocity = rb->initialVelocity;
    desc.invMass = rb->isStatic ? 0.0f : rb->invMass;
    desc.restitution = rb->restitution;
    desc.friction = rb->friction;
    desc.gravityScale = rb->gravityScale;
    desc.isStatic = rb->isStatic;
    desc.collider = MakeCollider(*collider);

    PhysicsWorld::BodyHandle handle = m_physicsWorld.CreateBody(desc);

    rb->handle = handle;
    rb->m_pWorld = &m_physicsWorld;
    m_entityToBody[_entity] = handle;
    m_bodyToEntity[handle.index] = _entity;

    return true;
}

void PhysicsSystem::SetGravity(Vect3f32 const& _gravity)
{
    m_physicsWorld.SetGravity(_gravity);
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
