#ifndef LIGHTSYSTEM_H_DEFINED
#define LIGHTSYSTEM_H_DEFINED

#include "Components/LightComponent.hpp"
#include "Components/TransformComponent.hpp"
#include "ECS/System.h"

struct LightSystem : public System
{
public:
    void OnRegister(World& world) override;
    void Update(World& world, float _dt) override;
    void OnUnregister(World& world) override;

private:
    Vector<LightDescriptor> lightDescriptors = Vector<LightDescriptor>();
    int count = 0;
};

#endif
