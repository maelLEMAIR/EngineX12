#ifndef COMPONENT_DISPATCHER_H_INCLUDED
#define COMPONENT_DISPATCHER_H_INCLUDED

#include "Core/define.h"
#include "Engine/define.h"
#include "Engine/ECS/World.h"

#include "Network/Serialization/Deserialization.h"

class World;

using ComponentApplyFn = std::function<void(
    Serialization::Deserializeration&, World&, EntityId)>;

class ComponentDispatcher
{
public:
    static ComponentDispatcher& Get()
    {
        static ComponentDispatcher instance;
        return instance;
    }

    void Register(uint32_t componentId, ComponentApplyFn fn)
    {
        m_handlers[componentId] = fn;
    }

    void Apply(uint32_t componentId,
               Serialization::Deserializeration& d,
               World& world, EntityId id)
    {
        auto it = m_handlers.find(componentId);
        if (it != m_handlers.end())
            it->second(d, world, id);
    }

private:
    std::unordered_map<uint32_t, ComponentApplyFn> m_handlers;
};

#endif