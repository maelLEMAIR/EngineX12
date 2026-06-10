#ifndef CAMERA_CPP_DEFINED
#define CAMERA_CPP_DEFINED

#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{

}

bool Camera::IsInFrustum(BoundingBox const& _b, XMFLOAT4X4 _world)
{
    XMFLOAT3 localMin = { _b.Center.x - _b.Extents.x, _b.Center.y - _b.Extents.y, _b.Center.z - _b.Extents.z };
    XMFLOAT3 localMax = { _b.Center.x + _b.Extents.x, _b.Center.y + _b.Extents.y, _b.Center.z + _b.Extents.z };

    XMMATRIX world = XMLoadFloat4x4(&_world);

    XMFLOAT3 corners[8] = {
        { localMin.x, localMin.y, localMin.z },
        { localMax.x, localMin.y, localMin.z },
        { localMin.x, localMax.y, localMin.z },
        { localMax.x, localMax.y, localMin.z },
        { localMin.x, localMin.y, localMax.z },
        { localMax.x, localMin.y, localMax.z },
        { localMin.x, localMax.y, localMax.z },
        { localMax.x, localMax.y, localMax.z },
    };

    XMFLOAT3 worldMin = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
    XMFLOAT3 worldMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (auto& corner : corners)
    {
        XMVECTOR v = XMVector3TransformCoord(XMLoadFloat3(&corner), world);
        XMFLOAT3 vf;
        XMStoreFloat3(&vf, v);

        worldMin.x = min(worldMin.x, vf.x);
        worldMin.y = min(worldMin.y, vf.y);
        worldMin.z = min(worldMin.z, vf.z);
        worldMax.x = max(worldMax.x, vf.x);
        worldMax.y = max(worldMax.y, vf.y);
        worldMax.z = max(worldMax.z, vf.z);
    }

    for (int i = 0; i < 6; i++)
    {
        XMFLOAT3 pv = {
            m_frustum[i].a >= 0 ? worldMax.x : worldMin.x,
            m_frustum[i].b >= 0 ? worldMax.y : worldMin.y,
            m_frustum[i].c >= 0 ? worldMax.z : worldMin.z,
        };

        float dot = m_frustum[i].a * pv.x + m_frustum[i].b * pv.y + m_frustum[i].c * pv.z + m_frustum[i].d;

        if (dot < 0) return false;
    }

    return true;
}

void Camera::SetWorld(XMFLOAT4X4& _world)
{
    m_world = _world;
    UpdateMatrices();
}

void Camera::SetProj(float _aspectRatio)
{
    XMMATRIX P = XMMatrixPerspectiveFovLH(fov, _aspectRatio, nearPlane, farPlane);
    XMStoreFloat4x4(&m_proj, P);

    UpdateMatrices();
}

void Camera::UpdateMatrices()
{
    XMMATRIX w = XMLoadFloat4x4(&m_world);
    XMMATRIX v = XMMatrixInverse(nullptr, w);
    XMMATRIX p = XMLoadFloat4x4(&m_proj);
    
    XMStoreFloat4x4(&m_view, v);

    XMMATRIX vp = XMMatrixMultiply(v, p);
    vp = XMMatrixTranspose(vp);

    XMStoreFloat4x4(&m_viewProj, vp);

    m_pos = {m_world._41, m_world._42, m_world._43};

    CalculateFrustum();
}

void Camera::SetRotation(XMFLOAT3 ypr)
{
    XMMATRIX rot = XMMatrixRotationRollPitchYaw(ypr.y, ypr.x, ypr.z);

    XMMATRIX world = rot;
    world.r[3] = XMVectorSet(m_pos.x, m_pos.y, m_pos.z, 1.0f);

    XMStoreFloat4x4(&m_world, world);

    UpdateMatrices();
}

void Camera::LookAt(XMFLOAT3 _target)
{
    XMVECTOR posV    = XMLoadFloat3(&m_pos);
    XMVECTOR targetV = XMLoadFloat3(&_target);

    // Guard against degenerate input
    XMVECTOR dir = XMVectorSubtract(targetV, posV);
    if (XMVector3Equal(dir, XMVectorZero()))
        return;

    XMFLOAT3 up = { 0.0f, 1.0f, 0.0f };
    XMMATRIX view = XMMatrixLookAtLH(posV, targetV, XMLoadFloat3(&up));

    XMMATRIX world = XMMatrixInverse(nullptr, view);
    XMStoreFloat4x4(&m_world, world);

    UpdateMatrices();
}

void Camera::SetPos(XMFLOAT3 _pos)
{
    m_pos = _pos;
    m_world._41 = _pos.x;
    m_world._42 = _pos.y;
    m_world._43 = _pos.z;

    UpdateMatrices();
}

void Camera::FillData(PassData* _passData)
{
    _passData->viewProj = m_viewProj;
    _passData->view = m_view;
    _passData->proj = m_proj;

    XMMATRIX temp = XMLoadFloat4x4(&m_viewProj);
    temp = XMMatrixInverse(nullptr, temp);
    XMStoreFloat4x4(&_passData->invViewProj, temp);

    _passData->eyePosW = m_pos;

    _passData->nearZ = nearPlane;
    _passData->farZ = farPlane;
}

void Camera::CalculateFrustum()
{
    XMMATRIX m = XMLoadFloat4x4(&m_viewProj);

    XMFLOAT4 r0, r1, r2, r3;
    XMStoreFloat4(&r0, m.r[0]);
    XMStoreFloat4(&r1, m.r[1]);
    XMStoreFloat4(&r2, m.r[2]);
    XMStoreFloat4(&r3, m.r[3]);

    m_frustum[0] = MakePlane(r3, r0);
    m_frustum[1] = MakePlane(r2, {-r0.x, -r0.y, -r0.z, -r0.w});
    m_frustum[2] = MakePlane(r3, r1);
    m_frustum[3] = MakePlane(r3, {-r1.x, -r1.y, -r1.z, -r1.w});
    m_frustum[4] = MakePlane(r2, {0.0f, 0.0f, 0.0f, 0.0f});
    m_frustum[5] = MakePlane(r3, {-r2.x, -r2.y, -r2.z, -r2.w});

    for (Plane& p : m_frustum)
    {
        float len = sqrtf(p.a*p.a + p.b*p.b + p.c*p.c);
        p = {p.a / len, p.b / len, p.c / len, p.d / len};
    }
}

Plane Camera::MakePlane(XMFLOAT4& _a, XMFLOAT4 _b)
{
    Plane p;
    p.a = _a.x + _b.x;
    p.b = _a.y + _b.y;
    p.c = _a.z + _b.z;
    p.d = _a.w + _b.w;
    return p;
}

#endif
