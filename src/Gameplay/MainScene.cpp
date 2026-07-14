#include "../Gameplay/MainScene.h"

void MainScene::OnInit()
{

	EntityId e1 = world.CreateEntity();
	MeshRenderer& renderer = world.AddComponent<MeshRenderer>(e1);
	renderer.geoId = RessourceManager::GetGeometryId("Cube");
	renderer.materialId = RessourceManager::GetMaterialId("Default");
	TransformComponent* transformComp = &world.AddComponent<TransformComponent>(e1);
	transformComp->world.pos = { 0.0f, 0.0f, 5.0f };

	EntityId camera = world.CreateEntity();
	TransformComponent& t = world.AddComponent<TransformComponent>(camera);
	t.local.SetPosition(XMFLOAT3(0.0f, 5.0f, -10.0f));
	t.local.AddYPR({ 0.0f, XM_PI / 8, 0.0f });
	CameraComponent& cam = world.AddComponent<CameraComponent>(camera);
	cam.camId = RessourceManager::GetCameraId("Default");
	cam.isMainCamera = true;



}