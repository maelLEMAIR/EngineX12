#ifndef VECTOR3_FWD_H_DEFINED
#define VECTOR3_FWD_H_DEFINED

#include "Vector2.h"

template <typename T>
class Matrix3;

class Quaternion;

template <typename T>
class Vector3
{
public:
    T x;
    T y;
    T z;
    
    Vector3();
    Vector3(T _x, T _y, T _z);
    Vector3(T _scalar);
    Vector3(Vector2<T> const& _xy, T _z);
    Vector3(T _x, Vector2<T> const& _yz);
    Vector3(std::initializer_list<T> _l);
    
    Vector3 operator+(Vector3 const& _o) const;
    Vector3 operator-(Vector3 const& _o) const;
    Vector3 operator-() const;
    Vector3 operator*(Vector3 const& _o) const;
    Vector3 operator/(Vector3 const& _o) const;
    Vector3 operator^(Vector3 const& _o) const;
    Vector3 operator*(float _scalar) const;
    Vector3 operator/(float _scalar) const;
    Vector3 operator*(Matrix3<T> const& _matrix) const;
    Vector3 operator*(Quaternion const& _q) const;
    
    Vector3& operator+=(Vector3 const& _o);
    Vector3& operator-=(Vector3 const& _o);
    Vector3& operator*=(Vector3 const& _o);
    Vector3& operator/=(Vector3 const& _o);
    Vector3& operator^=(Vector3 const& _o);
    Vector3& operator*=(float _scalar);
    Vector3& operator/=(float _scalar);
    Vector3& operator*=(Matrix3<T> const& _matrix);
    Vector3& operator*=(Quaternion const& _q);
    
    bool operator==(Vector3 const& _o) const;
    bool operator!=(Vector3 const& _o) const;
    
    bool IsNull() const;
    
    float   Dot(Vector3 const& _o) const;
    Vector3 Cross(Vector3 const& _o) const;
    
    float Length() const;
    float LengthSquared() const;
    
    Vector3  Normalized() const;
    Vector3& SelfNormalize();
    
    Vector3 Reflect(Vector3 const& _normal) const;
    Vector3 Project(Vector3 const& _target) const;
    Vector3 Perpendicular() const;
    
    Vector3 Abs() const;
    Vector3 Clamp(Vector3 const& _min, Vector3 const& _max) const;
    
    static Vector3 Zero();
    static Vector3 One();
    
    static float   Dot(Vector3 const& _v1, Vector3 const& _v2);
    static Vector3 Cross(Vector3 const& _v1, Vector3 const& _v2);
    static Vector3 Normalize(Vector3 const& _o);
    
    static Vector3 Min(Vector3 const& _v1, Vector3 const& _v2);
    static Vector3 Max(Vector3 const& _v1, Vector3 const& _v2);
    static Vector3 Abs(Vector3 const& _v);
    static Vector3 Clamp(Vector3 const& _v, Vector3 const& _min, Vector3 const& _max);
    
    static bool NearlyEqual(Vector3 const& _v1, Vector3 const& _v2);
    static bool NearlyEqual(Vector3 const& _v1, Vector3 const& _v2, float _margin);
    
    Vector2<T> xy() const;
    Vector2<T> yx() const;
    Vector2<T> xz() const;
    Vector2<T> zx() const;
    Vector2<T> yz() const;
    Vector2<T> zy() const;
    
    Vector3 xzy() const;
    Vector3 yxz() const;
    Vector3 yzx() const;
    Vector3 zxy() const;
    Vector3 zyx() const;
    
    T  operator[](int _i) const;
    T& operator[](int _i);
    T* Data();
    T const* Data() const;
};

template <typename T>
std::ostream& operator<<(std::ostream& _os, Vector3<T> const& _v);

template <typename T>
Vector3<T> operator*(T _scalar, Vector3<T> const& _o);

#include "Vector3.inl"

#endif
