#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include "../define.h"

#include "EventDispatcher.h"
#include "ComponentRegister.h"
#include "ArchetypeManager.h"
#include "EntityManager.h"
#include "ScriptManager.h"
#include "SystemManager.h"

class World
{
public:
    World();
    ~World();

    void        Update(float deltaTime);
    EntityId    CreateEntity();
    
    void        DestroyEntity(EntityId _entityId);

    template<typename T> T&          AddComponent(EntityId _entityId);
    template<typename T> void        RemoveComponent(EntityId _entityId);
    template<typename T> T*          GetComponent(EntityId _entityId);

    template<typename... Components>
    ComponentMask BuildQueryMask();
    template<typename... Components, typename Callback>
    void Query(Callback&& callback);
    template<typename... Components, typename Callback>
    void QueryWithEntity(Callback&& callback);

    template<typename T>
    T* RegisterSystem(int priority = 0);
    template<typename T>
    T* GetSystem();
    template<typename T>
    void SetSystemActive(bool active);

    template<typename T>
    ObserverId OnComponentAdded(std::function<void(World&, EntityId)> callback);
    template<typename T>
    ObserverId OnComponentRemoved(std::function<void(World&, EntityId)> callback);
    ObserverId OnEntityCreated  (std::function<void(World&, EntityId)> callback);
    ObserverId OnEntityDestroyed(std::function<void(World&, EntityId)> callback);
    void RemoveObserver(ObserverId id);

    template<typename T> T*   AddScript   (EntityId _entityId);
    template<typename T> T*   GetScript   (EntityId _entityId);
    template<typename T> void RemoveScript(EntityId _entityId);
    template<typename T> void SetScriptActive(EntityId _entityId, bool active);
    
    EntityManager       m_entityManager;
    ArchetypeManager    m_archetypeManager;
    ComponentRegister   m_componentRegister;
    SystemManager       m_systemManager;
    ScriptManager       m_scriptManager;
    EventDispatcher     m_eventDispatcher;
    
private:
    bool          m_isQuerying = false;
};

#include "World.inl"

#endif