#ifndef PLAYER_REGISTRY_H_INCLUDED
#define PLAYER_REGISTRY_H_INCLUDED

#include "define.h"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include "Chrono.h"

using EntityId = uint64_t;

struct SockAddrHash
{
    size_t operator()(const sockaddr_in& addr) const
    {
        size_t h1 = Hash<uint32_t>{}(addr.sin_addr.s_addr);
        size_t h2 = Hash<uint16_t>{}(addr.sin_port);
        return h1 ^ (h2 << 32);
    }
};

struct SockAddrEqual
{
    bool operator()(const sockaddr_in& a, const sockaddr_in& b) const
    {
        return a.sin_addr.s_addr == b.sin_addr.s_addr
            && a.sin_port        == b.sin_port;
    }
};

struct PlayerData
{
    EntityId  entityId;
    Chrono lastSeen;
};

class PlayerRegistry
{
public:
    static PlayerRegistry& Get()
    {
        static PlayerRegistry instance;
        return instance;
    }

    void Register(const sockaddr_in& addr, EntityId id)
    {
        PlayerData data;
        data.entityId = id;
        data.lastSeen.Reset();
        m_map[addr] = data;
    }

    void UpdateLastSeen(const sockaddr_in& addr)
    {
        if (m_map.contains(addr))
            m_map[addr].lastSeen.Reset();
    }

    Vector<sockaddr_in> GetTimedOut(float timeoutSeconds) const
    {
        Vector<sockaddr_in> timedOut;

        for (const auto& [addr, data] : m_map)
        {
            if (data.lastSeen.GetElapsedTime() > timeoutSeconds)
                timedOut.push_back(addr);
        }

        return timedOut;
    }
    
    void     Unregister(const sockaddr_in& addr)              { m_map.erase(addr); }
    bool     Has       (const sockaddr_in& addr) const        { return m_map.contains(addr); }
    EntityId GetEntity(const sockaddr_in& addr) const { return m_map.at(addr).entityId; }

private:
    UnorderedMap<sockaddr_in, PlayerData, SockAddrHash, SockAddrEqual> m_map;

};

#endif