#pragma once

#include <SDL2/SDL_timer.h>

#include "core/notifier.hpp"

struct Time : public Notifier<UselessEvent> // Will Notify() for Order execution
{
    uint32_t m_lastTime;
    uint32_t m_currentTime;
    float m_deltaTime;
    float m_delay;

    Time():
        m_lastTime(SDL_GetTicks()), m_delay(0.f)
    {}

    float GetDeltaTime() const
    {
        return m_deltaTime;
    }

    float GetDelay() const
    {
        return m_delay;
    }

    void Update()
    {
        m_currentTime = SDL_GetTicks();
        m_deltaTime = (m_currentTime-m_lastTime)/1000.f;
        m_lastTime = m_currentTime;

        if (m_delay > 0.f) {
            m_delay -= m_deltaTime;
            if (m_delay <= 0.f) {
                m_delay = 0.f;
                Notify(UselessEvent::None);
            }
        }
    }

    void SetDelay(const float delay)
    {
        m_delay = delay;
    }
};