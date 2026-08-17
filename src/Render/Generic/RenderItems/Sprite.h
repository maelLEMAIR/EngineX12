#ifndef SPRITE_H_DEFINED
#define SPRITE_H_DEFINED

#include "../../Common/Common.h"

struct UiVertex
{
    Vect2f32 position;
    Vect2f32 uv;
    Vect4f32 color = Vect4f32(1.0f, 1.0f, 1.0f, 1.0f);
};

class Sprite
{
public:
    struct SpriteBoundingBox
    {
        int centerX = 0;
        int centerY = 0;
        int distanceX = 0;
        int distanceY = 0;
    };
    
    virtual ~Sprite() = default;
    
    virtual void SetVertexData(const UiVertex* _data, uint64 _vertexCount) = 0;
    virtual void SetIndexData(const uint32* _indices, uint64 _indexCount) = 0;

    uint64 GetVertexCount() const { return m_vertexCount; }
    uint64 GetIndexCount() const { return m_indexCount; }
    bool IsIndexed() const { return m_indexCount > 0; }

    SpriteBoundingBox& GetBounds() { return m_boundingBox; }
    
protected:
    bool m_isDynamic = false;

    SpriteBoundingBox m_boundingBox;
    
    uint64 m_vertexCount = 0;
    uint64 m_indexCount = 0;

    void CalculateBounds(const UiVertex* _vertices, uint64 _vertexCount);
};

#endif
