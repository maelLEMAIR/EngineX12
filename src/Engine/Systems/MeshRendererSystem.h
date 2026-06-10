#ifndef MESH_RENDERER_SYSTEM_H_DEFINED
#define MESH_RENDERER_SYSTEM_H_DEFINED

#include "../ECS/System.h"

struct MeshRenderer;
class World;

class MeshRendererSystem : public System
{
public:
    void Update(World& world, float deltaTime) override;
};

#endif
