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

bool OBB::Intersects(AABB const& _a, Manifold* _manifold) const
{
    OBB a;
    a.position = _a.Center();
    a.extent = _a.Extent();
    a.orientation = Mat3f32::Identity();

    return Intersects(a, _manifold);
}

bool OBB::Intersects(Sphere const& _s, Manifold* _manifold) const
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

    bool inside = (closestLocal == localCenter);
    Vect3f32 diffLocal = closestLocal - localCenter;
    float distSq = diffLocal.LengthSquared();

    if (!inside && distSq > _s.radius * _s.radius)
        return false;

    if (_manifold != nullptr)
    {
        Vect3f32 normalLocal;
        Vect3f32 contactLocal;
        float penetration;

        if (inside)
        {
            float dist[6] =
            {
                localCenter.x - (-extent.x), extent.x - localCenter.x,
                localCenter.y - (-extent.y), extent.y - localCenter.y,
                localCenter.z - (-extent.z), extent.z - localCenter.z
            };

            int best = 0;
            for (int i = 1; i < 6; i++)
                if (dist[i] < dist[best])
                    best = i;

            int axis = best / 2;
            float sign = (best % 2 == 0) ? -1.0f : 1.0f; // face min -> outward normal -axis, face max -> +axis

            normalLocal = Vect3f32(0.0f, 0.0f, 0.0f);
            normalLocal[axis] = sign;

            contactLocal = localCenter;
            contactLocal[axis] = (best % 2 == 0) ? -extent[axis] : extent[axis];

            penetration = _s.radius + dist[best];
        }
        else
        {
            float dist = MathUtils::Sqrt(distSq);
            normalLocal = (dist > MathUtils::EPSILON) ? -diffLocal / dist : Vect3f32(0.0f, 1.0f, 0.0f);
            contactLocal = closestLocal;
            penetration = _s.radius - dist;
        }

        Vect3f32 normalWorld =  normalLocal.x * orientation.rows[0] +
                                normalLocal.y * orientation.rows[1] +
                                normalLocal.z * orientation.rows[2];

        Vect3f32 contactWorld = position +
                                contactLocal.x * orientation.rows[0] +
                                contactLocal.y * orientation.rows[1] +
                                contactLocal.z * orientation.rows[2];

        _manifold->normal = normalWorld;
        _manifold->contact = contactWorld;
        _manifold->penetration = penetration;
    }

    return true;
}

namespace
{
    void ClosestPtSegmentSegment(Vect3f32 const& _p1, Vect3f32 const& _q1, Vect3f32 const& _p2, Vect3f32 const& _q2, Vect3f32& _c1, Vect3f32& _c2)
    {
        Vect3f32 d1 = _q1 - _p1;
        Vect3f32 d2 = _q2 - _p2;
        Vect3f32 r = _p1 - _p2;

        float a = d1.LengthSquared();
        float e = d2.LengthSquared();
        float f = Vect3f32::Dot(d2, r);

        float s, t;

        if (a <= MathUtils::LARGE_EPSILON && e <= MathUtils::LARGE_EPSILON)
        {
            _c1 = _p1;
            _c2 = _p2;
            return;
        }

        if (a <= MathUtils::LARGE_EPSILON)
        {
            s = 0.0f;
            t = MathUtils::Clamp(f / e, 0.0f, 1.0f);
        }
        else
        {
            float c = Vect3f32::Dot(d1, r);

            if (e <= MathUtils::LARGE_EPSILON)
            {
                t = 0.0f;
                s = MathUtils::Clamp(-c / a, 0.0f, 1.0f);
            }
            else
            {
                float b = Vect3f32::Dot(d1, d2);
                float denom = a * e - b * b;

                s = (denom != 0.0f) ? MathUtils::Clamp((b * f - c * e) / denom, 0.0f, 1.0f) : 0.0f;
                t = (b * s + f) / e;

                if (t < 0.0f)
                {
                    t = 0.0f;
                    s = MathUtils::Clamp(-c / a, 0.0f, 1.0f);
                }
                else if (t > 1.0f)
                {
                    t = 1.0f;
                    s = MathUtils::Clamp((b - c) / a, 0.0f, 1.0f);
                }
            }
        }

        _c1 = _p1 + d1 * s;
        _c2 = _p2 + d2 * t;
    }
}

