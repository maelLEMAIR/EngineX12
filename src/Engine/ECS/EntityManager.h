#ifndef ENTITY_MANAGER_H_INCLUDED
#define ENTITY_MANAGER_H_INCLUDED

#include "../define.h"
#include "EntityRecord.hpp"

class EntityManager
{
public:
    EntityManager();

    EntityId      CreateEntity();
    void          DestroyEntity(EntityId _id);
    EntityRecord* GetEntity(EntityId _id) const;
    bool          IsAlive(EntityId _id) const;

private:
    struct EntitySlot
    {
        EntityRecord record;
        uint32_t     version = 0;
        bool         alive   = false;
    };

    uint32_t           m_nextIndex = 0;
    Vector<EntitySlot> m_slots;
    Vector<uint32_t>   m_freeList;
};

#endif