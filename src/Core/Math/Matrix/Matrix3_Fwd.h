#ifndef MATRIX3_FWD_H_DEFINED
#define MATRIX3_FWD_H_DEFINED

#include "../Vector/Vector3_Fwd.h"

template <typename T>
class Matrix4;

class Quaternion;

template <typename T>
class Matrix3
{
public:
    union 
    {
        struct
        {
            T m00, m01, m02;
            T m10, m11, m12;
            T m20, m21, m22;
        };
        
        Vector3<T> rows[3];
    };
    
    Matrix3();
    Matrix3(T _scalar);
    Matrix3(Vector3<T> _r1, Vector3<T> _r2, Vector3<T> _r3);
    Matrix3(T _00, T _01, T _02,
            T _10, T _11, T _12,
            T _20, T _21, T _22);
    Matrix3(std::initializer_list<std::initializer_list<T>> _l);
    
    static Matrix3<T> Identity();
    static Matrix3<T> Identity(T _scalar);
    
    Matrix3 operator+(Matrix3 const& _o) const;
    Matrix3 operator-(Matrix3 const& _o) const;
    
    Matrix3& operator+=(Matrix3 const& _o);
    Matrix3& operator-=(Matrix3 const& _o);
    
    Matrix3 operator*(T _scalar)                const;
    Vector3<T> operator*(Vector3<T> const& _v)  const;
    Matrix3 operator*(Matrix3 const& _o)        const;
    
    Matrix3& operator*=(T _scalar);
    Matrix3& operator*=(Matrix3 const& _o);
    
    Matrix3     Transposed() const;
    Matrix3&    SelfTranspose();
    
    float       FrobeniusNorm()         const;
    float       Determinant()           const;
    float       Minor(int _r, int _c)   const;
    Matrix3     Comatrix()              const;
    
    Matrix3     Inverted()          const;
    Matrix3&    SelfInvert();
    
    Matrix4<T>      ToMatrix4() const;
    Quaternion      ToQuaternion() const;

    static float    Determinant(Matrix3 const& _m);
    static Matrix3  Transpose(Matrix3 const& _m);
    static Matrix3  Invert(Matrix3 const& _m);
    
    static Matrix3  MakeScale(Vector3<T> const& _v);
    static Matrix3  MakeRotation(Vector3<T> const& _axis, T _angle);
    static Matrix3  MakeRotationX(T _angle);
    static Matrix3  MakeRotationY(T _angle);
    static Matrix3  MakeRotationZ(T _angle);
    static Matrix3  MakeRotationZYX(T _angleZ, T _angleY, T _angleX);
    static Matrix3  MakeRotationXYZ(T _angleX, T _angleY, T _angleZ);
    static Matrix3  MakeRotationYPR(T _yawY, T _pitchX, T _rollZ);
    static Matrix3  MakeRotationRPY(T _rollZ, T _pitchX, T _yawY);
    static Matrix3  MakeRotationQuat(Quaternion const& _quat);
    static Matrix3  MakeRotationAxisAngle(Vector3<T> const& _axis, T _angle);
    
    static Matrix3  MakeVectorRotation(Vector3<T> const& _start, Vector3<T> const& _end);
    static Matrix3  MakeCrossProduct(Vector3<T> const& _v1);
    
    bool operator==(const Matrix3&) const;
    bool operator!=(const Matrix3&) const;

    Vector3<T> GetColumn(int _i) const;
    
    Vector3<T> const& operator[](int _i) const;
    Vector3<T>&       operator[](int _i);
    T* Data();
    T const* Data() const;
};

template <typename T>
std::ostream& operator<<(std::ostream& _os, Matrix3<T> const& _m);

#endif
