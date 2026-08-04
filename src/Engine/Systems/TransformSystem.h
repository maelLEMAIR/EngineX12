#ifndef TRANSFORM_SYSTEM_H_DEFINED
#define TRANSFORM_SYSTEM_H_DEFINED

#include "../ECS/System.h"
#include "../Components/TransformComponent.hpp"

class TransformSystem : public System
{
public:
    void Update(World& world, float deltaTime) override;

    void Sync(World& world, EntityId _entity);

private:
    bool IsDirty(TransformD3D& _transform, uint32 _flag);
    void UpdateMatrix(TransformComponent& _t, EntityId _e);
};

#endif
