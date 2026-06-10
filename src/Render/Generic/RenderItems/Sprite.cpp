#include "Sprite.h"

void Sprite::CalculateBounds(const UiVertex* _vertices, uint64 _vertexCount)
{
    if (_vertexCount == 0) return;
    
    XMFLOAT2 min = _vertices[0].position;
    XMFLOAT2 max = _vertices[0].position;

    for (uint64 i = 0; i < _vertexCount; i++)
    {
        min.x = MathHelper::Min<float>(min.x, _vertices[i].position.x);
        min.y = MathHelper::Min<float>(min.y, _vertices[i].position.y);

        max.x = MathHelper::Max<float>(max.x, _vertices[i].position.x);
        max.y = MathHelper::Max<float>(max.y, _vertices[i].position.y);
    }
    
    m_boundingBox.distanceX = (int)((max.x - min.x) * 0.5f);
    m_boundingBox.distanceY = (int)((max.y - min.y) * 0.5f);
    
    m_boundingBox.centerX = (int)min.x + m_boundingBox.distanceX;
    m_boundingBox.centerX = (int)min.y + m_boundingBox.distanceY;
}
