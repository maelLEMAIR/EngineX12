#ifndef EVENT_DISPATCHER_CPP_INCLUDED
#define EVENT_DISPATCHER_CPP_INCLUDED

#include "EventDispatcher.h"

ObserverId EventDispatcher::RegisterObserver(
    Vector<Observer>& list,
    EntityCallback callback,
    ObserverLocation location)
{
    ObserverId id = m_nextObserverId++;

    list.push_back({ id, std::move(callback) });
    m_observerLocations[id] = location;

    return id;
}

ObserverId EventDispatcher::SubscribeComponentAdded(ComponentId compId, EntityCallback callback)
{
    return RegisterObserver(
        m_componentAddedObservers[compId],
        std::move(callback),
        { ObserverLocation::Type::ComponentAdded, compId }
    );
}

ObserverId EventDispatcher::SubscribeComponentRemoved(ComponentId compId, EntityCallback callback)
{
    return RegisterObserver(
        m_componentRemovedObservers[compId],
        std::move(callback),
        { ObserverLocation::Type::ComponentRemoved, compId }
    );
}

ObserverId EventDispatcher::SubscribeEntityCreated(EntityCallback callback)
{
    return RegisterObserver(
        m_entityCreatedObservers,
        std::move(callback),
        { ObserverLocation::Type::EntityCreated, 0 }
    );
}

ObserverId EventDispatcher::SubscribeEntityDestroyed(EntityCallback callback)
{
    return RegisterObserver(
        m_entityDestroyedObservers,
        std::move(callback),
        { ObserverLocation::Type::EntityDestroyed, 0 }
    );
}

void EventDispatcher::Unsubscribe(ObserverId id)
{
    auto locIt = m_observerLocations.find(id);
    if (locIt == m_observerLocations.end()) return;

    ObserverLocation loc = locIt->second;

    // Trouver la bonne liste
    Vector<Observer>* list = nullptr;
    switch (loc.type)
    {
        case ObserverLocation::Type::ComponentAdded:
            list = &m_componentAddedObservers[loc.compId]; break;
        case ObserverLocation::Type::ComponentRemoved:
            list = &m_componentRemovedObservers[loc.compId]; break;
        case ObserverLocation::Type::EntityCreated:
            list = &m_entityCreatedObservers; break;
        case ObserverLocation::Type::EntityDestroyed:
            list = &m_entityDestroyedObservers; break;
    }

    if (list == nullptr) return;

    // SwapRemove dans la liste d'observers
    for (size_t i = 0; i < list->size(); i++)
    {
        if ((*list)[i].id == id)
        {
            (*list)[i] = std::move(list->back());
            list->pop_back();
            break;
        }
    }

    m_observerLocations.erase(locIt);
}

// ── Dispatch ─────────────────────────────────────────────────────────────────

void EventDispatcher::DispatchComponentAdded(World& world, EntityId entity, ComponentId compId)
{
    auto it = m_componentAddedObservers.find(compId);
    if (it == m_componentAddedObservers.end()) return;
    for (auto& obs : it->second)
        obs.callback(world, entity);
}

void EventDispatcher::DispatchComponentRemoved(World& world, EntityId entity, ComponentId compId)
{
    auto it = m_componentRemovedObservers.find(compId);
    if (it == m_componentRemovedObservers.end()) return;
    for (auto& obs : it->second)
        obs.callback(world, entity);
}

void EventDispatcher::DispatchEntityCreated(World& world, EntityId entity)
{
    for (auto& obs : m_entityCreatedObservers)
        obs.callback(world, entity);
}

void EventDispatcher::DispatchEntityDestroyed(World& world, EntityId entity)
{
    for (auto& obs : m_entityDestroyedObservers)
        obs.callback(world, entity);
}

#endif
