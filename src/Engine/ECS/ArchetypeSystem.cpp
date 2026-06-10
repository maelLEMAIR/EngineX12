#ifndef ARCHETYPE_SYSTEM_CPP_INCLUDED
#define ARCHETYPE_SYSTEM_CPP_INCLUDED

#include "ArchetypeSystem.h"

void ArchetypeSystem::Initialize(ComponentRegister* _componentRegister)
{
    m_componentRegister = _componentRegister;
}

Vector<Archetype*> ArchetypeSystem::GetMatchingArchetypes(const ComponentMask& _queryMask)
{
    Vector<Archetype*> result;

    for (auto& [sig, arch] : m_mArchetypesMap)
        if ((sig & _queryMask) == _queryMask)
            result.push_back(arch);

    return result;
}

Archetype* ArchetypeSystem::GetEmptyArchetype()
{
    ComponentMask empty;
    auto it = m_mArchetypesMap.find(empty);
    if (it != m_mArchetypesMap.end())
        return it->second;
    return CreateArchetype(empty);
}

Archetype* ArchetypeSystem::GetOrCreateAddEdge(Archetype* _pCurrentArch, ComponentId _componentToAdd)
{
    assert(_componentToAdd < MAX_COMPONENTS, "componentId trop grand !");

    if (_pCurrentArch->addEdges[_componentToAdd] != nullptr)
        return _pCurrentArch->addEdges[_componentToAdd];

    if (_pCurrentArch->signature.test(_componentToAdd))
        return _pCurrentArch;

    ComponentMask newSig = _pCurrentArch->signature;
    newSig.set(_componentToAdd);

    Archetype* target;
    auto it = m_mArchetypesMap.find(newSig);
    if (it != m_mArchetypesMap.end())
        target = it->second;
    else
        target = CreateArchetype(newSig);

    target->removeEdges[_componentToAdd] = _pCurrentArch;
    _pCurrentArch->addEdges[_componentToAdd] = target;

    return target;
}

Archetype* ArchetypeSystem::GetOrCreateRemoveEdge(Archetype* _pCurrentArch, ComponentId _componentToRemove)
{
    assert(_componentToRemove < MAX_COMPONENTS, "componentId trop grand !");

    if (_pCurrentArch->removeEdges[_componentToRemove] != nullptr)
        return _pCurrentArch->removeEdges[_componentToRemove];

    if (!_pCurrentArch->signature.test(_componentToRemove))
        return _pCurrentArch;

    ComponentMask newSig = _pCurrentArch->signature;
    newSig.reset(_componentToRemove);

    Archetype* target;
    auto it = m_mArchetypesMap.find(newSig);
    if (it != m_mArchetypesMap.end())
        target = it->second;
    else
        target = CreateArchetype(newSig);

    target->addEdges[_componentToRemove] = _pCurrentArch;
    _pCurrentArch->removeEdges[_componentToRemove] = target;

    return target;
}

Archetype* ArchetypeSystem::CreateArchetype(ComponentMask _signature)
{
    Archetype* arch = new Archetype;

    arch->signature  = _signature;
    arch->entityCount = 0;

    for (int i = 0; i < MAX_COMPONENTS; i++) {
        arch->addEdges[i]    = nullptr;
        arch->removeEdges[i] = nullptr;
    }

    for (int i = 0; i < MAX_COMPONENTS; i++)
    {
        if (!_signature.test(i))
            continue;

        size_t compSize = m_componentRegister->GetComponentSize(i);

        Column col;
        col.InitializeColumn(compSize);

        arch->componentIds.push_back((ComponentId)i);
        arch->columns.push_back(col);
    }

    m_mArchetypesMap[_signature] = arch;
    return arch;
}

#endif
