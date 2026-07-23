#ifndef TRANSFORM_CPP_DEFINED
#define TRANSFORM_CPP_DEFINED

#include "TransformD3D.h"


TransformD3D::TransformD3D()
{
    dirty = 0;
    SetIdentity();
    ResetRotation();
}

TransformD3D::TransformD3D(const TransformD3D& other)
        : pos(other.pos),
          scale(other.scale),
          forward(other.forward),
          up(other.up),
          right(other.right),
          quat(other.quat),
          rotMatrix(other.rotMatrix),
          matrix(other.matrix),
          invMatrix(other.invMatrix),
          dirty(other.dirty)
{
}

TransformD3D::TransformD3D(TransformD3D&& other) noexcept
    : pos(std::move(other.pos)),
      scale(std::move(other.scale)),
      forward(std::move(other.forward)),
      up(std::move(other.up)),
      right(std::move(other.right)),
      quat(std::move(other.quat)),
      rotMatrix(std::move(other.rotMatrix)),
      matrix(std::move(other.matrix)),
      invMatrix(std::move(other.invMatrix)),
      dirty(other.dirty)
{
}

TransformD3D& TransformD3D::operator=(const TransformD3D& other)
{
    if (this == &other)
        return *this;
    pos = other.pos;
    scale = other.scale;
    forward = other.forward;
    up = other.up;
    right = other.right;
    quat = other.quat;
    rotMatrix = other.rotMatrix;
    matrix = other.matrix;
    invMatrix = other.invMatrix;
    dirty = other.dirty;
    return *this;
}

TransformD3D& TransformD3D::operator=(TransformD3D&& other) noexcept
{
    if (this == &other)
        return *this;
    pos  = std::move(other.pos);
    scale = std::move(other.scale);
    forward  = std::move(other.forward);
    up       = std::move(other.up);
    right    = std::move(other.right);
    quat = std::move(other.quat);
    rotMatrix  = std::move(other.rotMatrix);
    matrix = std::move(other.matrix);
    invMatrix = std::move(other.invMatrix);
    dirty     = other.dirty;
    return *this;
}

XMFLOAT4X4& TransformD3D::GetMatrix()
{
    if (dirty & (uint8)DIRTY_FLAG::WORLD)
        UpdateMatrix();
    
    return matrix;
}

XMFLOAT4X4& TransformD3D::GetInvMatrix()
{
    if (dirty & (uint8)DIRTY_FLAG::INVERSE)
        UpdateInvMatrix();
    
    return invMatrix;
}

void TransformD3D::SetIdentity()
{
    pos    = XMFLOAT3(0.0f, 0.0f, 0.0f);
    scale  = XMFLOAT3(1.0f, 1.0f, 1.0f);
    quat   = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    ResetRotation();
    XMStoreFloat4x4(&matrix, XMMatrixIdentity());
    XMStoreFloat4x4(&invMatrix, XMMatrixIdentity());
}

void TransformD3D::UpdateMatrix()
{
    XMVECTOR p = XMLoadFloat3(&pos);
    XMVECTOR s = XMLoadFloat3(&scale);
    XMVECTOR r = XMLoadFloat4(&quat);

    XMMATRIX m = XMMatrixAffineTransformation(s, XMVectorZero(), r, p);
    XMStoreFloat4x4(&matrix, m);
    
    dirty &= ~(uint8)DIRTY_FLAG::POS;
    dirty &= ~(uint8)DIRTY_FLAG::SCALE;
    dirty &= ~(uint8)DIRTY_FLAG::ROTATE;

    dirty |= (uint8)DIRTY_FLAG::WORLD;
    dirty |= (uint8)DIRTY_FLAG::INVERSE;
}

void TransformD3D::UpdateInvMatrix()
{
    if (dirty & (uint8)DIRTY_FLAG::WORLD)
        UpdateMatrix();
    
    XMMATRIX m = XMLoadFloat4x4(&matrix);
    XMStoreFloat4x4(&invMatrix, XMMatrixInverse(nullptr, m));

    dirty &= ~(uint8)DIRTY_FLAG::INVERSE;
}

