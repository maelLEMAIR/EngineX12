#ifndef TEST_ENGINE_MANAGER_HPP_DEFINED
#define TEST_ENGINE_MANAGER_HPP_DEFINED

#include "InputManager.h"
#include "Test.h"
#include "../Engine/Engine.h"

class MainScene : public Scene
{
    TransformComponent* transformComp = nullptr;
    void OnInit() override
    {

        RessourceManager::AddCamera("Default");
        RessourceManager::AddGeometry("Cube", GeometryFactory::LoadGeometry(EngineManager::GetDevice(), "../../res/Obj/xwing.obj"));
        
        EntityId e1 = world.CreateEntity();
        MeshRenderer& renderer = world.AddComponent<MeshRenderer>(e1);
        renderer.geoId = RessourceManager::GetGeometryId("Cube");
        renderer.materialId = RessourceManager::GetMaterialId("Default");
        transformComp = &world.AddComponent<TransformComponent>(e1);
        transformComp->world.pos = { 0.0f, 0.0f, 5.0f};

        EntityId camera = world.CreateEntity();
        TransformComponent& t = world.AddComponent<TransformComponent>(camera);
        t.local.SetPosition(XMFLOAT3(0.0f, 5.0f, -10.0f));
        t.local.AddYPR({0.0f, XM_PI / 8, 0.0f});
        CameraComponent& cam = world.AddComponent<CameraComponent>(camera);
        cam.camId = RessourceManager::GetCameraId("Default");
        cam.isMainCamera = true;

        EntityId light = world.CreateEntity();
        TransformComponent& lt = world.AddComponent<TransformComponent>(light);
        lt.local.SetPosition(XMFLOAT3(0.0f, 15.0f,5.0f));
        LightComponent& l = world.AddComponent<LightComponent>(light);
        l.type = LightType::Point;
        l.SetStrength(10.0f);
        l.SetPoint(0.10f, 30.0f);
        l.SetColor(XMFLOAT4(1.0f,0.0f,0.0f, 1.0f));
        
        Scene::OnInit();
    }

    void OnUpdate(float _dt) override
    {
        if (InputManager::IsKeyPressed(Q))
            transformComp->local.Move( transformComp->local.right, -50.0f * _dt);
        if (InputManager::IsKeyPressed(D))
            transformComp->local.Move( transformComp->local.right, 50.0f * _dt);
        if (InputManager::IsKeyPressed(Z))
            transformComp->local.Move( transformComp->local.forward, 50.0f * _dt);
        if (InputManager::IsKeyPressed(S))
            transformComp->local.Move( transformComp->local.forward, -50.0f * _dt);
        if (InputManager::IsKeyPressed(LEFT_ARROW))
            transformComp->local.AddYPR({XM_PI / 32.0f, 0.0f, 0.0f});
        if (InputManager::IsKeyPressed(RIGHT_ARROW))
            transformComp->local.AddYPR({XM_PI / -32.0f, 0.0f, 0.0f});
    }
};

class TestEngineManager : public Test
{
public: 
    static void Run()
    {
        EngineManager engineManager;
        engineManager.Initialize(1920, 1080, L"TestEngineManager", true);

        SceneManager::CreateSceneType<MainScene>("MainScene");
        SceneManager::SetCurrentScene("MainScene");

        engineManager.Run();
    }
};


#endif