bool OBB::Intersects(OBB const& _o, Manifold* _manifold) const
{
    Vect3f32 axesA[3] = { orientation.rows[0], orientation.rows[1], orientation.rows[2] };
    Vect3f32 axesB[3] = { _o.orientation.rows[0], _o.orientation.rows[1], _o.orientation.rows[2] };

    Vect3f32 d = _o.position - position;

    float bestFacePen = MathUtils::FLOAT_MAX;
    int   bestFaceSource = -1; // 0 = this, 1 = _o
    int   bestFaceIndex = -1;
    Vect3f32 bestFaceAxis;

    for (int i = 0; i < 3; i++)
    {
        Vect3f32 axis = axesA[i];
        float radA = extent[i];
        float radB =    _o.extent.x * MathUtils::Abs(Vect3f32::Dot(axesB[0], axis)) +
                        _o.extent.y * MathUtils::Abs(Vect3f32::Dot(axesB[1], axis)) +
                        _o.extent.z * MathUtils::Abs(Vect3f32::Dot(axesB[2], axis));

        float pen = (radA + radB) - MathUtils::Abs(Vect3f32::Dot(d, axis));
        if (pen <= 0.0f)
            return false;

        if (pen < bestFacePen)
        {
            bestFacePen = pen;
            bestFaceSource = 0;
            bestFaceIndex = i;
            bestFaceAxis = axis;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        Vect3f32 axis = axesB[i];
        float radB = _o.extent[i];
        float radA =    extent.x * MathUtils::Abs(Vect3f32::Dot(axesA[0], axis)) +
                        extent.y * MathUtils::Abs(Vect3f32::Dot(axesA[1], axis)) +
                        extent.z * MathUtils::Abs(Vect3f32::Dot(axesA[2], axis));

        float pen = (radA + radB) - MathUtils::Abs(Vect3f32::Dot(d, axis));
        if (pen <= 0.0f)
            return false;

        if (pen < bestFacePen)
        {
            bestFacePen = pen;
            bestFaceSource = 1;
            bestFaceIndex = i;
            bestFaceAxis = axis;
        }
    }

    float bestEdgePen = MathUtils::FLOAT_MAX;
    int bestEdgeI = -1;
    int bestEdgeJ = -1;
    Vect3f32 bestEdgeAxis;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            Vect3f32 axis = axesA[i] ^ axesB[j];

            if (axis.LengthSquared() < MathUtils::LARGE_EPSILON)
                continue;

            axis = axis.Normalized();

            float radA =    extent.x * MathUtils::Abs(Vect3f32::Dot(axesA[0], axis)) +
                            extent.y * MathUtils::Abs(Vect3f32::Dot(axesA[1], axis)) +
                            extent.z * MathUtils::Abs(Vect3f32::Dot(axesA[2], axis));

            float radB =    _o.extent.x * MathUtils::Abs(Vect3f32::Dot(axesB[0], axis)) +
                            _o.extent.y * MathUtils::Abs(Vect3f32::Dot(axesB[1], axis)) +
                            _o.extent.z * MathUtils::Abs(Vect3f32::Dot(axesB[2], axis));

            float pen = (radA + radB) - MathUtils::Abs(Vect3f32::Dot(d, axis));
            if (pen <= 0.0f)
                return false;

            if (pen < bestEdgePen)
            {
                bestEdgePen = pen;
                bestEdgeI = i;
                bestEdgeJ = j;
                bestEdgeAxis = axis;
            }
        }
    }

    if (_manifold == nullptr)
        return true;

    const float relTolerance = 0.95f;
    const float absTolerance = 0.01f;

    bool useFace = (bestEdgeI < 0) || (bestFacePen <= bestEdgePen * relTolerance + absTolerance);

    if (useFace)
    {
        Vect3f32 refAxis = bestFaceAxis;
        if (Vect3f32::Dot(d, refAxis) < 0.0f)
            refAxis = -refAxis;

        OBB const& refBox = (bestFaceSource == 0) ? *this : _o;
        OBB const& incBox = (bestFaceSource == 0) ? _o : *this;
        Vect3f32 const* incAxes = (bestFaceSource == 0) ? axesB : axesA;

        int incFace = 0;
        float bestAbsDot = MathUtils::Abs(Vect3f32::Dot(incAxes[0], refAxis));
        for (int i = 1; i < 3; i++)
        {
            float ad = MathUtils::Abs(Vect3f32::Dot(incAxes[i], refAxis));
            if (ad > bestAbsDot)
            {
                bestAbsDot = ad;
                incFace = i;
            }
        }

        float incFaceDot = Vect3f32::Dot(incAxes[incFace], refAxis);
        float incFaceSign = (incFaceDot > 0.0f) ? -1.0f : 1.0f;

        Vect3f32 deepestVertex = incBox.position + incAxes[incFace] * (incFaceSign * incBox.extent[incFace]);

        for (int k = 0; k < 3; k++)
        {
            if (k == incFace)
                continue;

            float sign = (Vect3f32::Dot(incAxes[k], refAxis) < 0.0f) ? 1.0f : -1.0f;
            deepestVertex += incAxes[k] * (sign * incBox.extent[k]);
        }

        Vect3f32 refFaceCenter = refBox.position + refAxis * refBox.extent[bestFaceIndex];
        Vect3f32 contact = deepestVertex - refAxis * Vect3f32::Dot(deepestVertex - refFaceCenter, refAxis);

        _manifold->normal = refAxis;
        _manifold->contact = contact;
        _manifold->penetration = bestFacePen;
    }
    else
    {
        Vect3f32 axis = bestEdgeAxis;
        if (Vect3f32::Dot(d, axis) < 0.0f)
            axis = -axis;

        int i = bestEdgeI;
        int j = bestEdgeJ;

        Vect3f32 baseA = position;
        for (int k = 0; k < 3; k++)
        {
            if (k == i)
                continue;

            float sign = (Vect3f32::Dot(d, axesA[k]) < 0.0f) ? -1.0f : 1.0f;
            baseA += axesA[k] * (sign * extent[k]);
        }

        Vect3f32 baseB = _o.position;
        for (int k = 0; k < 3; k++)
        {
            if (k == j)
                continue;

            float sign = (Vect3f32::Dot(d, axesB[k]) < 0.0f) ? 1.0f : -1.0f;
            baseB += axesB[k] * (sign * _o.extent[k]);
        }

        Vect3f32 pA0 = baseA - axesA[i] * extent[i];
        Vect3f32 pA1 = baseA + axesA[i] * extent[i];
        Vect3f32 pB0 = baseB - axesB[j] * _o.extent[j];
        Vect3f32 pB1 = baseB + axesB[j] * _o.extent[j];

        Vect3f32 c1, c2;
        ClosestPtSegmentSegment(pA0, pA1, pB0, pB1, c1, c2);

        _manifold->normal = axis;
        _manifold->contact = (c1 + c2) * 0.5f;
        _manifold->penetration = bestEdgePen;
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