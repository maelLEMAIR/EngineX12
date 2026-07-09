#ifndef PACKET_BUILDER_H_INCLUDED
#define PACKET_BUILDER_H_INCLUDED

#include "../NetworkIdentity.h"
#include "PacketDef.h"
#include "Engine/ECS/World.h"
#include "Components/TransformComponent.hpp"
#include "../NetworkComponent/INetworkComponent.h"
#include "Network/Serialization/Serialization.h"

class PacketBuilder
{
public:
    static Vector<uint8> EntityCreated(uint32_t networkId,
                                               uint32_t componentId,
                                               const INetworkComponent& comp)
    {
        Serialization::Serializer s;
        s.write((uint8_t)PacketType::EntityCreated);
        s.write(networkId);
        s.write(componentId);
        comp.Serialize(s);
        return s.GetBuffer();
    }

    static std::vector<uint8_t> EntityDestroyed(uint32_t networkId)
    {
        Serialization::Serializer s;
        s.write((uint8_t)PacketType::EntityDestroyed);
        s.write(networkId);
        return s.GetBuffer();
    }

    static std::vector<uint8_t> ComponentUpdate(uint32_t networkId,
                                                 uint32_t componentId,
                                                 const INetworkComponent& comp)
    {
        Serialization::Serializer s;
        s.write((uint8_t)PacketType::ComponentUpdate);
        s.write(networkId);
        s.write(componentId);
        comp.Serialize(s);
        return s.GetBuffer();
    }

    static Vector<uint8> Snapshot(World& world)
    {
        Serialization::Serializer s;
        s.write((uint8)PacketType::Snapshot);

        uint32_t count = 0;
        world.Query<NetworkIdentity>([&](NetworkIdentity&) { count++; });
        s.write(count);

        world.QueryWithEntity<NetworkIdentity, TransformComponent>(
            [&](EntityId id, NetworkIdentity& identity, TransformComponent& t)
            {
                s.write(identity.networkId);
                s.write(identity.isOwner);

                s.write(t.local.matrix);
            }
        );

        return s.GetBuffer();
    }
};

#endif