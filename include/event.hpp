#pragma once 

#include<iostream>
#include <vector>

#include <SDL2/SDL.h>

#include "drawable.hpp"
#include "entity.hpp"
#include "tilemap.hpp"
#include "type.hpp"

#define JOYSTICK_DEAD_ZONE 15000 // Must be a positive value between 0 and 32767 (should be a member in JoystickActionController to verify ?)

class ActionController
{
    public:
        virtual bool IsLeftAction() = 0;
        virtual bool IsRightAction() = 0;
        virtual bool IsUpAction() = 0;
        virtual bool IsDownAction() = 0;
        virtual void GetActions() = 0;
};

class KeyboardActionController : public ActionController
{
    private:
        const Uint8* m_state;

    public: 
        bool IsLeftAction() override;
        bool IsRightAction() override;
        bool IsUpAction() override;
        bool IsDownAction() override;
        void GetActions() override;
};

class JoystickActionController : public ActionController
{
    private:
        SDL_Joystick* m_joystick;
        int m_axis_x;
        int m_axis_y;

    public:
        JoystickActionController();
        bool IsLeftAction() override;
        bool IsRightAction() override;
        bool IsUpAction() override;
        bool IsDownAction() override;
        void GetActions() override;
};

class EventController
{
    protected:
        std::vector<SDL_Event> m_events;
        
    public:
        EventController();
        bool HandleWindowEvents() const;
        virtual void HandleEvents(); // = 0; Will be pure virtual when I have created BattleEventController 
        void PollAllEvents();
};

class GameplayEventController : public EventController
{
    private:
        ActionController* m_action_controller;

    public:
        GameplayEventController();
        void HandleEvents() override;

        MapDirection m_event_direction; // Should be private ?
};

class EditorEventController : public EventController
{
    private:
        ScreenPosition GetMouseScreenPosition() const;
        ScenePosition GetMouseScenePosition() const;
        
        ScenePosition m_last_camera_origin;

        // Should be in a struct EventContext ?
        Tile m_selected_tile;
        int m_selected_tileset;
        int m_selected_layer;
        bool m_is_camera_moving;
        bool m_is_replacing_tile;

        // Will be removed ?
        Tileset& m_tileset; // Should be a UiElement ?
        Tilemap& m_tilemap;
        Camera& m_camera;
        size_t m_layer_count; // Remove ?

    public:
        EditorEventController(Tileset& tileset, Camera& camera, Tilemap& tilemap);

        int GetSelectedLayer() const;
        void HandleEvents() override;
};