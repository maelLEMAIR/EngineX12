#include "Transform.h"

Transform::Transform()
{
    SetIdentity();
    m_dirty = 0;
}

Transform::Transform(Transform const& _o) :
    m_pos(_o.m_pos), m_scale(_o.m_scale),
    m_quat(_o.m_quat), m_rotMatrix(_o.m_rotMatrix),
    m_matrix(_o.m_matrix),
    m_invMatrix(_o.m_invMatrix),
    m_dirty(_o.m_dirty)
{
    
}

Transform::Transform(Transform&& _o) noexcept :
    m_pos(_o.m_pos), m_scale(_o.m_scale),
    m_quat(_o.m_quat), m_rotMatrix(_o.m_rotMatrix),
    m_matrix(_o.m_matrix),
    m_invMatrix(_o.m_invMatrix),
    m_dirty(_o.m_dirty)
{
    
}

Transform& Transform::operator=(Transform const& _o) 
{
    if (this == &_o)
        return *this;

    m_pos = _o.m_pos;
    m_scale = _o.m_scale;
    m_quat = _o.m_quat;
    m_rotMatrix = _o.m_rotMatrix;
    m_matrix = _o.m_matrix;
    m_invMatrix = _o.m_invMatrix;
    m_dirty = _o.m_dirty;

    return *this;
}

Transform& Transform::operator=(Transform&& _o) noexcept
{
    if (this == &_o)
        return *this;

    m_pos = _o.m_pos;
    m_scale = _o.m_scale;
    m_quat = _o.m_quat;
    m_rotMatrix = _o.m_rotMatrix;
    m_matrix = _o.m_matrix;
    m_invMatrix = _o.m_invMatrix;
    m_dirty = _o.m_dirty;


    return *this;
}

void Transform::SetIdentity()
{
    m_pos = { 0.0f, 0.0f, 0.0f };
    m_scale = { 1.0f, 1.0f, 1.0f };
    m_quat = Quaternion();

    m_matrix = Mat4f32::Identity();
    m_invMatrix = Mat4f32::Identity();

    ResetRotation();
}

void Transform::UpdateMatrix()
{
    if (GetDirtyState(World))
    {
        m_matrix = Mat4f32::MakeTransform(m_pos, m_scale, m_quat);
        RemoveFlag(World);
    }
    
    if (GetDirtyState(RotationScale))
    {
        Mat3f32 rotScale = GetRotMatrix();
        for (int i = 0; i < 3; i++)
        {
            rotScale.rows[i] *= m_scale[i];
            m_matrix.rows[i] = Vect4f32(rotScale.rows[i], 0);
        }
        
        RemoveFlag(RotationScale);
    }
    
    if (GetDirtyState(Position))
    {
        m_matrix.rows[3] = Vect4f32(m_pos, 1);
        RemoveFlag(Position);   
    }
}

void Transform::UpdateInvMatrix()
{
    if (IsWorldDirty() == true)
        UpdateMatrix();
    
    m_invMatrix = Mat4f32::Invert(m_matrix);
    RemoveFlag(Inverse);
}

bool Transform::IsWorldDirty() const
{
   return GetDirtyState(RotationScale) || GetDirtyState(Position);
}

bool Transform::IsInverseDirty() const
{
    return GetDirtyState(Inverse);
}

Mat4f32 const& Transform::GetMatrix()
{
    if (IsWorldDirty() == true)
        UpdateMatrix();

    return m_matrix;
}

Mat4f32 const& Transform::GetInvMatrix()
{
    if (IsInverseDirty() == true)
        UpdateInvMatrix();

    return m_invMatrix;
}

Mat4f32 Transform::UpdateFromParent(Mat4f32 const& _p)
{
    return GetMatrix() * _p;
}

void Transform::AddFlag(uint8 _flag)
{
    m_dirty |= _flag;
}

void Transform::RemoveFlag(uint8 _flag)
{
    m_dirty &= ~_flag;
}

bool Transform::GetDirtyState(uint8 _flag) const
{
    return (m_dirty & _flag) == _flag;   
}

#pragma region TRANSLATION

Vect3f32 const& Transform::GetPosition()
{
    return m_pos;
}

void Transform::SetPosition(Vect3f32 const& _position)
{
    m_pos = _position;
    AddFlag(Position | Inverse);
}

void Transform::Move(Vect3f32 const& _delta)
{
    m_pos += _delta;
    AddFlag(Position | Inverse);
}

void Transform::Move(Vect3f32 const& _dir, float _distance)
{
    m_pos += _dir * _distance;
    AddFlag(Position | Inverse);
}

