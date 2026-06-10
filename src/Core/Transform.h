#ifndef TRANSFORM_H_DEFINED
#define TRANSFORM_H_DEFINED

#include <DirectXMath.h>
#include <type_traits>
using namespace DirectX;

#include "define.h"

enum class DIRTY_FLAG : uint32
{
    WORLD =       0b00001,
    INVERSE =     0b00010,
    POS =         0b00100,
    SCALE =       0b01000,
    ROTATE =      0b10000,

    ALL =         0b11111
};

class Transform
{
public:
    Transform();
    ~Transform() = default;

    Transform(const Transform& other);
    Transform(Transform&& other) noexcept;
    Transform& operator=(const Transform& other);
    Transform& operator=(Transform&& other) noexcept;
    
    XMFLOAT4X4& GetMatrix();
    XMFLOAT4X4& GetInvMatrix();

    void SetIdentity();
    void UpdateMatrix();
    void UpdateInvMatrix();
    void UpdateFromParent(Transform const& parent);

    uint32 GetDirty() { return dirty; }
    
    // Pos
    
    XMFLOAT3& GetPosition() { return pos; };
    
    void SetPosition(XMFLOAT3 const& position);
    void Move(XMFLOAT3 const& delta);
    void Move(XMFLOAT3 const& dir, float distance);

    // Scale
   
    const XMFLOAT3& GetScale() { return scale; };
    
    void SetScale(XMFLOAT3 const& scale);
    void SetScale(float scale);
    void Scale(XMFLOAT3 const& scale);
    void Scale(float scale);

    // Rotate

    const XMFLOAT3& GetForward()    { return forward; }
    const XMFLOAT3& GetRight()      { return right; }
    const XMFLOAT3& GetUp()         { return up; }

    XMFLOAT4& GetRotation()   { return quat; }

    XMMATRIX GetRotMatrix()   { return XMLoadFloat4x4(&rotMatrix); }

    void LookAt(XMFLOAT3 const& target);
    void LookTo(XMFLOAT3 const& dir);
    
    void SetRotationMatrix(XMFLOAT4X4 const& rotation);
    void SetRotationQuaternion(XMFLOAT4 const& quat);
    
    void ResetRotation();
    
    void SetYPR(XMFLOAT3 const& ypr);
    void AddYPR(XMFLOAT3 const& ypr);

    void UpdateRotationFromAxes();
    void UpdateRotationFromQuaternion();
    void UpdateRotationFromMatrix();
    
    XMFLOAT3 pos   = { 0.0f, 0.0f, 0.0f };
    XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };

    XMFLOAT3 forward;
    XMFLOAT3 up;
    XMFLOAT3 right;

    XMFLOAT4 quat = { 0.0f, 0.0f, 0.0f, 1.0f };
    XMFLOAT4X4 rotMatrix;
    
    XMFLOAT4X4 matrix;
    XMFLOAT4X4 invMatrix;

    uint32 dirty = 0;
};

#endif
