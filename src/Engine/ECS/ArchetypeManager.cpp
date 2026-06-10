#ifndef ARCHETYPE_MANAGER_CPP_INCLUDED
#define  ARCHETYPE_MANAGER_CPP_INCLUDED

#include "ArchetypeManager.h"
#include "Archetype.hpp"
#include "Column.h"

ArchetypeManager::ArchetypeManager()
{
    archetypeSystem = ArchetypeSystem();
}

void ArchetypeManager::MoveEntity(EntityId entity, Archetype* src, Archetype* dst, function<EntityRecord*(EntityId)> getRecord)
{
    EntityRecord* record = getRecord(entity);
    size_t srcRow = record->rowId;

    for (ComponentId compId : src->componentIds)
    {
        int dstCol = dst->GetColumnIndex(compId);
        if (dstCol == -1) continue;
        int srcCol = src->GetColumnIndex(compId);
        dst->columns[dstCol].PushBack(src->columns[srcCol].GetElement(srcRow));
    }

    dst->entities.push_back(entity);
    dst->entityCount++;

    bool srcHasData = src->entityCount > 0 && src->columns.size() > 0;
    if (srcHasData)
        RemoveFromArchetype(entity, src, srcRow, getRecord);

    record->archetype = dst;
    record->rowId     = dst->entityCount - 1;
}


EntityId ArchetypeManager::RemoveFromArchetype(EntityId entity, Archetype* arch, size_t row, function<EntityRecord*(EntityId)> getRecord)
{
    if (arch->entityCount == 0)
        return INVALID_ENTITY;
    if (row >= arch->entityCount)
        return INVALID_ENTITY;

    EntityId swappedEntity = arch->entities[arch->entityCount - 1];

    for (Column& col : arch->columns)
        col.SwapRemove(row);

    arch->entities[row] = swappedEntity;
    arch->entities.pop_back();
    arch->entityCount--;

    if (swappedEntity != entity)
    {
        EntityRecord* swappedRecord = getRecord(swappedEntity);
        swappedRecord->rowId = row;
        return swappedEntity;
    }

    return INVALID_ENTITY;
}

void ArchetypeManager::DestroyEntity(EntityId entity, function<EntityRecord*(EntityId)> getRecord)
{
    EntityRecord* record = getRecord(entity);
    if (record == nullptr)
        return;

    Archetype* arch = record->archetype;
    if (arch == nullptr)
        return;

    if (arch->entityCount == 0)
        return;

    size_t row = record->rowId;
    RemoveFromArchetype(entity, arch, row, getRecord);
}

#endif
