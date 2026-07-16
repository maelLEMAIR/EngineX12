#ifndef PING_MANAGER_H_INCLUDED
#define PING_MANAGER_H_INCLUDED

#include "Core/Chrono.h"
#include "Network/Serialization/Serialization.h"
#include "Network/NetworkManager.h"
#include "Packet/PacketDef.h"

class PingManager
{
public:
    static PingManager& Get()
    {
        static PingManager instance;
        return instance;
    }

    void Update(float dt, NetworkManager& net)
    {
        m_timer += dt;

        if (m_timer >= PING_INTERVAL)
        {
            m_timer = 0.f;
            SendPing(net);
        }
    }

    void OnPongReceived()
    {
        float rtt = m_pingTime.GetElapsedTime() * 1000.f;
        m_latency = rtt * 0.5f;

        std::cout << "[PING] Latency: " << m_latency << " ms\n";
    }

    float GetLatency() const
    {
        return m_latency;
    }

private:
    void SendPing(NetworkManager& net)
    {
        m_pingTime.Reset();

        Serialization::Serializer s;
        s.write((uint8_t)PacketType::Ping);

        for (const auto& peer : net.GetPeers())
            net.SendTo(s.GetBuffer(), peer);
    }

    static constexpr float PING_INTERVAL = 1.f;

    Chrono m_pingTime;
    float m_timer = 0.f;
    float m_latency = 0.f;
};

#endif