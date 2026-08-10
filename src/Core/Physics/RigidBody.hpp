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
    float maxFallSpeed  = 0.0f; // vitesse max le long de la gravité, 0 = illimité
    bool  isStatic      = false;

    bool  isSleeping    = false; // corps dynamique au repos, exclu de l'intégration/broadphase statique
    float sleepTimer    = 0.0f;  // temps cumulé sous le seuil de vitesse de sommeil

    Collider collider;

    bool IsDynamic() const { return !isStatic && invMass > 0.0f; }
};

#endif
