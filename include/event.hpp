#pragma once 

#include<iostream>

#include <SDL2/SDL.h>

class EventController
{
    private:
        SDL_Event m_event;

    public:
    EventController();
    ~EventController();

    int HandleWindowEvents();
};