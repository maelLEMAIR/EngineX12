#ifndef INTERPOLATION_SYSTEM_H_INCLUDED
#define INTERPOLATION_SYSTEM_H_INCLUDED

#include "Core/define.h"
#include "Engine/ECS/System.h"
#include "../NetworkInterpolator.h"
#include "../NetworkIdentity.h"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/ECS/World.h"

class World;

class InterpolationSystem : public System
{
public:
    void Update(World& world, float dt) override;

private:
    float m_currentTime = 0.f;
};

#endif