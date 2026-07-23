#ifndef MATRIX4_INL_DEFINED
#define MATRIX4_INL_DEFINED

#include "Matrix4_Fwd.h"
#include "Matrix3_Fwd.h"
#include "../Quaternions/Quaternion.h"
#include "../Vector/Vector3_Fwd.h"

template <typename T>
Matrix4<T>::Matrix4() : m00(), m01(), m02(), m03(), 
                        m10(), m11(), m12(), m13(), 
                        m20(), m21(), m22(), m23(), 
                        m30(), m31(), m32(), m33() 
{
}

template <typename T>
Matrix4<T>::Matrix4(T _scalar)
{
    rows[0] = Vector4<T>(_scalar, T(0), T(0), T(0));
    rows[1] = Vector4<T>(T(0), _scalar, T(0), T(0));
    rows[2] = Vector4<T>(T(0), T(0), _scalar, T(0));
    rows[3] = Vector4<T>(T(0), T(0), T(0), _scalar);
}

template <typename T>
Matrix4<T>::Matrix4(Vector4<T> _r1, Vector4<T> _r2, Vector4<T> _r3, Vector4<T> _r4)
{
    rows[0] = _r1;
    rows[1] = _r2;
    rows[2] = _r3;
    rows[3] = _r4;
}

template <typename T>
Matrix4<T>::Matrix4(
    T _11, T _12, T _13, T _14, 
    T _21, T _22, T _23, T _24, 
    T _31, T _32, T _33, T _34, 
    T _41, T _42, T _43, T _44)
{
    rows[0] = Vector4<T>(_11, _12, _13, _14);
    rows[1] = Vector4<T>(_21, _22, _23, _24);
    rows[2] = Vector4<T>(_31, _32, _33, _34);
    rows[3] = Vector4<T>(_41, _42, _43, _44);
}

template <typename T>
Matrix4<T>::Matrix4(std::initializer_list<std::initializer_list<T>> _l)
{
    assert(_l.size() == 4 && "Incorrect init list size");
    const std::initializer_list<float>* row = _l.begin();
    for (int i = 0; i < 4; i++)
    {
        assert(row[i].size() == 4 && "Incorrect init list size");
        const float* col = row[i].begin();
        for (int j = 0; j < 4; j++)
        {
            rows[i][j] = col[j];
        }
    }
}

template <typename T>
Matrix4<T> Matrix4<T>::Identity()
{
    Matrix4<T> result;
    result.rows[0] = Vector4<T>(1, 0, 0, 0);
    result.rows[1] = Vector4<T>(0, 1, 0, 0);
    result.rows[2] = Vector4<T>(0, 0, 1, 0);
    result.rows[3] = Vector4<T>(0, 0, 0, 1);
    return result;
}

template <typename T>
Matrix4<T> Matrix4<T>::Identity(T _scalar)
{
    Matrix4<T> result;
    result.rows[0] = Vector4<T>(_scalar, 0, 0, 0);
    result.rows[1] = Vector4<T>(0, _scalar, 0, 0);
    result.rows[2] = Vector4<T>(0, 0, _scalar, 0);
    result.rows[3] = Vector4<T>(0, 0, 0, _scalar);
    return result;
}

template <typename T>
Matrix4<T> Matrix4<T>::operator+(Matrix4 const& _o) const
{
    return Matrix4<T>(
        rows[0] + _o.rows[0],
        rows[1] + _o.rows[1],
        rows[2] + _o.rows[2],
        rows[3] + _o.rows[3]
        );
}

template <typename T>
Matrix4<T> Matrix4<T>::operator-(Matrix4 const& _o) const
{
    return Matrix4<T>(
        rows[0] - _o.rows[0],
        rows[1] - _o.rows[1],
        rows[2] - _o.rows[2],
        rows[3] - _o.rows[3]
        );
}

template <typename T>
Matrix4<T>& Matrix4<T>::operator+=(Matrix4 const& _o)
{
    rows[0] += _o.rows[0];
    rows[1] += _o.rows[1];
    rows[2] += _o.rows[2];
    rows[3] += _o.rows[3];
    return *this;
}

