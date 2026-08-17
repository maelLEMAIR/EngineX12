#ifndef DEMO_GEOS_HPP_DEFINED
#define DEMO_GEOS_HPP_DEFINED

#include "Test.h"
#include "../Render/Generic/Render.h"

class DemoGeos : public Test
{
public:
    static void Run()
    {
        Window window(1080, 720, L"Test", true);
        window.InitD3D12();

        Device* pDevice = window.GetDevice();

        Shader* coloredS = ShaderFactory::CreateLitColored(pDevice);

        Material* orange = coloredS->CreateMaterial();
        orange->SetFloat4("DiffuseAlbedo", Vect4f32(1.0f, 0.5f, 0.0f, 1.0f));
        orange->SetFloat("Roughness", 0.2f);

        Material* green = coloredS->CreateMaterial();
        green->SetFloat4("DiffuseAlbedo", Vect4f32(0.0f, 0.7f, 0.0f, 1.0f));
        green->SetFloat("Roughness", 0.2f);

        Material* blue = coloredS->CreateMaterial();
        blue->SetFloat4("DiffuseAlbedo", Vect4f32(0.0f, 0.5f, 1.0f, 1.0f));
        blue->SetFloat("Roughness", 0.2f);

        Material* purple = coloredS->CreateMaterial();
        purple->SetFloat4("DiffuseAlbedo", Vect4f32(0.7f, 0.0f, 0.7f, 1.0f));
        purple->SetFloat("Roughness", 0.2f);

        Material* red = coloredS->CreateMaterial();
        red->SetFloat4("DiffuseAlbedo", Vect4f32(1.0f, 0.0f, 0.0f, 1.0f));
        red->SetFloat("Roughness", 0.2f);

        Geometry* Donut = GeometryFactory::BuildDonut(pDevice, 1.0f, 0.5f, 32, 16, true);
        Geometry* UVSphere = GeometryFactory::BuildUVSphere(pDevice, 16, 16);
        Geometry* IcoSphere = GeometryFactory::BuildIcosphere(pDevice, 5);
        Geometry* Cube = GeometryFactory::BuildCube(pDevice);
        Geometry* Pyramid = GeometryFactory::BuildPyramid(pDevice);
        Geometry* Cylinder = GeometryFactory::BuildCylinder(pDevice, 16);

        Transform donutMat;
        donutMat.SetPosition({-3.0f, 0.0f, 2.0f});
        Transform uvSphereMat;
        uvSphereMat.SetPosition({-3.0f, 0.0f, -2.0f});
        Transform IcosphereMat;
        IcosphereMat.SetPosition({0.0f, 0.0f, 2.0f});
        Transform CubeMat;
        CubeMat.SetPosition({0.0f, 0.0f, -2.0f});
        Transform PyramidMat;
        PyramidMat.SetPosition({3.0f, 0.0f, 2.0f});
        Transform CYlinderMat;
        CYlinderMat.SetPosition({3.0f, 0.0f, -2.0f});

        Camera cam;
        Transform camT;
        camT.SetPosition(Vect3f32(0.0f, -10.0f, -10.0f));
        camT.LookAt({0.0f, 0.0f, 0.0f});

        cam.SetWorld(camT.GetMatrix());
        pDevice->SetMainCamera(&cam);

        {
            LightDescriptor dirLight = LightHelper::CreateLight(LightType::Directional);
            dirLight.light.Direction = Vect3f32(-0.5f, 0.0f, 0.5f);
            dirLight.light.Strength = Vect3f32(2.0f, 2.0f, 2.0f);
            dirLight.light.Color = Vect4f32(1.0f, 1.0f, 1.0f, 1.0f);

            Vector<LightDescriptor> lights = { dirLight };
            pDevice->SetLights(lights);
        }

        float angle = 0.0f;

        while (window.IsOpen())
        {
            window.Update();

            angle += 0.01f;

            donutMat.SetYPR({angle, angle, angle});
            CubeMat.SetYPR({angle, angle, angle});
            PyramidMat.SetYPR({angle, angle, angle});
            CYlinderMat.SetYPR({angle, angle, angle});

            window.Clear();

            pDevice->SetMaterial(blue);
            pDevice->Draw(Donut, donutMat.GetMatrix());
            pDevice->SetMaterial(red);
            pDevice->Draw(UVSphere, uvSphereMat.GetMatrix());
            pDevice->SetMaterial(purple);
            pDevice->Draw(IcoSphere, IcosphereMat.GetMatrix());
            pDevice->SetMaterial(green);
            pDevice->Draw(Cube, CubeMat.GetMatrix());
            pDevice->SetMaterial(orange);
            pDevice->Draw(Pyramid, PyramidMat.GetMatrix());
            pDevice->Draw(Cylinder, CYlinderMat.GetMatrix());

            window.Display();
        }
    }
};

#endif
