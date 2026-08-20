#include "TransformSystem.h"
#include "../ECS/World.h"

void TransformSystem::Update(World& world, float deltaTime)
{
    world.QueryWithEntity<TransformComponent>([&](EntityId id, TransformComponent& _transform)
    {
        UpdateMatrix(_transform, id);
    });
}

void TransformSystem::Sync(World& world, EntityId _entity)
{
    TransformComponent* t = world.GetComponent<TransformComponent>(_entity);
    if (t == nullptr)
        return;

    UpdateMatrix(*t, _entity);
}

void TransformSystem::UpdateMatrix(TransformComponent& _t, EntityId _e)
{
    if (_t.hasParent == true)
    {
        TransformComponent& p = *m_pWorld->GetComponent<TransformComponent>(_t.parent);
        UpdateMatrix(p, _e);

        Mat4f32 worldMat = _t.local.UpdateFromParent(p.world.GetMatrix());
        Vect3f32 pos, scale;
        Quaternion rot;
        worldMat.FastDecompose(&pos, &scale, &rot);

        _t.world.SetPosition(pos);
        _t.world.SetScale(scale);
        _t.world.SetRotationQuaternion(rot);
    }
    else if (_t.local.IsWorldDirty())
    {
        _t.world = _t.local;
    }
}
