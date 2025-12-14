#pragma once 

#include<iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "drawable.hpp"
#include "entity.hpp"
#include "tilemap.hpp"
#include "type.hpp"

#define JOYSTICK_DEAD_ZONE 15000

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

class GameplayEventController : public EventController
{
    private:
        SDL_Joystick* m_joystick;

    public:
        GameplayEventController();
        ~GameplayEventController();

        MapDirection HandlePlayerEvent() const;
};

class EditorEventController : public EventController
{
    private:
        ScreenPosition GetMouseScreenPosition() const;
        ScenePosition GetMouseScenePosition(const Camera* camera) const;
        
        Tileset* m_tileset;
        ScenePosition m_last_camera_origin;

        Tile m_selected_tile;
        int m_selected_tileset;
        bool m_is_camera_moving;
        bool m_is_replacing_tile;

    public:
        EditorEventController(Tileset* tileset);
        ~EditorEventController();

        void HandleEditorEvent(Tileset* tileset, Tilemap* tilemap, Camera* camera); 
};