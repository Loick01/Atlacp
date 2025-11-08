#pragma once 

#include<iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "drawable.hpp"
#include "tilemap.hpp"
#include "type.hpp"

class EventController
{
    private:
        std::vector<SDL_Event> m_events;

    public:
    EventController();
    ~EventController();

    ScreenPosition GetMousePosition() const;
    MapPosition HandlePlayerEvent() const;
    int HandleWindowEvents() const;
    void HandleEditorEvent(Drawable* tileset, const Tilemap* tilemap) const;
    void PollAllEvents();
};