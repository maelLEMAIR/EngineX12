#ifndef VECTOR4_INL_DEFINED
#define VECTOR4_INL_DEFINED

#include "Vector4.h"
#include "../Matrix/Matrix4.h"

template <typename T>
Vector4<T>::Vector4()
{
    x = T();
    y = T();
    z = T();
    w = T();
}

template <typename T>
Vector4<T>::Vector4(T _scalar)
{
    x = _scalar;
    y = _scalar;
    z = _scalar;
    w = _scalar;
}

template <typename T>
Vector4<T>::Vector4(T _x, T _y, T _z, T _w)
{
    x = _x;
    y = _y;
    z = _z;
    w = _w;
}

template <typename T>
Vector4<T>::Vector4(Vector2<T> const& _xy, Vector2<T> const& _zw)
{
    x = _xy.x;
    y = _xy.y;
    z = _zw.x;
    w = _zw.y;
}

template <typename T>
Vector4<T>::Vector4(Vector3<T> const& _xyz, T _w)
{
    x = _xyz.x;
    y = _xyz.y;
    z = _xyz.z;
    w = _w;
}

template <typename T>
Vector4<T>::Vector4(T _x, Vector3<T> const& _yzw)
{
    x = _x;
    y = _yzw.x;
    z = _yzw.y;
    w = _yzw.z;
}

template <typename T>
Vector4<T>::Vector4(Vector2<T> const& _xy, T _z, T _w)
{
    x = _xy.x;
    y = _xy.y;
    z = _z;
    w = _w;
}

template <typename T>
Vector4<T>::Vector4(T _x, Vector2<T> const& _yz, T _w)
{
    x = _x;
    y = _yz.x;
    z = _yz.y;
    w = _w;
}

template <typename T>
Vector4<T>::Vector4(T _x, T _y, Vector2<T> const& _zw)
{
    x = _x;
    y = _y;
    z = _zw.x;
    w = _zw.y;
}

template <typename T>
Vector4<T>::Vector4(std::initializer_list<T> _l)
{
    assert(_l.size() == 4 && "Incorrect list size");

    x = _l.begin()[0];
    y = _l.begin()[1];
    z = _l.begin()[2];
    w = _l.begin()[3];  
}

template <typename T>
Vector4<T> Vector4<T>::operator+(Vector4 const& _o) const
{
    return Vector4(x + _o.x, y + _o.y, z + _o.z, w + _o.w);
}

template <typename T>
Vector4<T> Vector4<T>::operator-(Vector4 const& _o) const
{
    return Vector4(x - _o.x, y - _o.y, z - _o.z, w - _o.w);
}

template <typename T>
Vector4<T> Vector4<T>::operator-() const
{
    return Vector4<T>(-x, -y, -z, -w);
}

template <typename T>
Vector4<T> Vector4<T>::operator*(Vector4 const& _o) const
{
    return Vector4(x * _o.x, y * _o.y, z * _o.z, w * _o.w);
}

template <typename T>
Vector4<T> Vector4<T>::operator/(Vector4 const& _o) const
{
    assert(_o.x != 0.0f && _o.y != 0.0f && _o.z != 0.0f && _o.w != 0.0f && "Division by zero");
    return Vector4(x / _o.x, y / _o.y, z / _o.z, w / _o.w);
}

template <typename T>
Vector4<T> Vector4<T>::operator*(float _scalar) const
{
    return Vector4(x * _scalar, y * _scalar, z * _scalar, w * _scalar);
}

template <typename T>
Vector4<T> Vector4<T>::operator/(float _scalar) const
{
    assert(_scalar != 0.0f && "Division by zero");
    return Vector4(x / _scalar, y / _scalar, z / _scalar, w / _scalar);
}

