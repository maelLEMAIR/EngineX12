#ifndef MAP_BUILDER_HPP_INCLUDED
#define MAP_BUILDER_HPP_INCLUDED

#include "MapDefinition.hpp"
#include "Engine/ECS/World.h"
#include "Engine/Components/TransformComponent.hpp"
#include "Components/MeshRenderer.hpp"
#include "RessourceManager.h"

class MapBuilder
{
public:
    static void Build(World& world, bool withRendering)
    {
        for (const auto& obj : MapDefinition::GetLayout())
        {
            EntityId e = world.CreateEntity();

            TransformComponent& t = world.AddComponent<TransformComponent>(e);
            t.local.SetPosition(obj.position);
            t.local.SetScale(obj.scale);

            if (withRendering)
            {
                MeshRenderer& mesh = world.AddComponent<MeshRenderer>(e);
                mesh.geoId = (obj.shape == MapObject::Shape::Cube)
                    ? RessourceManager::GetGeometryId("Cube")
                    : RessourceManager::GetGeometryId("Sphere");
                mesh.materialId = RessourceManager::GetMaterialId("Default");
            }
        }
    }
};

#endif