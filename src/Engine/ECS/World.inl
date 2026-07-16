#ifndef WORLD_INL_INCLUDED
#define WORLD_INL_INCLUDED

#include "EntityRecord.hpp"
#include "ScriptCollection.h"

template<typename T>
T& World::AddComponent(EntityId _entityId)
{
    static_assert(std::is_trivially_copyable_v<T>,
        "Les composants ECS doivent etre trivially copyable (Column utilise memcpy en interne). "
        "Pas de std::string/std::vector/std::deque/std::function dans un composant : "
        "utilise un buffer fixe ou stocke les donnees ailleurs.");
    if (m_isQuerying)
    {
        m_isQuerying = false;
        T& ref = AddComponent<T>(_entityId);
        m_isQuerying = true;
        return ref;
    }

    EntityRecord* record = m_entityManager.GetEntity(_entityId);
    assert(record != nullptr && "AddComponent : entite inexistante");

    ComponentId compId = m_componentRegister.GetComponentId<T>();

    Archetype* src = record->archetype;

    if (src && src->signature.test(compId))
    {
        int col = src->GetColumnIndex(compId);
        return *reinterpret_cast<T*>(src->columns[col].GetElement(record->rowId));
    }
    
    if (src == nullptr)
        src = m_archetypeManager.archetypeSystem.GetEmptyArchetype();
    
    Archetype* dst = m_archetypeManager.archetypeSystem.GetOrCreateAddEdge(src, compId);

    if (src != dst)
    {
        m_archetypeManager.MoveEntity( _entityId,
            src, dst,
            [this](EntityId id) { return m_entityManager.GetEntity(id); }
        );
    }

    record = m_entityManager.GetEntity(_entityId);
    int col = dst->GetColumnIndex(compId);
    assert(col != -1 && "AddComponent : colonne introuvable dans dst");

    if (dst->columns[col].size <= record->rowId)
    {
        T zero{};
        dst->columns[col].PushBack(&zero);
    }
    
    m_eventDispatcher.DispatchComponentAdded(*this, _entityId, compId);
    return *reinterpret_cast<T*>(dst->columns[col].GetElement(record->rowId));
}

template<typename T>
void World::RemoveComponent(EntityId _entityId)
{
    EntityRecord* record = m_entityManager.GetEntity(_entityId);
    assert(record != nullptr && "RemoveComponent : entite inexistante");

    ComponentId compId = m_componentRegister.GetComponentId<T>();

    Archetype* src = record->archetype;
    Archetype* dst = m_archetypeManager.archetypeSystem.GetOrCreateRemoveEdge(src, compId);

    if (src != dst)
    {
        m_archetypeManager.MoveEntity(_entityId,
            src, dst,
            [this](EntityId id) { return m_entityManager.GetEntity(id); }
        );

        m_eventDispatcher.DispatchComponentRemoved(*this, _entityId, compId);
    }
}

template<typename T>
T* World::GetComponent(EntityId _entityId)
{
    EntityRecord* record = m_entityManager.GetEntity(_entityId);
    if (record == nullptr) return nullptr;

    ComponentId compId = m_componentRegister.GetComponentId<T>();
    Archetype* arch = record->archetype;
    if (arch == nullptr) return nullptr;

    int col = arch->GetColumnIndex(compId);
    if (col == -1) return nullptr;

    return static_cast<T*>(arch->columns[col].GetElement(record->rowId));
}

template<typename... Components>
ComponentMask World::BuildQueryMask()
{
    ComponentMask mask;
    (mask.set(m_componentRegister.GetComponentId<Components>()), ...);
    return mask;
}

template<typename T>
T& GetComponentFromArchetype(Archetype* arch, size_t row, ComponentId compId)
{
    int col = arch->GetColumnIndex(compId);
    assert(col != -1);
    return *static_cast<T*>(arch->columns[col].GetElement(row));
}

