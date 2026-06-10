#ifndef SYSTEM_MANAGER_INL_INCLUDED
#define SYSTEM_MANAGER_INL_INCLUDED

#include "SystemManager.h"

template<typename T>
T* SystemManager::RegisterSystem(World& world, int priority)
{
    static_assert(std::is_base_of<System, T>::value, "T doit hériter de System");

    std::type_index idx(typeid(T));
    for (auto& entry : m_systems)
        if (entry.typeIndex == idx)
            return static_cast<T*>(entry.system.get());

    auto system = make_unique<T>();
    system->m_priority = priority;

    T* ptr = system.get();

    m_systems.push_back({
        move(system),
        idx,
        priority
    });

    m_needsSort = true;

    ptr->OnRegister(world);

    return ptr;
}

template<typename T>
T* SystemManager::GetSystem()
{
    std::type_index idx(typeid(T));
    for (auto& entry : m_systems)
        if (entry.typeIndex == idx)
            return static_cast<T*>(entry.system.get());
    return nullptr;
}

template<typename T>
void SystemManager::SetActive(bool active)
{
    std::type_index idx(typeid(T));
    for (auto& entry : m_systems)
    {
        if (entry.typeIndex == idx)
        {
            entry.system->SetActive(active);
            return;
        }
    }
}

template<typename T>
void SystemManager::UnregisterSystem(World& world)
{
    std::type_index idx(typeid(T));
    for (auto it = m_systems.begin(); it != m_systems.end(); ++it)
    {
        if (it->typeIndex == idx)
        {
            it->system->OnUnregister(world);
            m_systems.erase(it);
            return;
        }
    }
}

#endif