void TransformD3D::UpdateFromParent(TransformD3D const& parent)
{
    XMVECTOR parentScale = XMLoadFloat3(&parent.scale);
    XMVECTOR localScale  = XMLoadFloat3(&scale);
    XMVECTOR worldScale  = XMVectorMultiply(parentScale, localScale);
    XMStoreFloat3(&scale, worldScale);
    
    XMVECTOR parentQuat = XMLoadFloat4(&parent.quat);
    XMVECTOR localQuat  = XMLoadFloat4(&quat);
    XMVECTOR worldQuat  = XMQuaternionMultiply(localQuat, parentQuat);
    worldQuat           = XMQuaternionNormalize(worldQuat);
    XMStoreFloat4(&quat, worldQuat);
    
    XMVECTOR localPos  = XMLoadFloat3(&pos);
    XMVECTOR parentPos = XMLoadFloat3(&parent.pos);
    
    localPos = XMVectorMultiply(localPos, parentScale);
    
    localPos = XMVector3Rotate(localPos, parentQuat);
    
    XMVECTOR worldPos = XMVectorAdd(localPos, parentPos);
    XMStoreFloat3(&pos, worldPos);
    
    UpdateRotationFromQuaternion();
    dirty |= (uint8)DIRTY_FLAG::WORLD | (uint8)DIRTY_FLAG::INVERSE;
}

///////////////////////////////////////////////////////////////////////////////////////
/// POSITION ///
///////////////////////////////////////////////////////////////////////////////////////

void TransformD3D::SetPosition(XMFLOAT3 const& position)
{
    pos = position;
    dirty |= (uint8)DIRTY_FLAG::POS;
}

void TransformD3D::Move(XMFLOAT3 const& delta)
{
    pos.x += delta.x;
    pos.y += delta.y;
    pos.z += delta.z;
    
    dirty |= (uint8)DIRTY_FLAG::POS;
}

void TransformD3D::Move(XMFLOAT3 const& dir, float distance)
{
    pos.x += dir.x * distance;
    pos.y += dir.y * distance;
    pos.z += dir.z * distance;

    dirty |= (uint8)DIRTY_FLAG::POS;
}

///////////////////////////////////////////////////////////////////////////////////////
/// SCALE ///
///////////////////////////////////////////////////////////////////////////////////////

void TransformD3D::SetScale(XMFLOAT3 const& _scale)
{
    scale = _scale;
    dirty |= (uint8)DIRTY_FLAG::SCALE;
}

void TransformD3D::SetScale(float _scale)
{
    XMStoreFloat3(&scale, XMVectorReplicate(_scale));
    dirty |= (uint8)DIRTY_FLAG::SCALE;
}

void TransformD3D::Scale(XMFLOAT3 const& _scale)
{
    XMVECTOR s = XMVectorMultiply(XMLoadFloat3(&scale), XMLoadFloat3(&_scale));
    XMStoreFloat3(&scale, s);
    dirty |= (uint8)DIRTY_FLAG::SCALE;
}

void TransformD3D::Scale(float _scale)
{
    XMVECTOR s = XMVectorMultiply(XMLoadFloat3(&scale), XMVectorReplicate(_scale));
    XMStoreFloat3(&scale, s);
    dirty |= (uint8)DIRTY_FLAG::SCALE;
}

///////////////////////////////////////////////////////////////////////////////////////
/// ROTATION ///
///////////////////////////////////////////////////////////////////////////////////////

static void ExtractAxesFromMatrix(const XMFLOAT4X4& m,
                                  XMFLOAT3& right,
                                  XMFLOAT3& up,
                                  XMFLOAT3& forward)
{
    right   = XMFLOAT3(m._11, m._12, m._13);
    up      = XMFLOAT3(m._21, m._22, m._23);
    forward = XMFLOAT3(m._31, m._32, m._33);
}

void TransformD3D::LookTo(XMFLOAT3 const& dir)
{
    XMVECTOR eye = XMLoadFloat3(&pos);
    XMVECTOR to  = XMLoadFloat3(&dir);
    XMVECTOR up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookToLH(eye, to, up);
    XMMATRIX rot  = XMMatrixTranspose(view);
    
    rot.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

    XMStoreFloat4x4(&rotMatrix, rot);
    UpdateRotationFromMatrix();

    dirty |= (uint8)DIRTY_FLAG::ROTATE;
}

