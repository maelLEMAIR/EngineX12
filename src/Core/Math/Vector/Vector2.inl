#ifndef VECTOR2_INL_DEFINED
#define VECTOR2_INL_DEFINED

#include "../MathUtils.hpp"
#include "Vector2_Fwd.h"

template <typename T>
Vector2<T>::Vector2()
{
    x = T();
    y = T();
}

template <typename T>
Vector2<T>::Vector2(T _x, T _y)
{
    x = _x;
    y = _y;
}

template <typename T>
Vector2<T>::Vector2(T _scalar)
{
    x = _scalar;
    y = _scalar;
}

template <typename T>
Vector2<T>::Vector2(std::initializer_list<T> _l)
{
    assert(_l.size() == 2 && "Incorrect list size");

    x = _l.begin()[0];
    y = _l.begin()[1];
}

template <typename T>
Vector2<T> Vector2<T>::operator+(const Vector2& _o) const
{
    return Vector2(x + _o.x, y + _o.y); 
}

template <typename T>
Vector2<T> Vector2<T>::operator-(const Vector2& _o) const
{
    return Vector2(x - _o.x, y - _o.y);
}

template <typename T>
Vector2<T> Vector2<T>::operator-() const
{
    return Vector2<T>(-x, -y);
}

template <typename T>
Vector2<T> Vector2<T>::operator*(const Vector2& _o) const
{
    return Vector2(x * _o.x, y * _o.y);
}

template <typename T>
Vector2<T> Vector2<T>::operator/(const Vector2& _o) const
{
    return Vector2(x / _o.x, y / _o.y);
}

template <typename T>
Vector2<T> Vector2<T>::operator*(float _scalar) const
{
    return Vector2(x * _scalar, y * _scalar);
}

template <typename T>
Vector2<T> Vector2<T>::operator/(float _scalar) const
{
    return Vector2(x / _scalar, y / _scalar);
}

template <typename T>
Vector2<T>& Vector2<T>::operator+=(const Vector2& _o)
{
    x += _o.x;
    y += _o.y;
    return *this;
}

template <typename T>
Vector2<T>& Vector2<T>::operator-=(const Vector2& _o)
{
    x -= _o.x;
    y -= _o.y;
    return *this;
}

template <typename T>
Vector2<T>& Vector2<T>::operator*=(const Vector2& _o)
{
    x *= _o.x;
    y *= _o.y;
    return *this;
}

template <typename T>
Vector2<T>& Vector2<T>::operator/=(const Vector2& _o)
{
    x /= _o.x;
    y /= _o.y;
    return *this;
}

template <typename T>
Vector2<T>& Vector2<T>::operator*=(float _scalar)
{
    x *= _scalar;
    y *= _scalar;
    return *this;
}

template <typename T>
Vector2<T>& Vector2<T>::operator/=(float _scalar)
{
    x /= _scalar;
    y /= _scalar;
    return *this;
}

template <typename T>
float Vector2<T>::Length() const
{
    return MathUtils::Sqrt(x * x + y * y);
}

template <typename T>
float Vector2<T>::LengthSquared() const
{
    return x * x + y * y;
}

template <typename T>
Vector2<T> Vector2<T>::Normalized() const
{
    float length = Length();
    if (length == 0.0f || length == 1.0f)
        return *this;
    return Vector2(x / length, y / length);
}

template <typename T>
Vector2<T>& Vector2<T>::SelfNormalize()
{
    float length = Length();
    if (length == 0.0f || length == 1.0f)
        return *this;
    x /= length;
    y /= length;
    return *this;
}

template <typename T>
Vector2<T> Vector2<T>::Reflect(const Vector2& _normal) const
{
    return *this - T(2) * Dot(*this, _normal) * _normal;
}

template <typename T>
Vector2<T> Vector2<T>::Project(Vector2 const& _target) const
{
    return _target * (Dot(*this, _target) / _target.LengthSquared());
}

template <typename T>
Vector2<T> Vector2<T>::Perpendicular() const
{
    return Vector2(-y, x);
}

template <typename T>
Vector2<T> Vector2<T>::Abs() const
{
    return Vector2(MathUtils::Abs(x), MathUtils::Abs(y));
}

template <typename T>
Vector2<T> Vector2<T>::Clamp(Vector2 const& _min, Vector2 const& _max) const
{
    return Vector2(MathUtils::Clamp(x, _min.x, _max.x), MathUtils::Clamp(y, _min.y, _max.y));
}

