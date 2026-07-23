#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include "define.h"

inline float Min(float a, float b)
{
    return (a < b) ? a : b;
}

inline float Max(float a, float b)
{
    return (a > b) ? a : b;
}

inline XMFLOAT3 Add(const XMFLOAT3& a, const XMFLOAT3& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

inline XMFLOAT3 Subtract(const XMFLOAT3& a, const XMFLOAT3& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

inline XMFLOAT3 Mul(const XMFLOAT3& v, float s)
{
    return { v.x * s, v.y * s, v.z * s };
}

inline XMFLOAT3 Mul(const XMFLOAT3& a, const XMFLOAT3& b)
{
    return { a.x * b.x, a.y * b.y, a.z * b.z };
}

inline XMFLOAT3 Div(const XMFLOAT3& v, float s)
{
    return { v.x / s, v.y / s, v.z / s };
}

inline XMFLOAT3 Div(const XMFLOAT3& a, const XMFLOAT3& b)
{
    return { a.x / b.x, a.y / b.y, a.z / b.z };
}

inline XMFLOAT3 Abs(const XMFLOAT3& v)
{
    return { fabsf(v.x), fabsf(v.y), fabsf(v.z) };
}

inline XMFLOAT3 Snap(const XMFLOAT3& _v, float _threshold)
{
    return
    {
        (fabsf(_v.x) <= _threshold) ? 0.0f : _v.x,
        (fabsf(_v.y) <= _threshold) ? 0.0f : _v.y,
        (fabsf(_v.z) <= _threshold) ? 0.0f : _v.z
    };
}

inline XMFLOAT3 Normalize(const XMFLOAT3& v)
{
    XMVECTOR vec = XMLoadFloat3(&v);
    vec = XMVector3Normalize(vec);
    XMFLOAT3 out;
    XMStoreFloat3(&out, vec);
    return out;
}

inline float GetAxisComponent(const XMFLOAT3& v, int i)
{
    return (i == 0) ? v.x : (i == 1) ? v.y : v.z;
}

inline XMFLOAT3 Cross(const XMFLOAT3& a, const XMFLOAT3& b)
{
    return 
    {
           a.y * b.z - a.z * b.y,
           a.z * b.x - a.x * b.z,
           a.x * b.y - a.y * b.x
    };
}

inline float LengthSq(const XMFLOAT3& v)
{
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

inline float Length(const XMFLOAT3& v)
{
    return sqrtf(LengthSq(v));
}

inline XMFLOAT3 NormalizeSafe(const XMFLOAT3& v, const XMFLOAT3& fallback = { 0.f, 1.f, 0.f })
{
    float len2 = LengthSq(v);
    if (len2 <= 1e-12f)
        return fallback;

    float invLen = 1.0f / sqrtf(len2);
    return { v.x * invLen, v.y * invLen, v.z * invLen };
}

inline float GetComponent(const XMFLOAT3& v, int i)
{
    return (i == 0) ? v.x : (i == 1) ? v.y : v.z;
}

inline void SetComponent(XMFLOAT3& v, int i, float value)
{
    if (i == 0) v.x = value;
    else if (i == 1) v.y = value;
    else v.z = value;
}

inline float Dot(const XMFLOAT3& a, const XMFLOAT3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline XMFLOAT3 Inverse(const XMFLOAT3& v)
{
    return XMFLOAT3(-v.x, -v.y, -v.z);
}

inline XMFLOAT3 MulMat3Vec3(float m[9], const XMFLOAT3& v)
{
    return
    {
        m[0] * v.x + m[1] * v.y + m[2] * v.z,
        m[3] * v.x + m[4] * v.y + m[5] * v.z,
        m[6] * v.x + m[7] * v.y + m[8] * v.z
    };
}

inline XMFLOAT3 AddScaled(const XMFLOAT3& a, const XMFLOAT3& b, float s)
{
    return { a.x + b.x * s, a.y + b.y * s, a.z + b.z * s };
}

inline float Clamp(float v, float min, float max)
{
    if ( v<min )
        return min;
    if ( v>max )
        return max;
    return v;
}

inline int Clamp(int v, int min, int max)
{
    if ( v<min )
        return min;
    if ( v>max )
        return max;
    return v;
}

inline XMFLOAT3 ToColor(XMINT3 _color)
{
    XMFLOAT3 color;
    color.x = Clamp(_color.x, 0, 255)/255.0f;
    color.y = Clamp(_color.y, 0, 255)/255.0f;
    color.z = Clamp(_color.z, 0, 255)/255.0f;
    return color;
}

inline XMFLOAT3 ToColor(int _r, int _g, int _b)
{
    XMFLOAT3 color;
    color.x = Clamp(_r, 0, 255)/255.0f;
    color.y = Clamp(_g, 0, 255)/255.0f;
    color.z = Clamp(_b, 0, 255)/255.0f;
    return color;
}

#endif
