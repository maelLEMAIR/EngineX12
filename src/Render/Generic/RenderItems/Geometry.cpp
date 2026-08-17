#include "Geometry.h"

void Geometry::SetPrimitiveTopology(PrimitiveTopology _topology)
{
    m_primitiveTopology = _topology;
}

void Geometry::SetBoundingVolumeType(BoundingVolumeType _boundingVolumeType)
{
    m_boundingVolumeType = _boundingVolumeType;
}

uint64 Geometry::GetVertexCount() const
{
    return m_vertexCount;
}

uint64 Geometry::GetIndexCount() const
{
    return m_indexCount;
}

PrimitiveTopology Geometry::GetTopology() const
{
    return m_primitiveTopology;
}

bool Geometry::IsIndexed() const
{
    return m_indexCount > 0;
}

bool Geometry::FrustumCheck(Frustum const& _frustum, Mat4f32 const& _world)
{
    switch ( m_boundingVolumeType )
    {
    case BoundingVolumeType::AABB_T:
        {
            AABB a = AABB::FromCenterExtent(m_center, m_extent);
            a = a.Transformed(_world);

            return _frustum.Intersects(a);
        }
    case BoundingVolumeType::SPHERE_T:
        {
            Sphere a(m_center, m_radius);
            a = a.Transformed(_world);

            return _frustum.Intersects(a);
        }
    case BoundingVolumeType::OBB_T:
        {
            OBB a = OBB(m_center, m_extent);
            a = a.Transformed(_world);
            
            return _frustum.Intersects(a);
        }
    default: return false;
    }
}

void Geometry::CalculateBounds(const Vertex* _data, uint64 _vertexCount)
{
    m_center = { 0.0f, 0.0f, 0.0f };
    m_extent = { 0.0f, 0.0f, 0.0f };
    m_radius = 0.0f;
 
    if (_vertexCount == 0) 
        return;
    
    Vect3f32 min = _data[0].position;
    Vect3f32 max = _data[0].position;
    
    for (uint64 i = 0; i < _vertexCount; i++)
    {
        min.x = MathUtils::Min( min.x, _data[i].position.x );
        min.y = MathUtils::Min( min.y, _data[i].position.y );
        min.z = MathUtils::Min( min.z, _data[i].position.z );
        
        max.x = MathUtils::Max( max.x, _data[i].position.x );
        max.y = MathUtils::Max( max.y, _data[i].position.y );
        max.z = MathUtils::Max( max.z, _data[i].position.z );
    }
    
    AABB a(min, max);
    m_center = a.Center();
    m_extent = a.Extent();
    m_radius = m_extent.Length();
}

Geometry::Geometry(bool _isDynamic, BoundingVolumeType _vType) : m_isDynamic(_isDynamic), m_boundingVolumeType(_vType), m_radius(0.0f)
{
    
}
