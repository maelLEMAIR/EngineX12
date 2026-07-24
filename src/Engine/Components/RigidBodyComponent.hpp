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

private:
    PhysicsWorld::BodyHandle handle;

    friend class PhysicsSystem;
};

#endif
