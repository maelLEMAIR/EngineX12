#ifndef UPPLOADBUFFER_H_DEFINED
#define UPPLOADBUFFER_H_DEFINED

#include "../define.h"
#include "../Utils/d3dUtil.h"
#include "UploadBufferBase.hpp"

#include "../Base/D3D12Context.h"

template <typename T>
class UploadBuffer : public UploadBufferBase
{
public:
    UploadBuffer(D3D12Context* _pContext, uint64 _elementCount, bool _isConstantBuffer = true) : 
        UploadBufferBase(_pContext, sizeof(T), _elementCount, _isConstantBuffer)
    {
        
    }

    void CopyData(uint64 _elementIndex, const T& _data)
    {
        CopyRawData(_elementIndex, &_data);
    }

    uint64 Append(const T& _data)
    {
        return AppendRaw(&_data);
    }
    
    uint64 AppendRange(const T* _data, uint64 _count)
    {
        return AppendRangeRaw(_data, _count);
    }
};

#endif
