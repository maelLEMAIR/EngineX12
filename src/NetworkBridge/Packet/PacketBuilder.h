#ifndef PACKET_BUILDER_H_INCLUDED
#define PACKET_BUILDER_H_INCLUDED

#include "../NetworkIdentity.h"
#include "PacketDef.h"
#include "Engine/ECS/World.h"
#include "Components/TransformComponent.hpp"
#include "../NetworkComponent/INetworkComponent.h"
#include "Components/MeshRenderer.hpp"
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
        world.Query<NetworkIdentity, TransformComponent, MeshRenderer>(
            [&](NetworkIdentity&, TransformComponent&, MeshRenderer&) { count++; });
        s.write(count);

        world.Query<NetworkIdentity, TransformComponent, MeshRenderer>(
            [&](NetworkIdentity& identity, TransformComponent& t, MeshRenderer& mesh)
            {
                s.write(identity.networkId);

                s.write(t.local.pos.x);   s.write(t.local.pos.y);   s.write(t.local.pos.z);
                s.write(t.local.scale.x); s.write(t.local.scale.y); s.write(t.local.scale.z);
                s.write(t.local.quat.x);  s.write(t.local.quat.y);
                s.write(t.local.quat.z);  s.write(t.local.quat.w);
                
                s.write(mesh.geoId);
                s.write(mesh.materialId);
            }
        );

        return s.GetBuffer();
    }
};

#endif