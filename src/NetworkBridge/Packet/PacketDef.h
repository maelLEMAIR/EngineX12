#ifndef PACKET_DEF_H_INCLUDED
#define PACKET_DEF_H_INCLUDED

#include "define.h"

enum class PacketType : uint8_t
{
    // Cycle de vie
    EntityCreated   = 0x01,
    EntityDestroyed = 0x02,

    // Composants
    ComponentUpdate = 0x03,
    Snapshot        = 0x04,

    // Futur
    Input           = 0x05,
    Ping            = 0x06,
    Pong            = 0x07,
    Connect         = 0x08,
    Disconnect      = 0x09,
};

struct PacketHeader
{
    PacketType type;
    uint32   networkId;
};

#endif