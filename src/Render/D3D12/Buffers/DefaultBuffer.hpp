#ifndef DEFAULTBUFFER_H_DEFINED
#define DEFAULTBUFFER_H_DEFINED

#include "DefaultBufferBase.hpp"

template <typename T>
class DefaultBuffer : public DefaultBufferBase
{
public:
    DefaultBuffer(D3D12Context* _pContext, uint64 _elementCount, const T* _data) : DefaultBufferBase(_pContext, sizeof(T),_elementCount, _data)
    {
        
    }
};

#endif