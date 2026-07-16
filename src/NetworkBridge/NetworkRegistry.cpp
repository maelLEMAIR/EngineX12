#ifndef NETWORK_REGISTRY_CPP_INCLUDED
#define NETWORK_REGISTRY_CPP_INCLUDED

#include "NetworkRegistry.h"

void NetworkRegistry::Register(uint32_t networkId, EntityId localId)
{
    m_map[networkId] = localId;
}

void NetworkRegistry::Unregister(uint32_t networkId)
{
    m_map.erase(networkId);
}

EntityId NetworkRegistry::GetLocalId(uint32_t networkId) const
{
    auto it = m_map.find(networkId);
    assert(it != m_map.end() && "NetworkRegistry: networkId inconnu");
    return it->second;
}

bool NetworkRegistry::HasNetworkId(uint32_t networkId) const
{
    for (const auto& it : m_map)
        std::cout << RED << it.first << " | ";

    std::cout << RESET << '\n';
    return m_map.contains(networkId);
}

#endif