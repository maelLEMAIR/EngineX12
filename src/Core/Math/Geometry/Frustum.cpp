#include "Frustum.h"

#include "AABB.h"
#include "OBB.h"
#include "Sphere.h"

Frustum::Frustum(bool _isGl)
{
    left = Plane(Vect3f32(1,0,0), -1.0f);
    right = Plane(Vect3f32(-1,0,0), -1.0f);
    top = Plane(Vect3f32(0, -1, 0), -1.0f);
    bottom = Plane(Vect3f32(0, 1, 0), -1.0f);
    farZ = Plane(Vect3f32(0, 0, -1), -1.0f);
    if (_isGl)
        nearZ = Plane(Vect3f32(0, 0, 1), -1.0f);
    else
        nearZ = Plane(Vect3f32(0, 0, 1), 0.0f);
}

Frustum::Frustum(Mat4f32 const& _view, Mat4f32 const& _proj, bool _isGl)
{
    Mat4f32 vp = _view * _proj;
    
    Vect4f32 C0 = vp.GetColumn(0);
    Vect4f32 C1 = vp.GetColumn(1);
    Vect4f32 C2 = vp.GetColumn(2);
    Vect4f32 C3 = vp.GetColumn(3);

    Vect4f32 raw[6] =
    {
        C3 + C0, // Left
        C3 - C0, // Right
        C3 + C1, // Bottom
        C3 - C1, // Top
        C2,      // Near
        C3 - C2, // Far
    };

    if (_isGl)
        raw[4] += C3;

    for (int i = 0; i < 6; i++)
    {
        Vect3f32 normal(raw[i].x, raw[i].y, raw[i].z);
        float len = normal.Length();
        planes[i] = Plane(normal / len, -raw[i].w / len);
    }
}

Frustum::Frustum(Mat4f32 const& _vp, bool _isGl)
{
    Vect4f32 C0 = _vp.GetColumn(0);
    Vect4f32 C1 = _vp.GetColumn(1);
    Vect4f32 C2 = _vp.GetColumn(2);
    Vect4f32 C3 = _vp.GetColumn(3);

    Vect4f32 raw[6] =
    {
        C3 + C0, // Left
        C3 - C0, // Right
        C3 + C1, // Bottom
        C3 - C1, // Top
        C2,      // Near
        C3 - C2, // Far
    };

    if (_isGl)
        raw[4] += C3;

    for (int i = 0; i < 6; i++)
    {
        Vect3f32 normal(raw[i].x, raw[i].y, raw[i].z);
        float len = normal.Length();
        planes[i] = Plane(normal / len, -raw[i].w / len);
    }
}

bool Frustum::Contains(Vect3f32 const& _p) const
{
    for (int i = 0; i < 6; i++)
    {
        if (planes[i].ClassifyPoint(_p) < 0)
            return false;
    }

    return true;
}

bool Frustum::Intersects(AABB const& _o) const
{
    for (int i = 0; i < 6; i++)
    {
        Vect3f32 n = planes[i].normal;
        
        Vect3f32 pVertex = {
            n.x >= 0 ? _o.max.x : _o.min.x,
            n.y >= 0 ? _o.max.y : _o.min.y,
            n.z >= 0 ? _o.max.z : _o.min.z
        };

        if (planes[i].ClassifyPoint(pVertex) < 0)
            return false;
    }

    return true;
}

bool Frustum::Intersects(Sphere const& _s) const
{
    for (int i = 0; i < 6; i++)
    {
        float d = planes[i].DistanceToPoint(_s.center);

        if (d < - _s.radius)
            return false;
    }

    return true;
}

bool Frustum::Intersects(OBB const& _o) const
{
    for (int i = 0; i < 6; i++)
    {
        Vect3f32 n = planes[i].normal;

        float s0 = Vect3f32::Dot(n, _o.orientation.rows[0]) >= 0 ? 1.0f : -1.0f;
        float s1 = Vect3f32::Dot(n, _o.orientation.rows[1]) >= 0 ? 1.0f : -1.0f;
        float s2 = Vect3f32::Dot(n, _o.orientation.rows[2]) >= 0 ? 1.0f : -1.0f;
        
        Vect3f32 pVertex = _o.position +
            s0 * _o.orientation.rows[0] * _o.extent.x +
            s1 * _o.orientation.rows[1] * _o.extent.y +
            s2 * _o.orientation.rows[2] * _o.extent.z;

        if (planes[i].ClassifyPoint(pVertex) < 0)
            return false;
    }

    return true;
}
