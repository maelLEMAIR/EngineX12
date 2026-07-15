#ifndef ENGINE_MANAGER_CPP_DEFINED
#define ENGINE_MANAGER_CPP_DEFINED

#include "EngineManager.h"
#include "Engine.h"
#include "Scene.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "Systems/TransformSystem.h"

#include "../Render/Generic/Render.h"
#include "../Render/Generic/Factories/ShaderFactory.hpp"


#include "../Network/NetworkQueue.h"

////////////// NETWORK BRIDGE
#include "NetworkBridge/ComponentDispatcher.h"
#include "NetworkBridge/NetworkBridgeInit.h"
#include "NetworkBridge/NetworkComponentIndex.h"
#include "NetworkBridge/NetworkContext.h"
#include "NetworkBridge/NetworkFlag.h"
#include "NetworkBridge/NetworkIdentity.h"
#include "NetworkBridge/NetworkInterpolator.h"
#include "NetworkBridge/NetworkLaunchArgs.h"
#include "NetworkBridge/NetworkRegistry.h"
#include "NetworkBridge/PingManager.h"
#include "NetworkBridge/PlayerRegistry.h"
#include "NetworkBridge/Packet/PacketBuilder.h"
#include "NetworkBridge/Packet/PacketDef.h"


EngineManager* EngineManager::s_pInstance = nullptr;

EngineManager::EngineManager()
{
    s_pInstance = this;
    m_chrono = Chrono();
}

EngineManager::~EngineManager()
{
    s_pInstance->Exit();
}

EngineManager& EngineManager::GetInstance()
{
    if (s_pInstance == nullptr)
        s_pInstance = new EngineManager();

    return *s_pInstance;
}

void EngineManager::Exit()
{
    
}

void EngineManager::Initialize(UINT _width, UINT _height, WString _title, bool _fullscreen, int argc, char* argv[])
{
    NetworkLaunchArgs netArgs = NetworkLaunchArgs::Parse(argc, argv);
    NetworkContext::Get().Initialize(netArgs);
    NetworkBridge::RegisterComponents();

    m_pPacketHandler = new PacketHandler();
    m_pPacketHandler->SetNetworkManager(&NetworkContext::Get().GetManager());

    if (m_pSceneManager == nullptr)
        m_pSceneManager = new SceneManager;
    
    if (NetworkContext::Get().IsServer())
    {
        AllocConsole();
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);

        return;
    }
    
    if (m_pWindow == nullptr)
    {
        m_pWindow = new Window((int)_width, (int)_height, _title);
        m_pWindow->InitD3D12();
        m_pDevice = m_pWindow->GetDevice();
        if ( _fullscreen)
            m_pWindow->ToggleFullScreen();
    }

    m_pDevice->SetClearColor(ToColor(87, 185, 255));
    m_pRessourceManager = new RessourceManager;

    RessourceManager::AddGeometry("Cube", GeometryFactory::BuildCube(m_pDevice));
    
    Shader* coloredS = ShaderFactory::CreateLitColored(m_pDevice);
    RessourceManager::AddShader("Color", coloredS);

    Material* white = coloredS->CreateMaterial();
    white->SetFloat4("DiffuseAlbedo", {1.0f, 1.0f, 1.0f, 1.0f});
    RessourceManager::AddMaterial("Default", white);

	Material* red = coloredS->CreateMaterial();
	red->SetFloat4("DiffuseAlbedo", { 1.0f, 0.0f, 0.0f, 1.0f });
	RessourceManager::AddMaterial("Red", red);

    RessourceManager::AddCamera("Default");

    InputManager::Initialize(m_pWindow->GetHWND());
}

void EngineManager::Run()
{
    m_chrono.Start();
    
    if (NetworkContext::Get().IsServer())
    {
        constexpr float TICK_RATE    = 60.f;
        constexpr float TICK_DELAY   = 1.f / TICK_RATE;
        float           accumulator  = 0.f;

        while (true)
        {
            float frameTime = m_chrono.Reset();
            accumulator += frameTime;

            NetworkPacket packet;
            while (NetworkContext::Get().GetManager().PopReceived(packet))
            {
                TryRegisterNewClients(packet, NetworkContext::Get().GetManager());
                m_pPacketHandler->Handle(packet, m_pSceneManager->GetCurrentScene()->world);
            }

            while (accumulator >= TICK_DELAY)
            {
                m_pPacketHandler->SetDeltaTime(TICK_DELAY);
                m_pSceneManager->GetCurrentScene()->Update(TICK_DELAY);
                accumulator -= TICK_DELAY;
            }
        }
    }
    
    while ( m_pWindow->IsOpen() )
    {
        m_deltaTime = m_chrono.Reset();

        m_pPacketHandler->SetCurrentTime(m_totalTime);
        m_totalTime += m_deltaTime;
        
        m_pWindow->Update();
        
        InputManager::Update(m_deltaTime);
        
        PingManager::Get().Update(m_deltaTime, NetworkContext::Get().GetManager());
        
        if (NetworkContext::Get().GetRole() != NetworkRole::None)
        {
            NetworkPacket packet;
            auto& netManager = NetworkContext::Get().GetManager();

            while (netManager.PopReceived(packet))
                m_pPacketHandler->Handle(packet, m_pSceneManager->GetCurrentScene()->world);
        }
        m_pSceneManager->GetCurrentScene()->Update(m_deltaTime);
    }
    
    NetworkContext::Get().Disconnect();
}

void EngineManager::TryRegisterNewClients(const NetworkPacket& packet, NetworkManager& net)
{
    if (packet.data.empty()) return;

    if (PlayerRegistry::Get().Has(packet.address))
    {
        PlayerRegistry::Get().UpdateLastSeen(packet.address);
        return;
    }
    
    if (static_cast<PacketType>(packet.data[0]) != PacketType::Connect) return;

    if (PlayerRegistry::Get().Has(packet.address)) return;

    net.AddPeerAddress(packet.address);

    World& world = m_pSceneManager->GetCurrentScene()->world;

    EntityId localId = world.CreateEntity();
    world.AddComponent<TransformComponent>(localId);
    world.AddComponent<NetworkIdentity>(localId);
    world.AddComponent<DirtyFlag>(localId);

    uint32_t netId = NetworkRegistry::Get().GenerateNetworkId();

    NetworkIdentity* identity = world.GetComponent<NetworkIdentity>(localId);
    identity->networkId = netId;
    identity->isOwner   = false;

    NetworkRegistry::Get().Register(netId, localId);
    PlayerRegistry::Get().Register(packet.address, localId);
    
    std::cout << "[SERVER] New client register " << "\n";
    
    Serialization::Serializer s;
    s.write((uint8)PacketType::EntityCreated);
    s.write(netId);
    s.write(true);
    net.SendTo(s.GetBuffer(), packet.address);
    
    auto snapshot = PacketBuilder::Snapshot(world);
    net.SendTo(snapshot, packet.address);

    std::cout << "[SERVER] Snapshot send to new client\n";
}

#endif
