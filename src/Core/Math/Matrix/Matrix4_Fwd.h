#ifndef MATRIX4_FWD_H_DEFINED
#define MATRIX4_FWD_H_DEFINED

#include "../Vector/Vector4_Fwd.h"

template <typename T>
class Vector3;

template <typename T>
class Matrix3;

class Quaternion;

template <typename T>
class Matrix4
{
public:
    union 
    {
        struct
        {
            T m00, m01, m02, m03;
            T m10, m11, m12, m13;
            T m20, m21, m22, m23;
            T m30, m31, m32, m33;
        };
        
        Vector4<T> rows[4];
    };
    
    Matrix4();
    Matrix4(T _scalar);
    Matrix4(Vector4<T> _r1, Vector4<T> _r2, Vector4<T> _r3, Vector4<T> _r4);
    Matrix4(T _00, T _01, T _02, T _03,
            T _10, T _11, T _12, T _13,
            T _20, T _21, T _22, T _23,
            T _30, T _31, T _32, T _33);
    Matrix4(std::initializer_list<std::initializer_list<T>> _l);
    
    static Matrix4<T> Identity();
    static Matrix4<T> Identity(T _scalar);
    
    Matrix4 operator+(Matrix4 const& _o) const;
    Matrix4 operator-(Matrix4 const& _o) const;
    
    Matrix4& operator+=(Matrix4 const& _o);
    Matrix4& operator-=(Matrix4 const& _o);
    
    Matrix4 operator*(T _scalar)                const;
    Vector4<T> operator*(Vector3<T> const& _v)  const;
    Vector4<T> operator*(Vector4<T> const& _v)  const;
    Matrix4 operator*(Matrix4 const& _o)        const;
    
    Matrix4& operator*=(T _scalar);
    Matrix4& operator*=(Matrix4 const& _o);
    
    Matrix4     Transposed() const;
    Matrix4&    SelfTranspose();
    
    float       Determinant()           const;
    float       Minor(int _r, int _c)   const;
    Matrix4     Comatrix()              const;
    
    Matrix4     Inverted()          const;
    Matrix4     InvertedAffine()    const;
    Matrix4&    SelfInvert();
    Matrix4&    SelfInvertAffine();
    
    Matrix3<T>  ToMatrix3()     const;
    Quaternion  ToQuaternion()  const;
    
    bool     FastDecompose(Vector3<T>* _translation, Vector3<T>* _scale, Quaternion* _rotation);
    bool     FastDecompose(Vector3<T>* _translation, Vector3<T>* _scale, Matrix3<T>* _rotation);
    bool     AffineDecompose(Vector3<T>* _translation, Vector3<T>* _scale, Quaternion* _rotation, Vector3<T>* _shear);
    bool     AffineDecompose(Vector3<T>* _translation, Vector3<T>* _scale, Matrix3<T>* _rotation, Vector3<T>* _shear);
    
    static bool FastDecompose(Matrix4 const& _m, Vector3<T>* _translation, Vector3<T>* _scale, Quaternion* _rotation);
    static bool FastDecompose(Matrix4 const& _m, Vector3<T>* _translation, Vector3<T>* _scale, Matrix3<T>* _rotation);
    static bool AffineDecompose(Matrix4 const& _m, Vector3<T>* _translation, Vector3<T>* _scale, Quaternion* _rotation, Vector3<T>* _shear);
    static bool AffineDecompose(Matrix4 const& _m, Vector3<T>* _translation, Vector3<T>* _scale, Matrix3<T>* _rotation, Vector3<T>* _shear);
    
    static float    Determinant(Matrix4 const& _m);
    static Matrix4  Transpose(Matrix4 const& _m);
    static Matrix4  Invert(Matrix4 const& _m);
    static Matrix4  InvertAffine(Matrix4 const& _m);
    
    static Matrix4  MakeTransform(Vector3<T> const& _pos, Vector3<T> const& _scale, Quaternion const& _rot);
    static Matrix4  MakeTransform(Vector3<T> const& _pos, Vector3<T> const& _scale, Matrix4 const& _rot);
    static Matrix4  MakeTranslation(Vector3<T> const& _v);
    static Matrix4  MakeScale(Vector3<T> const& _v);
    static Matrix4  MakeRotation(Vector3<T> const& _axis, T _angle);
    static Matrix4  MakeRotationX(T _angle);
    static Matrix4  MakeRotationY(T _angle);
    static Matrix4  MakeRotationZ(T _angle);
    static Matrix4  MakeRotationXYZ(T _angleX, T _angleY, T _angleZ);
    static Matrix4  MakeRotationZYX(T _angleZ, T _angleY, T _angleX);
    static Matrix4  MakeRotationYPR(T _yawY, T _pitchX, T _rollZ);
    static Matrix4  MakeRotationRPY(T _rollZ, T _pitchX, T _yawY);
    static Matrix4  MakeRotationQuat(Quaternion const& _quat);
    static Matrix4  MakeRotationAxisAngle(Vector3<T> const& _axis, T _angle);

    static Matrix4  MakeLineToLineTransform( Vector3<T> const& _a1, Vector3<T> const& _a2,
                                             Vector3<T> const& _b1, Vector3<T> const& _b2 );
    static Matrix4  MakeCrossProduct(Vector3<T> const& _v1);
    
    static Matrix4  MakePerspective(float _fov, float _aspectRatio, float _near, float _far);
    static Matrix4  MakeOrthographic(float _left, float _right, float _bottom, float _top ,float _near, float _far);
    static Matrix4  MakeLookAt(Vector3<T> const& _eye, Vector3<T> const& _target, Vector3<T> const& _up);
    
    bool operator==(const Matrix4& _o) const;
    bool operator!=(const Matrix4& _o) const;

    Vector4<T> GetColumn(int _i) const;
    
    Vector4<T> const& operator[](int _i) const;
    Vector4<T>&       operator[](int _i);
    T* Data();
    T const* Data() const;
    
private:
    Matrix3<T> PolarDecomposeRotation(Matrix3<T> const& _m);
};

template <typename T>
std::ostream& operator<<(std::ostream& _os, Matrix4<T> const& _m);

#endif
