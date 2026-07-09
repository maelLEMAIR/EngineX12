#ifndef DESERIALIZATION_CPP_INCLUDED
#define DESERIALIZATION_CPP_INCLUDED

#include "Serialization.h"
#include "Deserialization.h"
#include "Conversion.h"

namespace Serialization
{
    bool Deserializeration::readBytes(size_t nbBytes, uint8* buffer)
    {
        if (remainingBytes() < nbBytes)
            return false;

        for (size_t i = 0; i < nbBytes; ++i)
            buffer[i] = mBuffer[mBytesRead + i];

        mBytesRead += nbBytes;
        return true;
    }

    bool Deserializeration::read(uint8& data)
    {
        return readBytes(1, &data);
    }
    
    bool Deserializeration::read(uint16& data)
    {
        uint8 bytesRead[2];
        if (!readBytes(2, bytesRead))
            return false;
        uint16 raw;
        memcpy(&raw, bytesRead, sizeof(raw));
        Conversion::ToLocal(raw, data);
        return true;
    }
    
    bool Deserializeration::read(uint32& data)
    {
        uint8 bytesRead[4];
        if (!readBytes(4, bytesRead))
            return false;
        uint32 raw;
        memcpy(&raw, bytesRead, sizeof(raw));
        Conversion::ToLocal(raw, data);
        return true;
    }

    bool Deserializeration::read(bool& data)
    {
        uint8 byteRead;
        if (!readBytes(1, &byteRead))
            return false;
        data = (byteRead == BoolTrue);
        return true;
    }

    bool Deserializeration::read(int8& data)
    {
        return read(reinterpret_cast<uint8&>(data));
    }
    bool Deserializeration::read(int16& data)
    {
        return read(reinterpret_cast<uint16&>(data));
    }
    bool Deserializeration::read(int32& data)
    {
        return read(reinterpret_cast<uint32&>(data));
    } 

    bool Deserializeration::read(float32& data)
    {
        uint8 bytesRead[4];
        if (!readBytes(4, bytesRead))
            return false;
        uint32 raw;
        memcpy(&raw, bytesRead, sizeof(raw));
        Conversion::ToLocal(raw, data);
        return true;
    }
    bool Deserializeration::read(XMFLOAT4X4& data)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (!read(data.m[i][j]))
                    return false;
            }
        }
        return true;
    }
}

#endif