template<typename... Components, typename Callback>
void World::Query(Callback&& callback)
{
    ComponentMask mask;
    (mask.set(m_componentRegister.GetComponentId<Components>()), ...);

    Vector<Archetype*> archetypes =
        m_archetypeManager.archetypeSystem.GetMatchingArchetypes(mask);

    Vector<EntityId> snapshot;
    for (Archetype* arch : archetypes)
        for (size_t row = 0; row < arch->entityCount; row++)
            snapshot.push_back(arch->entities[row]);

    m_isQuerying = true;

    for (EntityId id : snapshot)
    {
        if (!m_entityManager.IsAlive(id)) continue;

        EntityRecord* record = m_entityManager.GetEntity(id);
        Archetype* arch = record->archetype;

        if ((arch->signature & mask) != mask) continue;

        size_t row = record->rowId;
        callback(
            *static_cast<Components*>(
                arch->columns[arch->GetColumnIndex(
                    m_componentRegister.GetComponentId<Components>()
                )].GetElement(row)
            )...
        );
    }

    m_isQuerying = false;
}

template<typename... Components, typename Callback>
void World::QueryWithEntity(Callback&& callback)
{
    ComponentMask mask;
    (mask.set(m_componentRegister.GetComponentId<Components>()), ...);

    Vector<Archetype*> archetypes =
        m_archetypeManager.archetypeSystem.GetMatchingArchetypes(mask);

    Vector<EntityId> snapshot;
    for (Archetype* arch : archetypes)
        for (size_t row = 0; row < arch->entityCount; row++)
            snapshot.push_back(arch->entities[row]);

    m_isQuerying = true;

    for (EntityId id : snapshot)
    {
        if (!m_entityManager.IsAlive(id)) continue;

        EntityRecord* record = m_entityManager.GetEntity(id);
        Archetype* arch = record->archetype;

        if ((arch->signature & mask) != mask) continue;

        size_t row = record->rowId;
        callback(
            id,
            *static_cast<Components*>(
                arch->columns[arch->GetColumnIndex(
                    m_componentRegister.GetComponentId<Components>()
                )].GetElement(row)
            )...
        );
    }

    m_isQuerying = false;
}

template<typename T>
T* World::RegisterSystem(int priority)
{
    return m_systemManager.RegisterSystem<T>(*this, priority);
}

template<typename T>
T* World::GetSystem()
{
    return m_systemManager.GetSystem<T>();
}

template<typename T>
void World::SetSystemActive(bool active)
{
    m_systemManager.SetActive<T>(active);
}

template<typename T>
ObserverId World::OnComponentAdded(std::function<void(World&, EntityId)> callback)
{
    ComponentId compId = m_componentRegister.GetComponentId<T>();
    return m_eventDispatcher.SubscribeComponentAdded(compId, std::move(callback));
}

template<typename T>
ObserverId World::OnComponentRemoved(std::function<void(World&, EntityId)> callback)
{
    ComponentId compId = m_componentRegister.GetComponentId<T>();
    return m_eventDispatcher.SubscribeComponentRemoved(compId, std::move(callback));
}

template<typename T>
T* World::AddScript(EntityId _entityId)
{
    ScriptCollection* col = m_scriptManager.GetOrCreate(_entityId);
    return col->Add<T>();
}

template<typename T>
T* World::GetScript(EntityId _entityId)
{
    ScriptCollection* col = m_scriptManager.Get(_entityId);
    if (col == nullptr) return nullptr;
    return col->Get<T>();
}

template<typename T>
void World::RemoveScript(EntityId _entityId)
{
    ScriptCollection* col = m_scriptManager.Get(_entityId);
    if (col == nullptr) return;
    col->Remove<T>();
}

template<typename T>
void World::SetScriptActive(EntityId _entityId, bool active)
{
    ScriptCollection* col = m_scriptManager.Get(_entityId);
    if (col == nullptr) return;
    col->SetActive<T>(active);
}

#endif