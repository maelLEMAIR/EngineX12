#ifndef SCRIPT_SYSTEM_H_INCLUDED
#define SCRIPT_SYSTEM_H_INCLUDED

#include "../ECS/System.h"
#include "../ECS/EventDispatcher.h"

class ScriptSystem : public System
{
public:
    void Update(World& world, float deltaTime) override;
    void OnRegister(World& world) override;

private:
    ObserverId m_onDestroyObserver = INVALID_OBSERVER;
};

#endif