#ifndef SYSTEM_MANAGER_CPP_INCLUDED
#define SYSTEM_MANAGER_CPP_INCLUDED

#include "SystemManager.h"
#include <algorithm>

#include "EngineManager.h"
#include "Generic/Base/Window.h"


void SystemManager::Update(World& world, float deltaTime)
{
    if (m_needsSort)
    {
        SortSystems();
        m_needsSort = false;
    }

    for (auto& entry : m_systems)
        if (entry.system->IsActive())
            entry.system->Update(world, deltaTime);
        
}

void SystemManager::SortSystems()
{
    std::stable_sort(
        m_systems.begin(),
        m_systems.end(),
        [](const SystemEntry& a, const SystemEntry& b) {
            return a.priority < b.priority;
        }
    );
}

#endif