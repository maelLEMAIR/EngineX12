#ifndef VECTOR4_FWD_H_DEFINED
#define VECTOR4_FWD_H_DEFINED

#include "Vector2.h"
#include "Vector3.h"

template <typename T>
class Matrix4;

template <typename T>
class Vector4
{
public:
    T x;
    T y;
    T z;
    T w;
    
    Vector4();
    Vector4(T _scalar);
    Vector4(T _x, T _y, T _z, T w);
    
    Vector4(Vector2<T> const& _xy, Vector2<T> const& _zw);
    Vector4(Vector3<T> const& _xyz, T _w);
    Vector4(T _x, Vector3<T> const& _yzw);
    Vector4(Vector2<T> const& _xy, T _z, T _w);
    Vector4(T _x, Vector2<T> const& _yz, T _w);
    Vector4(T _x, T _y, Vector2<T> const& _zw);
    Vector4(std::initializer_list<T> _l);
    
    Vector4 operator+(Vector4 const& _o) const;
    Vector4 operator-(Vector4 const& _o) const;
    Vector4 operator-() const;
    Vector4 operator*(Vector4 const& _o) const;
    Vector4 operator/(Vector4 const& _o) const;
    Vector4 operator*(float _scalar) const;
    Vector4 operator/(float _scalar) const;
    Vector4 operator*(Matrix4<T> const& _m) const;
    
    Vector4& operator+=(Vector4 const& _o);
    Vector4& operator-=(Vector4 const& _o);
    Vector4& operator*=(Vector4 const& _o);
    Vector4& operator/=(Vector4 const& _o);
    Vector4& operator*=(float _scalar);
    Vector4& operator/=(float _scalar);
    Vector4& operator*=(Matrix4<T> const& _m);
    
    bool operator==(Vector4 const& _o)const ;
    bool operator!=(Vector4 const& _o)const ;
    
    bool IsNull() const;
    
    float Dot(Vector4 const& _o) const;
    
    float Length() const;
    float LengthSquared() const;
    
    Vector4  Normalized() const;
    Vector4& SelfNormalize();
    
    Vector4 Abs() const;
    Vector4 Clamp(Vector4 const& _min, Vector4 const& _max) const;
    
    static Vector4 Zero();
    static Vector4 One();
    
    static float Dot(Vector4 const& _v1, Vector4 const& _v2);
    static Vector4 Normalize(Vector4 const& _o);
    
    static Vector4 Min(Vector4 const& _v1, Vector4 const& _v2);
    static Vector4 Max(Vector4 const& _v1, Vector4 const& _v2);
    static Vector4 Abs(Vector4 const& _v);
    static Vector4 Clamp(Vector4 const& _v, Vector4 const& _min, Vector4 const& _max);
    
    static bool NearlyEqual(Vector4 const& _v1, Vector4 const& _v2);
    static bool NearlyEqual(Vector4 const& _v1, Vector4 const& _v2, float _margin);
    
    Vector2<T> xy() const; Vector2<T> yx() const;
    Vector2<T> xz() const; Vector2<T> zx() const;
    Vector2<T> xw() const; Vector2<T> wx() const;
    Vector2<T> yz() const; Vector2<T> zy() const;
    Vector2<T> yw() const; Vector2<T> wy() const;
    Vector2<T> zw() const; Vector2<T> wz() const;
    
    Vector3<T> xyz() const; Vector3<T> xyw() const;
    Vector3<T> xzy() const; Vector3<T> xzw() const;
    Vector3<T> xwy() const; Vector3<T> xwz() const;

    Vector3<T> yxz() const; Vector3<T> yxw() const;
    Vector3<T> yzx() const; Vector3<T> yzw() const;
    Vector3<T> ywx() const; Vector3<T> ywz() const;

    Vector3<T> zxy() const; Vector3<T> zxw() const;
    Vector3<T> zyx() const; Vector3<T> zyw() const;
    Vector3<T> zwx() const; Vector3<T> zwy() const;

    Vector3<T> wxy() const; Vector3<T> wxz() const;
    Vector3<T> wyx() const; Vector3<T> wyz() const;
    Vector3<T> wzx() const; Vector3<T> wzy() const;
    
    Vector4<T> xywz() const;
    Vector4<T> xzyw() const; Vector4<T> xzwy() const;
    Vector4<T> xwyz() const; Vector4<T> xwzy() const;

    Vector4<T> yxzw() const; Vector4<T> yxwz() const;
    Vector4<T> yzxw() const; Vector4<T> yzwx() const;
    Vector4<T> ywxz() const; Vector4<T> ywzx() const;

    Vector4<T> zxyw() const; Vector4<T> zxwy() const;
    Vector4<T> zyxw() const; Vector4<T> zywx() const;
    Vector4<T> zwxy() const; Vector4<T> zwyx() const;

    Vector4<T> wxyz() const; Vector4<T> wxzy() const;
    Vector4<T> wyxz() const; Vector4<T> wyzx() const;
    Vector4<T> wzxy() const; Vector4<T> wzyx() const;
    
    T  operator[](int _i) const;
    T& operator[](int _i);
    T* Data();
    T const* Data() const;
};

template <typename T>
std::ostream& operator<<(std::ostream& _os, Vector4<T> const& _v);

template <typename T>
Vector4<T> operator*(T _scalar, Vector4<T> const& _o);

#endif
