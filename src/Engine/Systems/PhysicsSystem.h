#ifndef PHYSICS_SYSTEM_H_DEFINED
#define PHYSICS_SYSTEM_H_DEFINED

#include "../ECS/System.h"
#include "../Components/RigidBodyComponent.hpp"
#include "../Components/ColliderComponent.hpp"
#include "../Components/TransformComponent.hpp"
#include "../../Core/Physics/PhysicsWorld.h"

// Lien ECS <-> PhysicsWorld. Le PhysicsWorld tourne sur son propre thread ;
// Ce système ne fait que créer/détruire les corps et synchroniser, une fois
// par frame, la snapshot de positions vers TransformComponent.
class PhysicsSystem : public System
{
public:
    void OnRegister(World& world) override;
    void Update(World& world, float deltaTime) override;
    void OnUnregister(World& world) override;

    void SetGravity(Vect3f32 const& _gravity);

    void ApplyForce(EntityId _entity, Vect3f32 const& _force);
    void ApplyImpulse(EntityId _entity, Vect3f32 const& _impulse);
    void SetLinearVelocity(EntityId _entity, Vect3f32 const& _velocity);

private:
    void CreateBodyIfNeeded(EntityId _id, RigidBodyComponent& _rb, ColliderComponent& _collider, TransformComponent& _transform);
    void DestroyBodyFor(EntityId _id);

    Collider MakeCollider(ColliderComponent const& _c) const;

    PhysicsWorld m_physicsWorld;

    UnorderedMap<EntityId, PhysicsWorld::BodyHandle> m_entityToBody;
    UnorderedMap<uint32, EntityId>                   m_bodyToEntity;

    ObserverId m_onRigidBodyRemoved = 0;
    ObserverId m_onEntityDestroyed  = 0;
};

#endif
