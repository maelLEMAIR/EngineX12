#ifndef NETWORK_MANAGER_H_INCLUDED
#define NETWORK_MANAGER_H_INCLUDED

#include "Core/define.h"
#include "Engine/define.h"
#include "Serialization/Serialization.h"

#include "NetworkSocket.h"
#include "NetworkQueue.h"

class World;

class NetworkManager
{
public:
    ~NetworkManager() { Stop(); }

    bool Start(uint16_t localPort, bool isServer = false);
    void Stop();

    void SendTo(const std::vector<uint8_t>& data, const sockaddr_in& dest);
    bool PopReceived(NetworkPacket& out);

    void AddPeer(const std::string& ip, uint16_t port);
    void AddPeerAddress(const sockaddr_in& addr)
    {
        m_peers.push_back(addr);
    }

    void RemovePeer(const sockaddr_in& addr);

    const Vector<sockaddr_in>& GetPeers() const { return m_peers; }

private:
    void NetworkLoop();

    NetworkSocket           m_socket;
    NetworkQueue            m_inQueue;
    NetworkQueue            m_outQueue;

    std::thread             m_thread;
    std::atomic<bool>       m_running = false;

    std::vector<sockaddr_in> m_peers;

    static constexpr size_t BUFFER_SIZE = 4096;
};

#endif