#ifndef VECTOR2_FWD_H_DEFINED
#define VECTOR2_FWD_H_DEFINED

#include <iostream>

template <typename T>
class Vector2
{
public:
    T x;
    T y;
    
    Vector2();
    Vector2(T _x, T _y);
    Vector2(T _scalar);
    Vector2(std::initializer_list<T> _l);
    
    Vector2 operator+(Vector2 const& _o) const;
    Vector2 operator-(Vector2 const& _o) const;
    Vector2 operator-() const;
    Vector2 operator*(Vector2 const& _o) const;
    Vector2 operator/(Vector2 const& _o) const;
    Vector2 operator*(float _scalar) const;
    Vector2 operator/(float _scalar) const;
    
    Vector2& operator+=(Vector2 const& _o);
    Vector2& operator-=(Vector2 const& _o);
    Vector2& operator*=(Vector2 const& _o);
    Vector2& operator/=(Vector2 const& _o);
    Vector2& operator*=(float _scalar);
    Vector2& operator/=(float _scalar);
    
    bool operator==(Vector2 const& _o) const;
    bool operator!=(Vector2 const& _o) const;
    
    bool IsNull() const;
    
    float Dot(Vector2 const& _o) const;
    float DeltaAngle(Vector2 const& _o) const;
    
    float Length() const;
    float LengthSquared() const;
    
    Vector2  Normalized() const;
    Vector2& SelfNormalize();

    Vector2 Reflect(Vector2 const& _normal) const;
    Vector2 Project(Vector2 const& _target) const;
    Vector2 Perpendicular() const;
    
    Vector2 Abs() const;
    Vector2 Clamp(Vector2 const& _min, Vector2 const& _max) const;
    
    static Vector2 Zero();
    static Vector2 One();
    static Vector2 Scalar(T _scalar);
    
    static float Dot(Vector2 const& _v1, Vector2 const& _v2);
    static float DeltaAngle(Vector2 const& _v1, Vector2 const& _v2);
    static Vector2 Normalize(Vector2 const& _o);
    
    static Vector2 Min(Vector2 const& _v1, Vector2 const& _v2);
    static Vector2 Max(Vector2 const& _v1, Vector2 const& _v2);
    static Vector2 Abs(Vector2 const& _v);
    static Vector2 Clamp(Vector2 const& _v, Vector2 const& _min, Vector2 const& _max);
    
    static bool NearlyEqual(Vector2 const& _v1, Vector2 const& _v2);
    static bool NearlyEqual(Vector2 const& _v1, Vector2 const& _v2, float _margin);
    
    Vector2 yx() const;
    
    T  operator[](int _i) const;
    T& operator[](int _i);
    T* Data();
    T const* Data() const;
};

template <typename T>
std::ostream& operator<<(std::ostream& _os, Vector2<T> const& _v);

template <typename T>
Vector2<T> operator*(T _scalar, Vector2<T> const& _o);

#include "Vector2.inl"

#endif
