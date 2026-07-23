#ifndef VECTOR3_INL_DEFINED
#define VECTOR3_INL_DEFINED

#include "Vector3_Fwd.h"
#include "../Matrix/Matrix3.h"
#include "../Quaternions/Quaternion.h"

template <typename T>
Vector3<T>::Vector3()
{
    x = T();
    y = T();
    z = T();
}

template <typename T>
Vector3<T>::Vector3(T _x, T _y, T _z)
{
    x = _x;
    y = _y;
    z = _z;
}

template <typename T>
Vector3<T>::Vector3(T _scalar)
{
    x = _scalar;
    y = _scalar;
    z = _scalar;
}

template <typename T>
Vector3<T>::Vector3(Vector2<T> const& _xy, T _z)
{
    x = _xy.x;
    y = _xy.y;
    z = _z;
}

template <typename T>
Vector3<T>::Vector3(T _x, Vector2<T> const& _yz)
{
    x = _x;
    y = _yz.x;
    z = _yz.y;
}

template <typename T>
Vector3<T>::Vector3(std::initializer_list<T> _l)
{
    assert(_l.size() == 3 && "Incorrect list size" );

    x = _l.begin()[0];
    y = _l.begin()[1];
    z = _l.begin()[2];
}

template <typename T>
Vector3<T> Vector3<T>::operator+(Vector3 const& _o) const
{
    return Vector3<T>(x + _o.x, y + _o.y, z + _o.z);
}

template <typename T>
Vector3<T> Vector3<T>::operator-(Vector3 const& _o) const
{
    return Vector3<T>(x - _o.x, y - _o.y, z - _o.z);
}

template <typename T>
Vector3<T> Vector3<T>::operator-() const
{
    return Vector3<T>(-x, -y, -z);
}

template <typename T>
Vector3<T> Vector3<T>::operator*(Vector3 const& _o) const
{
    return Vector3<T>(x * _o.x, y * _o.y, z * _o.z);
}

template <typename T>
Vector3<T> Vector3<T>::operator/(Vector3 const& _o) const
{
    return Vector3<T>(x / _o.x, y / _o.y, z / _o.z);
}

template <typename T>
Vector3<T> Vector3<T>::operator^(Vector3 const& _o) const
{
    return Vector3<T>(y * _o.z - z * _o.y, z * _o.x - x * _o.z, x * _o.y - y * _o.x);
}

template <typename T>
Vector3<T> Vector3<T>::operator*(float _scalar) const
{
    return Vector3<T>(x * _scalar, y * _scalar, z * _scalar);
}

template <typename T>
Vector3<T> Vector3<T>::operator/(float _scalar) const
{
    assert(_scalar != 0 && "Can't divide by zero");
    return Vector3<T>(x / _scalar, y / _scalar, z / _scalar);
}

template <typename T>
Vector3<T> Vector3<T>::operator*(Matrix3<T> const& _m) const
{
    Vector3<T> result;
    result.x = x * _m[0][0] + y * _m[1][0] + z * _m[2][0];
    result.y = x * _m[0][1] + y * _m[1][1] + z * _m[2][1];
    result.z = x * _m[0][2] + y * _m[1][2] + z * _m[2][2];
    return result;
}

template <typename T>
Vector3<T> Vector3<T>::operator*(Quaternion const& _q) const
{
    Vector3 u(_q.x, _q.y, _q.z);
    
    float s = _q.w;
    
    Vector3 vprime = 2.0f * Dot(u) * u
                    + (s*s - u.Dot(u)) * (*this);
                    + 2.0f * s * cross(u, *this);
    
    return vprime;
}

template <typename T>
Vector3<T>& Vector3<T>::operator+=(Vector3 const& _o)
{
    x = x + _o.x;
    y = y + _o.y;
    z = z + _o.z;
    
    return *this;
}

template <typename T>
Vector3<T>& Vector3<T>::operator-=(Vector3 const& _o)
{
    x = x - _o.x;
    y = y - _o.y;
    z = z - _o.z;
    return *this;
}

template <typename T>
Vector3<T>& Vector3<T>::operator*=(Vector3 const& _o)
{
    x = x * _o.x;
    y = y * _o.y;
    z = z * _o.z;
    return *this;
}

template <typename T>
Vector3<T>& Vector3<T>::operator/=(Vector3 const& _o)
{
    x = x / _o.x;
    y = y / _o.y;
    z = z / _o.z;
    return *this;
}

template <typename T>
Vector3<T>& Vector3<T>::operator^=(Vector3 const& _o)
{
    T newX = y * _o.z - z * _o.y;
    T newY = z * _o.x - x * _o.z;
    T newZ = x * _o.y - y * _o.x;
    x = newX;
    y = newY;
    z = newZ;
    return *this;
}

