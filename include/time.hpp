#pragma once

#include <SDL2/SDL.h>

struct Time
{
    uint32_t m_last_time;
    uint32_t m_current_time;
    float m_delta_time;

    Time():
        m_last_time(SDL_GetTicks()) 
    {

    }

    void Update()
    {
        m_current_time = SDL_GetTicks();
        m_delta_time = (m_current_time-m_last_time)/1000.f;
        m_last_time = m_current_time;
    }

    float GetDeltaTime() const
    {
        return m_delta_time;
    }
};