#pragma once 

#include<iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "element.hpp"
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

        MapMovement HandlePlayerEvent() const;
};

class EditorEventController : public EventController
{
    private:
        ScreenPosition GetMouseScreenPosition() const;
        ScenePosition GetMouseScenePosition(const Camera* camera) const;
        
        Tileset* m_tileset;
        ScenePosition m_last_camera_origin;
        unsigned char m_selected_tile;
        int m_selected_tileset;
        bool m_is_camera_moving;

    public:
        EditorEventController(Tileset* tileset);
        ~EditorEventController();

        void HandleEditorEvent(Tileset* tileset, Tilemap* tilemap, Camera* camera); 
};