#pragma endregion

#pragma region SCALE

Vect3f32 const& Transform::GetScale()
{
    return m_scale;
}

void Transform::SetScale(Vect3f32 const& _scale)
{
    m_scale = _scale;
    AddFlag(RotationScale | Inverse);
}

void Transform::SetScale(float _scale)
{
    m_scale = Vect3f32(_scale);
    AddFlag(RotationScale | Inverse);
}

void Transform::Scale(Vect3f32 const& _scale)
{
    m_scale *= _scale;
    AddFlag(RotationScale | Inverse);
}

void Transform::Scale(float _scale)
{
    m_scale *= _scale;
    AddFlag(RotationScale | Inverse);
}

#pragma endregion 

#pragma region ROTATION

Quaternion const& Transform::GetRotation()
{
    return m_quat;
}

Vect3f32 Transform::GetEulerAngles()
{
    return m_quat.ToEulerAngles();
}

Vect3f32 const& Transform::GetRight()
{
    if ( GetDirtyState(RotationMatrix) == true )
        UpdateRotationFromQuaternion();

    return m_rotMatrix.rows[0];
}

Vect3f32 const& Transform::GetUp()
{
    if ( GetDirtyState(RotationMatrix) == true )
        UpdateRotationFromQuaternion();

    return m_rotMatrix.rows[1];
}

Vect3f32 const& Transform::GetForward()
{
    if ( GetDirtyState(RotationMatrix) == true )
        UpdateRotationFromQuaternion();

    return m_rotMatrix.rows[2];
}

Mat3f32 const& Transform::GetRotMatrix()
{
    if ( GetDirtyState(RotationMatrix) == true )
        UpdateRotationFromQuaternion();

    return m_rotMatrix;
}

void Transform::SetRotationMatrix(Mat3f32 const& _rotation)
{
    m_rotMatrix = _rotation;
    m_quat = m_rotMatrix.ToQuaternion();

    AddFlag(RotationScale | Inverse);
}

void Transform::SetRotationQuaternion(Quaternion const& _quat)
{
    m_quat = _quat;

    AddFlag(RotationMatrix | RotationScale | Inverse);
}

void Transform::SetYPR(Vect3f32 const& _ypr)
{
    m_quat = Quaternion::MakeYPR(_ypr.x, _ypr.y, _ypr.z);

    AddFlag(RotationMatrix | RotationScale | Inverse);
}

void Transform::AddYPR(Vect3f32 const& _ypr)
{
    m_quat *= Quaternion::MakeYPR(_ypr.x, _ypr.y, _ypr.z);
    AddFlag(RotationMatrix | RotationScale | Inverse);
}

void Transform::UpdateRotationFromQuaternion()
{
    m_rotMatrix = m_quat.ToMatrix3();
    RemoveFlag(RotationMatrix);
}

void Transform::ResetRotation()
{
    m_quat = Quaternion();

    m_rotMatrix = Mat3f32::Identity();
}

#pragma endregion

void Transform::LookAt(Vect3f32 const& _target, Vect3f32 const& _up)
{
    Vect3f32 up = _up;
    Vect3f32 forward = (_target - m_pos).Normalized();
    
    if (MathUtils::Abs(Vect3f32::Dot(forward, _up)) > 1.0f - MathUtils::EPSILON)
        up = Vect3f32(0.0f, 0.0f, 1.0f);
    
    Vect3f32 right = (up ^ forward).Normalized();
    up = forward ^ right;
    
    m_rotMatrix.rows[0] = right;
    m_rotMatrix.rows[1] = up;
    m_rotMatrix.rows[2] = forward;
    m_quat = m_rotMatrix.ToQuaternion();
    
    RemoveFlag(RotationMatrix);
    AddFlag(RotationScale | Inverse);
}

void Transform::LookTo(Vect3f32 const& _dir, Vect3f32 const& _up)
{
    Vect3f32 up = _up;
    Vect3f32 forward = _dir.Normalized();
    
    if (MathUtils::Abs(Vect3f32::Dot(forward, _up)) > 1.0f - MathUtils::EPSILON)
        up = Vect3f32(0.0f, 0.0f, 1.0f);
    
    Vect3f32 right = (up ^ forward).Normalized();
    up = forward ^ right;
    
    m_rotMatrix.rows[0] = right;
    m_rotMatrix.rows[1] = up;
    m_rotMatrix.rows[2] = forward;
    m_quat = m_rotMatrix.ToQuaternion();
    
    RemoveFlag(RotationMatrix);
    AddFlag(RotationScale | Inverse);
}
