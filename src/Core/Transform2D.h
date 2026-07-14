#ifndef TRANSFORM2D_H_DEFINED
#define TRANSFORM2D_H_DEFINED

#include <DirectXMath.h>
using namespace DirectX;

#include "define.h"

enum class DIRTY_FLAG_2D : uint32
{
    WORLD   = 0b00001,
    INVERSE = 0b00010,
    POS     = 0b00100,
    SCALE   = 0b01000,
    ROTATE  = 0b10000,

    ALL     = 0b11111
};

class Transform2D
{
public:
    Transform2D();
    ~Transform2D() = default;

    Transform2D(const Transform2D& other);
    Transform2D(Transform2D&& other) noexcept;
    Transform2D& operator=(const Transform2D& other);
    Transform2D& operator=(Transform2D&& other) noexcept;

    XMFLOAT4X4& GetMatrix();

    void SetIdentity();
    void UpdateMatrix();
    void UpdateFromParent(Transform2D const& parent);

    uint32 GetDirty() const { return dirty; }

    XMFLOAT2& GetPosition()         { return pos; }
    void SetPosition(XMFLOAT2 const& position);
    void Move(XMFLOAT2 const& delta);
    void Move(XMFLOAT2 const& dir, float distance);

    const XMFLOAT2& GetScale() const { return scale; }
    void SetScale(XMFLOAT2 const& _scale);
    void SetScale(float _scale);
    void Scale(XMFLOAT2 const& _scale);
    void Scale(float _scale);

    float GetRotation() const { return angle; }
    XMFLOAT2 GetRight()   const { return { cosf(angle),  sinf(angle) }; }
    XMFLOAT2 GetUp()      const { return { -sinf(angle), cosf(angle) }; }

    void SetRotation(float _angle);
    void Rotate(float _delta);
    void ResetRotation();

private:
    XMFLOAT2   pos   = { 0.0f, 0.0f };
    XMFLOAT2   scale = { 1.0f, 1.0f };
    float      angle = 0.0f;
    
    XMFLOAT4X4 matrix;

    uint32 dirty = 0;
};

#endif
