#ifndef SPRITE_FACTORY_HPP_DEFINED
#define SPRITE_FACTORY_HPP_DEFINED

#include "../Base/Device.h"
#include "../RenderItems/Sprite.h"

class SpriteFactory
{
public:
    static Sprite* BuildRectangle(Device* _pDevice, int _width, int _height, bool _isDynamic = false)
    {
        Sprite* sprite = _pDevice->CreateSprite(_isDynamic);

        Vector<UiVertex> vertices;
        Vector<uint32> indices;

        float midWidth = (float)_width / 2;
        float midHeight = (float)_height / 2;
        
        vertices = {
            UiVertex(XMFLOAT2(-midWidth, midHeight), XMFLOAT2(0.0f, 0.0f)),
            UiVertex(XMFLOAT2(midWidth, midHeight), XMFLOAT2(1.0f, 0.0f)),
            UiVertex(XMFLOAT2(-midWidth, -midHeight), XMFLOAT2(0.0f, 1.0f)),
            UiVertex(XMFLOAT2(midWidth, -midHeight), XMFLOAT2(1.0f, 1.0f)),
        };

        indices = {
            0, 1, 2,
            2, 1, 3
        };

        sprite->SetVertexData(vertices.data(), vertices.size());
        sprite->SetIndexData(indices.data(), indices.size());

        return sprite;
    }

    static Sprite* BuildRoundedRectangle(Device* _pDevice, int _width, int _height, float _radius, int _cornerSegments = 8, bool _isDynamic = false)
    {
        Sprite* sprite = _pDevice->CreateSprite(_isDynamic);

        Vector<UiVertex> vertices;
        Vector<uint32>   indices;

        float halfW = (float)_width  / 2.0f;
        float halfH = (float)_height / 2.0f;

        _radius = min(_radius, min(halfW, halfH));

        const XMFLOAT2 arcCenters[4] = {
            {  halfW - _radius,  halfH - _radius },
            { -halfW + _radius,  halfH - _radius },
            { -halfW + _radius, -halfH + _radius },
            {  halfW - _radius, -halfH + _radius },
        };

        const float startAngles[4] = {
            0.0f,                          
            XM_PIDIV2,                     
            XM_PI,                         
            XM_PI + XM_PIDIV2,             
        };
        
        vertices.push_back(UiVertex(XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.5f, 0.5f)));

        float angleStep = XM_PIDIV2 / (float)_cornerSegments;

        for (int corner = 0; corner < 4; ++corner)
        {
            float cx = arcCenters[corner].x;
            float cy = arcCenters[corner].y;

            for (int seg = 0; seg <= _cornerSegments; ++seg)
            {
                float angle = startAngles[corner] + seg * angleStep;

                float px = cx + _radius * cosf(angle);
                float py = cy + _radius * sinf(angle);
                
                float u = (px + halfW) / (float)_width;
                float v = 1.0f - (py + halfH) / (float)_height;

                vertices.push_back(UiVertex(XMFLOAT2(px, py), XMFLOAT2(u, v)));
            }
        }
        
        uint32 rimCount = (uint32)vertices.size() - 1;

        for (uint32 i = 0; i < rimCount; ++i)
        {
            uint32 curr = 1 + i;
            uint32 next = 1 + (i + 1) % rimCount;

            indices.push_back(0);
            indices.push_back(curr);
            indices.push_back(next);
        }

        sprite->SetVertexData(vertices.data(), vertices.size());
        sprite->SetIndexData(indices.data(), indices.size());

        return sprite;
    }
};

#endif