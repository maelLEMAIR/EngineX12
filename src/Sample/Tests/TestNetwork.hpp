#ifndef TEST_NETWORK_HPP_INCLUDED
#define TEST_NETWORK_HPP_INCLUDED

#include "Test.h"
#include "Engine.h"
#include "InputManager.h"
#include "NetworkSyncSystem.h"
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
        EngineManager::GetInstance().Initialize(1280, 720, L"TestNetwork", false, argc, argv);

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
            SendInputs(dt);
        }
    }

private:
    EntityId m_entity = 0;
    float    m_timer  = 0.f;

    void InitServer()
    {
        auto* syncSystem = world.RegisterSystem<NetworkSyncSystem>(10);
        syncSystem->SetNetworkManager(&NetworkContext::Get().GetManager());
        
        m_entity = world.CreateEntity();

        world.AddComponent<TransformComponent>(m_entity);
        world.AddComponent<NetworkIdentity>(m_entity);
        world.AddComponent<DirtyFlag>(m_entity);

        uint32_t netId = NetworkRegistry::Get().GenerateNetworkId();
    
        NetworkIdentity* identity = world.GetComponent<NetworkIdentity>(m_entity);
        identity->networkId = netId;
        identity->isOwner   = true;
        
        NetworkRegistry::Get().Register(netId, m_entity);
    }

    void InitClient()
    {
        Serialization::Serializer s;
        s.write((uint8)PacketType::Connect);

        auto& net = NetworkContext::Get().GetManager();
        for (const auto& peer : net.GetPeers())
            net.SendTo(s.GetBuffer(), peer);
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
            net.SendTo(s.GetBuffer(), peer);
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
                t->local.pos.x += 1.f;
                flag->Mark(0);
            }
        }
    }
};

#endif