#ifndef INTERPOLATION_SYSTEM_H_INCLUDED
#define INTERPOLATION_SYSTEM_H_INCLUDED

#include "Core/define.h"
#include "Core/Utils.hpp"
#include "Engine/ECS/System.h"
#include "NetworkInterpolator.h"
#include "NetworkIdentity.h"
#include "Engine/Components/TransformComponent.hpp"
#include "Engine/ECS/World.h"

#include <cmath>

namespace
{
    Quaternion Slerp(Quaternion const& _a, Quaternion const& _b, float _t)
    {
        Quaternion a = _a;
        Quaternion b = _b;

        float dot = Quaternion::Dot(a, b);
        if (dot < 0.0f)
        {
            b = Quaternion(-b.x, -b.y, -b.z, -b.w);
            dot = -dot;
        }

        const float epsilon = 1e-4f;
        if (dot > 1.0f - epsilon)
        {
            Quaternion result(
                a.x + (b.x - a.x) * _t,
                a.y + (b.y - a.y) * _t,
                a.z + (b.z - a.z) * _t,
                a.w + (b.w - a.w) * _t);
            return result.Normalized();
        }

        float theta0 = acosf(dot);
        float theta = theta0 * _t;
        float sinTheta = sinf(theta);
        float sinTheta0 = sinf(theta0);

        float s0 = cosf(theta) - dot * sinTheta / sinTheta0;
        float s1 = sinTheta / sinTheta0;

        return Quaternion(
            a.x * s0 + b.x * s1,
            a.y * s0 + b.y * s1,
            a.z * s0 + b.z * s1,
            a.w * s0 + b.w * s1);
    }
}

class InterpolationSystem : public System
{
public:
    void Update(World& world, float dt) override
    {
        m_currentTime += dt;
        world.QueryWithEntity<NetworkIdentity, NetworkInterpolator, TransformComponent>(
            [&](EntityId id, NetworkIdentity& identity,
                NetworkInterpolator& interp, TransformComponent& t)
            {
                float renderTime = m_currentTime - interp.GetDelay();

                if (identity.isOwner) return;
                if (interp.buffer.size() < 2) return;

                const TransformSnapshot* prev = nullptr;
                const TransformSnapshot* next = nullptr;

                for (size_t i = 0; i + 1 < interp.buffer.size(); i++)
                {
                    if (interp.buffer[i].timestamp     <= renderTime &&
                        interp.buffer[i + 1].timestamp >= renderTime)
                    {
                        prev = &interp.buffer[i];
                        next = &interp.buffer[i + 1];
                        break;
                    }
                }

                if (!prev || !next) return;

                float range = next->timestamp - prev->timestamp;
                if (range <= 0.f) return;
                float alpha = (renderTime - prev->timestamp) / range;
                alpha = Clamp(alpha, 0.f, 1.f);

                t.local.SetPosition(Vect3f32(
                    prev->pos.x + (next->pos.x - prev->pos.x) * alpha,
                    prev->pos.y + (next->pos.y - prev->pos.y) * alpha,
                    prev->pos.z + (next->pos.z - prev->pos.z) * alpha));

                t.local.SetScale(Vect3f32(
                    prev->scale.x + (next->scale.x - prev->scale.x) * alpha,
                    prev->scale.y + (next->scale.y - prev->scale.y) * alpha,
                    prev->scale.z + (next->scale.z - prev->scale.z) * alpha));

                t.local.SetRotationQuaternion(Slerp(prev->quat, next->quat, alpha));
            }
        );
    }

private:
    float m_currentTime = 0.f;
};

#endif