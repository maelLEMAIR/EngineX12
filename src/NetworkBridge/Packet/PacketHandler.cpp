#ifndef PACKET_HANDLER_CPP_INCLUDED
#define PACKET_HANDLER_CPP_INCLUDED

#include "PacketHandler.h"

#include "NetworkContext.h"
#include "NetworkFlag.h"
#include "NetworkIdentity.h"
#include "NetworkManager.h"
#include "PacketDef.h"
#include "PacketInput.h"
#include "PlayerRegistry.h"
#include "../NetworkRegistry.h"
#include "../Engine/ECS/World.h"
#include "../Engine/Components/TransformComponent.hpp"
#include "../ComponentDispatcher.h"

static void ApplyComponent(Serialization::Deserializeration& d, World& world, EntityId localId, uint32_t componentId);
void ApplyInput(const InputPacket& input, World& world);

void PacketHandler::Handle(const NetworkPacket& packet, World& world)
{
    Serialization::Deserializeration d(packet.data.data(), packet.data.size());

    uint8_t typeRaw;
    if (!d.read(typeRaw)) return;

    PacketType type = static_cast<PacketType>(typeRaw);

    switch (type)
    {
        case PacketType::EntityCreated:   HandleEntityCreated  (d, world);          break;
        case PacketType::EntityDestroyed: HandleEntityDestroyed(d, world);          break;
        case PacketType::ComponentUpdate: HandleComponentUpdate(d, world);          break;
        case PacketType::Input:           HandleInput(d, world, packet.address);    break;
        case PacketType::Snapshot:        HandleSnapshot       (d, world); 
        case PacketType::Connect:                                                         break;
        case PacketType::Disconnect: HandleDisconnect(d, world, packet.address);          break;
        default:
            break;
    }
}

void PacketHandler::HandleEntityCreated(Serialization::Deserializeration& d, World& world)
{
    uint32_t networkId, componentId;
    if (!d.read(networkId) || !d.read(componentId)) return;

    EntityId localId = world.CreateEntity();
    NetworkRegistry::Get().Register(networkId, localId);

    auto& identity    = world.AddComponent<NetworkIdentity>(localId);
    identity.networkId = networkId;
    identity.isOwner   = false;

    ApplyComponent(d, world, localId, componentId);
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

    if (!t || !flag) return;

    float speed = 50.0f * m_deltaTime;
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

    EntityId localId = NetworkRegistry::Get().GetLocalId(networkId);

    switch (componentId)
    {
    case 0x01:
        {
            TransformComponent* t = world.GetComponent<TransformComponent>(localId);
            if (!t) return;
            
            d.read(t->local.pos.x);   d.read(t->local.pos.y);   d.read(t->local.pos.z);
            d.read(t->local.scale.x); d.read(t->local.scale.y); d.read(t->local.scale.z);
            d.read(t->local.quat.x);  d.read(t->local.quat.y);
            d.read(t->local.quat.z);  d.read(t->local.quat.w);
            t->local.UpdateRotationFromQuaternion();
            t->local.UpdateMatrix();
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

    for (uint32_t i = 0; i < count; i++)
    {
        uint32_t networkId;
        bool     isOwner;

        if (!d.read(networkId) || !d.read(isOwner)) return;
        
        EntityId localId = world.CreateEntity();
        world.AddComponent<TransformComponent>(localId);
        world.AddComponent<NetworkIdentity>(localId);
        world.AddComponent<DirtyFlag>(localId);

        NetworkIdentity* identity = world.GetComponent<NetworkIdentity>(localId);
        identity->networkId = networkId;
        identity->isOwner   = isOwner;

        NetworkRegistry::Get().Register(networkId, localId);

        // Désérialiser le transform
        {
            TransformComponent* t = world.GetComponent<TransformComponent>(localId);

            d.read(t->local.matrix);

            t->local.UpdateRotationFromQuaternion();
            t->local.UpdateMatrix();
        }
    }
}

void PacketHandler::HandleDisconnect(Serialization::Deserializeration& d, World& world, const sockaddr_in& from)
{
    if (!PlayerRegistry::Get().Has(from)) return;

    EntityId localId = PlayerRegistry::Get().GetEntity(from);

    NetworkIdentity* identity = world.GetComponent<NetworkIdentity>(localId);
    NetworkManager& net = NetworkContext::Get().GetManager();
    if (identity)
    {
        uint32_t netId = identity->networkId;
        NetworkRegistry::Get().Unregister(netId);

        Serialization::Serializer s;
        s.write((uint8)PacketType::EntityDestroyed);
        s.write(netId);
        
        for (const auto& peer : net.GetPeers())
        {
            if (peer.sin_addr.s_addr == from.sin_addr.s_addr &&
                peer.sin_port        == from.sin_port) continue;
            net.SendTo(s.GetBuffer(), peer);
        }
    }

    PlayerRegistry::Get().Unregister(from);
    net.RemovePeer(from);
    world.DestroyEntity(localId);

    std::cout << "[SERVER] Client déconnecté\n";
}

static void ApplyComponent(Serialization::Deserializeration& d,
                           World& world, EntityId localId, uint32_t componentId)
{
    ComponentDispatcher::Get().Apply(componentId, d, world, localId);
}

#endif