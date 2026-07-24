#ifndef COLLIDER_HPP_INCLUDED
#define COLLIDER_HPP_INCLUDED

#include "../Math/Vector/Vector.h"
#include "../Math/Matrix/Matrix.h"

enum class ColliderShape
{
    Sphere,
    AABB,
    OBB
};

// Forme de collision d'un corps rigide, exprimée en espace local du corps.
// orientation n'est utilisée que pour ColliderShape::OBB et reste fixe
// (pas de dynamique angulaire en v1).
struct Collider
{
    ColliderShape shape = ColliderShape::Sphere;

    Vect3f32 offset  = Vect3f32(0.0f, 0.0f, 0.0f);
    Vect3f32 extent  = Vect3f32(0.5f, 0.5f, 0.5f); // demi-étendue (AABB/OBB)
    float    radius  = 0.5f;                       // Sphere

    Mat3f32 orientation = Mat3f32::Identity();     // OBB uniquement

    static Collider MakeSphere(float _radius, Vect3f32 const& _offset = Vect3f32(0.0f, 0.0f, 0.0f))
    {
        Collider c;
        c.shape  = ColliderShape::Sphere;
        c.radius = _radius;
        c.offset = _offset;
        return c;
    }

    static Collider MakeAABB(Vect3f32 const& _extent, Vect3f32 const& _offset = Vect3f32(0.0f, 0.0f, 0.0f))
    {
        Collider c;
        c.shape  = ColliderShape::AABB;
        c.extent = _extent;
        c.offset = _offset;
        return c;
    }

    static Collider MakeOBB(Vect3f32 const& _extent, Mat3f32 const& _orientation = Mat3f32::Identity(), Vect3f32 const& _offset = Vect3f32(0.0f, 0.0f, 0.0f))
    {
        Collider c;
        c.shape       = ColliderShape::OBB;
        c.extent      = _extent;
        c.orientation = _orientation;
        c.offset      = _offset;
        return c;
    }
};

#endif
