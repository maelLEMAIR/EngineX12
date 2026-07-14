#ifndef NETWORK_MANAGER_CPP_INCLUDED
#define NETWORK_MANAGER_CPP_INCLUDED

#include "NetworkManager.h"
#include "Engine/ECS/World.h"


bool NetworkManager::Start(uint16_t localPort, bool isServer)
{
    std::cout << "[NET] Start on port " << localPort << "\n";
    if (!m_socket.Bind(localPort))
    {
        std::cout << "[NET] Bind failed " << "\n";
        return false;
    }

    std::cout << "[NET] Thread network started " << "\n";
    m_running = true;
    m_thread  = std::thread(&NetworkManager::NetworkLoop, this);
    return true;
}

void NetworkManager::Stop()
{
    m_running = false;
    m_socket.Close();
    if (m_thread.joinable())
        m_thread.join();
}

void NetworkManager::AddPeer(const std::string& ip, uint16_t port)
{
    m_peers.push_back(NetworkSocket::MakeAddress(ip, port));
}

void NetworkManager::RemovePeer(const sockaddr_in& addr)
{
    m_peers.erase(
        std::remove_if(m_peers.begin(), m_peers.end(),
            [&addr](const sockaddr_in& p) {
                return p.sin_addr.s_addr == addr.sin_addr.s_addr
                    && p.sin_port        == addr.sin_port;
            }),
        m_peers.end()
    );
}

void NetworkManager::SendTo(const std::vector<uint8_t>& data, const sockaddr_in& dest)
{
    NetworkPacket packet;
    packet.data    = data;
    packet.address = dest;
    m_outQueue.Push(std::move(packet));
}

bool NetworkManager::PopReceived(NetworkPacket& out)
{
    return m_inQueue.Pop(out);
}

void NetworkManager::NetworkLoop()
{
    uint8_t buffer[BUFFER_SIZE];

    while (m_running)
    {
        NetworkPacket toSend;
        while (m_outQueue.Pop(toSend))
        {
            std::cout << "[NET] Send of " << toSend.data.size() << " bytes\n";
            m_socket.SendTo(toSend.data.data(), toSend.data.size(), toSend.address);
        }

        sockaddr_in from{};
        int received = m_socket.RecvFrom(buffer, BUFFER_SIZE, from);
        if (received > 0)
        {
            std::cout << "[NET] Receive " << received << " bytes\n";
            NetworkPacket packet;
            packet.data    = Vector<UINT8>(buffer, buffer + received);
            packet.address = from;
            m_inQueue.Push(std::move(packet));
        }
    }
}

#endif