template <typename T>
Vector2<T> Vector2<T>::yx() const
{
    return Vector2(y, x);
}

template <typename T>
bool Vector2<T>::operator==(Vector2 const& _o) const
{
    return x == _o.x && y == _o.y;
}

template <typename T>
bool Vector2<T>::operator!=(Vector2 const& _o) const
{
    return x != _o.x || y != _o.y;
}

template <typename T>
bool Vector2<T>::IsNull() const
{
    return x == T(0) && y == T(0);
}

template <typename T>
float Vector2<T>::Dot(Vector2 const& _o) const
{
    return x * _o.x + y * _o.y;
}

template <typename T>
float Vector2<T>::DeltaAngle(Vector2 const& _o) const
{
    float dot_val   = MathUtils::Clamp(Dot(*this, _o) / (Length() * _o.Length()), -1.0f, 1.0f);
    float angle     = std::acos(dot_val);          
    float cross_val = x * _o.y - y * _o.x;  
    return cross_val < 0.0f ? -angle : angle;     
}

template <typename T>
T Vector2<T>::operator[](int _i) const
{
    return (&x)[_i];
}

template <typename T>
T& Vector2<T>::operator[](int _i)
{
    return (&x)[_i];
}

template <typename T>
T* Vector2<T>::Data()
{
    return &x;
}

template <typename T>
T const* Vector2<T>::Data() const
{
    return &x;
}

template <typename T>
std::ostream& operator<<(std::ostream& _os, Vector2<T> const& _v)
{
    return _os << "(" << _v.x << ", " << _v.y << ")";
}

template <typename T>
Vector2<T> operator*(T _scalar, Vector2<T> const& _o)
{
    return _o * _scalar;
}

template <typename T>
Vector2<T> Vector2<T>::Zero()
{
    return Vector2(T(0), T(0));
}

template <typename T>
Vector2<T> Vector2<T>::One()
{
    return Vector2(T(1), T(1));
}

template <typename T>
Vector2<T> Vector2<T>::Scalar(T _scalar)
{
    return Vector2(_scalar, _scalar); 
}

template <typename T>
float Vector2<T>::Dot(Vector2 const& _v1, Vector2 const& _v2)
{
    return _v1.x * _v2.x + _v1.y * _v2.y;
}

template <typename T>
float Vector2<T>::DeltaAngle(Vector2 const& _v1, Vector2 const& _v2)
{
    float dot_val   = MathUtils::Clamp(Dot(*_v1, _v2) / (_v1.Length() * _v2.Length()), -1.0f, 1.0f);
    float angle     = std::acos(dot_val);          
    float cross_val = _v1.x * _v2.y - _v1.y * _v2.x;  
    return cross_val < 0.0f ? -angle : angle;     
}

template <typename T>
Vector2<T> Vector2<T>::Normalize(Vector2 const& _o)
{
    return _o.Normalized();
}

template <typename T>
Vector2<T> Vector2<T>::Min(Vector2 const& _v1, Vector2 const& _v2)
{
    return Vector2<T>(MathUtils::Min(_v2.x, _v1.x), MathUtils::Min(_v2.y, _v1.y));
}

template <typename T>
Vector2<T> Vector2<T>::Max(Vector2 const& _v1, Vector2 const& _v2)
{
    return Vector2<T>(MathUtils::Max(_v2.x, _v1.x), MathUtils::Max(_v2.y, _v1.y));
}

template <typename T>
Vector2<T> Vector2<T>::Abs(Vector2 const& _v)
{
    return Vector2<T>(MathUtils::Abs(_v.x), MathUtils::Abs(_v.y));
}

template <typename T>
Vector2<T> Vector2<T>::Clamp(Vector2 const& _v, Vector2 const& _min, Vector2 const& _max)
{
    return Vector2<T>(MathUtils::Clamp(_v.x, _min.x, _max.x), MathUtils::Clamp(_v.y, _min.y, _max.y));
}

template <typename T>
bool Vector2<T>::NearlyEqual(Vector2 const& _v1, Vector2 const& _v2)
{
    return MathUtils::NearlyEqual(_v1.x, _v2.x) && MathUtils::NearlyEqual(_v1.y, _v2.y);
}

template <typename T>
bool Vector2<T>::NearlyEqual(Vector2 const& _v1, Vector2 const& _v2, float _margin)
{
    return MathUtils::NearlyEqual(_v1.x, _v2.x, _margin) && MathUtils::NearlyEqual(_v1.y, _v2.y, _margin);
}

#endif













