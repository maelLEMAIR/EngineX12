#ifndef CAMERA_CPP_DEFINED
#define CAMERA_CPP_DEFINED

#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{

}

void Camera::SetWorld(Mat4f32 const& _world)
{
    m_world = _world;
    UpdateMatrices();
}

void Camera::SetFov(float _fov)
{
    m_fov = _fov;
    m_proj = Mat4f32::MakePerspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
    UpdateMatrices();
}

void Camera::SetNearDistance(float _nearPlane)
{
    m_nearPlane = _nearPlane;
    m_proj = Mat4f32::MakePerspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
    UpdateMatrices();
}

void Camera::SetFarDistance(float _farPlane)
{
    m_farPlane = _farPlane;
    m_proj = Mat4f32::MakePerspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
    UpdateMatrices();
}

void Camera::SetAspectRatio(float _aspectRatio)
{
    m_aspectRatio = _aspectRatio;
    m_proj = Mat4f32::MakePerspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
    UpdateMatrices();
}

float Camera::GetFov() const
{
    return m_fov;
}

float Camera::GetNearDistance() const
{
    return m_nearPlane;
}

float Camera::GetFarDistance() const
{
    return m_farPlane;
}

float Camera::GetAspectRatio() const
{
    return m_aspectRatio;
}

void Camera::UpdateMatrices()
{
    m_view = m_world.Inverted();

    m_viewProj = m_view * m_proj;
    m_frustum = Frustum(m_viewProj);
    m_viewProj.SelfTranspose(); // Transposed for the hlsl
    
    m_pos = {m_world.m30, m_world.m31, m_world.m32};
}

Frustum const& Camera::GetFrustum()
{
    return m_frustum;
}

void Camera::FillData(PassData* _passData)
{
    // TODO avoid redundant recomputation of the matrices (inverse)
    _passData->viewProj = m_viewProj;
    _passData->view = m_view;
    _passData->proj = m_proj;

    _passData->invViewProj = m_viewProj.Inverted();

    _passData->eyePosW = m_pos;

    _passData->nearZ = m_nearPlane;
    _passData->farZ = m_farPlane;
}

#endif
