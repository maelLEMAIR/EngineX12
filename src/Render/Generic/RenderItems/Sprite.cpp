#include "Sprite.h"

void Sprite::CalculateBounds(const UiVertex* _vertices, uint64 _vertexCount)
{
    if (_vertexCount == 0) return;
    
    Vect2f32 min = _vertices[0].position;
    Vect2f32 max = _vertices[0].position;

    for (uint64 i = 0; i < _vertexCount; i++)
    {
        min.x = MathUtils::Min(min.x, _vertices[i].position.x);
        min.y = MathUtils::Min(min.y, _vertices[i].position.y);

        max.x = MathUtils::Max(max.x, _vertices[i].position.x);
        max.y = MathUtils::Max(max.y, _vertices[i].position.y);
    }
    
    m_boundingBox.distanceX = (int)((max.x - min.x) * 0.5f);
    m_boundingBox.distanceY = (int)((max.y - min.y) * 0.5f);
    
    m_boundingBox.centerX = (int)min.x + m_boundingBox.distanceX;
    m_boundingBox.centerX = (int)min.y + m_boundingBox.distanceY;
}
