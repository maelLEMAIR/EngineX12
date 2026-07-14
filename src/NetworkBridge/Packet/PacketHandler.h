#ifndef PACKET_HANDLER_H_INCLUDED
#define PACKET_HANDLER_H_INCLUDED

#include "Network/NetworkManager.h"
#include "../Network/Serialization/Deserialization.h"
#include "../Network/NetworkQueue.h"

class World;

class PacketHandler
{
public:
    void Handle(const NetworkPacket& packet, World& world);
    void SetDeltaTime(float dt)                 { m_deltaTime = dt; }
    void SetCurrentTime(float t)                { m_currentTime = t; }
    void SetNetworkManager(NetworkManager* net) { m_networkManager = net; }
    
private:
    NetworkManager* m_networkManager = nullptr;
    
    float m_deltaTime = 0.f;
    float m_currentTime = 0.f;
    
    void HandleConnected(Serialization::Deserializeration& d, World& world, const sockaddr_in& from);
    void HandleDisconnect(Serialization::Deserializeration& d, World& world, const sockaddr_in& from);
    void HandlePing(Serialization::Deserializeration& d, const sockaddr_in& from);
    void HandlePong(Serialization::Deserializeration& d);
    
    void HandleEntityCreated  (Serialization::Deserializeration& d, World& world);
    void HandleEntityDestroyed(Serialization::Deserializeration& d, World& world);
    
    void HandleInput(Serialization::Deserializeration& d, World& world, const sockaddr_in& from);
    void HandleComponentUpdate(Serialization::Deserializeration& d, World& world);
    
    void HandleSnapshot(Serialization::Deserializeration& d, World& world);
};

#endif