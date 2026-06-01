#pragma once

#include <SDL2/SDL_timer.h>

struct Time
{
    uint32_t m_lastTime;
    uint32_t m_currentTime;
    float m_deltaTime;

    Time():
        m_lastTime(SDL_GetTicks()) 
    {}

    void Update()
    {
        m_currentTime = SDL_GetTicks();
        m_deltaTime = (m_currentTime-m_lastTime)/1000.f;
        m_lastTime = m_currentTime;
    }

    float GetDeltaTime() const
    {
        return m_deltaTime;
    }
};