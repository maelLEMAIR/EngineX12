#include "Ray.h"

#include "AABB.h"
#include "OBB.h"
#include "Plane.h"
#include "Sphere.h"

Ray::Ray(Vect3f32 _origin, Vect3f32 _direction, float _length)
{
    origin = _origin;
    direction = _direction.Normalized();
    length = _length;
}

Ray::Ray(Vect3f32 _origin, Vect3f32 _direction)
{
    origin = _origin;
    direction = _direction.Normalized();
    length = -1.0f;
}

bool Ray::Contains(Vect3f32 const& _p) const
{
    if (length == 0.0f) return false;
    
    Vect3f32 diff = _p - origin;
    Vect3f32 dir = diff.Normalized();

    if ( Vect3f32::Dot(direction, dir) != 1.0f ) return false;
    if ( length < 0.0f ) return true;
    
    float dist = diff.LengthSquared();
    return dist < length * length;
}

bool Ray::Intersects(Ray const& _ray, Vect3f32* _p) const
{
    Vect3f32 w = (origin - _ray.origin).Normalized();

    float a = Vect3f32::Dot(w, direction);
    float b = 1.0f; // Assumes normalized dir1
    float c = Vect3f32::Dot(direction, _ray.direction);
    float d = Vect3f32::Dot(_ray.direction, w);
    float e = 1.0f; // Assumes normalized dir2
    float denom = 1.0f - c*c;

    if ( MathUtils::Abs(denom) < MathUtils::EPSILON )
        return false;

    float dReciprocal = 1.0f / denom;
    float t = (d*c - a*e) * dReciprocal;
    float s = (d*b - a*c) * dReciprocal;

    bool valid1 = false;
    bool valid2 = false;

    if ( length < 0.0f )
        valid1 = t >= 0.0f;
    else
        valid1 = t <= length;
    
    if ( _ray.length < 0.0f )
        valid2 = s >= 0.0f;
    else
        valid2 = s <= _ray.length;

    if (valid1 && valid2)
    {
        if (_p != nullptr)
            *_p = origin + t * direction;
        
        return true;
    }

    return false;
}

bool Ray::Intersects(Plane const& _plane, Vect3f32* _p) const
{
    float denom = Vect3f32::Dot(_plane.normal, direction);
    bool isParallel = MathUtils::Abs(denom) <= MathUtils::EPSILON;

    if (isParallel) return false;

    float t = ( _plane.distance - Vect3f32::Dot(_plane.normal, origin) ) / denom;

    if (length < 0.0f && _p == nullptr)
        return true;

    if (t < 0.0f)
        return false;

    if (length >= 0.0f && t > length)
        return false;

    if (_p != nullptr)
        *_p = origin + t * direction;

    return true;
}

bool Ray::Intersects(AABB const& _aabb, Vect3f32* _p) const
{
    Vect3f32 min = _aabb.min;
    Vect3f32 max = _aabb.max;

    float tNear = - MathUtils::FLOAT_MAX;
    float tFar  = MathUtils::FLOAT_MAX;

    for (int i = 0; i < 3; i++)
    {
        float o = origin[i];
        float d = direction[i];
        float bMin = min[i];
        float bMax = max[i];

        if (MathUtils::Abs(d) < MathUtils::EPSILON)
        {
            if (o < bMin || o > bMax)
                return false;
        }
        else
        {
            float t0 = (bMin - o) / d;
            float t1 = (bMax - o) / d;

            // TODO Add Swap util method & use
            if (t0 > t1)
            {
                float temp = t0;
                t0 = t1;
                t1 = temp;
            }

            tNear = MathUtils::Max(tNear, t0);
            tFar  = MathUtils::Min(tFar, t1);

            if (tNear > tFar)
                return false;
        }
    }

    if (tFar < 0.0f)
        return false;
    if (tNear > length)
        return false;

    if (_p != nullptr)
    {
        if (tNear >= 0.0f)
            *_p = origin + tNear * direction;
        else
            *_p = origin + tFar * direction;
    }
    
    return true;
}

bool Ray::Intersects(Sphere const& _sphere, Vect3f32* _p) const
{
    Vect3f32 diff = origin - _sphere.center;

    float a = 1.0f; // Assumes normalized direction
    float b = 2.0f * Vect3f32::Dot(direction, diff);
    float c = Vect3f32::Dot(diff, diff) - _sphere.radius * _sphere.radius;

    float disc = b * b - 4.0f * a * c;

    if (disc < 0.0f) return false;

    if (length < 0.0f && _p == nullptr)
        return true;

    float dReciprocal = 1.0f / (2.0f * a);
    float t;
    if (disc == 0.0f)
    {
        t = -b * dReciprocal;
    }
    else
    {
        float sqrtDisc = MathUtils::Sqrt(disc);
        t = (-b - sqrtDisc) * dReciprocal;
        if (t < 0.0f)
            t = (-b + sqrtDisc) * dReciprocal;
    }

    if (t < 0.0f || t > length)
        return false;

    if (_p != nullptr)
        *_p = origin + t * direction;

    return true;
}

bool Ray::Intersects(OBB const& _obb, Vect3f32* _p) const
{
    for (int i = 0; i < 6; i++)
    {
        Plane p = _obb.GetPlane(i);
        Vect3f32 v;
        if (Intersects(p, &v))
        {
            Vect3f32 a = _obb.GetVertex(i, 0);
            Vect3f32 b = _obb.GetVertex(i, 1);
            Vect3f32 c = _obb.GetVertex(i, 3);

            Vect3f32 edge1 = b - a;
            Vect3f32 edge2 = c - a;

            Vect3f32 dist = v - a;

            float edg1Dist = Vect3f32::Dot(dist, edge1);
            float edg2Dist = Vect3f32::Dot(dist, edge2);
            float edg1Max = edge1.LengthSquared();
            float edg2Max = edge2.LengthSquared();

            if (edg1Dist < 0.0f || edg2Dist < 0.0f)
                continue;
            if (edg1Dist > edg1Max || edg2Dist > edg2Max)
                continue;

            if (_p != nullptr)
                *_p = v;

            return true;
        }
    }

    return false;
}
