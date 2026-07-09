#include "LightSystem.h"

#include "EngineManager.h"
#include "ECS/World.h"

void LightSystem::OnRegister(World& world)
{
    count = 0;
    lightDescriptors.clear();
    System::OnRegister(world);
}

void LightSystem::Update(World& world, float _dt)
{
    world.Query<LightComponent, TransformComponent>([&](LightComponent& _l, TransformComponent& _t)
    {
        if ( count < MAX_LIGHTS)
        {
            LightDescriptor desc;
            desc.light.Color = _l.color;
            desc.light.FalloffStart = _l.falloffStart;
            desc.light.FalloffEnd = _l.falloffEnd;
            desc.light.Strength = _l.strength;
            desc.light.SpotPower = _l.spotPower;

            desc.type = _l.type;

            desc.light.Position = _t.world.pos;
            desc.light.Direction = _t.world.forward;
            lightDescriptors.push_back(desc);
            count++;
        }
    });
    
}

void LightSystem::OnUnregister(World& world)
{
    EngineManager::GetDevice()->SetLights(lightDescriptors);
}
