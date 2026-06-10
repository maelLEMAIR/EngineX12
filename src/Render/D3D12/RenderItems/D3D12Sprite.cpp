#include "D3D12Sprite.h"

#include "../Base/D3D12Context.h"

D3D12Sprite::D3D12Sprite(D3D12Context* _pContext, bool _isDynamic)
{
    m_pContext = _pContext;
    m_isDynamic = _isDynamic;
    
    m_vertexBufferDynamic = nullptr;
    m_indexBufferDynamic = nullptr;
    m_vertexBufferDefault = nullptr;
    m_indexBufferDefault = nullptr;
}

void D3D12Sprite::SetVertexData(const UiVertex* _data, uint64 _vertexCount)
{
    if (m_isDynamic == false && m_vertexBufferDefault != nullptr) return;
    
    const uint64 vbByteSize = _vertexCount * sizeof(UiVertex);
    m_vertexCount = _vertexCount;
    m_vertexByteStride = sizeof(UiVertex);
    m_vertexBufferByteSize = vbByteSize;

    if (m_isDynamic)
    {
        if (m_vertexBufferDynamic == nullptr)
            m_vertexBufferDynamic = new UploadBuffer<UiVertex>(m_pContext, m_vertexCount, false);

        m_vertexBufferDynamic->Reset();
        m_vertexBufferDynamic->AppendRange(_data, m_vertexCount);
    }
    else
    {
        m_vertexBufferDefault = new DefaultBuffer<UiVertex>(m_pContext, m_vertexCount, _data);       
    }

    CalculateBounds(_data, _vertexCount);
}

void D3D12Sprite::SetIndexData(const uint32* _indices, uint64 _indexCount)
{
    if (m_isDynamic == false && m_indexBufferDynamic != nullptr) return;
    
    const uint64 ibByteSize = _indexCount * sizeof(uint32);
    m_indexCount = _indexCount;
    m_indexBufferByteSize = ibByteSize;

    if (m_isDynamic)
    {
        if (m_indexBufferDynamic == nullptr)
            m_indexBufferDynamic = new UploadBuffer<uint32>(m_pContext, m_indexCount, false);

        m_indexBufferDynamic->Reset();
        m_indexBufferDynamic->AppendRange(_indices, m_indexCount);
    }
    else
    {
        m_indexBufferDefault = new DefaultBuffer<uint32>(m_pContext, m_indexCount, _indices);       
    }
}

D3D12_VERTEX_BUFFER_VIEW D3D12Sprite::VertexBufferView() const
{
    assert((m_isDynamic ? m_vertexBufferDynamic != nullptr : m_vertexBufferDefault != nullptr) && "No data stored in UiVertex Buffer");
    
    D3D12_VERTEX_BUFFER_VIEW vbv;
    vbv.StrideInBytes = (UINT)m_vertexByteStride;
    vbv.SizeInBytes = (UINT)m_vertexBufferByteSize;

    if (m_isDynamic)
        vbv.BufferLocation = m_vertexBufferDynamic->GetGPUAddress(0);
    else
        vbv.BufferLocation = m_vertexBufferDefault->GetGPUAddress();
    
    return vbv;
}

D3D12_INDEX_BUFFER_VIEW D3D12Sprite::IndexBufferView() const
{
    assert((m_isDynamic ? m_indexBufferDynamic != nullptr : m_indexBufferDefault != nullptr) && "No data stored in Index Buffer");
    
    D3D12_INDEX_BUFFER_VIEW ibv;
    ibv.Format = m_indexFormat;
    ibv.SizeInBytes = (UINT)m_indexBufferByteSize;

    if (m_isDynamic)
        ibv.BufferLocation = m_indexBufferDynamic->GetGPUAddress(0);
    else
        ibv.BufferLocation = m_indexBufferDefault->GetGPUAddress();

    return ibv;
}

D3D12Sprite::~D3D12Sprite()
{
    delete m_vertexBufferDynamic;
    delete m_indexBufferDynamic;
    delete m_vertexBufferDefault;
    delete m_indexBufferDefault;
}
