#ifndef CAMERA_H_DEFINED
#define CAMERA_H_DEFINED

#include "../../Common/Common.h"

struct Plane
{
    float a, b, c, d;
};

struct PassData
{
    XMFLOAT4X4 viewProj;
    XMFLOAT4X4 view;
    XMFLOAT4X4 proj;
    XMFLOAT4X4 invViewProj;

    XMFLOAT3   eyePosW;
    float               nearZ;
    
    float               farZ;
    float               totalTime;
    float               deltaTime;
    float               pad0;

    XMFLOAT2   renderTargetSize;
    XMFLOAT2   invRenderTargetSize;
};

class Camera 
{
public:
    float fov = 0.25f * MathHelper::Pi;
    float nearPlane = 1.0f;
    float farPlane = 1000.0f;
    
    Camera();
    ~Camera();
    bool IsInFrustum(BoundingBox const& _b, XMFLOAT4X4 _world);
    
    void SetWorld(XMFLOAT4X4& _world);
    void SetProj(float _aspectRatio);
    void UpdateMatrices();

    // TODO REMOVE
    void SetRotation(XMFLOAT3 ypr);
    void LookAt(XMFLOAT3 _target);
    void SetPos(XMFLOAT3 _pos);
    
private:
    XMFLOAT4X4 m_world = MathHelper::Identity4x4();
    XMFLOAT4X4 m_view = MathHelper::Identity4x4();
    XMFLOAT4X4 m_proj = MathHelper::Identity4x4();
    XMFLOAT4X4 m_viewProj = MathHelper::Identity4x4();
    
    XMFLOAT3 m_pos = {0.0f, 0.0f, 0.0f};

    Plane m_frustum[6];
    
    void FillData(PassData* _passData);

    void CalculateFrustum();
    Plane MakePlane(XMFLOAT4& _a, XMFLOAT4 _b);
    
    friend class Device;
    friend class D3D12Device;
};

#endif
