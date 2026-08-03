#ifndef COMPONENT_REGISTER_INL_INCLUDED
#define COMPONENT_REGISTER_INL_INCLUDED

template<typename T>
ComponentId ComponentRegister::GetComponentId()
{
    std::type_index idx(typeid(T));

    auto it = m_typeToId.find(idx);
    if (it != m_typeToId.end())
        return it->second;

    ComponentId id = m_nextId++;
    m_typeToId[idx] = id;
    m_sizes[id] = sizeof(T);

    return id;
}

#endif