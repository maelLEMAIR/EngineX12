#include "MenuScene.h"
#include "../Engine/InputManager.h"

void MenuScene::OnInit()
{

    EntityId e1 = world.CreateEntity();
	m_entities.push_back(e1);
    MeshRenderer& renderer = world.AddComponent<MeshRenderer>(e1);
    renderer.geoId = RessourceManager::GetGeometryId("Cube");
    renderer.materialId = RessourceManager::GetMaterialId("Default");
    TransformComponent& transformComp = world.AddComponent<TransformComponent>(e1);
    transformComp.local.pos = { 0.0f, 0.0f, 0.0f };

    EntityId camera = world.CreateEntity();
	m_entities.push_back(camera);
    TransformComponent& t = world.AddComponent<TransformComponent>(camera);
    t.local.SetPosition(XMFLOAT3(0.0f, 10.0f, -20.0f));
    t.local.AddYPR({ 0.0f, XM_PI / 8, 0.0f });
    CameraComponent& cam = world.AddComponent<CameraComponent>(camera);
    cam.camId = RessourceManager::GetCameraId("Default");
    cam.isMainCamera = true;

	Scene::OnInit();
}

void MenuScene::OnUpdate(float _dt)
{
    if (InputManager::IsKeyDown(F1))
    {
		SceneManager::SetCurrentScene("MainScene");
    }

	Scene::OnUpdate(_dt);
}

void MenuScene::OnStart()
{
    InputManager::UnlockMouseCursor();
    InputManager::ShowMouseCursor();
	Scene::OnStart();
}

void MenuScene::OnEnd()
{
	Scene::OnEnd();
}

void MenuScene::LoadRessources()
{
    RessourceManager::AddGeometry("XWING", GeometryFactory::LoadGeometry(EngineManager::GetDevice(), "../../../../res/Obj/xwing.obj"));
	Scene::LoadRessources();
}