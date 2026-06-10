#ifndef CHRONO_CPP_DEFINED
#define CHRONO_CPP_DEFINED

#pragma comment(lib, "winmm.lib")


#include "Chrono.h"
#include <timeapi.h>

Chrono::Chrono() : m_lastTime(timeGetTime()), m_pauseTime(0), m_isPaused(false), m_totalPausedTime(0), m_startTime(timeGetTime())
{
}

Chrono::~Chrono()
{
}

void Chrono::Start()
{
    if (!m_isPaused) return;

    // Account for time spent paused
    DWORD pausedDuration = timeGetTime() - m_pauseTime;
    m_lastTime += pausedDuration;
    m_totalPausedTime += pausedDuration;
    m_isPaused = false;
}

void Chrono::Pause()
{
    if (m_isPaused) return;

    m_pauseTime = timeGetTime();
    m_isPaused = true;
}

float Chrono::GetElapsedTime()
{
    if (m_isPaused)
    {
        DWORD elapsedTime = m_pauseTime - m_lastTime;
        return (float)elapsedTime / 1000.0f;
    }

    DWORD currentTime = timeGetTime();
    DWORD elapsedTime = currentTime - m_lastTime;
    return (float)elapsedTime / 1000.0f;
}

float Chrono::GetTotalTime()
{
    DWORD currentTime = timeGetTime();
    
    if (m_isPaused)
    {
        // Total time is: (pause time - start time) - total paused time
        DWORD totalTime = m_pauseTime - m_startTime - m_totalPausedTime;
        return (float)totalTime / 1000.0f;
    }
    
    // Total time is: (current time - start time) - total paused time
    DWORD totalTime = currentTime - m_startTime - m_totalPausedTime;
    return (float)totalTime / 1000.0f;
}

float Chrono::Reset()
{
    DWORD currentTime = timeGetTime();

    if (m_isPaused)
    {
        DWORD elapsedTime = m_pauseTime - m_lastTime;
        m_lastTime = currentTime;
        m_pauseTime = currentTime;
        m_startTime = currentTime;
        m_totalPausedTime = 0;
        return (float)elapsedTime / 1000.0f;
    }

    DWORD elapsedTime = currentTime - m_lastTime;
    m_lastTime = currentTime;
    m_startTime = currentTime;
    m_totalPausedTime = 0;
    return (float)elapsedTime / 1000.0f;
}

#endif