template <typename T>
Vector4<T> Vector4<T>::operator*(Matrix4<T> const& _m) const
{
    Vector4<T> result;
    result.x = x * _m[0][0] + y * _m[1][0] + z * _m[2][0] + w * _m[3][0];
    result.y = x * _m[0][1] + y * _m[1][1] + z * _m[2][1] + w * _m[3][1];
    result.z = x * _m[0][2] + y * _m[1][2] + z * _m[2][2] + w * _m[3][2];
    result.w = x * _m[0][3] + y * _m[1][3] + z * _m[2][3] + w * _m[3][3];
    return result;
}

template <typename T>
Vector4<T>& Vector4<T>::operator+=(Vector4 const& _o)
{
    x += _o.x;
    y += _o.y;
    z += _o.z;
    w += _o.w;
    return *this;
}

template <typename T>
Vector4<T>& Vector4<T>::operator-=(Vector4 const& _o)
{
    x -= _o.x;
    y -= _o.y;
    z -= _o.z;
    w -= _o.w;
    return *this;
}

template <typename T>
Vector4<T>& Vector4<T>::operator*=(Vector4 const& _o)
{
    x *= _o.x;
    y *= _o.y;
    z *= _o.z;
    w *= _o.w;
    return *this;
}

template <typename T>
Vector4<T>& Vector4<T>::operator/=(Vector4 const& _o)
{
    assert(_o.x != 0.0f && _o.y != 0.0f && _o.z != 0.0f && _o.w != 0.0f && "Division by zero");
    x /= _o.x;
    y /= _o.y;
    z /= _o.z;
    w /= _o.w;
    return *this;
}

template <typename T>
Vector4<T>& Vector4<T>::operator*=(float _scalar)
{
    x *= _scalar;
    y *= _scalar;
    z *= _scalar;
    w *= _scalar;
    return *this;
}

template <typename T>
Vector4<T>& Vector4<T>::operator/=(float _scalar)
{
    assert(_scalar != 0.0f && "Division by zero");
    x /= _scalar;
    y /= _scalar;
    z /= _scalar;
    w /= _scalar;
    return *this;
}

template <typename T>
Vector4<T>& Vector4<T>::operator*=(Matrix4<T> const& _m)
{
    Vector4<T> result;
    result.x = x * _m[0][0] + y * _m[1][0] + z * _m[2][0] + w * _m[3][0];
    result.y = x * _m[0][1] + y * _m[1][1] + z * _m[2][1] + w * _m[3][1];
    result.z = x * _m[0][2] + y * _m[1][2] + z * _m[2][2] + w * _m[3][2];
    result.w = x * _m[0][3] + y * _m[1][3] + z * _m[2][3] + w * _m[3][3];
    
    x = result.x;
    y = result.y;
    z = result.z;
    w = result.w;
    return *this;
}

template <typename T>
bool Vector4<T>::operator==(Vector4 const& _o) const
{
    return x == _o.x && y == _o.y && z == _o.z && w == _o.w;
}

template <typename T>
bool Vector4<T>::operator!=(Vector4 const& _o) const
{
    return x != _o.x || y != _o.y || z != _o.z || w != _o.w;
}

template <typename T>
bool Vector4<T>::IsNull() const
{
    return x == 0.0f && y == 0.0f && z == 0.0f && w == 0.0f;
}

template <typename T>
float Vector4<T>::Dot(Vector4 const& _o) const
{
    return x * _o.x + y * _o.y + z * _o.z + w * _o.w;
}

template <typename T>
float Vector4<T>::Length() const
{
    return MathUtils::Sqrt(x * x + y * y + z * z + w * w);
}

template <typename T>
float Vector4<T>::LengthSquared() const
{
    return x * x + y * y + z * z + w * w;
}

template <typename T>
Vector4<T> Vector4<T>::Normalized() const
{
    float length = Length();
    if (length == 0.0f || length == 1.0f)
        return *this;
    return Vector4<T>(x / length, y / length, z / length, w / length);
}

