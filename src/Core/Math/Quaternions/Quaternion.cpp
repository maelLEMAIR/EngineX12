
#include "Quaternion.h"
#include "../Matrix/Matrix3.h"
#include "../Matrix/Matrix4.h"
#include "../Vector/Vector4.h"
#include "../Vector/Vector3.h"

Quaternion::Quaternion() : x(0), y(0), z(0), w(1)
{
    
}

Quaternion::Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w)
{
}

Quaternion::Quaternion(Vector4<float> const& _v) :
    x(_v.x), y(_v.y), z(_v.z), w(_v.w)
{
    
}

Quaternion::Quaternion(std::initializer_list<float> _l)
{
    assert(_l.size() == 4 && "Incorrect init list size");
    
    const float* begin = _l.begin();
    x = begin[0];
    y = begin[1];
    z = begin[2];
    w = begin[3];
}

Quaternion Quaternion::FromEulerAngles(float _yaw, float _pitch, float _roll)
{
    Quaternion quat;
    quat.x = MathUtils::Sin(_roll / 2) * MathUtils::Cos(_pitch / 2) * MathUtils::Cos(_roll / 2) - MathUtils::Cos(_roll / 2) * MathUtils::Sin(_pitch / 2) * MathUtils::Sin(_roll / 2);
    quat.y = MathUtils::Cos(_roll / 2) * MathUtils::Sin(_pitch / 2) * MathUtils::Cos(_roll / 2) + MathUtils::Sin(_roll / 2) * MathUtils::Cos(_pitch / 2) * MathUtils::Sin(_roll / 2);
    quat.z = MathUtils::Cos(_roll / 2) * MathUtils::Cos(_pitch / 2) * MathUtils::Sin(_roll / 2) - MathUtils::Sin(_roll / 2) * MathUtils::Sin(_pitch / 2) * MathUtils::Cos(_roll / 2);
    quat.w = MathUtils::Cos(_roll / 2) * MathUtils::Cos(_pitch / 2) * MathUtils::Cos(_roll / 2) + MathUtils::Sin(_roll / 2) * MathUtils::Sin(_pitch / 2) * MathUtils::Sin(_roll / 2);
    return quat;
}

Quaternion Quaternion::FromEulerAngles(Vector3<float> const& _angles)
{
    Quaternion quat;
    quat.x = MathUtils::Sin(_angles.z / 2) * MathUtils::Cos(_angles.y / 2) * MathUtils::Cos(_angles.x / 2) - MathUtils::Cos(_angles.z / 2) * MathUtils::Sin(_angles.y / 2) * MathUtils::Sin(_angles.x / 2);
    quat.y = MathUtils::Cos(_angles.z / 2) * MathUtils::Sin(_angles.y / 2) * MathUtils::Cos(_angles.x / 2) + MathUtils::Sin(_angles.z / 2) * MathUtils::Cos(_angles.y / 2) * MathUtils::Sin(_angles.x / 2);
    quat.z = MathUtils::Cos(_angles.z / 2) * MathUtils::Cos(_angles.y / 2) * MathUtils::Sin(_angles.x / 2) - MathUtils::Sin(_angles.z / 2) * MathUtils::Sin(_angles.y / 2) * MathUtils::Cos(_angles.x / 2);
    quat.w = MathUtils::Cos(_angles.z / 2) * MathUtils::Cos(_angles.y / 2) * MathUtils::Cos(_angles.x / 2) + MathUtils::Sin(_angles.z / 2) * MathUtils::Sin(_angles.y / 2) * MathUtils::Sin(_angles.x / 2);
    return quat;
}

Quaternion Quaternion::FromAxisAngle(Vector3<float> const& _axis, float _angle)
{
    assert(_axis.Length() == 1.0f && "axis should be normalized");
    
    _angle = MathUtils::WrapAngle(_angle);
    
    Quaternion quat;
    quat.x = _axis.x * MathUtils::Sin(_angle / 2);
    quat.y = _axis.y * MathUtils::Sin(_angle / 2);
    quat.z = _axis.z * MathUtils::Sin(_angle / 2);
    quat.w = MathUtils::Cos(_angle / 2);
    return quat;
}

