#ifndef DEMO_GEOS_HPP_DEFINED
#define DEMO_GEOS_HPP_DEFINED

#include "Test.h"
#include "../Render/Generic/Render.h"

class DemoGeos : public Test
{
    static void SetPosition(XMFLOAT3 position, XMFLOAT4X4& matrix)
    {
        matrix._41 = position.x;
        matrix._42 = position.y;
        matrix._43 = position.z;
    }
    
public: 
    static void Run()
    {
        Window window(1080, 720, L"Test", true);
        window.InitD3D12();

        Device* pDevice = window.GetDevice();
        
        Shader* coloredS = ShaderFactory::CreateLitColored(pDevice);

        Material* orange = coloredS->CreateMaterial();
        orange->SetFloat4("DiffuseAlbedo", XMFLOAT4(1.0f, 0.5f, 0.0f, 1.0f));
        orange->SetFloat("Roughness", 0.2f);

        Material* green = coloredS->CreateMaterial();
        green->SetFloat4("DiffuseAlbedo", XMFLOAT4(0.0f, 0.7f, 0.0f, 1.0f));
        green->SetFloat("Roughness", 0.2f);

        Material* blue = coloredS->CreateMaterial();
        blue->SetFloat4("DiffuseAlbedo", XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f));
        blue->SetFloat("Roughness", 0.2f);

        Material* purple = coloredS->CreateMaterial();
        purple->SetFloat4("DiffuseAlbedo", XMFLOAT4(0.7f, 0.0f, 0.7f, 1.0f));
        purple->SetFloat("Roughness", 0.2f);

        Material* red = coloredS->CreateMaterial();
        red->SetFloat4("DiffuseAlbedo", XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
        red->SetFloat("Roughness", 0.2f);
        
        Geometry* Donut = GeometryFactory::BuildDonut(pDevice, 1.0f, 0.5f, 32, 16, true);
        Geometry* UVSphere = GeometryFactory::BuildUVSphere(pDevice, 16, 16);
        Geometry* IcoSphere = GeometryFactory::BuildIcosphere(pDevice, 5);
        Geometry* Cube = GeometryFactory::BuildCube(pDevice);
        Geometry* Pyramid = GeometryFactory::BuildPyramid(pDevice);
        Geometry* Cylinder = GeometryFactory::BuildCylinder(pDevice, 16);
        
        XMFLOAT4X4 donutMat = MathHelper::Identity4x4();
        SetPosition({-3.0f, 0.0f, 2.0f}, donutMat);
        XMFLOAT4X4 uvSphereMat = MathHelper::Identity4x4();
        SetPosition({-3.0f, 0.0f, -2.0f}, uvSphereMat);
        XMFLOAT4X4 IcosphereMat = MathHelper::Identity4x4();
        SetPosition({0.0f, 0.0f, 2.0f}, IcosphereMat);
        XMFLOAT4X4 CubeMat = MathHelper::Identity4x4();
        SetPosition({0.0f, 0.0f, -2.0f}, CubeMat);
        XMFLOAT4X4 PyramidMat = MathHelper::Identity4x4();
        SetPosition({3.0f, 0.0f, 2.0f}, PyramidMat);
        XMFLOAT4X4 CYlinderMat = MathHelper::Identity4x4();
        SetPosition({3.0f, 0.0f, -2.0f}, CYlinderMat);

        XMFLOAT3 axis = XMFLOAT3(1.0f, 1.0f, 0.0f);
        XMMATRIX rotationMat;

        Camera cam;
        XMFLOAT3 pos = XMFLOAT3(0.0f, -10.0f, -10.0f);
        cam.SetPos(pos);
        XMFLOAT3 target = XMFLOAT3(0.0f, 0.0f, 0.0f);
        cam.LookAt(target);
        
        pDevice->SetMainCamera(&cam);

        {
            LightDescriptor dirLight = LightHelper::CreateLight(LightType::Directional);
            dirLight.light.Direction = XMFLOAT3(-0.5f, 0.0f, 0.5f);
            dirLight.light.Strength = XMFLOAT3(2.0f, 2.0f, 2.0f);
            dirLight.light.Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        
            Vector<LightDescriptor> lights = { dirLight };
            pDevice->SetLights(lights);
        }

        float angle = 0.0f;
        
        while (window.IsOpen())
        {
            window.Update();

            angle += 0.01f;
            
            XMMATRIX rot = XMMatrixRotationY(angle) * XMMatrixRotationX(angle) * XMMatrixRotationZ(angle);

            XMFLOAT4X4 donutM = MathHelper::Identity4x4();
            XMFLOAT4X4 CubeM = MathHelper::Identity4x4();
            XMFLOAT4X4 PyramidM = MathHelper::Identity4x4();
            XMFLOAT4X4 CYlinderM = MathHelper::Identity4x4();
            XMStoreFloat4x4(&donutM,    rot * XMLoadFloat4x4(&donutMat));
            XMStoreFloat4x4(&CubeM,     rot * XMLoadFloat4x4(&CubeM));
            XMStoreFloat4x4(&PyramidM,  rot * XMLoadFloat4x4(&PyramidMat));
            XMStoreFloat4x4(&CYlinderM, rot * XMLoadFloat4x4(&CYlinderMat));
            
            window.Clear();
            
            pDevice->SetMaterial(blue);
            pDevice->Draw(Donut, donutM);
            pDevice->SetMaterial(red);
            pDevice->Draw(UVSphere, uvSphereMat);
            pDevice->SetMaterial(purple);
            pDevice->Draw(IcoSphere, IcosphereMat);
            pDevice->SetMaterial(green);
            pDevice->Draw(Cube, CubeM);
            pDevice->SetMaterial(orange);
            pDevice->Draw(Pyramid, PyramidM);
            pDevice->Draw(Cylinder, CYlinderM);

            window.Display();
        }
    }
};

#endif