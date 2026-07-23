#ifndef MATRIX3_INL_DEFINED
#define MATRIX3_INL_DEFINED

#include "Matrix3_Fwd.h"
#include "../Vector/Vector3.h"
#include "../Quaternions/Quaternion.h"
#include "Matrix4_Fwd.h"
#include "../MathUtils.hpp"

template <typename T>
Matrix3<T>::Matrix3()
{
    rows[0] = Vector3<T>();
    rows[1] = Vector3<T>();
    rows[2] = Vector3<T>();
}

template <typename T>
Matrix3<T>::Matrix3(T _scalar)
{
    rows[0] = Vector3<T>(_scalar, T(0), T(0));
    rows[1] = Vector3<T>(T(0), _scalar, T(0));
    rows[2] = Vector3<T>(T(0), T(0), _scalar);
}

template <typename T>
Matrix3<T>::Matrix3(Vector3<T> _r1, Vector3<T> _r2, Vector3<T> _r3)
{
    rows[0] = _r1;
    rows[1] = _r2;
    rows[2] = _r3;
}

template <typename T>
Matrix3<T>::Matrix3(T _00, T _01, T _02, T _10, T _11, T _12, T _20, T _21, T _22)
{
    rows[0] = Vector3<T>(_00, _01, _02);
    rows[1] = Vector3<T>(_10, _11, _12);
    rows[2] = Vector3<T>(_20, _21, _22);
}

template <typename T>
Matrix3<T>::Matrix3(std::initializer_list<std::initializer_list<T>> _l)
{
    assert(_l.size() == 3 && "Incorrect init list size");
    const std::initializer_list<float>* row = _l.begin();
    for (int i = 0; i < 3; i++)
    {
        assert(row[i].size() == 3 && "Incorrect init list size");
        const float* col = row[i].begin();
        for (int j = 0; j < 3; j++)
        {
            rows[i][j] = col[j];
        }
    }
}

template <typename T>
Matrix3<T> Matrix3<T>::Identity()
{
    Matrix3<T> result;
    result.rows[0] = Vector3<T>(T(1), T(0), T(0));
    result.rows[1] = Vector3<T>(T(0), T(1), T(0));
    result.rows[2] = Vector3<T>(T(0), T(0), T(1));
    return result;
}

template <typename T>
Matrix3<T> Matrix3<T>::Identity(T _scalar)
{
    Matrix3<T> result;
    result.rows[0] = Vector3<T>(_scalar, T(0), T(0));
    result.rows[1] = Vector3<T>(T(0), _scalar, T(0));
    result.rows[2] = Vector3<T>(T(0), T(0), _scalar);
    return result;
}

template <typename T>
Matrix3<T> Matrix3<T>::operator+(Matrix3 const& _o) const
{
    Matrix3<T> result;
    result.rows[0] = rows[0] + _o.rows[0];
    result.rows[1] = rows[1] + _o.rows[1];
    result.rows[2] = rows[2] + _o.rows[2];
    return result;
}

template <typename T>
Matrix3<T> Matrix3<T>::operator-(Matrix3 const& _o) const
{
    Matrix3<T> result;
    result.rows[0] = rows[0] - _o.rows[0];
    result.rows[1] = rows[1] - _o.rows[1];
    result.rows[2] = rows[2] - _o.rows[2];
    return result;
}

template <typename T>
Matrix3<T>& Matrix3<T>::operator+=(Matrix3 const& _o)
{
    rows[0] = rows[0] + _o.rows[0];
    rows[1] = rows[1] + _o.rows[1];
    rows[2] = rows[2] + _o.rows[2];
    return *this;
}

template <typename T>
Matrix3<T>& Matrix3<T>::operator-=(Matrix3 const& _o)
{
    rows[0] = rows[0] - _o.rows[0];
    rows[1] = rows[1] - _o.rows[1];
    rows[2] = rows[2] - _o.rows[2];
    return *this;
}

template <typename T>
Matrix3<T> Matrix3<T>::operator*(T _scalar) const
{
    Matrix3<T> result;
    result.rows[0] = rows[0] * _scalar;
    result.rows[1] = rows[1] * _scalar;
    result.rows[2] = rows[2] * _scalar;
    return result;
}

template <typename T>
Vector3<T> Matrix3<T>::operator*(Vector3<T> const& _v) const
{
    Vector3<T> result;
    result.x = m00 * _v.x + m01 * _v.y + m02 * _v.z;
    result.y = m10 * _v.x + m11 * _v.y + m12 * _v.z;
    result.z = m20 * _v.x + m21 * _v.y + m22 * _v.z;
    return result;
}

