#ifndef DEMO_TEXTURE_HPP_DEFINED
#define DEMO_TEXTURE_HPP_DEFINED

#include "Test.h"
#include "../Render/Generic/Render.h"

class DemoTexture : public Test
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
        
        Shader* litTextured = ShaderFactory::CreateLitTextured(pDevice);

        Material* litBrick = litTextured->CreateMaterial();
        Texture* brickAlbedo = pDevice->CreateTexture(RES("/Textures/Bricks/Albedo.dds"));
        litBrick->SetTexture("Albedo", brickAlbedo);
        Texture* brickRoughness = pDevice->CreateTexture(RES("/Textures/Bricks/Roughness.dds"));
        litBrick->SetTexture("Roughness", brickRoughness);
        Texture* brickNormal = pDevice->CreateTexture(RES("/Textures/Bricks/NormalDX.dds"));
        litBrick->SetTexture("Normal", brickNormal);
        Texture* brickAmbient = pDevice->CreateTexture(RES("/Textures/Bricks/Ambient.dds"));
        litBrick->SetTexture("Ambient", brickAmbient);

        Material* litWood = litTextured->CreateMaterial();
        Texture* woodAlbedo = pDevice->CreateTexture(RES("/Textures/Wood/Albedo.dds"));
        litWood->SetTexture("Albedo", woodAlbedo);
        Texture* woodRoughness = pDevice->CreateTexture(RES("/Textures/Wood/Roughness.dds"));
        litWood->SetTexture("Roughness", woodRoughness);
        Texture* woodNormal = pDevice->CreateTexture(RES("/Textures/Wood/Normal.dds"));
        litWood->SetTexture("Normal", woodNormal);

        Material* litConcrete = litTextured->CreateMaterial();
        Texture* concreteAlbedo = pDevice->CreateTexture(RES("/Textures/Concrete/Albedo.dds"));
        litConcrete->SetTexture("Albedo", concreteAlbedo);
        Texture* concreteRoughness = pDevice->CreateTexture(RES("/Textures/Concrete/Roughness.dds"));
        litConcrete->SetTexture("Roughness", concreteRoughness);
        Texture* concreteNormal = pDevice->CreateTexture(RES("/Textures/Concrete/Normal.dds"));
        litConcrete->SetTexture("Normal", concreteNormal);
        Texture* concreteAmbient = pDevice->CreateTexture(RES("/Textures/Concrete/Ambient.dds"));
        litConcrete->SetTexture("Ambient", concreteAmbient);

        Material* litRock = litTextured->CreateMaterial();
        Texture* rockAlbedo = pDevice->CreateTexture(RES("/Textures/Rock/Albedo.dds"));
        litRock->SetTexture("Albedo", rockAlbedo);
        Texture* rockRoughness = pDevice->CreateTexture(RES("/Textures/Rock/Roughness.dds"));
        litRock->SetTexture("Roughness", rockRoughness);
        Texture* rockNormal = pDevice->CreateTexture(RES("/Textures/Rock/Normal.dds"));
        litRock->SetTexture("Normal", rockNormal);
        Texture* rockAmbient = pDevice->CreateTexture(RES("/Textures/Rock/Ambient.dds"));
        litRock->SetTexture("Ambient", rockAmbient);
        
        Geometry* Donut = GeometryFactory::BuildDonut(pDevice, 1.0f, 0.5f, 32, 16, true);
        Geometry* UVSphere = GeometryFactory::BuildUVSphere(pDevice, 16, 16);
        Geometry* IcoSphere = GeometryFactory::BuildIcosphere(pDevice, 5);
        Geometry* Cube = GeometryFactory::BuildCube(pDevice);
        Geometry* Pyramid = GeometryFactory::BuildPyramid(pDevice);
        Geometry* Cylinder = GeometryFactory::BuildCylinder(pDevice, 16);
        
        XMFLOAT4X4 donutMat = MathHelper::Identity4x4();
        SetPosition({-0.0f, 0.0f, 0.0f}, donutMat);

        XMFLOAT3 axis = XMFLOAT3(1.0f, 1.0f, 0.0f);
        XMMATRIX rotationMat;

        Camera cam;
        XMFLOAT3 pos = XMFLOAT3(0.0f, -3.0f, -3.0f);
        cam.SetPos(pos);
        XMFLOAT3 target = XMFLOAT3(0.0f, 0.0f, 0.0f);
        cam.LookAt(target);
        
        pDevice->SetMainCamera(&cam);

        {
            LightDescriptor dirLight = LightHelper::CreateLight(LightType::Directional);
            dirLight.light.Direction = XMFLOAT3(-0.5f, 0.0f, 0.5f);
            dirLight.light.Strength = XMFLOAT3(1.0f, 1.0f, 1.0f);
            dirLight.light.Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        
            Vector<LightDescriptor> lights = { dirLight };
            pDevice->SetLights(lights);
        }

        float angle = 0.0f;
        
        while (window.IsOpen())
        {
            window.Update();

            angle += 0.001f;
            
            XMMATRIX rot = XMMatrixRotationY(angle) * XMMatrixRotationX(angle) * XMMatrixRotationZ(angle);

            XMFLOAT4X4 donutM = MathHelper::Identity4x4();
            XMStoreFloat4x4(&donutM,    rot * XMLoadFloat4x4(&donutMat));
            
            window.Clear();

            pDevice->SetMaterial(litConcrete);
            pDevice->Draw(Donut, donutM);
            
            window.Display();
        }
    }
};

#endif