template <typename T>
Vector4<T>& Vector4<T>::SelfNormalize()
{
    float length = Length();
    if (length == 0.0f || length == 1.0f)
        return *this;
    
    x /= length;
    y /= length;
    z /= length;
    w /= length;
    return *this;
}

template <typename T>
Vector4<T> Vector4<T>::Abs() const
{
    return Vector4<T>(
        MathUtils::Abs(x),
        MathUtils::Abs(y),
        MathUtils::Abs(z),
        MathUtils::Abs(w)
        );
}

template <typename T>
Vector4<T> Vector4<T>::Clamp(Vector4 const& _min, Vector4 const& _max) const
{
    return Vector4<T>(
        MathUtils::Clamp(x, _min.x, _max.x),
        MathUtils::Clamp(y, _min.y, _max.y),
        MathUtils::Clamp(z, _min.z, _max.z),
        MathUtils::Clamp(w, _min.w, _max.w)
        );
}

template <typename T>
Vector4<T> Vector4<T>::Zero()
{
    return Vector4<T>(0, 0, 0, 0);
}

template <typename T>
Vector4<T> Vector4<T>::One()
{
    return Vector4<T>(1, 1, 1, 1);
}

template <typename T>
float Vector4<T>::Dot(Vector4 const& _v1, Vector4 const& _v2)
{
    return _v1.x * _v2.x + _v1.y * _v2.y + _v1.z * _v2.z + _v1.w * _v2.w;
}

template <typename T>
Vector4<T> Vector4<T>::Normalize(Vector4 const& _o)
{
    return _o.Normalized();
}

template <typename T>
Vector4<T> Vector4<T>::Min(Vector4 const& _v1, Vector4 const& _v2)
{
    return Vector4<T>(MathUtils::Min(_v1.x, _v2.x), MathUtils::Min(_v1.y, _v2.y), MathUtils::Min(_v1.z, _v2.z), MathUtils::Min(_v1.w, _v2.w));
}

template <typename T>
Vector4<T> Vector4<T>::Max(Vector4 const& _v1, Vector4 const& _v2)
{
    return Vector4<T>(MathUtils::Max(_v1.x, _v2.x), MathUtils::Max(_v1.y, _v2.y), MathUtils::Max(_v1.z, _v2.z), MathUtils::Max(_v1.w, _v2.w));
}

template <typename T>
Vector4<T> Vector4<T>::Abs(Vector4 const& _v)
{
    return Vector4<T>(MathUtils::Abs(_v.x), MathUtils::Abs(_v.y), MathUtils::Abs(_v.z), MathUtils::Abs(_v.w));
}

template <typename T>
Vector4<T> Vector4<T>::Clamp(Vector4 const& _v, Vector4 const& _min, Vector4 const& _max)
{
    return Vector4<T>(MathUtils::Clamp(_v.x, _min.x, _max.x), MathUtils::Clamp(_v.y, _min.y, _max.y), MathUtils::Clamp(_v.z, _min.z, _max.z), MathUtils::Clamp(_v.w, _min.w, _max.w));
}

template <typename T>
bool Vector4<T>::NearlyEqual(Vector4 const& _v1, Vector4 const& _v2)
{
    return  MathUtils::NearlyEqual(_v1.x, _v2.x, MathUtils::EPSILON) &&
            MathUtils::NearlyEqual(_v1.y, _v2.y, MathUtils::EPSILON) &&
            MathUtils::NearlyEqual(_v1.z, _v2.z, MathUtils::EPSILON) &&
            MathUtils::NearlyEqual(_v1.w, _v2.w, MathUtils::EPSILON);
}

template <typename T>
bool Vector4<T>::NearlyEqual(Vector4 const& _v1, Vector4 const& _v2, float _margin)
{
    return  MathUtils::NearlyEqual(_v1.x, _v2.x, _margin) &&
            MathUtils::NearlyEqual(_v1.y, _v2.y, _margin) &&
            MathUtils::NearlyEqual(_v1.z, _v2.z, _margin) &&
            MathUtils::NearlyEqual(_v1.w, _v2.w, _margin);
}