void TransformD3D::LookAt(XMFLOAT3 const& target)
{
    XMVECTOR eye = XMLoadFloat3(&pos);
    XMVECTOR to  = XMLoadFloat3(&target);
    XMVECTOR up  = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(eye, to, up);
    XMMATRIX rot  = XMMatrixTranspose(view);
    
    rot.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

    XMStoreFloat4x4(&rotMatrix, rot);
    UpdateRotationFromMatrix();

    dirty |= (uint8)DIRTY_FLAG::ROTATE;
}

void TransformD3D::SetRotationMatrix(XMFLOAT4X4 const& rotation)
{
    rotMatrix = rotation;
    UpdateRotationFromMatrix();

    dirty |= (uint8)DIRTY_FLAG::ROTATE;
}

void TransformD3D::SetRotationQuaternion(XMFLOAT4 const& _quat)
{
    quat = _quat;
    UpdateRotationFromQuaternion();

    dirty |= (uint8)DIRTY_FLAG::ROTATE;
}

void TransformD3D::ResetRotation()
{
    right   = XMFLOAT3(1.0f, 0.0f, 0.0f);
    up      = XMFLOAT3(0.0f, 1.0f, 0.0f);
    forward = XMFLOAT3(0.0f, 0.0f, 1.0f);

    XMStoreFloat4x4(&rotMatrix,  XMMatrixIdentity());
    XMStoreFloat4  (&quat, XMQuaternionIdentity());

    dirty |= (uint8)DIRTY_FLAG::ROTATE;
}

void TransformD3D::SetYPR(XMFLOAT3 const& ypr)
{
    ResetRotation();
    AddYPR(ypr);
}

void TransformD3D::AddYPR(const XMFLOAT3& ypr)
{
    XMVECTOR qRot = XMLoadFloat4(&quat);

    if (ypr.x != 0.0f) // yaw (Y world axis)
    {
        XMVECTOR qYaw = XMQuaternionRotationAxis(XMVectorSet(0,1,0,0), ypr.x);
        qRot = XMQuaternionMultiply(qRot, qYaw);
    }

    if (ypr.y != 0.0f) // pitch (local X axis)
    {
        XMVECTOR right  = XMVector3Rotate(XMVectorSet(1,0,0,0), qRot);
        XMVECTOR qPitch = XMQuaternionRotationAxis(right, ypr.y);
        qRot = XMQuaternionMultiply(qRot, qPitch);
    }

    if (ypr.z != 0.0f) // roll (local Z axis)
    {
        XMVECTOR qRoll = XMQuaternionRotationAxis(XMVectorSet(0,0,1,0), ypr.z);
        qRot = XMQuaternionMultiply(qRot, qRoll);
    }

    qRot = XMQuaternionNormalize(qRot);
    XMStoreFloat4(&quat, qRot);

    UpdateRotationFromQuaternion();
}

void TransformD3D::UpdateRotationFromAxes()
{
    rotMatrix._11 = right.x;   rotMatrix._12 = right.y;   rotMatrix._13 = right.z;
    rotMatrix._21 = up.x;      rotMatrix._22 = up.y;      rotMatrix._23 = up.z;
    rotMatrix._31 = forward.x; rotMatrix._32 = forward.y; rotMatrix._33 = forward.z;
    rotMatrix._14 = 0.0f; rotMatrix._24 = 0.0f; rotMatrix._34 = 0.0f;
    rotMatrix._41 = 0.0f; rotMatrix._42 = 0.0f; rotMatrix._43 = 0.0f; rotMatrix._44 = 1.0f;

    XMStoreFloat4(&quat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&rotMatrix)));

    dirty |= (uint8)DIRTY_FLAG::ROTATE;
}

void TransformD3D::UpdateRotationFromQuaternion()
{
    XMStoreFloat4x4(&rotMatrix, XMMatrixRotationQuaternion(XMLoadFloat4(&quat)));
    ExtractAxesFromMatrix(rotMatrix, right, up, forward);
    dirty |= (uint8)DIRTY_FLAG::ROTATE;
}

void TransformD3D::UpdateRotationFromMatrix()
{
    XMVECTOR rotQuat = XMQuaternionRotationMatrix(XMLoadFloat4x4(&rotMatrix));
    XMStoreFloat4(&quat, rotQuat);

    ExtractAxesFromMatrix(rotMatrix, right, up, forward);

    dirty |= (uint8)DIRTY_FLAG::ROTATE;
}

///////////////////////////////////////////////////////////////////////////////////////

#endif