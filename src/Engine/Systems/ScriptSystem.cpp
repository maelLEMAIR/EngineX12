#include "ScriptSystem.h"
#include "../ECS/ScriptCollection.h"
#include "../ECS/World.h"

void ScriptSystem::Update(World& world, float deltaTime)
{
    for (auto& [id, col] : world.m_scriptManager.GetAll())
    {
        if (!world.m_entityManager.IsAlive(id)) continue;

        for (auto& entry : col.scripts)
        {
            Script* s = entry.script.get();
            if (!s->IsActive()) continue;

            if (!s->IsStarted())
            {
                s->m_started = true;
                s->Start(world, id);
            }

            s->Update(world, id, deltaTime);
        }
    }
}

void ScriptSystem::OnRegister(World& world)
{
    m_onDestroyObserver = world.OnEntityDestroyed(
        [](World& w, EntityId id)
        {
            ScriptCollection* col = w.m_scriptManager.Get(id);
            if (col == nullptr) return;

            for (auto& entry : col->scripts)
                if (entry.script->IsStarted())
                    entry.script->OnDestroy(w, id);

            w.m_scriptManager.Remove(id);
        }
    );
}