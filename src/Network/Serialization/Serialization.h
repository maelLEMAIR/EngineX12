#ifndef SERIALIZATION_H_INCLUDED
#define SERIALIZATION_H_INCLUDED

#include "Core/define.h"
#include "Core/Math/Matrix/Matrix.h"

namespace Serialization
{
    static constexpr uint8 BoolTrue = 0x01;
    static constexpr uint8 BoolFalse = 0;
    
    class Serializer
    {
    public:
        Serializer() = default;

        bool writeBytes(const uint8* buffer, size_t nbBytes);
        bool write(uint8 data);
        bool write(uint16 data);
        bool write(uint32 data);
        bool write(bool data);
        bool write(int8 data);
        bool write(int16 data);
        bool write(int32 data); 
        bool write(float32 data);
        bool write(const XMFLOAT4X4& data);
        bool write(const Mat4f32& data);
        template <class CONTAINER>
        bool writeContainer(const CONTAINER& container);

        template<class T>
        bool write(const std::vector<T>& data) { return writeContainer(data); }
        bool write(const std::string& data) { return writeContainer(data); }
        bool write(char data) { return write(*reinterpret_cast<uint8*>(&data)); }

        const uint8* buffer() const { return mBuffer.data(); }
        size_t bufferSize() const { return mBuffer.size(); }
        const Vector<uint8>& GetBuffer() const { return mBuffer; }

    private:
        std::vector<uint8> mBuffer;
    };
}
#include "Serialization.inl"

#endif