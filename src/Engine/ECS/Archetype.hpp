#ifndef ARCHETYPE_HPP_INCLUDED
#define ARCHETYPE_HPP_INCLUDED

#include "Column.h"
#include "../define.h"

struct Archetype {
    ComponentMask           signature;
    Vector<ColumnIndex>     columnIndices;
    Vector<ComponentId>     componentIds;
    Vector<Column>          columns;
    Vector<EntityId>        entities;
    size_t                  entityCount;

    Archetype* addEdges[MAX_COMPONENTS];
    Archetype* removeEdges[MAX_COMPONENTS];

    int GetColumnIndex(ComponentId compId) const {
        for (size_t i = 0; i < componentIds.size(); i++)
            if (componentIds[i] == compId)
                return (int)i;
        return -1;
    }
};

#endif