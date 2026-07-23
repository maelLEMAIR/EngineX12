#ifndef QUATERNION_H_DEFINED
#define QUATERNION_H_DEFINED
#include <initializer_list>
#include <iosfwd>

template <typename T>
class Vector4;
template <typename T>
class Vector3;
template <typename T>
class Matrix4;
template <typename T>
class Matrix3;

class Quaternion
{
public:
    union
    {
        struct
        {
            float x, y, z, w;
        };
        
        float Data[4];
    };
    
    Quaternion();
    Quaternion(float _x, float _y, float _z, float _w);
    Quaternion(Vector4<float> const& _v);
    Quaternion(std::initializer_list<float> _l);
    
    static Quaternion FromEulerAngles(float _yaw, float _pitch, float _roll);
    static Quaternion FromEulerAngles(Vector3<float> const& _angles);
    static Quaternion FromAxisAngle(Vector3<float> const& _axis, float _angle);
    static Quaternion FromRotationMatrix(Matrix3<float> const& _rotMat);
    static Quaternion FromRotationMatrix(Matrix4<float> const& _rotMat);

    static Quaternion MakeXYZ(float _x, float _y, float _z);
    static Quaternion MakeZYX(float _x, float _y, float _z);
    static Quaternion MakeYPR(float _yawY, float _pitchX, float _rollZ);
    static Quaternion MakeRPY(float _rollZ, float _pitchX, float _yawY);
    
    Quaternion operator+(Quaternion const& _q) const;
    Quaternion operator-(Quaternion const& _q) const;
    Quaternion operator*(Quaternion const& _q) const;
    
    Quaternion& operator+=(Quaternion const& _q);
    Quaternion& operator-=(Quaternion const& _q);
    Quaternion& operator*=(Quaternion const& _q);
    
    float Dot(Quaternion const& _q) const;
    
    float           Length() const;
    float           LengthSquared() const;
    
    Quaternion      Conjugate() const;
    
    Quaternion      Inverted() const;
    Quaternion&     SelfInvert();
    
    Quaternion      Normalized() const;
    Quaternion&     SelfNormalize();
    Quaternion      SafeNormalized() const;
    Quaternion&     SafeSelfNormalize();
    
    Matrix3<float>  ToMatrix3();
    Matrix4<float>  ToMatrix4();
    Vector4<float>  ToAxisAngle();
    Vector3<float>  ToEulerAngles();
    
    static float        Dot(Quaternion const& _q1, Quaternion const& _q2);
    
    static float        Length(Quaternion const& _q);
    static float        LengthSquared(Quaternion const& _q);
    
    static Quaternion   Conjugate(Quaternion const& _q);
    static Quaternion   Invert(Quaternion const& _q);
    static Quaternion   Normalize(Quaternion const& _q);
    static Quaternion   SafeNormalize(Quaternion const& _q);
    
    bool operator==(Quaternion const& _q) const;
    bool operator!=(Quaternion const& _q) const;
};

std::ostream& operator<<(std::ostream& _os, Quaternion const& _q);

#endif
