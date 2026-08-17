#ifndef GEOMETRY_H_DEFINED
#define GEOMETRY_H_DEFINED

#include "../../Common/Common.h"

#include "../../Common/Utils/RenderItemHelper.hpp"

#define DEFAULT_BOUNDING_VOLUME_TYPE BoundingVolumeType::SPHERE_T

class Geometry
{
public:
    virtual ~Geometry() = default;
    
    virtual void SetVertexData(const Vertex* _data, uint64 _vertexCount) = 0;
    virtual void SetIndexData(const uint32* _indices, uint64 _indexCount) = 0;
    
    void SetPrimitiveTopology(PrimitiveTopology _topology);
    void SetBoundingVolumeType(BoundingVolumeType _boundingVolumeType);
    
    uint64 GetVertexCount() const;
    uint64 GetIndexCount() const;
    
    PrimitiveTopology GetTopology() const;
    bool IsIndexed() const;
    
    bool FrustumCheck(Frustum const& _frustum, Mat4f32 const& _world);
    void CalculateBounds(const Vertex* _data, uint64 _vertexCount);
    
protected:
    uint64 m_vertexCount = 0;
    uint64 m_indexCount = 0;
    PrimitiveTopology m_primitiveTopology = PrimitiveTopology::TriangleList;

    bool m_isDynamic = false;
    
    BoundingVolumeType m_boundingVolumeType;
    float m_radius;
    Vect3f32 m_center;
    Vect3f32 m_extent;
    
    Geometry(bool _isDynamic, BoundingVolumeType _vType = DEFAULT_BOUNDING_VOLUME_TYPE);
};
#endif
