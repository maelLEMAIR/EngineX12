#ifndef NETWORK_CONTEXT_H_INCLUDED
#define NETWORK_CONTEXT_H_INCLUDED

#include "NetworkRole.h"
#include "Packet/PacketDef.h"
#include "NetworkLaunchArgs.h"
#include "Network/NetworkManager.h"

class NetworkContext
{
public:
    static NetworkContext& Get()
    {
        static NetworkContext instance;
        return instance;
    }

    bool Initialize(const NetworkLaunchArgs& args)
    {
        m_role = args.role;
        m_args = args;

        if (m_role == NetworkRole::None) return true;
        
        if (!m_networkManager.Start(args.localPort, args.role == NetworkRole::Server))
            return false;

        if (m_role == NetworkRole::Client)
            m_networkManager.AddPeer(args.serverIp, args.serverPort);

        return true;
    }

    void Disconnect()
    {
        if (m_role != NetworkRole::Client) return;

        Serialization::Serializer s;
        s.write((uint8)PacketType::Disconnect);

        for (const auto& peer : m_networkManager.GetPeers())
            m_networkManager.SendTo(s.GetBuffer(), peer);

        m_networkManager.Stop();
    }

    NetworkRole     GetRole()    const { return m_role; }
    NetworkManager& GetManager()       { return m_networkManager; }

    bool IsServer() const { return m_role == NetworkRole::Server; }
    bool IsClient() const { return m_role == NetworkRole::Client; }

private:
    NetworkRole        m_role = NetworkRole::None;
    NetworkLaunchArgs  m_args;
    NetworkManager     m_networkManager;
};

#endif