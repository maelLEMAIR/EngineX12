#ifndef RENDERITEM_UTILS_HPP
#define RENDERITEM_UTILS_HPP

#include "../../Common/Common.h"

struct Vertex
{
    Vect3f32 position;
    Vect2f32 uv;
    Vect3f32 normal = Vect3f32(0.0f, 0.0f, 0.0f);
    Vect4f32 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    Vect3f32 tangent = Vect3f32(0.0f, 0.0f, 0.0f);
};

enum class PrimitiveTopology
{
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip
};

enum BoundingVolumeType
{
    AABB_T, OBB_T, SPHERE_T
};

#endif