template <typename T>
T Vector4<T>::operator[](int _i) const
{
    return (&x)[_i];
}

template <typename T>
T& Vector4<T>::operator[](int _i)
{
    return (&x)[_i];
}

template <typename T>
T* Vector4<T>::Data()
{
    return &x;
}

template <typename T>
T const* Vector4<T>::Data() const
{
    return &x;
}

template <typename T>
std::ostream& operator<<(std::ostream& _os, Vector4<T> const& _v)
{
    return _os << "(" << _v.x << ", " << _v.y << ", " << _v.z << ", " << _v.w << ")";
}

template <typename T>
Vector4<T> operator*(T _scalar, Vector4<T> const& _o)
{
    return _o * _scalar;
}


#pragma region 2ComponentGetters

template <typename T>
Vector2<T> Vector4<T>::xy() const
{
    return Vector2<T>(x, y);
}

template <typename T>
Vector2<T> Vector4<T>::yx() const
{
    return Vector2<T>(y, x);
}

template <typename T>
Vector2<T> Vector4<T>::xz() const
{
    return Vector2<T>(x, z);
}

template <typename T>
Vector2<T> Vector4<T>::zx() const
{
    return Vector2<T>(z, x);
}

template <typename T>
Vector2<T> Vector4<T>::xw() const
{
    return Vector2<T>(x, w);
}

template <typename T>
Vector2<T> Vector4<T>::wx() const
{
    return Vector2<T>(w, x);
}

template <typename T>
Vector2<T> Vector4<T>::yz() const
{
    return Vector2<T>(y, z);
}

template <typename T>
Vector2<T> Vector4<T>::zy() const
{
    return Vector2<T>(z, y);
}

template <typename T>
Vector2<T> Vector4<T>::yw() const
{
    return Vector2<T>(y, w);
}

template <typename T>
Vector2<T> Vector4<T>::wy() const
{
    return Vector2<T>(w, y);
}

template <typename T>
Vector2<T> Vector4<T>::zw() const
{
    return Vector2<T>(z, w);
}

template <typename T>
Vector2<T> Vector4<T>::wz() const
{
    return Vector2<T>(w, z);
}

#pragma endregion

#pragma region 3ComponentGetters

template <typename T>
Vector3<T> Vector4<T>::xyz() const { return Vector3<T>(x, y, z); }

template <typename T>
Vector3<T> Vector4<T>::xyw() const { return Vector3<T>(x, y, w); }

template <typename T>
Vector3<T> Vector4<T>::xzy() const { return Vector3<T>(x, z, y); }

template <typename T>
Vector3<T> Vector4<T>::xzw() const { return Vector3<T>(x, z, w); }

template <typename T>
Vector3<T> Vector4<T>::xwy() const { return Vector3<T>(x, w, y); }

template <typename T>
Vector3<T> Vector4<T>::xwz() const { return Vector3<T>(x, w, z); }

template <typename T>
Vector3<T> Vector4<T>::yxz() const { return Vector3<T>(y, x, z); }

template <typename T>
Vector3<T> Vector4<T>::yxw() const { return Vector3<T>(y, x, w); }

template <typename T>
Vector3<T> Vector4<T>::yzx() const { return Vector3<T>(y, z, x); }

template <typename T>
Vector3<T> Vector4<T>::yzw() const { return Vector3<T>(y, z, w); }

template <typename T>
Vector3<T> Vector4<T>::ywx() const { return Vector3<T>(y, w, x); }

template <typename T>
Vector3<T> Vector4<T>::ywz() const { return Vector3<T>(y, w, z); }

template <typename T>
Vector3<T> Vector4<T>::zxy() const { return Vector3<T>(z, x, y); }

template <typename T>
Vector3<T> Vector4<T>::zxw() const { return Vector3<T>(z, x, w); }

template <typename T>
Vector3<T> Vector4<T>::zyx() const { return Vector3<T>(z, y, x); }

