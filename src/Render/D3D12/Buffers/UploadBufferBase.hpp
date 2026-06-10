#ifndef UPPLOADBUFFERBASE_H_DEFINED
#define UPPLOADBUFFERBASE_H_DEFINED

#include "../../Common/Common.h"
#include "../define.h"

#include "../Utils/d3dUtil.h"

#include "../Base/D3D12Context.h"

class UploadBufferBase
{
public:
    UploadBufferBase(D3D12Context* _pContext, UINT32 _elementSize, uint64 _elementCount, bool _isConstantBuffer = true) : 
        m_isConstantBuffer(_isConstantBuffer), m_elementCount(_elementCount), m_elementByteSize(_elementSize), m_pContext(_pContext)
    {
        if(_isConstantBuffer)
            m_elementByteSize = d3dUtil::CalcConstantBufferByteSize(_elementSize);

        CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_elementByteSize * _elementCount);

        CD3DX12_RANGE readRange(0, 0);
        
        ThrowIfFailed(m_pContext->GetDevice()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_uploadBuffer)));
            ThrowIfFailed(m_uploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedData)));
    }
    
    UploadBufferBase(const UploadBufferBase& rhs) = delete;
    UploadBufferBase& operator=(const UploadBufferBase& rhs) = delete;
    UploadBufferBase(UploadBufferBase&& rhs) = delete;
    UploadBufferBase& operator=(UploadBufferBase&& rhs) = delete;
    
    ~UploadBufferBase()
    {
        if(m_uploadBuffer != nullptr)
        {
            if (m_mappedData != nullptr)
                m_uploadBuffer->Unmap(0, nullptr);
            m_uploadBuffer->Release();
        }
        m_mappedData = nullptr;
    }

    void Resize()
    {
        uint64 oldCount = m_elementCount;
        uint64 oldByteSize = m_elementByteSize * oldCount;
        std::vector<BYTE> temp(oldByteSize);
        memcpy(temp.data(), m_mappedData, oldByteSize);

        m_pContext->FlushCommandQueue();
        
        // Now safe to release
        if (m_uploadBuffer != nullptr)
        {
            m_uploadBuffer->Unmap(0, nullptr);
            m_uploadBuffer->Release();
            m_uploadBuffer = nullptr;
            m_mappedData = nullptr;
        }

        m_elementCount *= 2;
        
        m_mappedData = nullptr;
        
        CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_elementByteSize * m_elementCount);

        CD3DX12_RANGE readRange(0, 0);
        
        ThrowIfFailed(m_pContext->GetDevice()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_uploadBuffer)));
        ThrowIfFailed(m_uploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedData)));

        memcpy(m_mappedData, temp.data(), oldByteSize);
    }
    
    ID3D12Resource* Resource()const
    {
        return m_uploadBuffer;
    }

    void Reset()
    {
        m_firstAvailableIndex = 0;
    }
    
    void CopyRawData(uint64 elementIndex, const void* data)
    {
        assert(elementIndex < m_elementCount && "Element index out of bounds");
        assert(m_mappedData != nullptr && "Buffer is not mapped");
        assert(reinterpret_cast<uintptr_t>(m_mappedData) > 0xFFFF && "Mapped pointer looks invalid");
        assert(elementIndex * m_elementByteSize + m_elementByteSize <= m_elementByteSize * m_elementCount);

        assert(*reinterpret_cast<const volatile BYTE*>(data) || true);               // first byte
        assert(*reinterpret_cast<const volatile BYTE*>((const BYTE*)data + m_elementByteSize - 1) || true); // last byte
        
        m_firstAvailableIndex = MathHelper::Max(elementIndex + 1, m_firstAvailableIndex);
        
        memcpy(&m_mappedData[elementIndex * m_elementByteSize], data, 
        m_elementByteSize);
    }

    uint64 AppendRaw(const void* _data)
    {
        if (m_firstAvailableIndex >= m_elementCount)
            Resize();

        uint64 index = m_firstAvailableIndex;
        memcpy(&m_mappedData[index * m_elementByteSize], _data, m_elementByteSize);
        m_firstAvailableIndex += 1;
        return index;
    }

    uint64 AppendRangeRaw(const void* _data, const uint64 _count)
    {
        while (m_firstAvailableIndex + _count > m_elementCount)
            Resize();

        uint64 index = m_firstAvailableIndex;
        memcpy(&m_mappedData[index * m_elementByteSize], _data, _count * m_elementByteSize);
        m_firstAvailableIndex += _count;
        return index;
    }

    uint64 GetElementByteSize() const
    {
        return m_elementByteSize;
    }
    
    uint64 GetCapacity() const
    {
        return m_elementCount;
    }

    uint64 GetFirstAvailable() const
    {
        return m_firstAvailableIndex;
    }
    
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress(uint64 index) const
    {
        assert(index < m_elementCount && "Index out of bounds");
        return m_uploadBuffer->GetGPUVirtualAddress() + (index * m_elementByteSize);
    }
    
protected:
    ID3D12Resource* m_uploadBuffer = nullptr;
    D3D12Context* m_pContext;
    
    BYTE* m_mappedData = nullptr;
    uint64 m_elementByteSize = 0;
    uint64 m_elementCount = 0;
    uint64 m_firstAvailableIndex = 0;
    bool m_isConstantBuffer = false;
};

#endif