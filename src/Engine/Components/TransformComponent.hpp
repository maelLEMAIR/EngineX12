#ifndef TRANSFORM_COMPONENT_HPP_DEFINED
#define TRANSFORM_COMPONENT_HPP_DEFINED

#include "define.h"
#include "../Core/Transform.h"

struct TransformComponent
{
    Transform local;
    Transform world;

    EntityId parent;
    bool hasParent = false;

    void SetParent(EntityId _parent)
    {
        parent = _parent;
        hasParent = true;
    }
    void RemoveParent() { hasParent = false; }
};

#endif