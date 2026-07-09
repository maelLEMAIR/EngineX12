#ifndef PACKET_INPUT_H_INCLUDED
#define PACKET_INPUT_H_INCLUDED
#include "PacketDef.h"
#include "Network/Serialization/Serialization.h"
#include "Network/Serialization/Deserialization.h"

struct InputPacket
{
    // Directions
    bool moveForward  = false;
    bool moveBackward = false;
    bool moveLeft     = false;
    bool moveRight    = false;
    bool jump         = false;

    // Souris
    INT32 mouseDeltaX = 0;
    INT32 mouseDeltaY = 0;
    bool mouseLeft   = false;
    bool mouseRight  = false;

    void Serialize(Serialization::Serializer& s) const
    {
        s.write((uint8)PacketType::Input);
        s.write(moveForward);
        s.write(moveBackward);
        s.write(moveLeft);
        s.write(moveRight);
        s.write(jump);
        s.write(mouseDeltaX);
        s.write(mouseDeltaY);
        s.write(mouseLeft);
        s.write(mouseRight);
    }

    static InputPacket Deserialize(Serialization::Deserializeration& d)
    {
        InputPacket p;
        d.read(p.moveForward);
        d.read(p.moveBackward);
        d.read(p.moveLeft);
        d.read(p.moveRight);
        d.read(p.jump);
        d.read(p.mouseDeltaX);
        d.read(p.mouseDeltaY);
        d.read(p.mouseLeft);
        d.read(p.mouseRight);
        return p;
    }
};

#endif