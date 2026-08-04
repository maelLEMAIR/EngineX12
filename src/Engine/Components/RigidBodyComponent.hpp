#ifndef RIGID_BODY_COMPONENT_HPP_DEFINED
#define RIGID_BODY_COMPONENT_HPP_DEFINED

#include "../../Core/Physics/PhysicsWorld.h"

// Paramètres physiques d'une entité. PhysicsSystem crée le RigidBody
// correspondant dans le PhysicsWorld et range le handle obtenu ici.
struct RigidBodyComponent
{
    float invMass       = 1.0f; // 0 = statique/kinematic
    float restitution   = 0.2f;
    float friction      = 0.5f;
    float gravityScale  = 1.0f;
    bool  isStatic      = false;

    Vect3f32 initialVelocity = Vect3f32(0.0f, 0.0f, 0.0f);

    // Mouvement applicable directement depuis le composant, sans passer par
    // PhysicsSystem : valides une fois le corps ajouté au PhysicsWorld
    // (cf. PhysicsSystem::AddToPhysicWorld), no-op sinon.
    void ApplyForce(Vect3f32 const& _force)
    {
        if (m_pWorld != nullptr && handle.IsValid())
            m_pWorld->ApplyForce(handle, _force);
    }

    void ApplyImpulse(Vect3f32 const& _impulse)
    {
        if (m_pWorld != nullptr && handle.IsValid())
            m_pWorld->ApplyImpulse(handle, _impulse);
    }

    void SetLinearVelocity(Vect3f32 const& _velocity)
    {
        if (m_pWorld != nullptr && handle.IsValid())
            m_pWorld->SetLinearVelocity(handle, _velocity);
    }

private:
    PhysicsWorld::BodyHandle handle;
    PhysicsWorld*            m_pWorld = nullptr;

    friend class PhysicsSystem;
};

#endif
