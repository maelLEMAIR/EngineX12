#ifndef PACKET_HANDLER_CPP_INCLUDED
#define PACKET_HANDLER_CPP_INCLUDED

#include "PacketHandler.h"

#include "EngineManager.h"
#include "NetworkContext.h"
#include "NetworkFlag.h"
#include "NetworkIdentity.h"
#include "NetworkInterpolator.h"
#include "NetworkManager.h"
#include "PacketDef.h"
#include "PacketInput.h"
#include "PingManager.h"
#include "PlayerRegistry.h"
#include "RessourceManager.h"
#include "../NetworkRegistry.h"
#include "../Engine/ECS/World.h"
#include "../Engine/Components/TransformComponent.hpp"
#include "Components/MeshRenderer.hpp"

void ApplyInput(const InputPacket& input, World& world);

void PacketHandler::Handle(const NetworkPacket& packet, World& world)
{    
    Serialization::Deserializeration d(packet.data.data(), packet.data.size());

    uint8_t typeRaw;
    if (!d.read(typeRaw)) return;

    PacketType type = static_cast<PacketType>(typeRaw);

    switch (type)
    {
        case PacketType::EntityCreated:   HandleEntityCreated  (d, world);               break;
        case PacketType::EntityDestroyed: HandleEntityDestroyed(d, world);               break;
        case PacketType::ComponentUpdate: HandleComponentUpdate(d, world);               break;
        case PacketType::Input:           HandleInput(d, world, packet.address);         break;
        case PacketType::Snapshot:        HandleSnapshot       (d, world);               break;
        case PacketType::Connect:         HandleConnected(d, world, packet.address);     break;
        case PacketType::Ping:            HandlePing(d, packet.address);                    break;
        case PacketType::Pong:            HandlePong(d);                                    break;
        case PacketType::Disconnect:      HandleDisconnect(d, world, packet.address);    break;
        default:                                                                               break;
    }
}


void PacketHandler::HandleConnected(Serialization::Deserializeration& d, World& world, const sockaddr_in& from)
{
    std::cout << "[SERVER] Client Connected " << "\n";
}

void PacketHandler::HandleDisconnect(Serialization::Deserializeration& d, World& world, const sockaddr_in& from)
{
    if (!PlayerRegistry::Get().Has(from)) return;

    EntityId localId = PlayerRegistry::Get().GetEntity(from);

    NetworkIdentity* identity = world.GetComponent<NetworkIdentity>(localId);
    if (identity)
    {
        uint32_t netId = identity->networkId;
        NetworkRegistry::Get().Unregister(netId);

        Serialization::Serializer s;
        s.write((uint8)PacketType::EntityDestroyed);
        s.write(netId);
        
        for (const auto& peer : m_networkManager->GetPeers())
        {
            if (peer.sin_addr.s_addr == from.sin_addr.s_addr &&
                peer.sin_port        == from.sin_port) continue;
            m_networkManager->SendTo(s.GetBuffer(), peer);
        }
    }

    PlayerRegistry::Get().Unregister(from);
    m_networkManager->RemovePeer(from);
    world.DestroyEntity(localId);

    std::cout << "[SERVER] Client disconnected\n";
}

void PacketHandler::HandlePing(Serialization::Deserializeration& d,
                                const sockaddr_in& from)
{
    Serialization::Serializer s;
    s.write((uint8_t)PacketType::Pong);
    m_networkManager->SendTo(s.GetBuffer(), from);
}

void PacketHandler::HandlePong(Serialization::Deserializeration& d)
{
    PingManager::Get().OnPongReceived();
}

void PacketHandler::HandleEntityCreated(Serialization::Deserializeration& d, World& world)
{ 
    uint32_t networkId;
    if (!d.read(networkId)) return;

    EntityId localId = world.CreateEntity();
    world.AddComponent<NetworkInterpolator>(localId);
    world.AddComponent<DirtyFlag>(localId);
    
    MeshRenderer& mesh = world.AddComponent<MeshRenderer>(localId);
    mesh.geoId = RessourceManager::GetGeometryId("Cube");

    auto& identity    = world.AddComponent<NetworkIdentity>(localId);
    identity.networkId = networkId;

    NetworkRegistry::Get().Register(networkId, localId);
}

