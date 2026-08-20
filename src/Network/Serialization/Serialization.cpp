#ifndef SERIALIZATION_CPP_INCLUDED
#define SERIALIZATION_CPP_INCLUDED

#include "Serialization.h"
#include "Conversion.h"

namespace Serialization
{
    bool Serializer::writeBytes(const uint8* buffer, size_t nbBytes)
    {
        mBuffer.insert(mBuffer.cend(), buffer, buffer + nbBytes);
        return true;
    }

    ///////////////////////////////////////////////////////////////
    ///INT
    bool Serializer::write(int8 data)
    {
        return write(*reinterpret_cast<uint8*>(&data));
    }
    bool Serializer::write(int16 data)
    {
        return write(*reinterpret_cast<uint16*>(&data));
    }
    bool Serializer::write(int32 data)
    {
        return write(*reinterpret_cast<uint32*>(&data));
    }

    ///////////////////////////////////////////////////////////////
    ///UINT
    bool Serializer::write(uint8 data)
    {
        return writeBytes(&data, 1);
    }
    
    bool Serializer::write(uint16 data)
    {
        uint16 conv;
        Conversion::ToNetwork(data, conv);
        return writeBytes(reinterpret_cast<const uint8*>(&conv), 2);
    }
    
    bool Serializer::write(uint32 data)
    {
        uint32 conv;
        Conversion::ToNetwork(data, conv);
        return writeBytes(reinterpret_cast<const uint8*>(&conv), 4);
    }
    
    ///////////////////////////////////////////////////////////////
    ///BOOL
    bool Serializer::write(bool data)
    {
        return write(data ? BoolTrue : BoolFalse);
    }

    ///////////////////////////////////////////////////////////////
    ///FLOAT
    bool Serializer::write(float32 data)
    {
        uint32 conv;
        Conversion::ToNetwork(data, conv);
        return writeBytes(reinterpret_cast<const uint8*>(&conv), 4);
    }
    
    bool Serializer::write(const XMFLOAT4X4& data)
    {
        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                if (!write(data.m[i][j]))
                    return false;
            }
        }
        return true;
    }

    bool Serializer::write(const Mat4f32& data)
    {
        for (int i = 0; i < 4; ++i)
        {
            if (!write(data.rows[i].x) || !write(data.rows[i].y) ||
                !write(data.rows[i].z) || !write(data.rows[i].w))
                return false;
        }
        return true;
    }
}

#endif