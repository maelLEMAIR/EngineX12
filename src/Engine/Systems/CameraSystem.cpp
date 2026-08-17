#include "CameraSystem.h"

#include "RessourceManager.h"
#include "../../Render/Generic/Render.h"
#include "../EngineManager.h"
#include "ECS/World.h"

void CameraSystem::Update(World& world, float _dt)
{
    world.Query<CameraComponent, TransformComponent>([&](CameraComponent& _camera, TransformComponent& _transform)
    {
        Camera* cam = RessourceManager::GetCamera(_camera.camId);
        if (cam == nullptr) return;
    
        Device* d = EngineManager::GetInstance().GetDevice();

        
        cam->SetWorld(_transform.world.ToMat4f32());
        if (_camera.isMainCamera)
        {
            d->SetMainCamera(cam);
            _transform.world.dirty &= ~(uint8)DIRTY_FLAG::WORLD;
        }
    });
}
