#ifndef ARCHETYPE_MANAGER_H_INCLUDED
#define ARCHETYPE_MANAGER_H_INCLUDED

#include "ArchetypeSystem.h"
#include "EntityRecord.hpp"
#include "../define.h"

struct Archetype;

class ArchetypeManager
{
public:
    ArchetypeManager();
    void MoveEntity(EntityId entity, Archetype* src, Archetype* dst, function<EntityRecord*(EntityId)> getRecord);
    void DestroyEntity(EntityId entity, function<EntityRecord*(EntityId)> getRecord);
    ArchetypeSystem archetypeSystem;
    
protected:
private:
    EntityId RemoveFromArchetype(EntityId entity, Archetype* arch, size_t row, function<EntityRecord*(EntityId)> getRecord);
};

#endif