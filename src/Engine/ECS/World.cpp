#ifndef WORLD_CPP_INCLUDED
#define WORLD_CPP_INCLUDED

#include "World.h"

#include "Systems/ScriptSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/MeshRendererSystem.h"
#include "Systems/TransformSystem.h"
#include "Systems/CameraSystem.h"
#include "Systems/LightSystem.h"

#include "NetworkBridge/NetworkSyncSystem.h"

World::World()
{
    m_entityManager = EntityManager();
    m_componentRegister = ComponentRegister();
    m_archetypeManager.archetypeSystem.Initialize(&m_componentRegister);

    m_systemManager.RegisterSystem<ScriptSystem>(       *this, -1);
    m_systemManager.RegisterSystem<PhysicsSystem>(      *this, 0);
    m_systemManager.RegisterSystem<TransformSystem>(    *this, 1);
    m_systemManager.RegisterSystem<CameraSystem>(       *this, 2);
    m_systemManager.RegisterSystem<LightSystem>(        *this, 3);
    m_systemManager.RegisterSystem<MeshRendererSystem>( *this, 4);
}

World::~World() 
{
    
}

void World::Update(float deltaTime)
{
    m_systemManager.Update(*this, deltaTime);
}

EntityId World::CreateEntity()
{
    EntityId id = m_entityManager.CreateEntity();
    EntityRecord* record = m_entityManager.GetEntity(id);

    record->archetype = m_archetypeManager.archetypeSystem.GetEmptyArchetype();
    record->rowId     = 0;

    m_eventDispatcher.DispatchEntityCreated(*this, id);
    return id;
}

void World::DestroyEntity(EntityId _entityId)
{
    assert(m_entityManager.IsAlive(_entityId) && "DestroyEntity : entité inexistante");

    m_eventDispatcher.DispatchEntityDestroyed(*this, _entityId);
    
    m_archetypeManager.DestroyEntity(
        _entityId,
        [this](EntityId id) { return m_entityManager.GetEntity(id); }
    );
    m_entityManager.DestroyEntity(_entityId);
}

ObserverId World::OnEntityCreated(std::function<void(World&, EntityId)> callback)
{
    return m_eventDispatcher.SubscribeEntityCreated(std::move(callback));
}

ObserverId World::OnEntityDestroyed(std::function<void(World&, EntityId)> callback)
{
    return m_eventDispatcher.SubscribeEntityDestroyed(std::move(callback));
}

void World::RemoveObserver(ObserverId id)
{
    m_eventDispatcher.Unsubscribe(id);
}

#endif
