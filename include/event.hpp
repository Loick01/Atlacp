#pragma once 

#include <memory>
#include <stdexcept>
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
        virtual ~ActionController() = default;
        virtual bool IsLeftAction() = 0;
        virtual bool IsRightAction() = 0;
        virtual bool IsUpAction() = 0;
        virtual bool IsDownAction() = 0;
        virtual bool IsPrimaryAction() = 0; // Rename ?
        virtual bool IsSecondaryAction() = 0; // Rename ?
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
        bool IsPrimaryAction() override;
        bool IsSecondaryAction() override;
        void GetActions() override;
};

class JoystickActionController : public ActionController
{
    private:
        SDL_Joystick* m_joystick;
        int m_axisX;
        int m_axisY;

    public:
        JoystickActionController();
        bool IsLeftAction() override;
        bool IsRightAction() override;
        bool IsUpAction() override;
        bool IsDownAction() override;
        bool IsPrimaryAction() override;
        bool IsSecondaryAction() override;
        void GetActions() override;
};

class EventController
{
    protected:
        std::vector<SDL_Event> m_events;
        
    public:
        EventController() = default;
        virtual ~EventController() = default;
        bool HandleWindowEvents() const;
        virtual void HandleEvents(); // = 0; Will be pure virtual when I have created BattleEventController 
        void PollAllEvents();
};

class GameplayEventController : public EventController
{
    private:
        std::unique_ptr<ActionController> m_actionController;

    public:
        GameplayEventController();
        void HandleEvents() override;

        MapDirection m_eventDirection;
        bool m_isPlayerInteract;
        bool m_isPlayerRunnning;
};

class EditorEventController : public EventController
{
    private:
        ScreenPosition GetMouseScreenPosition() const;
        ScenePosition GetMouseScenePosition() const;
        
        ScenePosition m_lastCameraOrigin;

        // Should be in a struct EventContext ?
        Tile m_selectedTile;
        int m_selectedTileset;
        int m_selectedLayer;
        bool m_isCameraMoving;
        bool m_isReplacingTile;

        // Will be removed ?
        Tileset& m_tileset; // Should be a UiElement ?
        Tilemap& m_tilemap;
        Camera& m_camera;
        size_t m_layerCount; // Remove ?

    public:
        EditorEventController(Tileset& tileset, Camera& camera, Tilemap& tilemap);

        int GetSelectedLayer() const;
        void HandleEvents() override;
};