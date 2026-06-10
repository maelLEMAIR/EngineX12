#ifndef SCRIPT_H_INCLUDED
#define SCRIPT_H_INCLUDED

#include "../define.h"

class World;

class Script
{
public:
    virtual ~Script() = default;

    virtual void Start    (World& world, EntityId self)                  {}
    virtual void Update   (World& world, EntityId self, float deltaTime) {}
    virtual void OnDestroy(World& world, EntityId self)                  {}

    bool  IsActive()           const { return m_active; }
    void  SetActive(bool active)     { m_active = active; }

    bool  IsStarted()          const { return m_started; }
    
    bool m_started = false;
private:
    bool m_active  = true;
    friend class ScriptCollection;
};

#endif