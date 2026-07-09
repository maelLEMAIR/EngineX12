#ifndef EVENT_DISPATCHER_H_INCLUDED
#define EVENT_DISPATCHER_H_INCLUDED

#include "../define.h"
#include <functional>

class World;

constexpr ObserverId INVALID_OBSERVER = (std::numeric_limits<ObserverId>::max)();

class EventDispatcher
{
public:
    using EntityCallback    = std::function<void(World&, EntityId)>;

    ObserverId SubscribeComponentAdded  (ComponentId compId, EntityCallback callback);
    ObserverId SubscribeComponentRemoved(ComponentId compId, EntityCallback callback);
    ObserverId SubscribeEntityCreated   (EntityCallback callback);
    ObserverId SubscribeEntityDestroyed (EntityCallback callback);

    void Unsubscribe(ObserverId id);

    void DispatchComponentAdded  (World& world, EntityId entity, ComponentId compId);
    void DispatchComponentRemoved(World& world, EntityId entity, ComponentId compId);
    void DispatchEntityCreated   (World& world, EntityId entity);
    void DispatchEntityDestroyed (World& world, EntityId entity);

private:
    struct Observer
    {
        ObserverId     id;
        EntityCallback callback;
    };

    ObserverId m_nextObserverId = 0;

    UnorderedMap<ComponentId, Vector<Observer>> m_componentAddedObservers;
    UnorderedMap<ComponentId, Vector<Observer>> m_componentRemovedObservers;

    Vector<Observer> m_entityCreatedObservers;
    Vector<Observer> m_entityDestroyedObservers;

    struct ObserverLocation
    {
        enum class Type { ComponentAdded, ComponentRemoved, EntityCreated, EntityDestroyed };
        Type       type;
        ComponentId compId;
    };
    UnorderedMap<ObserverId, ObserverLocation> m_observerLocations;

    ObserverId RegisterObserver(Vector<Observer>& list,
                                EntityCallback callback,
                                ObserverLocation location);
};

#endif