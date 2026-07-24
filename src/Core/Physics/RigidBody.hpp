#ifndef RIGID_BODY_HPP_INCLUDED
#define RIGID_BODY_HPP_INCLUDED

#include "../Math/Vector/Vector.h"
#include "Collider.hpp"

// Etat d'un corps rigide en translation pure (pas de rotation physique en v1).
struct RigidBody
{
    Vect3f32 position        = Vect3f32(0.0f, 0.0f, 0.0f);
    Vect3f32 linearVelocity  = Vect3f32(0.0f, 0.0f, 0.0f);
    Vect3f32 forceAccum      = Vect3f32(0.0f, 0.0f, 0.0f);

    float invMass       = 1.0f; // 0 = corps statique/kinematic
    float restitution   = 0.2f;
    float friction      = 0.5f;
    float gravityScale  = 1.0f;
    bool  isStatic      = false;

    Collider collider;

    bool IsDynamic() const { return !isStatic && invMass > 0.0f; }
};

#endif
