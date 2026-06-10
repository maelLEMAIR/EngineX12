#ifndef CAMERA_SYSTEM_H_DEFINED
#define CAMERA_SYSTEM_H_DEFINED

#include "../ECS/System.h"
#include "Components/CameraComponent.hpp"
#include "Components/TransformComponent.hpp"

struct CameraSystem : public System
{
    void Update(World& world, float deltaTime) override;
};

#endif
