#ifndef GEOMETRY_H_DEFINED
#define GEOMETRY_H_DEFINED

#include "../../Common/Common.h"

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT2 uv;
    XMFLOAT3 normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    XMFLOAT3 tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
};

enum class PrimitiveTopology
{
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip
};

class Geometry
{
public:
    virtual ~Geometry() = default;
    
    virtual void SetVertexData(const Vertex* _data, uint64 _vertexCount) = 0;
    virtual void SetIndexData(const uint32* _indices, uint64 _indexCount) = 0;
    
    void SetPrimitiveTopology(PrimitiveTopology _topology) { m_primitiveTopology = _topology; }
    
    uint64 GetVertexCount() const { return m_vertexCount; }
    uint64 GetIndexCount() const { return m_indexCount; }
    PrimitiveTopology GetTopology() const { return m_primitiveTopology; }
    bool IsIndexed() const { return m_indexCount > 0; }

    BoundingBox& GetBounds() { return m_bounds; }
protected:
    uint64 m_vertexCount = 0;
    uint64 m_indexCount = 0;
    PrimitiveTopology m_primitiveTopology = PrimitiveTopology::TriangleList;

    BoundingBox m_bounds;

    bool m_isDynamic = false;
    
    Geometry(bool _isDynamic);
    
    void CalculateBounds(const Vertex* _vertices, uint64 _vertexCount);
};
#endif
