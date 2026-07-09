#ifndef NETWORK_QUEUE_H_INCLUDED
#define NETWORK_QUEUE_H_INCLUDED

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include "Core/define.h"

struct NetworkPacket
{
    std::vector<uint8_t> data;
    struct sockaddr_in address;
};

class NetworkQueue
{
public:
    void Push(NetworkPacket&& packet)
    {
        LockGuard lock(m_mutex);
        m_queue.push(std::move(packet));
    }

    bool Pop(NetworkPacket& out)
    {
        LockGuard lock(m_mutex);
        if (m_queue.empty()) return false;
        out = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

private:
    Queue<NetworkPacket> m_queue;
    Mutex                m_mutex;
};

#endif