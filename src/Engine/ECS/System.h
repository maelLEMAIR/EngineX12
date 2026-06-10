#ifndef SYSTEM_H_INCLUDED
#define SYSTEM_H_INCLUDED

#include "../define.h"

class World;

class System
{
public:
    virtual ~System() = default;
    virtual void Update(World& world, float deltaTime) = 0;
    virtual void OnRegister(World& world) { m_pWorld = &world; }
    virtual void OnUnregister(World& world) {}

    bool  IsActive()           const { return m_active; }
    void  SetActive(bool active)     { m_active = active; }
    int   GetPriority()        const { return m_priority; }

protected:
    World* m_pWorld;
    
private:
    friend class SystemManager;
    bool m_active   = true;
    int  m_priority = 0;
};

#endif