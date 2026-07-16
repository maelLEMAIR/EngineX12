#ifndef INTERPOLATION_SYSTEM_CPP_INCLUDED
#define INTERPOLATION_SYSTEM_CPP_INCLUDED

#include "InterpolationSystem.h"

#include "EngineManager.h"

void InterpolationSystem::Update(World& world, float deltaTime)
{
    float now = EngineManager::GetInstance().GetTotalTime();
    world.Query<NetworkIdentity, NetworkInterpolator, TransformComponent>(
        [&](NetworkIdentity& identity, NetworkInterpolator& interp, TransformComponent& t)
        {
            float renderTime = now - interp.GetDelay();

            if (interp.Size() < 2) return;

            const TransformSnapshot* prev = nullptr;
            const TransformSnapshot* next = nullptr;

            for (size_t i = 0; i + 1 < interp.Size(); i++)
            {
                if (interp.At(i).timestamp     <= renderTime &&
                    interp.At(i + 1).timestamp >= renderTime)
                {
                    prev = &interp.At(i);
                    next = &interp.At(i + 1);
                    break;
                }
            }

            if (!prev || !next) return;
            
            float range = next->timestamp - prev->timestamp;
            if (range <= 0.f) return;
            float alpha = (renderTime - prev->timestamp) / range;
            alpha = std::clamp(alpha, 0.f, 1.f);

            t.local.pos.x = prev->pos.x + (next->pos.x - prev->pos.x) * alpha;
            t.local.pos.y = prev->pos.y + (next->pos.y - prev->pos.y) * alpha;
            t.local.pos.z = prev->pos.z + (next->pos.z - prev->pos.z) * alpha;

            t.local.scale.x = prev->scale.x + (next->scale.x - prev->scale.x) * alpha;
            t.local.scale.y = prev->scale.y + (next->scale.y - prev->scale.y) * alpha;
            t.local.scale.z = prev->scale.z + (next->scale.z - prev->scale.z) * alpha;

            XMVECTOR q1 = XMLoadFloat4(&prev->quat);
            XMVECTOR q2 = XMLoadFloat4(&next->quat);
            XMVECTOR qr = XMQuaternionSlerp(q1, q2, alpha);
            XMStoreFloat4(&t.local.quat, qr);

            t.local.UpdateRotationFromQuaternion();
            t.world = t.local;
            t.world.UpdateMatrix();
        }
    );
}

#endif