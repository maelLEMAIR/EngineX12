#ifndef TEST_NETWORK_HPP_INCLUDED
#define TEST_NETWORK_HPP_INCLUDED

#include "Test.h"
#include "Engine.h"
#include "InputManager.h"
#include "InterpolationSystem.h"
#include "NetworkSyncSystem.h"
#include "Components/TextComponent.h"
#include "NetworkBridge/NetworkContext.h"
#include "NetworkBridge/NetworkFlag.h"
#include "NetworkBridge/NetworkIdentity.h"
#include "NetworkBridge/NetworkRegistry.h"
#include "NetworkBridge/Packet/PacketInput.h"
#include "Engine/Components/TransformComponent.hpp"
#include "Packet/PacketDef.h"

class TestNetworkScene;

class TestNetwork : public Test
{
public: 
    static void Run(int argc, char* argv[])
    {
        EngineManager::GetInstance().Initialize(1280, 720, L"TestNetwork", true, argc, argv);

        SceneManager::CreateSceneType<TestNetworkScene>("TestNetwork");
        SceneManager::SetCurrentScene("TestNetwork");

        EngineManager::GetInstance().Run();
    }
};

class TestNetworkScene : public Scene
{
public:
    void OnInit() override
    {
        if (NetworkContext::Get().IsServer())
            InitServer();
        else if (NetworkContext::Get().IsClient())
            InitClient();
    }

    void OnUpdate(float dt) override
    {
        /*if (NetworkContext::Get().IsServer())
            UpdateServer(dt);*/
        if (NetworkContext::Get().IsClient())
        {
            /*String text = std::to_string(PingManager::Get().GetLatency()) + " ms";
            m_pText->SetString( text );*/
            SendInputs(dt);
        }
    }

private:
    EntityId m_entity   = 0;
    EntityId m_textPing = 0;

    Text* m_pText       = nullptr;
    float    m_timer    = 0.f;

    void InitServer()
    {
        std::cout << "[SERVER] Start on port 7777\n";
        
        world.RegisterSystem<InterpolationSystem>(9);
        auto* syncSystem = world.RegisterSystem<NetworkSyncSystem>(10);
        syncSystem->SetNetworkManager(&NetworkContext::Get().GetManager());
    }

    void InitClient()
    {
        RessourceManager::AddCamera("Default");
        
        Serialization::Serializer s;
        s.write((uint8)PacketType::Connect);
        
        auto& net = NetworkContext::Get().GetManager();
        
        std::cout << "[CLIENT] Send Connect\n";
        
        for (const auto& peer : net.GetPeers())
            net.SendTo(s.GetBuffer(), peer);
        
        /*m_entity = world.CreateEntity();
        TransformComponent& transformEntity = world.AddComponent<TransformComponent>(m_entity);
        transformEntity.local.SetPosition(XMFLOAT3(0.0f, 0.0f, 10.0f));
        MeshRenderer& rendererEntity = world.AddComponent<MeshRenderer>(m_entity);
        rendererEntity.geoId = RessourceManager::GetGeometryId("Cube");*/
        
        {
            RenderFont* font = EngineManager::GetDevice()->CreateRenderFont(RES("/Font/GoldenVarsity.ttf"), 100.0f);

            m_pText = EngineManager::GetDevice()->CreateText(font);
            m_pText->SetString("0 ms");
            RessourceManager::AddText("PingLabel", m_pText);

            m_textPing = world.CreateEntity();
            TextComponent& dayText = world.AddComponent<TextComponent>(m_textPing);
            dayText.textId = RessourceManager::GetTextId("PingLabel");
            dayText.transform.SetPosition(XMFLOAT2(-900.0f, -500.0f));
        }

        EntityId camera = world.CreateEntity();
        TransformComponent& transformCamera = world.AddComponent<TransformComponent>(camera);
        transformCamera.local.SetPosition(Vect3f32(0.0f, 0.0f, -10.0f));
        CameraComponent& cam = world.AddComponent<CameraComponent>(camera);
        cam.camId = RessourceManager::GetCameraId("Default");
        cam.isMainCamera = true;
    }

    void SendInputs(float dt)
    {
        static float timer = 0.f;
        timer += dt;
        if (timer < 1.f / 20.f) return;
        timer = 0.f;
        
        InputPacket input;
        input.moveForward  = InputManager::IsKeyPressed(Z) || InputManager::IsKeyDown(Z);
        input.moveBackward = InputManager::IsKeyPressed(S) || InputManager::IsKeyDown(S);
        input.moveLeft     = InputManager::IsKeyPressed(Q) || InputManager::IsKeyDown(Q);
        input.moveRight    = InputManager::IsKeyPressed(D) || InputManager::IsKeyDown(D);
        input.jump         = InputManager::IsKeyDown(SPACE);

        auto mousePos = InputManager::GetMousePositionCenter();
        input.mouseLeft   = InputManager::IsMouseButtonPressed(LEFT_MOUSE) 
                         || InputManager::IsMouseButtonDown(LEFT_MOUSE);
        input.mouseRight  = InputManager::IsMouseButtonPressed(RIGHT_MOUSE) 
                         || InputManager::IsMouseButtonDown(RIGHT_MOUSE);
        
        if ( !input.moveForward && !input.moveBackward &&
            !input.moveLeft && !input.moveRight && !input.jump &&
            !input.mouseLeft && !input.mouseRight )
            return;
        
        Serialization::Serializer s;
        input.Serialize(s);

        auto& net = NetworkContext::Get().GetManager();
        for (const auto& peer : net.GetPeers())
        {
            net.SendTo(s.GetBuffer(), peer);
        }
    }
    
    void UpdateServer(float dt)
    {
        m_timer += dt;
        if (m_timer >= 1.f)
        {
            m_timer = 0.f;

            TransformComponent* t    = world.GetComponent<TransformComponent>(m_entity);
            DirtyFlag*          flag = world.GetComponent<DirtyFlag>(m_entity);

            if (t && flag)
            {
                t->local.Move(Vect3f32(1.f, 0.f, 0.f));
                flag->Mark(0);
            }
        }
    }
};

#endif