template <typename T>
Matrix4<T>& Matrix4<T>::operator-=(Matrix4 const& _o)
{
    rows[0] -= _o.rows[0];
    rows[1] -= _o.rows[1];
    rows[2] -= _o.rows[2];
    rows[3] -= _o.rows[3];
    return *this;
}

template <typename T>
Matrix4<T> Matrix4<T>::operator*(T _scalar) const
{
    return Matrix4<T>(
        rows[0] * _scalar,
        rows[1] * _scalar,
        rows[2] * _scalar,
        rows[3] * _scalar);
}

template <typename T>
Vector4<T> Matrix4<T>::operator*(Vector3<T> const& _v) const
{
    Vector4<T> result;
    result.x = m00 * _v.x + m01 * _v.y + m02 * _v.z + m03;
    result.y = m10 * _v.x + m11 * _v.y + m12 * _v.z + m13;
    result.z = m20 * _v.x + m21 * _v.y + m22 * _v.z + m23;
    result.w = m30 * _v.x + m31 * _v.y + m32 * _v.z + m33;
    return result;
}

template <typename T>
Vector4<T> Matrix4<T>::operator*(Vector4<T> const& _v) const
{
    Vector4<T> result;
    result.x = m00 * _v.x + m01 * _v.y + m02 * _v.z + m03 * _v.w;
    result.y = m10 * _v.x + m11 * _v.y + m12 * _v.z + m13 * _v.w;
    result.z = m20 * _v.x + m21 * _v.y + m22 * _v.z + m23 * _v.w;
    result.w = m30 * _v.x + m31 * _v.y + m32 * _v.z + m33 * _v.w;
    return result;
}

template <typename T>
Matrix4<T> Matrix4<T>::operator*(Matrix4 const& _o) const
{
    Matrix4<T> result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result[i][j] =
                (*this)[i][0] * _o[0][j] +
                (*this)[i][1] * _o[1][j] +
                (*this)[i][2] * _o[2][j] +
                (*this)[i][3] * _o[3][j];
        }
    }
    return result;
}

template <typename T>
Matrix4<T>& Matrix4<T>::operator*=(T _scalar)
{
    rows[0] *= _scalar;
    rows[1] *= _scalar;
    rows[2] *= _scalar;
    rows[3] *= _scalar;
    return *this;
}

template <typename T>
Matrix4<T>& Matrix4<T>::operator*=(Matrix4 const& _o)
{
    Matrix4<T> result;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result[i][j] =
                (*this)[i][0] * _o[0][j] +
                (*this)[i][1] * _o[1][j] +
                (*this)[i][2] * _o[2][j] +
                (*this)[i][3] * _o[3][j];
        }
    }
    rows[0] = result.rows[0];
    rows[1] = result.rows[1];
    rows[2] = result.rows[2];
    rows[3] = result.rows[3];
    
    return *this;
}

template <typename T>
Matrix4<T> Matrix4<T>::Transposed() const
{
    Matrix4<T> result;
    result.rows[0] = Vector4<T>(m00, m10, m20, m30);
    result.rows[1] = Vector4<T>(m01, m11, m21, m31);
    result.rows[2] = Vector4<T>(m02, m12, m22, m32);
    result.rows[3] = Vector4<T>(m03, m13, m23, m33);
    return result;
}

template <typename T>
Matrix4<T>& Matrix4<T>::SelfTranspose()
{
    Matrix4<T> result;
    result.rows[0] = Vector4<T>(m00, m10, m20, m30);
    result.rows[1] = Vector4<T>(m01, m11, m21, m31);
    result.rows[2] = Vector4<T>(m02, m12, m22, m32);
    result.rows[3] = Vector4<T>(m03, m13, m23, m33);
    
    rows[0] = result.rows[0];
    rows[1] = result.rows[1];
    rows[2] = result.rows[2];
    rows[3] = result.rows[3];
    return *this;
}

