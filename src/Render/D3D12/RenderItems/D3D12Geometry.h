#ifndef D3D12GEOMETRY_H_DEFINED
#define D3D12GEOMETRY_H_DEFINED

#include "../../Generic/RenderItems/Geometry.h"
#include "../define.h"
#include "../Buffers/UploadBuffer.hpp"
#include "../Buffers/DefaultBuffer.hpp"

class D3D12Context;

class D3D12Geometry : public Geometry
{
public:
    void SetVertexData(const Vertex* _data, uint64 _vertexCount);
    void SetIndexData(const uint32* _indices, uint64 _indexCount);

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
    D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

    ~D3D12Geometry() override;
private:
    UploadBuffer<Vertex>* m_vertexBufferDynamic;
    UploadBuffer<uint32>* m_indexBufferDynamic;

    DefaultBuffer<Vertex>* m_vertexBufferDefault;
    DefaultBuffer<uint32>* m_indexBufferDefault;

    uint64 m_vertexByteStride = sizeof(Vertex);
    uint64 m_vertexBufferByteSize = 0;
    DXGI_FORMAT m_indexFormat = DXGI_FORMAT_R32_UINT;
    uint64 m_indexBufferByteSize = 0;

    D3D12Context* m_pContext = nullptr;
    
    D3D12Geometry(D3D12Context* _pContext, bool _isDynamic = false);
    
    friend class D3D12Device;
};

#endif
