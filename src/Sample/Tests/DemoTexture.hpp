#ifndef DEMO_TEXTURE_HPP_DEFINED
#define DEMO_TEXTURE_HPP_DEFINED

#include "Test.h"
#include "../Render/Generic/Render.h"

class DemoTexture : public Test
{
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

        Transform donutMat;
        donutMat.SetPosition({-0.0f, 0.0f, 0.0f});

        Camera cam;
        Transform camT;
        camT.SetPosition(Vect3f32(0.0f, -3.0f, -3.0f));
        camT.LookAt({0.0f, 0.0f, 0.0f});

        cam.SetWorld(camT.GetMatrix());
        pDevice->SetMainCamera(&cam);

        {
            LightDescriptor dirLight = LightHelper::CreateLight(LightType::Directional);
            dirLight.light.Direction = Vect3f32(-0.5f, 0.0f, 0.5f);
            dirLight.light.Strength = Vect3f32(1.0f, 1.0f, 1.0f);
            dirLight.light.Color = Vect4f32(1.0f, 1.0f, 1.0f, 1.0f);

            Vector<LightDescriptor> lights = { dirLight };
            pDevice->SetLights(lights);
        }

        float angle = 0.0f;

        while (window.IsOpen())
        {
            window.Update();

            angle += 0.001f;

            donutMat.SetYPR({angle, angle, angle});

            window.Clear();

            pDevice->SetMaterial(litConcrete);
            pDevice->Draw(Donut, donutMat.GetMatrix());

            window.Display();
        }
    }
};

#endif
