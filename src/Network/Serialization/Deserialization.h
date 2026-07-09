#ifndef DESERIALIZATION_H_INCLUDED
#define DESERIALIZATION_H_INCLUDED

#include "define.h"

namespace Serialization
{
    class Deserializeration
    {
    public:
        Deserializeration(const uint8* buffer, const size_t bufferSize) : mBuffer(buffer) , mBufferSize(bufferSize) {}

        bool read(uint8& data);
        bool read(uint16& data);
        bool read(uint32& data);
        bool read(bool& data);
        bool read(int8& data);
        bool read(int16& data);
        bool read(int32& data); 
        bool read(float32& data);
        bool read(XMFLOAT4X4& data);
        bool read(char& data) { return read(reinterpret_cast<uint8&>(data)); }
        bool read(std::string& data) { return readContainer(data); }
        
        template<class T>
        bool read(std::vector<T>& data) { return readContainer(data); }

        inline size_t remainingBytes() const { return mBufferSize - mBytesRead; }

    private:
        bool readBytes(size_t nbBytes, uint8* buffer);
        template<class CONTAINER>
        bool readContainer(CONTAINER& container);

    private:
        const uint8* mBuffer;
        const size_t mBufferSize;
        size_t mBytesRead{ 0 };
    };

}
#include "Deserialization.inl"

#endif