template <typename T>
Vector3<T>& Vector3<T>::operator*=(float _scalar)
{
    x = x * _scalar;
    y = y * _scalar;
    z = z * _scalar;
    return *this;
}

template <typename T>
Vector3<T>& Vector3<T>::operator/=(float _scalar)
{
    assert(_scalar != 0 && "Can't divide by zero");
    x = x / _scalar;
    y = y / _scalar;
    z = z / _scalar;
    return *this;
}

template <typename T>
Vector3<T>& Vector3<T>::operator*=(Matrix3<T> const& _m)
{
    Vector3<T> result;
    result.x = x * _m[0][0] + y * _m[1][0] + z * _m[2][0];
    result.y = x * _m[0][1] + y * _m[1][1] + z * _m[2][1];
    result.z = x * _m[0][2] + y * _m[1][2] + z * _m[2][2];
    
    x = result.x;
    y = result.y;
    z = result.z;
    return *this;
}

template <typename T>
Vector3<T>& Vector3<T>::operator*=(Quaternion const& _q)
{
    Vector3 u(_q.x, _q.y, _q.z);
    
    float s = _q.w;
    
    Vector3 vprime = 2.0f * Dot(u) * u
                    + (s*s - u.Dot(u)) * (*this);
                    + 2.0f * s * cross(u, *this);
    
    x = vprime.x;
    y = vprime.y;
    z = vprime.z;
    
    return *this;
}

template <typename T>
bool Vector3<T>::operator==(Vector3 const& _o) const
{
    return x == _o.x && y == _o.y && z == _o.z;
}

template <typename T>
bool Vector3<T>::operator!=(Vector3 const& _o) const
{
    return x != _o.x || y != _o.y || z != _o.z;
}

template <typename T>
bool Vector3<T>::IsNull() const
{
    return x == 0.0f && y == 0.0f && z == 0.0f;
}

template <typename T>
float Vector3<T>::Dot(Vector3 const& _o) const
{
    return x * _o.x + y * _o.y + z * _o.z;
}

template <typename T>
Vector3<T> Vector3<T>::Cross(Vector3 const& _o) const
{
    T newX = y * _o.z - z * _o.y;
    T newY = z * _o.x - x * _o.z;
    T newZ = x * _o.y - y * _o.x;

    return Vector3(newX, newY, newZ);
}

template <typename T>
float Vector3<T>::Length() const
{
    return MathUtils::Sqrt(x * x + y * y + z * z);
}

template <typename T>
float Vector3<T>::LengthSquared() const
{
    return x * x + y * y + z * z;
}

template <typename T>
Vector3<T> Vector3<T>::Normalized() const
{
    float length = Length();
    if (length == 0.0f || length == 1.0f)
        return *this;
    
    return Vector3(x / length, y / length, z / length); 
}

template <typename T>
Vector3<T>& Vector3<T>::SelfNormalize()
{
    float length = Length();
    if (length == 0.0f || length == 1.0f)
        return *this;
    
    x /= length;
    y /= length;
    z /= length;
    return *this;
}

template <typename T>
Vector3<T> Vector3<T>::Reflect(Vector3 const& _normal) const
{
    return *this - 2 * Dot(*this, _normal) * _normal;
}

template <typename T>
Vector3<T> Vector3<T>::Project(Vector3 const& _target) const
{
    return _target * (Dot(*this, _target) / _target.LengthSquared());
}

template <typename T>
Vector3<T> Vector3<T>::Perpendicular() const
{
    return Vector3(-y, x, T());
}

template <typename T>
Vector3<T> Vector3<T>::Abs() const
{
    return Vector3(MathUtils::Abs(x), MathUtils::Abs(y), MathUtils::Abs(z));
}

template <typename T>
Vector3<T> Vector3<T>::Clamp(Vector3 const& _min, Vector3 const& _max) const
{
    return Vector3(MathUtils::Clamp(x, _min.x, _max.x), MathUtils::Clamp(y, _min.y, _max.y), MathUtils::Clamp(z, _min.z, _max.z));
}

template <typename T>
Vector3<T> Vector3<T>::Zero()
{
    return Vector3(0, 0, 0);
}

template <typename T>
Vector3<T> Vector3<T>::One()
{
    return Vector3(1, 1, 1);
}

template <typename T>
float Vector3<T>::Dot(Vector3 const& _v1, Vector3 const& _v2)
{
    return _v1.x * _v2.x + _v1.y * _v2.y + _v1.z * _v2.z;
}

