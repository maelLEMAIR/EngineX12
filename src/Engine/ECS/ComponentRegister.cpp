#ifndef COMPONENT_REGISTER_CPP_INCLUDED
#define COMPONENT_REGISTER_CPP_INCLUDED

#include "ComponentRegister.h"

size_t ComponentRegister::GetComponentSize(ComponentId _id) const
{
    auto it = m_sizes.find(_id);
    if (it == m_sizes.end())
        return 0;
    return it->second;
}

bool ComponentRegister::IsRegistered(ComponentId _id) const
{
    return m_sizes.count(_id) > 0;
}

#endif