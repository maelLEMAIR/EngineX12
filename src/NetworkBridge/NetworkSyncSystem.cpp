#ifndef NETWORK_SYNC_SYSTEM_CPP_INCLUDED
#define NETWORK_SYNC_SYSTEM_CPP_INCLUDED

#include "NetworkSyncSystem.h"
#include "Engine/ECS/World.h"
#include "Engine/Components/TransformComponent.hpp"

void NetworkSyncSystem::Update(World& world, float deltaTime)
{
    m_timer += deltaTime;
    if (m_timer < 1.f / 20.f) return;
    m_timer = 0.f;
    
    world.QueryWithEntity<NetworkIdentity, DirtyFlag>(
        [&](EntityId id, NetworkIdentity& identity, DirtyFlag& dirty)
        {
            if (!dirty.AnyDirty()) return;

            SendDirtyComponents(world, id, identity.networkId, dirty);
            dirty.ClearAll();
        }
    );
}

void NetworkSyncSystem::SendDirtyComponents(World& world, EntityId id,
                                            uint32_t networkId, const DirtyFlag& dirty)
{
    if (dirty.IsDirty(0))
    {
        std::cout << "[SERVER] Send transform networkId=" << networkId << "\n";
        
        TransformComponent* t = world.GetComponent<TransformComponent>(id);
        if (t)
        {
            Serialization::Serializer s;
            s.write((uint8)PacketType::ComponentUpdate);
            s.write(networkId);
            s.write((uint32)0x01);

            s.write(t->local.GetPosition().x);   s.write(t->local.GetPosition().y);   s.write(t->local.GetPosition().z);
            s.write(t->local.GetScale().x); s.write(t->local.GetScale().y); s.write(t->local.GetScale().z);
            s.write(t->local.GetRotation().x);  s.write(t->local.GetRotation().y);
            s.write(t->local.GetRotation().z);  s.write(t->local.GetRotation().w);

            for (const auto& peer : m_net->GetPeers())
                m_net->SendTo(s.GetBuffer(), peer);
        }
    }

    // Health (index 1)
    // if (dirty.IsDirty(1)) { ... }
}

#endif