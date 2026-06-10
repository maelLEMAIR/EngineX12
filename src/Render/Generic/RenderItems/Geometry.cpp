#include "Geometry.h"

Geometry::Geometry(bool _isDynamic) : m_isDynamic(_isDynamic)
{
    m_bounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_bounds.Extents = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Geometry::CalculateBounds(const Vertex* _vertices, uint64 _vertexCount)
{
    if (_vertexCount == 0) return;
    
    XMFLOAT3 min = _vertices[0].position;
    XMFLOAT3 max = _vertices[0].position;

    for (uint64 i = 0; i < _vertexCount; i++)
    {
        min.x = MathHelper::Min<float>(min.x, _vertices[i].position.x);
        min.y = MathHelper::Min<float>(min.y, _vertices[i].position.y);
        min.z = MathHelper::Min<float>(min.z, _vertices[i].position.z);

        max.x = MathHelper::Max<float>(max.x, _vertices[i].position.x);
        max.y = MathHelper::Max<float>(max.y, _vertices[i].position.y);
        max.z = MathHelper::Max<float>(max.z, _vertices[i].position.z);
    }

    XMFLOAT3 distance = {0.0f, 0.0f, 0.0f};
    distance.x = (max.x - min.x) * 0.5f;
    distance.y = (max.y - min.y) * 0.5f;
    distance.z = (max.z - min.z) * 0.5f;
    
    XMFLOAT3 center = {0.0f, 0.0f, 0.0f};
    center.x = min.x + distance.x;
    center.y = min.y + distance.y;
    center.z = min.z + distance.z;

    m_bounds.Center = center;
    m_bounds.Extents = distance;
}