template <typename T>
Matrix3<T> Matrix3<T>::operator*(Matrix3 const& _o) const
{
    Matrix3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            result[i][j] =
                (*this)[i][0] * _o[0][j] +
                (*this)[i][1] * _o[1][j] +
                (*this)[i][2] * _o[2][j];
        }
    }
    return result;
}

template <typename T>
Matrix3<T>& Matrix3<T>::operator*=(T _scalar)
{
    rows[0] = rows[0] * _scalar;
    rows[1] = rows[1] * _scalar;
    rows[2] = rows[2] * _scalar;
    return *this;
}

template <typename T>
Matrix3<T>& Matrix3<T>::operator*=(Matrix3 const& _o)
{
    Matrix3<T> result;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            result[i][j] =
                (*this)[i][0] * _o[0][j] +
                (*this)[i][1] * _o[1][j] +
                (*this)[i][2] * _o[2][j];
        }
    }
    
    rows[0] = result.rows[0];
    rows[1] = result.rows[1];
    rows[2] = result.rows[2];
    
    return *this; 
}

template <typename T>
Matrix3<T> Matrix3<T>::Transposed() const
{
    Matrix3<T> result;
    result.rows[0] = Vector3<T>(m00, m10, m20);
    result.rows[1] = Vector3<T>(m01, m11, m21);
    result.rows[2] = Vector3<T>(m02, m12, m22);
    return result;
}

template <typename T>
Matrix3<T>& Matrix3<T>::SelfTranspose()
{
    Matrix3<T> result;
    result.rows[0] = Vector3<T>(m00, m10, m20);
    result.rows[1] = Vector3<T>(m01, m11, m21);
    result.rows[2] = Vector3<T>(m02, m12, m22);
    
    rows[0] = result.rows[0];
    rows[1] = result.rows[1];
    rows[2] = result.rows[2];
    return *this;
}

template <typename T>
float Matrix3<T>::FrobeniusNorm() const
{
    float sum = 0.0f;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            sum += rows[i][j] * rows[i][j];
        }
    }
    
    return MathUtils::Sqrt(sum);
}

template <typename T>
float Matrix3<T>::Determinant() const
{
    auto Det = [](float _a, float _b, float _c, float _d) -> float
    {
        return _a * _d - _b * _c;
    };
    
    return  m00 * Det(m11, m12, m21, m22) -
            m01 * Det(m10, m12, m20, m22) +
            m02 * Det(m10, m11, m20, m21);
}

template <typename T>
float Matrix3<T>::Minor(int _r, int _c) const
{
    T sub[2][2];
    int sRowIndex = 0;
    
    for (int i = 0; i < 3; i++)
    {
        if (i == _r) continue;
        int sColIndex = 0;
        for (int j = 0; j < 3; j++)
        {
            if (j == _c) continue;
            sub[sRowIndex][sColIndex] = rows[i][j];
            sColIndex++;
        }
        sRowIndex++;
    }
    
    return sub[0][0] * sub[1][1] - sub[0][1] * sub[1][0];
}

template <typename T>
Matrix3<T> Matrix3<T>::Comatrix() const
{
    Matrix3 result;
    for (int r = 0; r < 3; r++)
    {
        for (int c = 0; c < 3; c++)
        {
            T sign = ((r + c) % 2 == 0) ? T(1) : T(-1);
            result[r][c] = sign * Minor(r, c);
        }   
    }
    return result;
}

template <typename T>
Matrix3<T> Matrix3<T>::Inverted() const
{
    float det = Determinant();
    assert(det != T(0) && "Matrix is not invertible");
    
    float invDet = 1.0f / det;
    Matrix3<T> result = Comatrix() * invDet;
    return result;
}

template <typename T>
Matrix3<T>& Matrix3<T>::SelfInvert()
{
    float det = Determinant();
    assert(det != T(0) && "Matrix is not invertible");
    
    float invDet = 1.0f / det;
    Matrix3<T> result = Comatrix() * invDet;
    
    rows[0] = result[0];
    rows[1] = result[1];
    rows[2] = result[2];
    return *this;
}

template <typename T>
Matrix4<T> Matrix3<T>::ToMatrix4() const
{
    return {
        { m00, m01, m02, T(0) },
        { m10, m11, m12, T(0) },
        { m20, m21, m22, T(0) },
        { T(0), T(0), T(0), T(1) }
    };
}

