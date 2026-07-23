#include "OBB.h"

#include "AABB.h"
#include "Utils.hpp"
#include "Plane.h"
#include "Ray.h"
#include "Sphere.h"

OBB::OBB()
{
    position = Vect3f32();
    extent = Vect3f32(1.0f);
    
    orientation = Mat3f32::Identity();
}

OBB::OBB(Vect3f32 const& _pos, Vect3f32 const& _extent)
{
    position = _pos;
    extent = _extent;
    orientation = Mat3f32::Identity();
}

OBB::OBB(Vect3f32 const& _pos, Vect3f32 const& _extent, Mat3f32 const& _orientation)
{
    position = _pos;
    extent = _extent;
    orientation = _orientation;
}

Plane OBB::GetPlane(int _index) const
{
    _index %= 6;
    float sign = _index > 2 ? -1.0f : 1.0f;

    Vect3f32 normal = sign * orientation.rows[_index % 3];
    Vect3f32 pos = position + normal * extent[_index % 3];

    return Plane(normal, pos);
}

Vect3f32 OBB::GetVertex(int _planeIndex, int _vertexIndex) const
{
    _planeIndex %= 6;
    _vertexIndex %= 4;

    int indexX = 0;
    int indexY = 1;
    if (_planeIndex % 3 == 0)
        indexX = 2;
    else if (_planeIndex % 3 == 1)
        indexY = 2;

    float faceSign = (_planeIndex > 2)          ? -1.0f : 1.0f;
    float xSign    = (_vertexIndex > 1)         ? -1.0f : 1.0f;
    float ySign    = (_vertexIndex % 2 == 1)    ? -1.0f : 1.0f;

    int normalIndex = _planeIndex % 3;

    Vect3f32 res = position;
    res += faceSign * orientation.rows[normalIndex] * extent[normalIndex];
    res += xSign    * orientation.rows[indexX]      * extent[indexX];     
    res += ySign    * orientation.rows[indexY]      * extent[indexY];     

    return res;
}

void OBB::Expand(float _scalar)
{
    extent *= _scalar;
}

void OBB::Transform(Mat4f32 const& _t)
{
    Mat4f32 start = Mat4f32::MakeTransform(position, extent * 2.0f, orientation.ToMatrix4());
    
    start *= _t;
    
    start.FastDecompose(&position, &extent, &orientation);
    extent *= 0.5f;
}

OBB OBB::Transformed(Mat4f32 const& _t) const
{
    OBB result(position, extent, orientation);
    result.Transform(_t);
    
    return result;
}

bool OBB::Contains(Vect3f32 const& _pos) const
{
    Vect3f32 d = _pos - position;
    Mat3f32 rot = orientation;

    float px = Vect3f32::Dot(d, rot.rows[0]);
    if ( MathUtils::Abs(px) < extent.x )
        return false;
    
    float py = Vect3f32::Dot(d, rot.rows[1]);
    if ( MathUtils::Abs(py) < extent.y )
        return false;
    
    float pz = Vect3f32::Dot(d, rot.rows[2]);
    if ( MathUtils::Abs(pz) < extent.z )
        return false;
    
    return true;
}

bool OBB::Intersects(Ray const& _r, Vect3f32* _p) const
{
    return _r.Intersects(*this, _p);
}

bool OBB::Intersects(Plane const& _plane) const
{
    return _plane.Intersects(*this);
}

bool OBB::Intersects(AABB const& _a) const
{
    OBB a;
    a.position = _a.Center();
    a.extent = _a.Extent();
    a.orientation = Mat3f32::Identity();

    return Intersects(a);
}

bool OBB::Intersects(Sphere const& _s) const
{
    Vect3f32 diff = _s.center - position;

    Vect3f32 localCenter;
    localCenter.x = Vect3f32::Dot(diff, orientation.rows[0]);
    localCenter.y = Vect3f32::Dot(diff, orientation.rows[1]);
    localCenter.z = Vect3f32::Dot(diff, orientation.rows[2]);

    Vect3f32 closestLocal;
    closestLocal.x = MathUtils::Clamp(localCenter.x, -extent.x, extent.x);
    closestLocal.y = MathUtils::Clamp(localCenter.y, -extent.y, extent.y);
    closestLocal.z = MathUtils::Clamp(localCenter.z, -extent.z, extent.z);

    Vect3f32 diffLocal = closestLocal - localCenter;

    return diffLocal.LengthSquared() <= _s.radius * _s.radius;
}

bool OBB::Intersects(OBB const& _o) const
{
    Vect3f32 axesA[3] = { orientation.rows[0], orientation.rows[1], orientation.rows[2] };
    Vect3f32 axesB[3] = { _o.orientation.rows[0], _o.orientation.rows[1], _o.orientation.rows[2] };

    for (int i = 0; i < 3; i++)
        if (IsSeparate(axesA[i], *this, _o))
            return false;
    for (int i = 0; i < 3; i++)
        if (IsSeparate(axesB[i], *this, _o))
            return false;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            if ( IsSeparate(axesA[i] ^ axesB[j], *this, _o) )
                return false;
    }

    return true;
}

OBB OBB::Expand(OBB const& _o, float _scalar)
{
    OBB res(_o.position, _o.extent, _o.orientation);
    res.Expand(_scalar);

    return res;
}

OBB OBB::Transform(OBB const& _o, Mat4f32 const& _t)
{
    OBB res;
    Mat4f32 start = Mat4f32::MakeTransform(_o.position, _o.extent * 2.0f, _o.orientation.ToMatrix4());
    
    start *= _t;
    
    start.FastDecompose(&res.position, &res.extent, &res.orientation);
    res.extent *= 0.5f;
    return res;
}

bool OBB::IsSeparate(Vect3f32 const& _axis, OBB const& _a, OBB const& _b) const
{
    if ( _axis.LengthSquared() < MathUtils::LARGE_EPSILON )
        return false;
    
    float dist = MathUtils::Abs(Vect3f32::Dot(_b.position - _a.position, _axis));

    float radA =    _a.extent.x * MathUtils::Abs(Vect3f32::Dot(_a.orientation.rows[0], _axis)) +
                    _a.extent.y * MathUtils::Abs(Vect3f32::Dot(_a.orientation.rows[1], _axis)) +
                    _a.extent.z * MathUtils::Abs(Vect3f32::Dot(_a.orientation.rows[2], _axis));
    
    float radB =    _b.extent.x * MathUtils::Abs(Vect3f32::Dot(_b.orientation.rows[0], _axis)) +
                    _b.extent.y * MathUtils::Abs(Vect3f32::Dot(_b.orientation.rows[1], _axis)) +
                    _b.extent.z * MathUtils::Abs(Vect3f32::Dot(_b.orientation.rows[2], _axis));

    return dist > radA + radB;
}