template <typename T>
Vector3<T> Vector3<T>::Cross(Vector3 const& _v1, Vector3 const& _v2)
{
    T newX = _v1.y * _v2.z - _v1.z * _v2.y;
    T newY = _v1.z * _v2.x - _v1.x * _v2.z;
    T newZ = _v1.x * _v2.y - _v1.y * _v2.x;

    return Vector3(newX, newY, newZ);
}

template <typename T>
Vector3<T> Vector3<T>::Normalize(Vector3 const& _o)
{
    float length = _o.Length();
    if (length == 0.0f || length == 1.0f)
        return _o;
    
    return Vector3(_o.x / length, _o.y / length, _o.z / length);
}

template <typename T>
Vector3<T> Vector3<T>::Min(Vector3 const& _v1, Vector3 const& _v2)
{
    return Vector3(MathUtils::Min(_v1.x, _v2.x), MathUtils::Min(_v1.y, _v2.y), MathUtils::Min(_v1.z, _v2.z));
}

template <typename T>
Vector3<T> Vector3<T>::Max(Vector3 const& _v1, Vector3 const& _v2)
{
    return Vector3(MathUtils::Max(_v1.x, _v2.x), MathUtils::Max(_v1.y, _v2.y), MathUtils::Max(_v1.z, _v2.z));
}

template <typename T>
Vector3<T> Vector3<T>::Abs(Vector3 const& _v)
{
    return Vector3(MathUtils::Abs(_v.x), MathUtils::Abs(_v.y), MathUtils::Abs(_v.z));
}

template <typename T>
Vector3<T> Vector3<T>::Clamp(Vector3 const& _v, Vector3 const& _min, Vector3 const& _max)
{
    return Vector3(MathUtils::Clamp(_v.x, _min.x, _max.x), MathUtils::Clamp(_v.y, _min.y, _max.y), MathUtils::Clamp(_v.z, _min.z, _max.z));
}

template <typename T>
bool Vector3<T>::NearlyEqual(Vector3 const& _v1, Vector3 const& _v2)
{
    return MathUtils::NearlyEqual(_v1.x, _v2.x) && MathUtils::NearlyEqual(_v1.y, _v2.y) && MathUtils::NearlyEqual(_v1.z, _v2.z);
}

template <typename T>
bool Vector3<T>::NearlyEqual(Vector3 const& _v1, Vector3 const& _v2, float _margin)
{
    return  MathUtils::NearlyEqual(_v1.x, _v2.x, _margin) && 
            MathUtils::NearlyEqual(_v1.y, _v2.y, _margin) && 
            MathUtils::NearlyEqual(_v1.z, _v2.z, _margin);
}

template <typename T>
Vector2<T> Vector3<T>::xy() const
{
    return Vector2<T>(x, y);
}

template <typename T>
Vector2<T> Vector3<T>::yx() const
{
    return Vector2<T>(y, x);
}

template <typename T>
Vector2<T> Vector3<T>::xz() const
{
    return Vector2<T>(x, z);
}

template <typename T>
Vector2<T> Vector3<T>::zx() const
{
    return Vector2<T>(z, x);
}

template <typename T>
Vector2<T> Vector3<T>::yz() const
{
    return Vector2<T>(y, z);
}

template <typename T>
Vector2<T> Vector3<T>::zy() const
{
    return Vector2<T>(z, y);
}

template <typename T>
Vector3<T> Vector3<T>::xzy() const
{
    return Vector3<T>(x, z, y);
}

template <typename T>
Vector3<T> Vector3<T>::yxz() const
{
    return Vector3<T>(y, x, z);
}

template <typename T>
Vector3<T> Vector3<T>::yzx() const
{
    return Vector3<T>(y, z, x);
}

template <typename T>
Vector3<T> Vector3<T>::zxy() const
{
    return Vector3<T>(z, x, y);
}

template <typename T>
Vector3<T> Vector3<T>::zyx() const
{
    return Vector3<T>(z, y, x);
}

template <typename T>
T Vector3<T>::operator[](int _i) const
{
    return (&x)[_i];
}

template <typename T>
T& Vector3<T>::operator[](int _i)
{
    return (&x)[_i];
}

template <typename T>
T* Vector3<T>::Data()
{
    return &x;
}

template <typename T>
T const* Vector3<T>::Data() const
{
    return &x;
}

template <typename T>
std::ostream& operator<<(std::ostream& _os, Vector3<T> const& _v)
{
    return _os << "(" << _v.x << ", " << _v.y << ", " << _v.z << ")";
}

template <typename T>
Vector3<T> operator*(T _scalar, Vector3<T> const& _o)
{
    return _o * _scalar;
}


#endif
