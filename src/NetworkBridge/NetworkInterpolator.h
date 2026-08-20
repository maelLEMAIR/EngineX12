#ifndef NETWORK_INTERPOLATOR_H_INCLUDED
#define NETWORK_INTERPOLATOR_H_INCLUDED

#include <deque>
#include <chrono>

#include "Core/define.h"

#include "PingManager.h"
#include "Core/Transform.h"

struct TransformSnapshot
{
    float timestamp;
    Vect3f32 pos;
    Vect3f32 scale;
    Quaternion quat;
};

struct NetworkInterpolator
{
    std::deque<TransformSnapshot> buffer;
    static constexpr size_t MAX_BUFFER_SIZE = 32;

    float GetDelay() const
    {
        float latency = PingManager::Get().GetLatency() / 1000.f;
        return (std::max)(0.1f, latency * 2.f);
    }

    void AddSnapshot(const Vect3f32& pos, const Vect3f32& scale,
                     const Quaternion& quat, float timestamp)
    {
        buffer.push_back({ timestamp, pos, scale, quat });
        if (buffer.size() > MAX_BUFFER_SIZE)
            buffer.pop_front();
    }
};

#endif