#ifndef NETWORK_REGISTRY_H_INCLUDED
#define NETWORK_REGISTRY_H_INCLUDED
#include "Core/define.h"
#include "Engine/define.h"

class NetworkRegistry
{
public:
    static NetworkRegistry& Get()
    {
        static NetworkRegistry instance;
        return instance;
    }

    void     Register(uint32 networkId, EntityId localId);
    void     Unregister(uint32 networkId);
    EntityId GetLocalId(uint32 networkId) const;
    bool     HasNetworkId(uint32 networkId) const;

    uint32 GenerateNetworkId() { return m_nextId++; }

private:
    std::unordered_map<uint32, EntityId> m_map;
    uint32 m_nextId = 1;
};

#endif