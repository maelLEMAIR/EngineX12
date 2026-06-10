#ifndef SYSTEM_MANAGER_H_INCLUDED
#define SYSTEM_MANAGER_H_INCLUDED

#include "../define.h"
#include "System.h"
#include <memory>
#include <typeindex>

class World;
class EngineManager;

class SystemManager
{
public:
    SystemManager& Get();
    template<typename T>
    T* RegisterSystem(World& world, int priority = 0);
    template<typename T>
    T* GetSystem();
    template<typename T>
    void SetActive(bool active);
    template<typename T>
    void UnregisterSystem(World& world);

    void Update(World& world, float deltaTime);


private:
    static SystemManager* s_pInstance;
    
    void SortSystems();

    struct SystemEntry
    {
        unique_ptr<System>  system;
        type_index          typeIndex;
        int                      priority;
    };

    Vector<SystemEntry> m_systems;
    bool m_needsSort = false;
};

#include "SystemManager.inl"

#endif