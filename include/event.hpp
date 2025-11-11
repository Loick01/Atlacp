#pragma once 

#include<iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "drawable.hpp"
#include "tilemap.hpp"
#include "type.hpp"

class EventController
{
    protected:
        EventController();
        ~EventController();

        std::vector<SDL_Event> m_events;
        
    public:
        bool HandleWindowEvents() const;
        void PollAllEvents();
        //virtual void HandleEvents() const = 0;
};

class MapEventController : public EventController
{
    private:

    public:
        MapEventController();
        ~MapEventController();

        MapPosition HandlePlayerEvent() const;
};

class EditorEventController : public EventController
{
    private:
        ScreenPosition GetMousePosition() const;
        unsigned char m_selected_tile;

    public:
        EditorEventController();
        ~EditorEventController();

        void HandleEditorEvent(Tileset* tileset, Tilemap* tilemap); 
};