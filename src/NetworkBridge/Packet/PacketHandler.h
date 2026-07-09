#ifndef PACKET_HANDLER_H_INCLUDED
#define PACKET_HANDLER_H_INCLUDED

#include "../Network/Serialization/Deserialization.h"
#include "../Network/NetworkQueue.h"

class World;

class PacketHandler
{
public:
    void Handle(const NetworkPacket& packet, World& world);
    void SetDeltaTime(float dt) { m_deltaTime = dt; }

private:
    float m_deltaTime = 0.f;
    
    void HandleEntityCreated  (Serialization::Deserializeration& d, World& world);
    void HandleEntityDestroyed(Serialization::Deserializeration& d, World& world);
    void HandleInput(Serialization::Deserializeration& d, World& world, const sockaddr_in& from);
    void HandleComponentUpdate(Serialization::Deserializeration& d, World& world);
    void HandleSnapshot(Serialization::Deserializeration& d, World& world);
    void HandleDisconnect(Serialization::Deserializeration& d, World& world, const sockaddr_in& from);
};

#endif