#ifndef COMPONENT_REGISTER_H_INCLUDED
#define COMPONENT_REGISTER_H_INCLUDED

#include "../define.h"
#include <typeindex>

class ComponentRegister
{
public:
    template<typename T>
    ComponentId GetComponentId();

    size_t GetComponentSize(ComponentId _id) const;
    bool IsRegistered(ComponentId _id) const;

private:
    ComponentId m_nextId = 0;
    std::unordered_map<ComponentId, size_t> m_sizes;
    std::unordered_map<std::type_index, ComponentId> m_typeToId;
};

#include "ComponentRegister.inl"

#endif