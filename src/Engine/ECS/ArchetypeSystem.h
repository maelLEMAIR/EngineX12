#ifndef ARCHETYPE_SYSTEM_H_INCLUDED
#define ARCHETYPE_SYSTEM_H_INCLUDED

#include "Archetype.hpp"
#include "ComponentRegister.h"
#include "../define.h"

struct ComponentMaskHasher {
    size_t operator()(const ComponentMask& mask) const {
        return std::hash<std::string>{}(mask.to_string());
    }
};

class ArchetypeSystem {
public:
    void Initialize(ComponentRegister* _componentRegister);

    Vector<Archetype*> GetMatchingArchetypes(const ComponentMask& _queryMask);
    
    Archetype* GetOrCreateAddEdge(Archetype* _pCurrentArch, ComponentId _componentToAdd);
    Archetype* GetOrCreateRemoveEdge(Archetype* _pCurrentArch, ComponentId _componentToRemove);
    Archetype* GetEmptyArchetype();

private:
    Archetype* CreateArchetype(ComponentMask _signature);

    ComponentRegister* m_componentRegister; // ← ajoute ça
    std::unordered_map<ComponentMask, Archetype*, ComponentMaskHasher> m_mArchetypesMap;
};


#endif