template <typename T>
float Matrix4<T>::Determinant() const
{
    // Hardcoded Laplace Formula
    
    auto Det = [](float _a, float _b, float _c, float _d) -> float
    {
        return _a * _d - _b * _c;
    };
    
    float s0 = Det(m00, m01, m10, m11);
    float s1 = Det(m00, m02, m10, m12);
    float s2 = Det(m00, m03, m10, m13);
    float s3 = Det(m01, m02, m11, m12);
    float s4 = Det(m01, m03, m11, m13);
    float s5 = Det(m02, m03, m12, m13);
    
    float c0 = Det(m20, m21, m30, m31);
    float c1 = Det(m20, m22, m30, m32);
    float c2 = Det(m20, m23, m30, m33);
    float c3 = Det(m21, m22, m31, m32);
    float c4 = Det(m21, m23, m31, m33);
    float c5 = Det(m22, m23, m32, m33);
    
    return s0*c5 - s1*c4 + s2*c3 + s3*c2 - s4*c1 + s5*c0;
}

template <typename T>
float Matrix4<T>::Minor(int _r, int _c) const
{
    T sub[3][3];
    int sRowIndex = 0;
    
    for (int i = 0; i < 4; i++)
    {
        if (i == _r) continue;
        int sColIndex = 0;
        for (int j = 0; j < 4; j++)
        {
            if (j == _c) continue;
            sub[sRowIndex][sColIndex] = rows[i][j];
            sColIndex++;
        }
        sRowIndex++;
    }
    
    return sub[0][0] * (sub[1][1]*sub[2][2] - sub[1][2]*sub[2][1])
         - sub[0][1] * (sub[1][0]*sub[2][2] - sub[1][2]*sub[2][0])
         + sub[0][2] * (sub[1][0]*sub[2][1] - sub[1][1]*sub[2][0]); 
}

template <typename T>
Matrix4<T> Matrix4<T>::Comatrix() const
{
    Matrix4 result;
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            T sign = ((r + c) % 2 == 0) ? T(1) : T(-1);
            result[r][c] = sign * Minor(r, c);
        }   
    }
    return result;
}

template <typename T>
Matrix4<T> Matrix4<T>::Inverted() const
{
    float det = Determinant();
    assert(det != 0 && "Matrix isn't invertible");
    
    float invDet = 1.0f / det;
    
    Matrix4<T> result = Comatrix().Transposed() * invDet;
    return result;
}

template <typename T>
Matrix4<T> Matrix4<T>::InvertedAffine() const
{
    Matrix3<T> rotScal = Matrix3<T>(rows[0].xyz(), rows[1].xyz(), rows[2].xyz());
    Matrix3<T> inv = rotScal.Inverted();
    
    Vector3<T> translation = rows[3].xyz();
    Vector3<T> invTrans = translation * inv;
    invTrans = -invTrans;
    
    Matrix4<T> result;
    result.rows[0] = Vector4<T>(inv.rows[0], T(0));
    result.rows[1] = Vector4<T>(inv.rows[1], T(0));
    result.rows[2] = Vector4<T>(inv.rows[2], T(0));
    result.rows[3] = Vector4<T>(invTrans,    T(1));
    return result;
}

template <typename T>
Matrix4<T>& Matrix4<T>::SelfInvert()
{
    float det = Determinant();
    assert(det != 0 && "Matrix isn't invertible");
    
    float invDet = 1.0f / det;
    
    Matrix4<T> result = Comatrix().Transposed() * invDet;
    rows[0] = result.rows[0];
    rows[1] = result.rows[1];
    rows[2] = result.rows[2];
    rows[3] = result.rows[3];
    return *this;
}

template <typename T>
Matrix4<T>& Matrix4<T>::SelfInvertAffine()
{
    Matrix3<T> rotScal = Matrix3<T>(rows[0].xyz(), rows[1].xyz(), rows[2].xyz());
    Matrix3<T> inv = rotScal.Inverted();
    
    Vector3<T> translation = rows[3].xyz();
    Vector3<T> invTrans = translation * inv;
    invTrans = -invTrans;
    
    rows[0] = Vector4<T>(inv.rows[0], T(0));
    rows[1] = Vector4<T>(inv.rows[1], T(0));
    rows[2] = Vector4<T>(inv.rows[2], T(0));
    rows[3] = Vector4<T>(invTrans,    T(1));
    return *this;
}

template <typename T>
Matrix3<T> Matrix4<T>::ToMatrix3() const
{
    return {
        { m00, m01, m02 },
        { m10, m11, m12 },
        { m20, m21, m22 }
    };
}