Quaternion Quaternion::FromRotationMatrix(Matrix3<float> const& _rotMat)
{
    Matrix4<float> m = 
        {
            _rotMat.m00, _rotMat.m01, _rotMat.m02, 0.0f,
            _rotMat.m10, _rotMat.m11, _rotMat.m12, 0.0f,
            _rotMat.m20, _rotMat.m21, _rotMat.m22, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    return FromRotationMatrix(m);
}

Quaternion Quaternion::FromRotationMatrix(Matrix4<float> const& _rotMat)
{
    float const trace = _rotMat.m00 + _rotMat.m11 + _rotMat.m22;

    float x, y, z, w;

    if (trace > 0.0f)
    {
        float s = MathUtils::Sqrt(trace + 1.0f) * 2.0f; // s = 4w
        float d = 1.0f / s;
        w = 0.25f * s;
        x = (_rotMat.m12 - _rotMat.m21) * d;
        y = (_rotMat.m20 - _rotMat.m02) * d;
        z = (_rotMat.m01 - _rotMat.m10) * d;
    }
    else if (_rotMat.m00 >= _rotMat.m11 && _rotMat.m00 >= _rotMat.m22)
    {
        float s = MathUtils::Sqrt(1.0f + _rotMat.m00 - _rotMat.m11 - _rotMat.m22) * 2.0f; // s = 4x
        float d = 1.0f / s;
        x = 0.25f * s;
        y = (_rotMat.m01 + _rotMat.m10) * d;
        z = (_rotMat.m20 + _rotMat.m02) * d;
        w = (_rotMat.m12 - _rotMat.m21) * d;
    }
    else if (_rotMat.m11 >= _rotMat.m22)
    {
        float s = MathUtils::Sqrt(1.0f + _rotMat.m11 - _rotMat.m00 - _rotMat.m22) * 2.0f; // s = 4y
        float d = 1.0f / s;
        y = 0.25f * s;
        x = (_rotMat.m01 + _rotMat.m10) * d;
        z = (_rotMat.m12 + _rotMat.m21) * d;
        w = (_rotMat.m20 - _rotMat.m02) * d;
    }
    else
    {
        float s = MathUtils::Sqrt(1.0f + _rotMat.m22 - _rotMat.m00 - _rotMat.m11) * 2.0f; // s = 4z
        float d = 1.0f / s;
        z = 0.25f * s;
        x = (_rotMat.m20 + _rotMat.m02) * d;
        y = (_rotMat.m12 + _rotMat.m21) * d;
        w = (_rotMat.m01 - _rotMat.m10) * d;
    }

    return Quaternion(x, y, z, w);
}

Quaternion Quaternion::MakeXYZ(float _x, float _y, float _z)
{
    float sx = MathUtils::Sin(_x * 0.5f), cx = MathUtils::Cos(_x * 0.5f);
    float sy = MathUtils::Sin(_y * 0.5f), cy = MathUtils::Cos(_y * 0.5f);
    float sz = MathUtils::Sin(_z * 0.5f), cz = MathUtils::Cos(_z * 0.5f);

    return Quaternion(
        cz*cy*sx - sz*cx*sy,   // x
        cz*cx*sy + sz*cy*sx,   // y
        sz*cy*cx - cz*sy*sx,   // z
        cz*cy*cx + sz*sy*sx    // w
    );
}

Quaternion Quaternion::MakeZYX(float _x, float _y, float _z)
{
    float sz = MathUtils::Sin(_z * 0.5f), cz = MathUtils::Cos(_z * 0.5f);
    float sy = MathUtils::Sin(_y * 0.5f), cy = MathUtils::Cos(_y * 0.5f);
    float sx = MathUtils::Sin(_x * 0.5f), cx = MathUtils::Cos(_x * 0.5f);

    return Quaternion(
        cx*sy*sz + sx*cy*cz,   // x
        cx*sy*cz - sx*cy*sz,   // y
        cx*cy*sz + sx*sy*cz,   // z
        cx*cy*cz - sx*sy*sz    // w
    );
}

Quaternion Quaternion::MakeYPR(float _yawY, float _pitchX, float _rollZ)
{
    float halfY = _yawY   * 0.5f;
    float halfP = _pitchX * 0.5f;
    float halfR = _rollZ  * 0.5f;

    float sy = MathUtils::Sin(halfY), cy = MathUtils::Cos(halfY);
    float sx = MathUtils::Sin(halfP), cx = MathUtils::Cos(halfP);
    float sz = MathUtils::Sin(halfR), cz = MathUtils::Cos(halfR);

    return Quaternion(
        cz*sx*cy - sz*cx*sy,   // x
        cz*cx*sy + sz*sx*cy,   // y
        cz*sx*sy + sz*cx*cy,   // z
        cz*cx*cy - sz*sx*sy    // w
    );
}

Quaternion Quaternion::MakeRPY(float _rollZ, float _pitchX, float _yawY)
{
    float halfY = _yawY   * 0.5f;
    float halfP = _pitchX * 0.5f;
    float halfR = _rollZ  * 0.5f;

    float sy = MathUtils::Sin(halfY), cy = MathUtils::Cos(halfY);
    float sx = MathUtils::Sin(halfP), cx = MathUtils::Cos(halfP);
    float sz = MathUtils::Sin(halfR), cz = MathUtils::Cos(halfR);

    return Quaternion(
        cy*sx*cz + sy*cx*sz,   // x
        sy*cx*cz - cy*sx*sz,   // y
        cy*cx*sz - sy*sx*cz,   // z
        cy*cx*cz - sy*sx*sz    // w
    );
}

Quaternion Quaternion::operator+(Quaternion const& _q) const
{
    return Quaternion(x + _q.x, y + _q.y, z + _q.z, w + _q.w);
}

Quaternion Quaternion::operator-(Quaternion const& _q) const
{
    return Quaternion(x - _q.x, y - _q.y, z - _q.z, w - _q.w);
}

Quaternion Quaternion::operator*(Quaternion const& _q) const
{
    float rW = w*_q.w - x*_q.x - y*_q.y - z*_q.z;
    float rX = w*_q.x + x*_q.w + y*_q.z - z*_q.y;
    float rY = w*_q.y + y*_q.w + z*_q.x - x*_q.z;
    float rZ = w*_q.z + z*_q.w + x*_q.y - y*_q.x;
    return Quaternion(rX, rY, rZ, rW);
}

Quaternion& Quaternion::operator+=(Quaternion const& _q)
{
    x = x + _q.x;
    y = y + _q.y;
    z = z + _q.z;
    w = w + _q.w;
    return *this;
}

Quaternion& Quaternion::operator-=(Quaternion const& _q)
{
    x = x - _q.x;
    y = y - _q.y;
    z = z - _q.z;
    w = w - _q.w;
    return *this;
}

Quaternion& Quaternion::operator*=(Quaternion const& _q)
{
    float rW = w*_q.w - x*_q.x - y*_q.y - z*_q.z;
    float rX = w*_q.x + x*_q.w + y*_q.z - z*_q.y;
    float rY = w*_q.y + y*_q.w + z*_q.x - x*_q.z;
    float rZ = w*_q.z + z*_q.w + x*_q.y - y*_q.x;
    
    x = rX;
    y = rY;
    z = rZ;
    w = rW;
    return *this;
}

float Quaternion::Dot(Quaternion const& _q) const
{
    return x * _q.x + y * _q.y + z * _q.z + w * _q.w;
}

float Quaternion::Length() const
{
    return MathUtils::Sqrt(x*x + y*y + z*z + w*w);
}

float Quaternion::LengthSquared() const
{
    return x*x + y*y + z*z + w*w;
}

Quaternion Quaternion::Conjugate() const
{
    return Quaternion(-x, -y, -z, w);
}

Quaternion Quaternion::Inverted() const
{
    float d = x * x + y * y + z * z + w * w;
    
    float rX = -x / d;
    float rY = -y / d;
    float rZ = -z / d;
    float rW =  w / d;
    
    return Quaternion(rX, rY, rZ, rW);
}

Quaternion& Quaternion::SelfInvert()
{
    float d = x * x + y * y + z * z + w * w;
    
    x = -x / d;
    y = -y / d;
    z = -z / d;
    w =  w / d;
    return *this;
}

Quaternion Quaternion::Normalized() const
{
    float length = Length();
    assert(length > 0.0f && "Can't normalized null quaternion");
    
    return Quaternion(x / length, y / length, z / length, w / length);
}

Quaternion& Quaternion::SelfNormalize()
{
    float length = Length();
    assert(length > 0.0f && "Can't normalized null quaternion");
    
    x = x / length;
    y = y / length;
    z = z / length;
    w = w / length;
    return *this;
}

Quaternion Quaternion::SafeNormalized() const
{
    float length = Length();
    if (length == 0.0f || length == 1.0f) return *this;
    
    return Quaternion(x / length, y / length, z / length, w / length);
}

Quaternion& Quaternion::SafeSelfNormalize()
{
    float length = Length();
    if (length == 0.0f || length == 1.0f) return *this;
    
    x = x / length;
    y = y / length;
    z = z / length;
    w = w / length;
    return *this;
}

Matrix3<float> Quaternion::ToMatrix3()
{
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    
    return {
        { 1.0f - 2*y2 - 2*z2, 2*x*y + 2*z*w, 2*x*z - 2*y*w },
          { 2*x*y - 2*z*w, 1.0f - 2*x2 - 2*z2, 2*y*z + 2*x*w },
          { 2*x*z + 2*y*w, 2*y*z - 2*x*w, 1.0f - 2*x2 - 2*y2 }
    };
}

Matrix4<float> Quaternion::ToMatrix4()
{
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    
    return {
            { 1.0f - 2*y2 - 2*z2, 2*x*y + 2*z*w, 2*x*z - 2*y*w, 0.0f },
              { 2*x*y - 2*z*w, 1.0f - 2*x2 - 2*z2, 2*y*z + 2*x*w, 0.0f },
              { 2*x*z + 2*y*w, 2*y*z - 2*x*w, 1.0f - 2*x2 - 2*y2, 0.0f },
              { 0.0f, 0.0f, 0.0f, 1.0f}
    };
}

Vector4<float> Quaternion::ToAxisAngle()
{
    float angle = 2 * MathUtils::Acos(w);
    
    Vector4<float> result;
    result.w = angle;
    
    float d = MathUtils::Sqrt(1.0f - w * w);
    if (d == 0.0f)
        return Vector4<float>(1.0f, 0.0f, 0.0f, angle);
    
    result.x = x / d;
    result.y = y / d;
    result.z = z / d;
    
    return result;
}

Vector3<float> Quaternion::ToEulerAngles()
{
    Vector3<float> result;
    
    float sinY = 2.0f * (x*z - w*y);
    sinY = MathUtils::Clamp(sinY, -1.0f, 1.0f);
    
    if (MathUtils::Abs(sinY) > 1.0f - MathUtils::LARGE_EPSILON)
    {
        result.y = (sinY > 0.0f) ? MathUtils::HALF_PI : -MathUtils::HALF_PI;
        result.x = 2.0f * MathUtils::Atan2(y, w) * MathUtils::Sign(sinY);
        result.z = 0.0f;
        return result;
    }
    
    result.y = MathUtils::Asin(sinY);
    result.x = MathUtils::Atan2(-2.0f * (y*z + w*x), 1.0f - 2.0f * (x*x + y*y));
    result.z = MathUtils::Atan2(-2.0f * (x*y - w*z), 1.0f - 2.0f * (y*y + z*z));
    return result;
}

float Quaternion::Dot(Quaternion const& _q1, Quaternion const& _q2)
{
    return _q1.Dot(_q2);   
}

float Quaternion::Length(Quaternion const& _q)
{
    return _q.Length();
}

float Quaternion::LengthSquared(Quaternion const& _q)
{
    return _q.LengthSquared();
}

Quaternion Quaternion::Conjugate(Quaternion const& _q)
{
    return Quaternion(-_q.x, -_q.y, -_q.z, _q.w);
}

Quaternion Quaternion::Invert(Quaternion const& _q)
{
    return _q.Inverted();
}

Quaternion Quaternion::Normalize(Quaternion const& _q)
{
    return _q.Normalized();
}

Quaternion Quaternion::SafeNormalize(Quaternion const& _q)
{
    return _q.SafeNormalized();
}

bool Quaternion::operator==(Quaternion const& _q) const
{
    return x == _q.x && y == _q.y && z == _q.z && w == _q.w;
}

bool Quaternion::operator!=(Quaternion const& _q) const
{
    return x != _q.x || y != _q.y || z != _q.z || w != _q.w;
}

std::ostream& operator<<(std::ostream& _os, Quaternion const& _q)
{
    return _os << "w : " << _q.w << " | xyz : (" << _q.x << ", " << _q.y << ", " << _q.z << ")";
}






