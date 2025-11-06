#pragma once 

#include<iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "type.hpp"

class EventController
{
    private:
        std::vector<SDL_Event> m_events;

    public:
    EventController();
    ~EventController();

    void PollAllEvents();
    int HandleWindowEvents() const;
    Position HandlePlayerEvent() const;
};