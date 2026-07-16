#ifndef NETWORK_SYNC_SYSTEM_H_INCLUDED
#define NETWORK_SYNC_SYSTEM_H_INCLUDED

#include "Engine/ECS/System.h"
#include "Network/NetworkManager.h"
#include "../Packet/PacketBuilder.h"
#include "../NetworkComponentIndex.h"
#include "../NetworkFlag.h"
#include "../NetworkIdentity.h"

class World;

class NetworkSyncSystem : public System
{
public:
    NetworkSyncSystem() = default;
    void SetNetworkManager(NetworkManager* net) { m_net = net; }

    void Update(World& world, float deltaTime) override;

private:
    float m_timer = 0.f;
    
    void SendDirtyComponents(World& world, EntityId id,
                             uint32_t networkId, const DirtyFlag& dirty);

    NetworkManager* m_net = nullptr;
};

#endif