#ifndef COMPONENT_REGISTER_INL_INCLUDED
#define COMPONENT_REGISTER_INL_INCLUDED

template<typename T>
ComponentId ComponentRegister::GetComponentId()
{
    static ComponentId id = m_nextId++;

    if (m_sizes.count(id) == 0)
        m_sizes[id] = sizeof(T);
    
    return id;
}

#endif