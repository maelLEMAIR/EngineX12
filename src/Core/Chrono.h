#ifndef CHRONO_H_DEFINED
#define CHRONO_H_DEFINED

#include "define.h"

class Chrono
{
public:
    Chrono();
    ~Chrono();

    void Start();
    void Pause();

    float GetTotalTime();
    float GetElapsedTime();
    float Reset();

private:
    DWORD m_lastTime;
    DWORD m_pauseTime;
    DWORD m_totalPausedTime;
    DWORD m_startTime;
    bool m_isPaused;
};

#endif