void PacketHandler::HandleEntityDestroyed(Serialization::Deserializeration& d, World& world)
{
    uint32_t networkId;
    if (!d.read(networkId)) return;

    if (!NetworkRegistry::Get().HasNetworkId(networkId)) return;

    EntityId localId = NetworkRegistry::Get().GetLocalId(networkId);
    NetworkRegistry::Get().Unregister(networkId);
    world.DestroyEntity(localId);
}

void PacketHandler::HandleInput(Serialization::Deserializeration& d, World& world, const sockaddr_in& from)
{
    if (!PlayerRegistry::Get().Has(from)) return;

    EntityId localId = PlayerRegistry::Get().GetEntity(from);
    InputPacket input = InputPacket::Deserialize(d);

    TransformComponent* t       = world.GetComponent<TransformComponent>(localId);
    DirtyFlag* flag             = world.GetComponent<DirtyFlag>(localId);

    if (!t || !flag)
    {
        std::cout << "[SERVER] HandleInput: component unfindable\n";
        return;
    }
    
    float speed = 10.0f * m_deltaTime;
    if (input.moveForward)  t->local.Move(t->local.forward, speed);
    if (input.moveBackward) t->local.Move(t->local.forward, -speed);
    if (input.moveLeft)     t->local.Move(t->local.right, -speed);
    if (input.moveRight)    t->local.Move(t->local.right, speed);

    flag->Mark(0);
}

void PacketHandler::HandleComponentUpdate(Serialization::Deserializeration& d, World& world)
{
    uint32_t networkId, componentId;
    
    if (!d.read(networkId) || !d.read(componentId)) return;
    
    if (!NetworkRegistry::Get().HasNetworkId(networkId)) return;

    switch (componentId)
    {
    case 0x01:
    {
        EntityId localId = NetworkRegistry::Get().GetLocalId(networkId);

        XMFLOAT3 pos, scale;
        XMFLOAT4 quat;

        d.read(pos.x);   d.read(pos.y);   d.read(pos.z);
        d.read(scale.x); d.read(scale.y); d.read(scale.z);
        d.read(quat.x);  d.read(quat.y);  d.read(quat.z); d.read(quat.w);

        NetworkInterpolator* interp = world.GetComponent<NetworkInterpolator>(localId);
            
        if (interp)
        {
            interp->AddSnapshot(pos, scale, quat, EngineManager::GetInstance().GetTotalTime());
        }
        else
        {
            TransformComponent* t = world.GetComponent<TransformComponent>(localId);
            if (!t) return;
            t->local.pos   = pos;
            t->local.scale = scale;
            t->local.quat  = quat;
            t->local.UpdateRotationFromQuaternion();
            t->world = t->local;
            t->world.UpdateMatrix();
        }
            
        break;
    }
    case 0x02: // Health
        {
            // d.read(...);
            break;
        }

        
    default:
        break;
    }
}

void PacketHandler::HandleSnapshot(Serialization::Deserializeration& d, World& world)
{
    uint32_t count;
    if (!d.read(count)) return;

    std::cout << "[CLIENT] Snapshot receive, " << count << " entities " << "\n";
    
    for (uint32_t i = 0; i < count; i++)
    {
        uint32 networkId;
        
        if (!d.read(networkId)) return;

        std::cout << BLUE << networkId << "\n" << RESET;
        EntityId localId = world.CreateEntity();
        world.AddComponent<NetworkIdentity>(localId);
        world.AddComponent<DirtyFlag>(localId);
        world.AddComponent<NetworkInterpolator>(localId);
        world.AddComponent<MeshRenderer>(localId);
        
        NetworkIdentity* identity = world.GetComponent<NetworkIdentity>(localId);
        identity->networkId = networkId;
        
        NetworkRegistry::Get().Register(networkId, localId);

        // Deserialize le transform
        {
            TransformComponent* t = world.GetComponent<TransformComponent>(localId);

            d.read(t->local.matrix);

            t->local.UpdateRotationFromQuaternion();
            t->local.UpdateMatrix();
        }
    }
}

#endif