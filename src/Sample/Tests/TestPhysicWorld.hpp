#ifndef TEST_PHYSIC_WORLD_HPP_DEFINED
#define TEST_PHYSIC_WORLD_HPP_DEFINED

#include "InputManager.h"
#include "Test.h"
#include "../Engine/Engine.h"

class TestPhysicWorldScene : public Scene
{
    EntityId camera = 0;

    EntityId floorEntity = 0;
	EntityId cubeEntity = 0;

    float logTimer = 0.0f;
    int   logFrameCount = 0;
    int   dynamicBodyCount = 0;

    void OnInit() override
    {
        RessourceManager::AddCamera("Default");

        Shader* coloredS = ShaderFactory::CreateLitColored(EngineManager::GetDevice());

        Material* orange = coloredS->CreateMaterial();
        orange->SetFloat4("DiffuseAlbedo", Vect4f32(1.0f, 0.5f, 0.0f, 1.0f));
        orange->SetFloat("Roughness", 0.2f);
		RessourceManager::AddMaterial("orange", orange);
        
        floorEntity = world.CreateEntity();
        MeshRenderer& floorR = world.AddComponent<MeshRenderer>(floorEntity);
        floorR.geoId = RessourceManager::GetGeometryId("Cube");
        floorR.materialId = RessourceManager::GetMaterialId("orange");
        TransformComponent* floorT = &world.AddComponent<TransformComponent>(floorEntity);
		floorT->local.SetScale(Vect3f32(10.0f, 1.0f, 10.0f));
        floorT->local.SetPosition(Vect3f32(0.0f, 0.0f, 0.0f));

        ColliderComponent* floorC = &world.AddComponent<ColliderComponent>(floorEntity);
        floorC->shape = ColliderShape::AABB;
        floorC->extent = Vect3f32(5.0f, 0.5f, 5.0f);
        RigidBodyComponent* floorRB = &world.AddComponent<RigidBodyComponent>(floorEntity);
        floorRB->isStatic = true;

		cubeEntity = world.CreateEntity();
		MeshRenderer& cubeR = world.AddComponent<MeshRenderer>(cubeEntity);
		cubeR.geoId = RessourceManager::GetGeometryId("Cube");
		cubeR.materialId = RessourceManager::GetMaterialId("Default");
		TransformComponent* cubeT = &world.AddComponent<TransformComponent>(cubeEntity);
		cubeT->local.SetScale(Vect3f32(1.0f, 1.0f, 1.0f));
		cubeT->local.SetPosition(Vect3f32(10.0f, 5.0f, 10.0f)); // à l'écart du pavé de stress-test (x/z dans [-3.675, 3.675])
		ColliderComponent* cubeC = &world.AddComponent<ColliderComponent>(cubeEntity);
		cubeC->shape = ColliderShape::AABB;
        RigidBodyComponent* cubeRB = &world.AddComponent<RigidBodyComponent>(cubeEntity);

		EntityId cubeEntity2 = world.CreateEntity();
		MeshRenderer& cubeR2 = world.AddComponent<MeshRenderer>(cubeEntity2);
		cubeR2.geoId = RessourceManager::GetGeometryId("Cube");
		cubeR2.materialId = RessourceManager::GetMaterialId("Default");
		TransformComponent* cubeT2 = &world.AddComponent<TransformComponent>(cubeEntity2);
		cubeT2->local.SetScale(Vect3f32(1.0f, 1.0f, 1.0f));
		cubeT2->local.SetPosition(Vect3f32(10.0f, 10.0f, 0.0f)); // à l'écart du pavé de stress-test
		ColliderComponent* cubeC2 = &world.AddComponent<ColliderComponent>(cubeEntity2);
		cubeC2->shape = ColliderShape::AABB;
		RigidBodyComponent* cubeRB2 = &world.AddComponent<RigidBodyComponent>(cubeEntity2);

		world.GetSystem<PhysicsSystem>()->AddToPhysicWorld(world, floorEntity);
		world.GetSystem<PhysicsSystem>()->AddToPhysicWorld(world, cubeEntity);
		world.GetSystem<PhysicsSystem>()->AddToPhysicWorld(world, cubeEntity2);

        camera = world.CreateEntity();
        TransformComponent* camT = &world.AddComponent<TransformComponent>(camera);
        camT->local.SetPosition(Vect3f32(0.0f, 5.0f, -10.0f));
        camT->local.AddYPR(Vect3f32(0.0f, MathUtils::PI / 8, 0.0f));
        CameraComponent& cam = world.AddComponent<CameraComponent>(camera);
        cam.camId = RessourceManager::GetCameraId("Default");
        cam.isMainCamera = true;
		EngineManager::GetDevice()->SetMainCamera(RessourceManager::GetCamera("Default"));

        EntityId light = world.CreateEntity();
        TransformComponent& lt = world.AddComponent<TransformComponent>(light);
        lt.local.SetPosition(Vect3f32(0.0f, 15.0f, 5.0f));
        LightComponent& l = world.AddComponent<LightComponent>(light);
        l.type = LightType::Point;
        l.SetStrength(10.0f);
        l.SetPoint(0.10f, 30.0f);
        l.SetColor(Vect4f32(1.0f, 0.0f, 0.0f, 1.0f));

        Scene::OnInit();
    }

    void OnUpdate(float _dt) override
    {
		TransformComponent* camT = world.GetComponent<TransformComponent>(camera);
        if (InputManager::IsKeyPressed(Q))
            camT->local.Move(camT->local.GetRight(), -50.0f * _dt);
        if (InputManager::IsKeyPressed(D))
            camT->local.Move(camT->local.GetRight(), 50.0f * _dt);
        if (InputManager::IsKeyPressed(Z))
            camT->local.Move(camT->local.GetForward(), 50.0f * _dt);
        if (InputManager::IsKeyPressed(S))
            camT->local.Move(camT->local.GetForward(), -50.0f * _dt);

		RigidBodyComponent* cubeRB = world.GetComponent<RigidBodyComponent>(cubeEntity);

		if (InputManager::IsKeyPressed(SPACE))
			cubeRB->SetLinearVelocity(Vect3f32(0.0f, 10.0f, 0.0f));

		if (InputManager::IsKeyPressed(E))
            cubeRB->ApplyForce(Vect3f32(100.0f, 0.0f, 0.0f));

		if (InputManager::IsKeyPressed(A))
			cubeRB->ApplyForce(Vect3f32(-100.0f, 0.0f, 0.0f));
    }
};

class TestPhysicWorld : public Test
{
public:
    static void Run()
    {
        EngineManager engineManager;
        engineManager.Initialize(1920, 1080, L"TestPhysicWorld", true);

        SceneManager::CreateSceneType<TestPhysicWorldScene>("TestPhysicWorldScene");
        SceneManager::SetCurrentScene("TestPhysicWorldScene");

        engineManager.Run();
    }
};


#endif