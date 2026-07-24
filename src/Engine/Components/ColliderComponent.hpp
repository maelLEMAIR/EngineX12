#ifndef COLLIDER_COMPONENT_HPP_DEFINED
#define COLLIDER_COMPONENT_HPP_DEFINED

#include "../../Core/Physics/Collider.hpp"

// Forme de collision d'une entité, en espace local. Convertie en Collider
// (Core) par PhysicsSystem lors de la création du corps rigide.
struct ColliderComponent
{
    ColliderShape shape = ColliderShape::Sphere;

    Vect3f32 offset = Vect3f32(0.0f, 0.0f, 0.0f);
    Vect3f32 extent = Vect3f32(0.5f, 0.5f, 0.5f); // demi-étendue (AABB/OBB)
    float    radius = 0.5f;                       // Sphere

    Mat3f32 orientation = Mat3f32::Identity();    // OBB uniquement, fixe
};

#endif
