// ScriptManager.h
#ifndef SCRIPT_MANAGER_H_INCLUDED
#define SCRIPT_MANAGER_H_INCLUDED

#include "../define.h"
#include "ScriptCollection.h"

class ScriptManager
{
public:
    ScriptCollection* GetOrCreate(EntityId id)
    {
        return &m_scripts[id];
    }

    ScriptCollection* Get(EntityId id)
    {
        auto it = m_scripts.find(id);
        if (it == m_scripts.end()) return nullptr;
        return &it->second;
    }

    void Remove(EntityId id)
    {
        m_scripts.erase(id);
    }

    UnorderedMap<EntityId, ScriptCollection>& GetAll()
    {
        return m_scripts;
    }

private:
    UnorderedMap<EntityId, ScriptCollection> m_scripts;
};

#endif