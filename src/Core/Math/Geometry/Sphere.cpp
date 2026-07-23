#include "Sphere.h"

#include "AABB.h"
#include "OBB.h"
#include "Ray.h"
#include "Plane.h"
#include "Utils.hpp"

Sphere::Sphere()
{
    center = Vect3f32(0.0f);
    radius = 1.0f;
}

Sphere::Sphere(Vect3f32 const& _center, float _radius)
{
    center = _center;
    radius = _radius;
}

void Sphere::Merge(Sphere const& _sphere)
{
    center = (center + _sphere.center) / 2.0f;
    radius = (radius + _sphere.radius) / 2.0f;
}

void Sphere::Union(Sphere const& _sphere)
{
    Vect3f32 newCenter = (center + _sphere.center) / 2.0f;
    float disToCenter = (newCenter - center).Length();
    
    center = newCenter;
    radius = disToCenter + MathUtils::Max(radius, _sphere.radius);
}

void Sphere::Expand(float _scalar)
{
    radius *= _scalar;
}

void Sphere::Transform(Mat4f32 const& _t)
{
    Vect3f32 position;
    Vect3f32 scale;
    
    Mat4f32::FastDecompose(_t, &position, &scale, (Quaternion*)nullptr);
    
    center = position;
    radius *= (scale.x + scale.y + scale.z) / 3.0f; 
}

Sphere Sphere::Transformed(Mat4f32 const& _t)
{
    Vect3f32 position;
    Vect3f32 scale;
    
    Mat4f32::FastDecompose(_t, &position, &scale, (Quaternion*)nullptr);
    
    Vect3f32 nCenter = position;
    float nRadius = radius * (scale.x + scale.y + scale.z) / 3.0f; 
    return Sphere(nCenter, nRadius);
}

// TODO use Squared length when possible : avoid expensive Sqrt calculations

bool Sphere::Contains(Vect3f32 const& _p) const
{
    float dist = (center - _p).Length();
    
    return dist <= radius;
}

bool Sphere::Contains(Sphere const& _o) const
{
    float dist = (center - _o.center).Length();
    
    return dist + _o.radius <= radius;
}

bool Sphere::Intersects(Ray const& _r, Vect3f32* _p) const
{
    return _r.Intersects(*this, _p);
}

bool Sphere::Intersects(Plane const& _plane) const
{
    return _plane.Intersects(*this);
}

bool Sphere::Intersects(AABB const& _a) const
{
    Vect3f32 closest;
    closest.x = MathUtils::Clamp(center.x, _a.min.x, _a.max.x);
    closest.y = MathUtils::Clamp(center.y, _a.min.y, _a.max.y);
    closest.z = MathUtils::Clamp(center.z, _a.min.z, _a.max.z);

    Vect3f32 diff = closest - center;
    return diff.LengthSquared() <= radius * radius;
}

bool Sphere::Intersects(Sphere const& _o) const
{
    float dist = (center - _o.center).LengthSquared();

    return dist <= (radius + _o.radius) * (radius * _o.radius);
}

bool Sphere::Intersects(OBB const& _o) const
{
    return _o.Intersects(*this);
}

Sphere Sphere::Merge(Sphere const& _sphere, Sphere const& _o)
{
    Vect3f32 nCenter = (_sphere.center + _o.center) / 2.0f;
    float nRadius = (_sphere.radius + _o.radius) / 2.0f;
    
    return Sphere(nCenter, nRadius);
}

Sphere Sphere::Union(Sphere const& _sphere, Sphere const& _o)
{
    Vect3f32 newCenter = (_sphere.center + _o.center) / 2.0f;
    float disToCenter = (newCenter - _sphere.center).Length();
    
    Vect3f32 nCenter = newCenter;
    float nRadius = disToCenter + MathUtils::Max(_o.radius, _sphere.radius);
    
    return Sphere(nCenter, nRadius);
}

Sphere Sphere::Expand(Sphere const& _sphere, float _scalar)
{
    return Sphere(_sphere.center, _sphere.radius * _scalar);
}

Sphere Sphere::Transform(Sphere const& _sphere, Mat4f32 const& _t)
{
    Vect3f32 position;
    Vect3f32 scale;
    
    Mat4f32::FastDecompose(_t, &position, &scale, (Quaternion*)nullptr);
    
    Vect3f32 nCenter = position;
    float nRadius = _sphere.radius * (scale.x + scale.y + scale.z) / 3.0f; 
    return Sphere(nCenter, nRadius);
}

















