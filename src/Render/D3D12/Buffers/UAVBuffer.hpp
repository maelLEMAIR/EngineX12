#pragma once

#include "../../Common/Common.h"
#include "../define.h"
#include "../Base/D3D12Context.h"
#include "../Utils/d3dUtil.h"

class UAVBuffer
{
public:
    template<typename T>
    UAVBuffer(D3D12Context* _pContext, Vector<T> const& _data)
        : m_pContext(_pContext)
        , m_elementByteSize(sizeof(T))
        , m_elementCount(static_cast<uint64>(_data.size()))
    {
        CreateGPUBuffer();
        Upload(_data.data(), _data.size() * sizeof(T));
    }
    
    template<typename T>
    UAVBuffer(D3D12Context* _pContext, uint64 _count, T const& _defaultValue = T{})
        : m_pContext(_pContext)
        , m_elementByteSize(sizeof(T))
        , m_elementCount(_count)
    {
        CreateGPUBuffer();
        Vector<T> temp(_count, _defaultValue);
        Upload(temp.data(), temp.size() * sizeof(T));
    }

    UAVBuffer(D3D12Context* _pContext, uint64 _count, const void* _data, uint64 _elementByteSize)
        : m_pContext(_pContext)
        , m_elementByteSize(_elementByteSize)
        , m_elementCount(_count)
    {
        CreateGPUBuffer();
        Upload(_data, _count * _elementByteSize);
    }

    UAVBuffer(D3D12Context* _pContext, uint64 _count, uint64 _elementByteSize, const void* _defaultValue = nullptr)
        : m_pContext(_pContext)
        , m_elementByteSize(_elementByteSize)
        , m_elementCount(_count)
    {
        CreateGPUBuffer();
        Vector<uint8> temp(_count * _elementByteSize, 0);
        if (_defaultValue != nullptr)
        {
            for (uint64 i = 0; i < _count; ++i)
                memcpy(temp.data() + i * _elementByteSize, _defaultValue, _elementByteSize);
        }
        Upload(temp.data(), temp.size());
    }

    UAVBuffer(const UAVBuffer&)            = delete;
    UAVBuffer& operator=(const UAVBuffer&) = delete;
    UAVBuffer(UAVBuffer&&)                 = delete;
    UAVBuffer& operator=(UAVBuffer&&)      = delete;

    ~UAVBuffer()
    {
        if (m_buffer)        { m_buffer->Release();        m_buffer        = nullptr; }
        if (m_uploadBuffer)  { m_uploadBuffer->Release();  m_uploadBuffer  = nullptr; }
        if (m_readbackBuffer){ m_readbackBuffer->Release(); m_readbackBuffer = nullptr; }
    }
    
    void SetOutput()
    {
        Transition(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    }
    
    void SetInput()
    {
        Transition(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    }
    
    void SetCopy()
    {
        Transition(D3D12_RESOURCE_STATE_COPY_SOURCE);
    }
    
    void SetFree()
    {
        Transition(D3D12_RESOURCE_STATE_COMMON);
    }
    
    void FlushWrites()
    {
        auto barrier = CD3DX12_RESOURCE_BARRIER::UAV(m_buffer);
        m_pContext->GetCommandList()->ResourceBarrier(1, &barrier);
    }
    
    template<typename T>
    void ReadbackToCPU(Vector<T>& _outData)
    {
        const uint64 byteSize = m_elementByteSize * m_elementCount;
        
        if (!m_readbackBuffer)
            CreateReadbackBuffer(byteSize);
        
        m_pContext->GetCommandList()->CopyBufferRegion(
            m_readbackBuffer, 0, m_buffer, 0, byteSize);
        
        m_pContext->FlushCommandQueue();
        
        _outData.resize(m_elementCount);
        void* pMapped = nullptr;
        CD3DX12_RANGE readRange(0, byteSize);
        ThrowIfFailed(m_readbackBuffer->Map(0, &readRange, &pMapped));
        memcpy(_outData.data(), pMapped, byteSize);
        m_readbackBuffer->Unmap(0, nullptr);
    }

    ID3D12Resource*           Resource()      const { return m_buffer;           }
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUAddress() const { return m_buffer->GetGPUVirtualAddress(); }
    uint64                    GetCount()      const { return m_elementCount;      }
    uint64                    GetByteSize()   const { return m_elementByteSize * m_elementCount; }
    uint64                    GetStride()     const { return m_elementByteSize;   }
    D3D12_RESOURCE_STATES     GetState()      const { return m_currentState;      }

private:

    D3D12Context*         m_pContext        = nullptr;

    ID3D12Resource*       m_buffer          = nullptr; 
    ID3D12Resource*       m_uploadBuffer    = nullptr; 
    ID3D12Resource*       m_readbackBuffer  = nullptr; 

    D3D12_RESOURCE_STATES m_currentState    = D3D12_RESOURCE_STATE_COMMON;
    uint64                m_elementByteSize = 0;
    uint64                m_elementCount    = 0;
    
    void CreateGPUBuffer()
    {
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC   desc = CD3DX12_RESOURCE_DESC::Buffer(
            m_elementByteSize * m_elementCount,
            D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

        ThrowIfFailed(m_pContext->GetDevice()->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COMMON,
            nullptr,
            IID_PPV_ARGS(&m_buffer)));

        m_currentState = D3D12_RESOURCE_STATE_COMMON;
    }
    
    void Upload(const void* _data, uint64 _byteSize)
    {
        // Create upload (staging) buffer
        CD3DX12_HEAP_PROPERTIES uploadHeap(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC   uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(_byteSize);

        ThrowIfFailed(m_pContext->GetDevice()->CreateCommittedResource(
            &uploadHeap,
            D3D12_HEAP_FLAG_NONE,
            &uploadDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_uploadBuffer)));
        
        void* pMapped = nullptr;
        CD3DX12_RANGE readRange(0, 0);
        ThrowIfFailed(m_uploadBuffer->Map(0, &readRange, &pMapped));
        memcpy(pMapped, _data, _byteSize);
        m_uploadBuffer->Unmap(0, nullptr);
        
        Transition(D3D12_RESOURCE_STATE_COPY_DEST);
        m_pContext->GetCommandList()->CopyBufferRegion(
            m_buffer, 0, m_uploadBuffer, 0, _byteSize);
        Transition(D3D12_RESOURCE_STATE_COMMON);
        
        m_pContext->FlushCommandQueue();
    }

    void CreateReadbackBuffer(uint64 _byteSize)
    {
        CD3DX12_HEAP_PROPERTIES readbackHeap(D3D12_HEAP_TYPE_READBACK);
        CD3DX12_RESOURCE_DESC   desc = CD3DX12_RESOURCE_DESC::Buffer(_byteSize);

        ThrowIfFailed(m_pContext->GetDevice()->CreateCommittedResource(
            &readbackHeap,
            D3D12_HEAP_FLAG_NONE,
            &desc,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&m_readbackBuffer)));
    }
    
    void Transition(D3D12_RESOURCE_STATES _newState)
    {
        if (m_currentState == _newState) return;

        auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_buffer, m_currentState, _newState);
        m_pContext->GetCommandList()->ResourceBarrier(1, &barrier);
        m_currentState = _newState;
    }
};