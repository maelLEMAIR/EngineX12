#ifndef MESH_RENDERER_SYSTEM_CPP_INCLUDED
#define MESH_RENDERER_SYSTEM_CPP_INCLUDED

#include "MeshRendererSystem.h"

#include "EngineManager.h"
#include "RessourceManager.h"
#include "../Components/MeshRenderer.hpp"
#include "../Components/TransformComponent.hpp"
#include "ECS/World.h"

void MeshRendererSystem::Update(World& world, float deltaTime)
{
    Device* pDevice = EngineManager::GetDevice();
    world.Query<MeshRenderer, TransformComponent>([&](MeshRenderer& _mesh, TransformComponent& _transform)
    {   
        Geometry* geo = RessourceManager::GetGeometry(_mesh.geoId);
        Material* mat = RessourceManager::GetMaterial(_mesh.materialId);
        
        if (mat == nullptr || geo == nullptr) return;

        pDevice->SetMaterial(mat);
        pDevice->Draw(geo, _transform.world.ToMat4f32());
    });
}

#endif