template <typename T>
Quaternion Matrix3<T>::ToQuaternion() const
{
    return Quaternion::FromRotationMatrix(*this);
}

template <typename T>
float Matrix3<T>::Determinant(Matrix3 const& _m)
{
    return _m.Determinant();
}

template <typename T>
Matrix3<T> Matrix3<T>::Transpose(Matrix3 const& _m)
{
    return _m.Transposed();
}

template <typename T>
Matrix3<T> Matrix3<T>::Invert(Matrix3 const& _m)
{
    return _m.Inverted();
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeScale(Vector3<T> const& _v)
{
    return {
        {_v.x, T(0), T(0) },
        {T(0), _v.y, T(0) },
        {T(0), T(0), _v.z }
    };
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeRotation(Vector3<T> const& _axis, T _angle)
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
        { x2 * ic + c, x * y * ic + z * s, x * z * ic - y * s },
        { x * y * ic - z * s, y2 * ic + c, y * z * ic + x * s },
        { x * z * ic + y * s, y * z * ic - x * s, z2 * ic + c }
    };
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeRotationX(T _angle)
{
    return {
            {T(1),                    T(0),                   T(0) },
              {T(0),  MathUtils::Cos(_angle), MathUtils::Sin(_angle) },
              {T(0), -MathUtils::Sin(_angle), MathUtils::Cos(_angle) }
    };
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeRotationY(T _angle)
{
    return {
            { MathUtils::Cos(_angle), T(0), -MathUtils::Sin(_angle) },
              {                   T(0), T(1),                    T(0) },
              { MathUtils::Sin(_angle), T(0),  MathUtils::Cos(_angle) }
    };
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeRotationZ(T _angle)
{
    return {
            {  MathUtils::Cos(_angle), MathUtils::Sin(_angle), T(0) },
              { -MathUtils::Sin(_angle), MathUtils::Cos(_angle), T(0) },
              {                    T(0),                   T(0), T(1) }
    };
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeRotationZYX(T _angleZ, T _angleY, T _angleX)
{
    float cX = MathUtils::Cos(_angleX);
    float sX = MathUtils::Sin(_angleX);
    float cY = MathUtils::Cos(_angleY);
    float sY = MathUtils::Sin(_angleY);
    float cZ = MathUtils::Cos(_angleZ);
    float sZ = MathUtils::Sin(_angleZ);
    
    return {
            {  cY*cZ, cX*sZ + sX*sY*cZ, sX*sZ - cX*sY*cZ },
            { -cY*sZ, cX*cZ - sX*sY*sZ, sX*cZ + cX*sY*sZ },
            {     sY,           -sX*cY,            cX*cY }
    };
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeRotationXYZ(T _angleX, T _angleY, T _angleZ)
{
    float cX = MathUtils::Cos(_angleX);
    float sX = MathUtils::Sin(_angleX);
    float cY = MathUtils::Cos(_angleY);
    float sY = MathUtils::Sin(_angleY);
    float cZ = MathUtils::Cos(_angleZ);
    float sZ = MathUtils::Sin(_angleZ);
    
    return {
                {            cY*cZ,            cY*sZ,   -sY },
                { sX*sY*cZ - cX*sZ, sX*sY*sZ + cX*cZ, sX*cY },
                { cX*sY*cZ + sX*sZ, cX*sY*sZ - sX*cZ, cX*cY },
    };
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeRotationYPR(T _yawY, T _pitchX, T _rollZ)
{
    float cX = MathUtils::Cos(_pitchX);
    float sX = MathUtils::Sin(_pitchX);
    float cY = MathUtils::Cos(_yawY);
    float sY = MathUtils::Sin(_yawY);
    float cZ = MathUtils::Cos(_rollZ);
    float sZ = MathUtils::Sin(_rollZ);
    
    return {
        { cY*cZ - sX*sY*sZ, cY*sZ + sX*sY*cZ, -cX*sY },
        {           -cX*sZ,            cX*cZ,     sX },
        { sY*cZ + sX*cY*sZ, sY*sZ - sX*cY*cZ,  cX*cY }
    };
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeRotationRPY(T _rollZ, T _pitchX, T _yawY)
{
    float cX = MathUtils::Cos(_pitchX);
    float sX = MathUtils::Sin(_pitchX);
    float cY = MathUtils::Cos(_yawY);
    float sY = MathUtils::Sin(_yawY);
    float cZ = MathUtils::Cos(_rollZ);
    float sZ = MathUtils::Sin(_rollZ);
    
    return {
        {  cY*cZ + sX*sY*sZ, cX*sZ, -sY*cZ + sX*cY*sZ },
        { -cY*sZ + sX*sY*cZ, cX*cZ,  sY*sZ + sX*cY*cZ },
        {             cX*sY,   -sX,             cX*cY }
    };
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeRotationQuat(Quaternion const& _quat)
{
    float x = _quat.x;
    float y = _quat.y;
    float z = _quat.z;
    float w = _quat.w;
    
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    
    return {
            { 1.0f - 2*y2 - 2*z2, 2*x*y + 2*z*w, 2*x*z - 2*y*w },
            { 2*x*y - 2*z*w, 1.0f - 2*x2 - 2*z2, 2*y*z + 2*x*w },
            { 2*x*z + 2*y*w, 2*y*z - 2*x*w, 1.0f - 2*x2 - 2*y2 }
    };
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeRotationAxisAngle(Vector3<T> const& _axis, T _angle)
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
        { c + x2*(1 - c), x*y*(1 - c) + z*s, x*z*(1 - c) - y*s },
        { x*y*(1 - c) - z*s, c + y2*(1 - c), y*z*(1 - c) + x*s },
        { x*z*(1 - c) + y*s, y*z*(1 - c) - x*s, c + z2*(1 - c) }
    };
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeVectorRotation(Vector3<T> const& _start, Vector3<T> const& _end)
{
    Vector3<T> a = _start.Normalized();
    Vector3<T> b = _end.Normalized();

    float dot = Vector3<T>::Dot(a, b);

    if ( 1.0f - MathUtils::EPSILON < dot )
        return Matrix3<T>::Identity();

    if ( -1.0f + MathUtils::EPSILON > dot )
    {
        Vector3<T> axis = Vector3<T>(1, 0, 0) ^ a;
        if ( axis.LengthSquared() < MathUtils::EPSILON * MathUtils::EPSILON )
            axis = Vector3<T>(0, 1, 0) ^ a;

        axis.SelfNormalize();
        return MakeRotationAxisAngle(axis, MathUtils::PI);
    }

    Vector3<T> axis = a ^ b;
    float s = axis.Length();

    Matrix3 K = MakeCrossProduct(axis);

    // Rodriguez's formula
    Matrix3 R = Identity() + K + ( K * K ) * ((1 - dot) / (s*s));
    return R;
}

template <typename T>
Matrix3<T> Matrix3<T>::MakeCrossProduct(Vector3<T> const& _v1)
{
    return {
        {   T(0),  _v1.z, -_v1.y },
          { -_v1.z,   T(0),  _v1.x },
          {  _v1.y, -_v1.x, T  (0) }
    };
}

template <typename T>
bool Matrix3<T>::operator==(const Matrix3& _o) const
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            if (rows[i][j] != _o.rows[i][j]) return false;
    }
    return true;
}

template <typename T>
bool Matrix3<T>::operator!=(const Matrix3& _o) const
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
            if (rows[i][j] != _o.rows[i][j]) return true;
    }
    return false;
}

template <typename T>
Vector3<T> Matrix3<T>::GetColumn(int _i) const
{
    Vector3<T> res;
    _i = _i % 3;

    for (int i = 0; i < 3; i++)
    {
        res[i] = rows[i][_i];
    }
    return res;
}

template <typename T>
Vector3<T> const& Matrix3<T>::operator[](int _i) const
{
    return rows[_i];
}

template <typename T>
Vector3<T>& Matrix3<T>::operator[](int _i)
{
    return rows[_i];
}

template <typename T>
T* Matrix3<T>::Data()
{
    return &m00;
}

template <typename T>
T const* Matrix3<T>::Data() const
{
    return &m00;
}

template <typename T>
std::ostream& operator<<(std::ostream& _os, Matrix3<T> const& _m)
{
    return _os << "\n" <<
        "| " << _m[0][0] << ", " << _m[0][1] << ", " << _m[0][2] << " | " << "\n" <<
        "| " << _m[1][0] << ", " << _m[1][1] << ", " << _m[1][2] << " | " << "\n" <<
        "| " << _m[2][0] << ", " << _m[2][1] << ", " << _m[2][2] << " | " << "\n";
}

#endif