template <typename T>
Vector3<T> Vector4<T>::zyw() const { return Vector3<T>(z, y, w); }

template <typename T>
Vector3<T> Vector4<T>::zwx() const { return Vector3<T>(z, w, x); }

template <typename T>
Vector3<T> Vector4<T>::zwy() const { return Vector3<T>(z, w, y); }

template <typename T>
Vector3<T> Vector4<T>::wxy() const { return Vector3<T>(w, x, y); }

template <typename T>
Vector3<T> Vector4<T>::wxz() const { return Vector3<T>(w, x, z); }

template <typename T>
Vector3<T> Vector4<T>::wyx() const { return Vector3<T>(w, y, x); }

template <typename T>
Vector3<T> Vector4<T>::wyz() const { return Vector3<T>(w, y, z); }

template <typename T>
Vector3<T> Vector4<T>::wzx() const { return Vector3<T>(w, z, x); }

template <typename T>
Vector3<T> Vector4<T>::wzy() const { return Vector3<T>(w, z, y); }

#pragma endregion

#pragma region 4ComponentGetters
template <typename T>
Vector4<T> Vector4<T>::xywz() const { return Vector4<T>(x, y, w, z); }

template <typename T>
Vector4<T> Vector4<T>::xzyw() const { return Vector4<T>(x, z, y, w); }

template <typename T>
Vector4<T> Vector4<T>::xzwy() const { return Vector4<T>(x, z, w, y); }

template <typename T>
Vector4<T> Vector4<T>::xwyz() const { return Vector4<T>(x, w, y, z); }

template <typename T>
Vector4<T> Vector4<T>::xwzy() const { return Vector4<T>(x, w, z, y); }

template <typename T>
Vector4<T> Vector4<T>::yxzw() const { return Vector4<T>(y, x, z, w); }

template <typename T>
Vector4<T> Vector4<T>::yxwz() const { return Vector4<T>(y, x, w, z); }

template <typename T>
Vector4<T> Vector4<T>::yzxw() const { return Vector4<T>(y, z, x, w); }

template <typename T>
Vector4<T> Vector4<T>::yzwx() const { return Vector4<T>(y, z, w, x); }

template <typename T>
Vector4<T> Vector4<T>::ywxz() const { return Vector4<T>(y, w, x, z); }

template <typename T>
Vector4<T> Vector4<T>::ywzx() const { return Vector4<T>(y, w, z, x); }

template <typename T>
Vector4<T> Vector4<T>::zxyw() const { return Vector4<T>(z, x, y, w); }

template <typename T>
Vector4<T> Vector4<T>::zxwy() const { return Vector4<T>(z, x, w, y); }

template <typename T>
Vector4<T> Vector4<T>::zyxw() const { return Vector4<T>(z, y, x, w); }

template <typename T>
Vector4<T> Vector4<T>::zywx() const { return Vector4<T>(z, y, w, x); }

template <typename T>
Vector4<T> Vector4<T>::zwxy() const { return Vector4<T>(z, w, x, y); }

template <typename T>
Vector4<T> Vector4<T>::zwyx() const { return Vector4<T>(z, w, y, x); }

template <typename T>
Vector4<T> Vector4<T>::wxyz() const { return Vector4<T>(w, x, y, z); }

template <typename T>
Vector4<T> Vector4<T>::wxzy() const { return Vector4<T>(w, x, z, y); }

template <typename T>
Vector4<T> Vector4<T>::wyxz() const { return Vector4<T>(w, y, x, z); }

template <typename T>
Vector4<T> Vector4<T>::wyzx() const { return Vector4<T>(w, y, z, x); }

template <typename T>
Vector4<T> Vector4<T>::wzxy() const { return Vector4<T>(w, z, x, y); }

template <typename T>
Vector4<T> Vector4<T>::wzyx() const { return Vector4<T>(w, z, y, x); }

#pragma endregion

#endif
