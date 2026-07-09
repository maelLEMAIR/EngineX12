#ifndef NETWORK_COMPONENT_INDEX_H_INCLUDED
#define NETWORK_COMPONENT_INDEX_H_INCLUDED

#include "define.h"

class NetworkComponentIndex
{
public:
    static NetworkComponentIndex& Get()
    {
        static NetworkComponentIndex instance;
        return instance;
    }

    void Register(uint32 componentId)
    {
        if (!m_map.contains(componentId))
        {
            m_map[componentId] = m_nextIndex++;
        }
    }

    uint32 GetIndex(uint32 componentId) const
    {
        return m_map.at(componentId);
    }

    bool Has(uint32 componentId) const
    {
        return m_map.contains(componentId);
    }

private:
    UnorderedMap<uint32, uint32> m_map;
    uint32 m_nextIndex = 0;
};

#endif