template <typename T>
Quaternion Matrix4<T>::ToQuaternion() const
{
    return Quaternion::FromRotationMatrix(*this);
}

template <typename T>
float Matrix4<T>::Determinant(Matrix4 const& _m)
{
    return _m.Determinant(); 
}

template <typename T>
Matrix4<T> Matrix4<T>::Transpose(Matrix4 const& _m)
{
    return _m.Transposed();
}

template <typename T>
Matrix4<T> Matrix4<T>::Invert(Matrix4 const& _m)
{
    return _m.Inverted();
}

template <typename T>
Matrix4<T> Matrix4<T>::InvertAffine(Matrix4 const& _m)
{
    return _m.InvertedAffine();
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeTransform(Vector3<T> const& _pos, Vector3<T> const& _scale, Quaternion const& _rot)
{
    Matrix4<T> rot = MakeRotationQuat(_rot);
    
    Matrix4<T> result;
    result.rows[0] = _scale.x * rot.rows[0];
    result.rows[1] = _scale.y * rot.rows[1];
    result.rows[2] = _scale.z * rot.rows[2];
    result.rows[3] = Vector4<T>(_pos, T(1));
    return result;
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeTransform(Vector3<T> const& _pos, Vector3<T> const& _scale, Matrix4 const& _rot)
{
    Matrix4<T> result;
    result.rows[0] = _scale.x * _rot.rows[0];
    result.rows[1] = _scale.y * _rot.rows[1];
    result.rows[2] = _scale.z * _rot.rows[2];
    result.rows[3] = Vector4<T>(_pos, T(1));
    return result;
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeTranslation(Vector3<T> const& _v)
{
    return {
        {T(1), T(0), T(0), T(0)},
        {T(0), T(1), T(0), T(0)},
        {T(0), T(0), T(1), T(0)},
        {_v.x, _v.y, _v.z, T(1)}
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeScale(Vector3<T> const& _v)
{
    return {
        {_v.x, T(0), T(0), T(0)},
        {T(0), _v.y, T(0), T(0)},
        {T(0), T(0), _v.z, T(0)},
        {T(0), T(0), T(0), T(1)}
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotation(Vector3<T> const& _axis, T _angle)
{
    assert(_axis.Length() == 1 && "Axis vector should be an unit vector");
    
    float c = MathUtils::Cos(_angle);
    float s = MathUtils::Sin(_angle);
    
    float ic = 1 - c;
    
    float x = _axis.x;
    float y = _axis.y;
    float z = _axis.z;
    
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    
    return {
        { x2 * ic + c, x * y * ic + z * s, x * z * ic - y * s , T(0) },
        { x * y * ic - z * s, y2 * ic + c, y * z * ic + x * s , T(0) },
        { x * z * ic + y * s, y * z * ic - x * s, z2 * ic + c , T(0) },
        {               T(0),               T(0),         T(0), T(1) }
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationX(T _angle)
{
    return {
            {T(1),                    T(0),                   T(0), T(0)},
              {T(0),  MathUtils::Cos(_angle), MathUtils::Sin(_angle), T(0)},
              {T(0), -MathUtils::Sin(_angle), MathUtils::Cos(_angle), T(0)},
              {T(0),                    T(0),                   T(0), T(1)}
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationY(T _angle)
{
    return {
            { MathUtils::Cos(_angle), T(0), -MathUtils::Sin(_angle), T(0)},
              {                   T(0), T(1),                    T(0), T(0)},
              { MathUtils::Sin(_angle), T(0),  MathUtils::Cos(_angle), T(0)},
              {                   T(0), T(0),                    T(0), T(1)}
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationZ(T _angle)
{
    return {
            {  MathUtils::Cos(_angle), MathUtils::Sin(_angle), T(0), T(0)},
              { -MathUtils::Sin(_angle), MathUtils::Cos(_angle), T(0), T(0)},
              {                    T(0),                   T(0), T(1), T(0)},
              {                    T(0),                   T(0), T(0), T(1)}
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationXYZ(T _angleX, T _angleY, T _angleZ)
{
    float cX = MathUtils::Cos(_angleX);
    float sX = MathUtils::Sin(_angleX);
    float cY = MathUtils::Cos(_angleY);
    float sY = MathUtils::Sin(_angleY);
    float cZ = MathUtils::Cos(_angleZ);
    float sZ = MathUtils::Sin(_angleZ);
    
    return {
                {            cY*cZ,            cY*sZ,   -sY, T(0) },
                { sX*sY*cZ - cX*sZ, sX*sY*sZ + cX*cZ, sX*cY, T(0) },
                { cX*sY*cZ + sX*sZ, cX*sY*sZ - sX*cZ, cX*cY, T(0) },
                {             T(0),             T(0),  T(0), T(1) }
        };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationZYX(T _angleZ, T _angleY, T _angleX)
{
    float cX = MathUtils::Cos(_angleX);
    float sX = MathUtils::Sin(_angleX);
    float cY = MathUtils::Cos(_angleY);
    float sY = MathUtils::Sin(_angleY);
    float cZ = MathUtils::Cos(_angleZ);
    float sZ = MathUtils::Sin(_angleZ);
    
    return {
            {  cY*cZ, cX*sZ + sX*sY*cZ, sX*sZ - cX*sY*cZ, T(0) },
            { -cY*sZ, cX*cZ - sX*sY*sZ, sX*cZ + cX*sY*sZ, T(0) },
            {     sY,           -sX*cY,            cX*cY, T(0) },
            {   T(0),             T(0),             T(0), T(1) }
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationYPR(T _yawY, T _pitchX, T _rollZ)
{
    float cX = MathUtils::Cos(_pitchX);
    float sX = MathUtils::Sin(_pitchX);
    float cY = MathUtils::Cos(_yawY);
    float sY = MathUtils::Sin(_yawY);
    float cZ = MathUtils::Cos(_rollZ);
    float sZ = MathUtils::Sin(_rollZ);
    
    return {
            { cY*cZ - sX*sY*sZ, cY*sZ + sX*sY*cZ, -cX*sY, T(0) },
            {           -cX*sZ,            cX*cZ,     sX, T(0) },
            { sY*cZ + sX*cY*sZ, sY*sZ - sX*cY*cZ,  cX*cY, T(0) },
            {             T(0),             T(0),   T(0), T(1) }
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationRPY(T _rollZ, T _pitchX, T _yawY)
{
    float cX = MathUtils::Cos(_pitchX);
    float sX = MathUtils::Sin(_pitchX);
    float cY = MathUtils::Cos(_yawY);
    float sY = MathUtils::Sin(_yawY);
    float cZ = MathUtils::Cos(_rollZ);
    float sZ = MathUtils::Sin(_rollZ);
    
    return {
            {  cY*cZ + sX*sY*sZ, cX*sZ, -sY*cZ + sX*cY*sZ, T(0) },
            { -cY*sZ + sX*sY*cZ, cX*cZ,  sY*sZ + sX*cY*cZ, T(0) },
            {             cX*sY,   -sX,             cX*cY, T(0) },
            {              T(0),  T(0),              T(0), T(1) }
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationQuat(Quaternion const& _quat)
{
    float x = _quat.x;
    float y = _quat.y;
    float z = _quat.z;
    float w = _quat.w;
    
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    
    return {
            { 1.0f - 2*y2 - 2*z2, 2*x*y + 2*z*w, 2*x*z - 2*y*w, T(0) },
            { 2*x*y - 2*z*w, 1.0f - 2*x2 - 2*z2, 2*y*z + 2*x*w, T(0) },
            { 2*x*z + 2*y*w, 2*y*z - 2*x*w, 1.0f - 2*x2 - 2*y2, T(0) },
            {          T(0),          T(0),               T(0), T(1) }
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeRotationAxisAngle(Vector3<T> const& _axis, T _angle)
{
    float x = _axis.x;
    float y = _axis.y;
    float z = _axis.z;
    
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;

    float c = MathUtils::Cos(_angle);
    float s = MathUtils::Sin(_angle);
    
    return {
            {    c + x2*(1 - c), x*y*(1 - c) + z*s, x*z*(1 - c) - y*s, T(0) },
            { x*y*(1 - c) - z*s,    c + y2*(1 - c), y*z*(1 - c) + x*s, T(0) },
            { x*z*(1 - c) + y*s, y*z*(1 - c) - x*s,    c + z2*(1 - c), T(0) },
            {              T(0),              T(0),              T(0), T(1) }
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeLineToLineTransform(Vector3<T> const& _a1, Vector3<T> const& _a2, Vector3<T> const& _b1,
    Vector3<T> const& _b2)
{
    Vector3<T> v1 = _a2 - _a1;
    Vector3<T> v2 = _b2 - _b1;

    float len1 = v1.Length();
    float len2 = v2.Length();

    assert( len1 != 0.0f && "Can't transform null vector" );
    float scale = len2 / len1;
    
    Matrix3<T> R = Matrix3<T>::MakeVectorRotation(v1, v2);
    
    Matrix4 t = Matrix4<T>::MakeTranslation(-_a1) *
                R.ToMatrix4() *
                Matrix4<T>::MakeScale(Vector3<T>(scale, scale, scale)) *
                Matrix4<T>::MakeTranslation(_b1);

    return t;
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeCrossProduct(Vector3<T> const& _v1)
{
    return {
            {   T(0),  _v1.z, -_v1.y, T(0) },
              { -_v1.z,   T(0),  _v1.x, T(0) },
              {  _v1.y, -_v1.x, T  (0), T(0) },
              {   T(0),   T(0),   T(0), T(1) }
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakePerspective(float _fov, float _aspectRatio, float _near, float _far)
{
    float _00 = 1.0f / (_aspectRatio * MathUtils::Tan(_fov / 2));
    float _11 = 1.0f / MathUtils::Tan(_fov / 2);
    float _22 = -(_near + _far) / (_near - _far);
    float _23 = 1.0f;
    float _32 = -(2.0f * _far * _near) / (_far - _near);
    
    return {
        {  _00, T(0), T(0), T(0) },
        { T(0),  _11, T(0), T(0) },
        { T(0), T(0),  _22, _23  },
        { T(0), T(0),  _32, T(0) },
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeOrthographic(float _left, float _right, float _bottom, float _top, float _near, float _far)
{
    float _00 = 2.0f / (_right - _left);
    float _11 = 2.0f / (_top - _bottom);
    float _22 = - 2.0f / (_far - _near);
    
    float _03 = - (_right + _left) / (_right - _left);
    float _13 = - (_top + _bottom) / (_top - _bottom);
    float _23 = - (_near + _far)   / (_far - _near);
    float _33 = 1.0f;
    
    return {
        {  _00, T(0), T(0), _03 },
        { T(0),  _11, T(0), _13 },
        { T(0), T(0),  _22, _23 },
        { T(0), T(0), T(0), _33 }
    };
}

template <typename T>
Matrix4<T> Matrix4<T>::MakeLookAt(Vector3<T> const& _eye, Vector3<T> const& _target, Vector3<T> const& _up)
{
    Vector3<T> forward = (_eye - _target).Normalized();
    Vector3<T> right = (_up ^ forward).Normalized();
    Vector3<T> up = forward ^ right;
    
    float tx = -Vector3<T>::Dot(right, _eye);
    float ty = -Vector3<T>::Dot(up, _eye);
    float tz = -Vector3<T>::Dot(forward, _eye);
    
    return {
        { right.x, up.x, forward.x, T(0) },
        { right.y, up.y, forward.y, T(0) },
        { right.z, up.z, forward.z, T(0) },
        {      tx,   ty,        tz, T(1) }
    };
}

template <typename T>
bool Matrix4<T>::operator==(const Matrix4& _o) const
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            if (rows[i][j] != _o.rows[i][j]) return false;
    }
    return true;
}

template <typename T>
bool Matrix4<T>::operator!=(const Matrix4& _o) const
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
            if (rows[i][j] != _o.rows[i][j]) return true;
    }
    return false;
}

template <typename T>
Vector4<T> Matrix4<T>::GetColumn(int _i) const
{
    Vector4<T> res;
    _i = _i % 4;

    for (int i = 0; i < 4; i++)
    {
        res[i] = rows[i][_i];
    }
    return res;
}

template <typename T>
bool Matrix4<T>::FastDecompose(Vector3<T>* _translation, Vector3<T>* _scale, Quaternion* _rotation)
{
    Matrix3<T> rot;
    bool result = FastDecompose(_translation, _scale, &rot);
    
    if (result == false)
        return false;
    
    if (_rotation != nullptr)
        *_rotation = rot.ToQuaternion();
    
    return true;
}

template <typename T>
bool Matrix4<T>::FastDecompose(Vector3<T>* _translation, Vector3<T>* _scale, Matrix3<T>* _rotation)
{
    bool hasTranslation = _translation != nullptr;
    bool hasScale = _scale != nullptr;
    bool hasRotation = _rotation != nullptr;
    
    if (hasTranslation)
    {
        _translation->x = m30;
        _translation->y = m31;
        _translation->z = m32;
    }
    
    if (hasScale || hasRotation)
    {
        float sX = rows[0].xyz().Length();
        float sY = rows[1].xyz().Length();
        float sZ = rows[2].xyz().Length();
        
        if (sX == 0.0f || sY == 0.0f || sZ == 0.0f)
            return false;
        
        if (hasScale)
        {
            _scale->x = sX;
            _scale->y = sY;
            _scale->z = sZ;
        }
        
        if (hasRotation)
        {
            Matrix3<T> rot = {
                { m00 / sX, m01 / sX, m02 / sX },
                { m10 / sY, m11 / sY, m12 / sY },
                { m20 / sZ, m21 / sZ, m22 / sZ }
            };
            
            *_rotation = rot;
        }
    }
    return true;
}

template <typename T>
bool Matrix4<T>::AffineDecompose(Vector3<T>* _translation, Vector3<T>* _scale, Quaternion* _rotation, Vector3<T>* _shear)
{
    Matrix3<T> rot;
    bool result = FastDecompose(_translation, _scale, &rot, _shear);
    
    if (result == false)
        return false;
    
    if (_rotation != nullptr)
        *_rotation = rot.ToQuaternion();
    
    return true;
}

template <typename T>
bool Matrix4<T>::AffineDecompose(Vector3<T>* _translation, Vector3<T>* _scale, Matrix3<T>* _rotation,
    Vector3<T>* _shear)
{
    bool hasTranslation = _translation != nullptr;
    bool hasScale = _scale != nullptr;
    bool hasRotation = _rotation != nullptr;
    bool hasShear = _shear != nullptr;
    
    if (hasTranslation)
    {
        _translation->x = m30;
        _translation->y = m31;
        _translation->z = m32;
    }
    
    if (hasRotation || hasScale || hasShear)
    {
        Matrix3<T> upper = ToMatrix3();
        Matrix3<T> rot = PolarDecomposeRotation(upper);
        
        if (hasRotation)
            *_rotation = rot;
        
        if (hasScale || hasShear)
        {
            Matrix3<T> scale = upper * rot.Transposed();
            
            if (hasScale)
            {
                _scale->x = scale.m00;
                _scale->y = scale.m11;
                _scale->z = scale.m12;   
            }
            
            if (hasShear)
            {
                _shear.x = scale.m01;
                _shear.y = scale.m02;
                _shear.z = scale.m12;
            }
        }
    }
    
    return true;
}

template <typename T>
bool Matrix4<T>::FastDecompose(Matrix4 const& _m, Vector3<T>* _translation, Vector3<T>* _scale, Quaternion* _rotation)
{
    Matrix3<T> rot;
    bool result = Matrix4<T>::FastDecompose(_m, _translation, _scale, &rot);
    
    if (result == false)
        return false;
    
    if (_rotation != nullptr)
        *_rotation = rot.ToQuaternion();
    
    return true;
}

template <typename T>
bool Matrix4<T>::FastDecompose(Matrix4 const& _m, Vector3<T>* _translation, Vector3<T>* _scale, Matrix3<T>* _rotation)
{
    bool hasTranslation = _translation != nullptr;
    bool hasScale = _scale != nullptr;
    bool hasRotation = _rotation != nullptr;
    
    if (hasTranslation)
    {
        _translation->x = _m.m30;
        _translation->y = _m.m31;
        _translation->z = _m.m32;
    }
    
    if (hasScale || hasRotation)
    {
        float sX = _m.rows[0].xyz().Length();
        float sY = _m.rows[1].xyz().Length();
        float sZ = _m.rows[2].xyz().Length();
        
        if (sX == 0.0f || sY == 0.0f || sZ == 0.0f)
            return false;
        
        if (hasScale)
        {
            _scale->x = sX;
            _scale->y = sY;
            _scale->z = sZ;
        }
        
        if (hasRotation)
        {
            Matrix3<T> rot = {
                { _m.m00 / sX, _m.m01 / sX, _m.m02 / sX },
                { _m.m10 / sY, _m.m11 / sY, _m.m12 / sY },
                { _m.m20 / sZ, _m.m21 / sZ, _m.m22 / sZ }
            };
            
            *_rotation = rot;
        }
    }
    return true;
}

template <typename T>
bool Matrix4<T>::AffineDecompose(Matrix4 const& _m, Vector3<T>* _translation, Vector3<T>* _scale, Quaternion* _rotation,
    Vector3<T>* _shear)
{
    Matrix3<T> rot;
    bool result = Matrix4<T>::AffineDecompose(_m, _translation, _scale, &rot, _shear);
    
    if (result == false)
        return false;
    
    if (_rotation != nullptr)
        *_rotation = rot.ToQuaternion();
    return true;
}

template <typename T>
bool Matrix4<T>::AffineDecompose(Matrix4 const& _m, Vector3<T>* _translation, Vector3<T>* _scale, Matrix3<T>* _rotation,
    Vector3<T>* _shear)
{
    bool hasTranslation = _translation != nullptr;
    bool hasScale = _scale != nullptr;
    bool hasRotation = _rotation != nullptr;
    bool hasShear = _shear != nullptr;
    
    if (hasTranslation)
    {
        _translation->x = _m.m30;
        _translation->y = _m.m31;
        _translation->z = _m.m32;
    }
    
    if (hasRotation || hasScale || hasShear)
    {
        Matrix3<T> upper = _m.ToMatrix3();
        Matrix3<T> rot = PolarDecomposeRotation(upper);
        
        if (hasRotation)
            *_rotation = rot;
        
        if (hasScale || hasShear)
        {
            Matrix3<T> scale = upper * rot.Transposed();
            
            if (hasScale)
            {
                _scale->x = scale.m00;
                _scale->y = scale.m11;
                _scale->z = scale.m12;   
            }
            
            if (hasShear)
            {
                _shear.x = scale.m01;
                _shear.y = scale.m02;
                _shear.z = scale.m12;
            }
        }
    }
    
    return true;
}

template <typename T>
Vector4<T> const& Matrix4<T>::operator[](int _i) const
{
    return rows[_i];
}

template <typename T>
Vector4<T>& Matrix4<T>::operator[](int _i)
{
    return rows[_i];
}

template <typename T>
T* Matrix4<T>::Data()
{
    return &m00;
}

template <typename T>
T const* Matrix4<T>::Data() const
{
    return &m00;
}

template <typename T>
Matrix3<T> Matrix4<T>::PolarDecomposeRotation(Matrix3<T> const& _m)
{
    for (int i = 0; i < 16; ++i)
    {
        Matrix3<T> Minv = _m.Inverse();
        Matrix3<T> Minv_T = Minv.Transposed();
        
        _m = (_m + Minv_T) * T(0.5);
        
        Matrix3<T> shouldBeIdentity = _m * _m.Transposed();
        Matrix3<T> identity = Matrix3<T>::Identity();
        if ((shouldBeIdentity - identity).FrobeniusNorm() < T(1e-6))
            break;
    }
    return _m;
}

template <typename T>
std::ostream& operator<<(std::ostream& _os, Matrix4<T> const& _m)
{
    return _os << "\n" <<
        "| " << _m[0][0] << ", " << _m[0][1] << ", " << _m[0][2] << ", " << _m[0][3] << " | " << "\n" <<
        "| " << _m[1][0] << ", " << _m[1][1] << ", " << _m[1][2] << ", " << _m[1][3] << " | " << "\n" <<
        "| " << _m[2][0] << ", " << _m[2][1] << ", " << _m[2][2] << ", " << _m[2][3] << " | " << "\n" <<
        "| " << _m[3][0] << ", " << _m[3][1] << ", " << _m[3][2] << ", " << _m[3][3] << " | " << "\n";
}


#endif
