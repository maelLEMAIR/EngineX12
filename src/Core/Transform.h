#ifndef TRANSFORM_H_DEFINED
#define TRANSFORM_H_DEFINED

#include "define.h"

#include "Math/Matrix/Matrix.h"
#include "Math/Vector/Vector.h"
#include "Math/Quaternions/Quaternion.h"

class Transform
{
public:
    enum LocalDirty : uint8
    {
        None            = 0,
        Position        = 1 << 0,
        RotationScale   = 1 << 1,
        RotationMatrix  = 1 << 2,
        Inverse         = 1 << 3,
        World           = Position | RotationScale,
        All             = World | Inverse | RotationMatrix
    };
    
    Transform();
    ~Transform() = default;

    Transform(Transform const& _o);
    Transform(Transform&& _o) noexcept;
    Transform& operator=(Transform const& _o);
    Transform& operator=(Transform&& _o) noexcept;
    
    void SetIdentity();
    
    void UpdateMatrix();
    void UpdateInvMatrix();
    
    bool IsWorldDirty()     const;
    bool IsInverseDirty()   const;
    
    Mat4f32 const& GetMatrix();
    Mat4f32 const& GetInvMatrix();
    
    Mat4f32 UpdateFromParent(Mat4f32 const& _p);
    
    //////////////////// Pos //////////////////////////
    
    Vect3f32 const& GetPosition();
    
    void SetPosition(Vect3f32 const& _position);
    void Move(Vect3f32 const& _delta);
    void Move(Vect3f32 const& _dir, float _distance);

    ////////////////////// Scale //////////////////////
   
    Vect3f32 const& GetScale();
    
    void SetScale(Vect3f32 const& _scale);
    void SetScale(float _scale);
    void Scale(Vect3f32 const& _scale);
    void Scale(float _scale);

    ///////////////////// Rotation /////////////////////

    Quaternion const& GetRotation();
    Vect3f32 GetEulerAngles();
    
    Vect3f32 const& GetRight();
    Vect3f32 const& GetUp();
    Vect3f32 const& GetForward();

    Mat3f32 const& GetRotMatrix();

    void SetRotationMatrix(Mat3f32 const& _rotation);
    void SetRotationQuaternion(Quaternion const& _quat);

    void SetYPR(Vect3f32 const& _ypr);
    void AddYPR(Vect3f32 const& _ypr);
    
    void UpdateRotationFromQuaternion();
    void ResetRotation();

    ///////////////// Other ////////////////////////////
    
    void LookAt(Vect3f32 const& _target, Vect3f32 const& _up = Vect3f32(0.0f, 1.0f, 0.0f));
    void LookTo(Vect3f32 const& _dir, Vect3f32 const& _up = Vect3f32(0.0f, 1.0f, 0.0f));

private:
    Vect3f32 m_pos;
    Vect3f32 m_scale;

    Quaternion m_quat;
    Mat3f32 m_rotMatrix;
    
    Mat4f32 m_matrix;
    Mat4f32 m_invMatrix;

    uint8 m_dirty;

    void AddFlag(uint8 _flag);
    void RemoveFlag(uint8 _flag);
    bool GetDirtyState(uint8 _flag) const;
};

#endif
