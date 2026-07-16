#ifndef NETWORK_INTERPOLATOR_H_INCLUDED
#define NETWORK_INTERPOLATOR_H_INCLUDED

#include <chrono>

#include "Core/define.h"

#include "PingManager.h"
#include "Core/Transform.h"

struct TransformSnapshot
{
    float timestamp;
    XMFLOAT3 pos;
    XMFLOAT3 scale;
    XMFLOAT4 quat;
};

struct NetworkInterpolator
{
    static constexpr size_t MAX_BUFFER_SIZE = 32;

    TransformSnapshot buffer[MAX_BUFFER_SIZE]{};
    size_t count = 0;
    size_t head  = 0;

    float GetDelay() const
    {
        float latency = PingManager::Get().GetLatency() / 1000.f;
        return (std::max)(0.1f, latency * 2.f);
    }

    void AddSnapshot(const XMFLOAT3& pos, const XMFLOAT3& scale,
                      const XMFLOAT4& quat, float timestamp)
    {
        size_t writeIndex = (head + count) % MAX_BUFFER_SIZE;
        buffer[writeIndex] = { timestamp, pos, scale, quat };

        if (count < MAX_BUFFER_SIZE)
            count++;
        else
            head = (head + 1) % MAX_BUFFER_SIZE;
    }

    TransformSnapshot& At(size_t i) { return buffer[(head + i) % MAX_BUFFER_SIZE]; }
    size_t Size() const { return count; }
};

#endif