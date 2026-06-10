#ifndef ENTITY_MANAGER_CPP_INCLUDED
#define ENTITY_MANAGER_CPP_INCLUDED

#include "EntityManager.h"

EntityManager::EntityManager() : m_nextIndex(0) {}

EntityId EntityManager::CreateEntity()
{
    uint32_t index;
    uint32_t version;

    if (!m_freeList.empty())
    {
        index = m_freeList.back();
        m_freeList.pop_back();

        version = m_slots[index].version;
    }
    else
    {
        index   = m_nextIndex++;
        version = 0;
        m_slots.push_back(EntitySlot{});
    }

    EntitySlot& slot    = m_slots[index];
    slot.alive          = true;
    slot.version        = version;
    slot.record.archetype = nullptr;
    slot.record.rowId     = 0;

    return MakeEntityId(index, version);
}

void EntityManager::DestroyEntity(EntityId _id)
{
    uint32_t index   = GetEntityIndex(_id);
    uint32_t version = GetEntityVersion(_id);

    if (index >= m_slots.size())
        return;
    
    EntitySlot& slot = m_slots[index];
    
    if (!slot.alive || slot.version != version)
        return;

    slot.alive = false;
    slot.version++;
    slot.record = EntityRecord{};

    m_freeList.push_back(index);
}

EntityRecord* EntityManager::GetEntity(EntityId _id) const
{
    uint32_t index   = GetEntityIndex(_id);
    uint32_t version = GetEntityVersion(_id);

    if (index >= m_slots.size())
        return nullptr;

    const EntitySlot& slot = m_slots[index];

    if (!slot.alive || slot.version != version)
        return nullptr;

    return const_cast<EntityRecord*>(&slot.record);
}

bool EntityManager::IsAlive(EntityId _id) const
{
    uint32_t index   = GetEntityIndex(_id);
    uint32_t version = GetEntityVersion(_id);

    if (index >= m_slots.size())
        return false;

    const EntitySlot& slot = m_slots[index];
    
    return slot.alive && slot.version == version;
}

#endif
