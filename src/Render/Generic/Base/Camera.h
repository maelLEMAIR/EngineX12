#ifndef CAMERA_H_DEFINED
#define CAMERA_H_DEFINED

#include "../../Common/Common.h"

struct PassData
{
    Mat4f32 viewProj;
    Mat4f32 view;
    Mat4f32 proj;
    Mat4f32 invViewProj;

    Vect3f32   eyePosW;
    float               nearZ;
    
    float               farZ;
    float               totalTime;
    float               deltaTime;
    float               pad0;

    Vect2f32   renderTargetSize;
    Vect2f32   invRenderTargetSize;
};

class Camera 
{
public:
    Camera();
    ~Camera();
    
    void SetWorld(Mat4f32 const& _world);
    void SetFov(float _fov);
    void SetNearDistance(float _nearPlane);
    void SetFarDistance(float _farPlane);
    void SetAspectRatio(float _aspectRatio);
    
    float GetFov() const;
    float GetNearDistance() const;
    float GetFarDistance() const;
    float GetAspectRatio() const;
    
    void UpdateMatrices();
    Frustum const& GetFrustum();
    
private:
    Mat4f32 m_world      = Mat4f32::Identity();
    Mat4f32 m_view       = Mat4f32::Identity();
    Mat4f32 m_proj       = Mat4f32::Identity();
    Mat4f32 m_viewProj   = Mat4f32::Identity();
    
    Vect3f32 m_pos = { 0.0f, 0.0f, 0.0f };
    
    float m_fov = 0.25f * MathUtils::PI;
    float m_nearPlane = 1.0f;
    float m_farPlane = 1000.0f;
    float m_aspectRatio = 0.0f;
    
    Frustum m_frustum;
    
    void FillData(PassData* _passData);
    
    friend class Device;
    friend class D3D12Device;
};

#endif
