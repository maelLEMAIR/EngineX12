#include "../Gameplay/MainScene.h"
#include "../Gameplay/Components/CharacterController.h"
#include "../Gameplay/Scripts/PlayerMovementScript.h"
#include "../Gameplay/Scripts/PlayerInputScript.h"
#include "../Gameplay/Scripts/FirstPersonViewScript.h"
#include "../Engine/InputManager.h"

void MainScene::OnInit()
{
	
	EntityId e1 = world.CreateEntity();
	m_entities.push_back(e1);
	MeshRenderer& renderer = world.AddComponent<MeshRenderer>(e1);
	renderer.geoId = RessourceManager::GetGeometryId("Cube");
	renderer.materialId = RessourceManager::GetMaterialId("Default");
	TransformComponent& transformComp = world.AddComponent<TransformComponent>(e1);
	transformComp.local.SetPosition(XMFLOAT3(0.0f, 0.5f, 0.0f));
	CharacterController& controller = world.AddComponent<CharacterController>(e1);
	world.AddScript<PlayerMovementScript>(e1);
	world.AddScript<PlayerInputScript>(e1);

	EntityId camera = world.CreateEntity();
	m_entities.push_back(camera);
	TransformComponent& t = world.AddComponent<TransformComponent>(camera);
	t.local.SetPosition(XMFLOAT3(0.0f, 0.5f, 0.0f));
	t.SetParent(e1);
	CameraComponent& cam = world.AddComponent<CameraComponent>(camera);
	cam.camId = RessourceManager::GetCameraId("Default");
	cam.isMainCamera = true;
	world.AddScript<FirstPersonViewScript>(camera);

	EntityId ground = world.CreateEntity();
	m_entities.push_back(ground);
	MeshRenderer& groundRenderer = world.AddComponent<MeshRenderer>(ground);
	groundRenderer.geoId = RessourceManager::GetGeometryId("Cube");
	groundRenderer.materialId = RessourceManager::GetMaterialId("Red");
	TransformComponent& groundTransform = world.AddComponent<TransformComponent>(ground);
	groundTransform.local.SetPosition(XMFLOAT3(0.0f, -5.0f, 0.0f));
	groundTransform.local.SetScale(XMFLOAT3(50.0f, 1.0f, 50.0f));


	Scene::OnInit();
}

void MainScene::OnUpdate(float _dt)
{
	if (InputManager::IsKeyDown(F1))
	{
		SceneManager::SetCurrentScene("MenuScene");
	}

	Scene::OnUpdate(_dt);
}

void MainScene::OnStart()
{
	Scene::OnStart();
}

void MainScene::OnEnd()
{
	Scene::OnEnd();
}

void MainScene::LoadRessources()
{
}
