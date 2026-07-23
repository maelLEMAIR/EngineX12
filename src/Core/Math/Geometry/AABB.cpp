#include "AABB.h"

#include "OBB.h"
#include "Ray.h"
#include "Plane.h"
#include "Sphere.h"

AABB AABB::FromMinMax(Vect3f32 const& _min, Vect3f32 const& _max)
{
    return AABB(_min, _max);
}

AABB AABB::FromCenterExtent(Vect3f32 const& _center, Vect3f32 const& _extent)
{
    return AABB(_center - _extent, _center + _extent);
}

Vect3f32 AABB::Center() const
{
    return (max + min) / 2.0f;
}

Vect3f32 AABB::Extent() const
{
    return (max - min) / 2.0f;
}

float AABB::Volume() const
{
    Vect3f32 l = Extent() * 2.0f;
    
    return l.x * l.y * l.x;
}

void AABB::SetCenter(Vect3f32 const& _center)
{
    Vect3f32 extent = Extent();
    
    min = _center - extent;
    max = _center + extent;
}

void AABB::SetExtent(Vect3f32 const& _extent)
{
    Vect3f32 center = Center();
    
    min = center - _extent;
    max = center + _extent;
}

void AABB::Merge(AABB const& _o)
{
    Vect3f32 c = (Center() + _o.Center()) / 2.0f;
    Vect3f32 extent = (Extent() + _o.Extent()) / 2.0f;
    
    min = c - extent;
    max = c + extent;
}

void AABB::Union(AABB const& _o)
{
    min = Vect3f32::Min(min, _o.min);
    max = Vect3f32::Max(max, _o.max);
}

void AABB::Expand(float _scalar)
{
    Vect3f32 extent = Extent() * _scalar;
    Vect3f32 c = Center();
    
    min = c - extent;
    max = c + extent;
}

void AABB::Transform(Mat4f32 const& _t)
{
    Vect3f32 newMin = { _t[3][0], _t[3][1], _t[3][2] };
    Vect3f32 newMax = newMin;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            float coeff = _t[i][j];
            float minContrib = coeff * (coeff > 0.f ? min[i] : max[i]);
            float maxContrib = coeff * (coeff > 0.f ? max[i] : min[i]);
            newMin[j] += minContrib;
            newMax[j] += maxContrib;
        }
    }

    min = newMin;
    max = newMax;
}

AABB AABB::Transformed(Mat4f32 const& _t) const
{
    Vect3f32 newMin = { _t[3][0], _t[3][1], _t[3][2] };
    Vect3f32 newMax = newMin;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            float coeff = _t[i][j];
            float minContrib = coeff * (coeff > 0.f ? min[i] : max[i]);
            float maxContrib = coeff * (coeff > 0.f ? max[i] : min[i]);
            newMin[j] += minContrib;
            newMax[j] += maxContrib;
        }
    }

    return FromMinMax(newMin, newMax);
}

bool AABB::Contains(Vect3f32 const& _p) const
{
    bool validX = min.x <= _p.x && _p.x <= max.x;
    bool validY = min.y <= _p.y && _p.y <= max.y;
    bool validZ = min.z <= _p.z && _p.z <= max.z;
    
    return validX && validY && validZ;
}

bool AABB::Contains(AABB const& _o) const
{
    bool validX = min.x <= _o.min.x && max.x >= _o.max.x;
    bool validY = min.y <= _o.min.y && max.y >= _o.max.y;
    bool validZ = min.z <= _o.min.z && max.z >= _o.max.z;
    
    return validX && validY && validZ;
}

bool AABB::Intersects(Ray const& _r, Vect3f32* _p) const
{
    return _r.Intersects(*this, _p);
}

bool AABB::Intersects(Plane const& _plane) const
{
    return _plane.Intersects(*this);
}

bool AABB::Intersects(AABB const& _o) const
{
    Vect3f32 c = Center();
    Vect3f32 oC = _o.Center();
    
    Vect3f32 d = Vect3f32::Abs(c - oC);
    
    Vect3f32 extent = Extent();
    Vect3f32 oExtent = _o.Extent();
    
    bool validX = d.x <= extent.x + oExtent.x;
    bool validY = d.y <= extent.y + oExtent.y;
    bool validZ = d.z <= extent.z + oExtent.z;
    
    return validX && validY && validZ;
}

bool AABB::Intersects(Sphere const& _o) const
{
    return _o.Intersects(*this);
}

bool AABB::Intersects(OBB const& _o) const
{
    return _o.Intersects(*this);
}

AABB AABB::Merge(AABB const& _a, AABB const& _b)
{
    Vect3f32 c = (_a.Center() + _b.Center()) / 2.0f;
    Vect3f32 extent = (_a.Extent() + _b.Extent()) / 2.0f;
    
    return FromCenterExtent(c, extent);
}

AABB AABB::Union(AABB const& _a, AABB const& _b)
{
    return FromMinMax(Vect3f32::Min(_a.min, _b.min), Vect3f32::Max(_a.max, _b.max));   
}

AABB AABB::Expand(AABB const& _o, float _scalar)
{
    Vect3f32 extent = _o.Extent() * _scalar;
    Vect3f32 c = _o.Center();
    
    return FromCenterExtent(c, extent);
}

AABB AABB::Transform(AABB const& _o, Mat4f32 const& _t)
{
    AABB res;
    
    Vect3f32 newMin = { _t[3][0], _t[3][1], _t[3][2] };
    Vect3f32 newMax = newMin;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            float coeff = _t[i][j];
            float minContrib = coeff * (coeff > 0.f ? _o.min[i] : _o.max[i]);
            float maxContrib = coeff * (coeff > 0.f ? _o.max[i] : _o.min[i]);
            newMin[j] += minContrib;
            newMax[j] += maxContrib;
        }
    }

    res.min = newMin;
    res.max = newMax;

    return res;
}

AABB::AABB()
{
    min = Vect3f32(-1.0f);
    max = Vect3f32(1.0f);
}

AABB::AABB(Vect3f32 const& _min, Vect3f32 const& _max)
{
    min = _min;
    max = _max;
}


