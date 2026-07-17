#ifndef MAP_DEFINITION_HPP_INCLUDED
#define MAP_DEFINITION_HPP_INCLUDED

#include "define.h"

struct MapObject
{
    enum class Shape { Cube, Sphere };

    Shape    shape;
    XMFLOAT3 position;
    XMFLOAT3 scale = { 1.f, 1.f, 1.f };
};

class MapDefinition
{
public:
    static const std::vector<MapObject>& GetLayout()
    {
        static std::vector<MapObject> layout = {
            { MapObject::Shape::Cube,   {  0.f, -1.f,  0.f}, {20.f, 0.2f, 20.f} },

            { MapObject::Shape::Cube,   { -4.f,  0.5f,  3.f} },
            { MapObject::Shape::Cube,   {  4.f,  0.5f,  3.f} },
            { MapObject::Shape::Sphere, {  0.f,  0.5f, -5.f} },
            { MapObject::Shape::Sphere, {  6.f,  0.5f,  0.f} },
            { MapObject::Shape::Sphere, { -6.f,  0.5f,  0.f} },
        };
        return layout;
    }
};

#endif