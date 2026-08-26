#pragma once 

#include <memory>
#include <vector>

#include <SDL2/SDL.h>

#include "event/action.hpp"
#include "event/event_types.hpp" // GameMapEventState, EditorMapEventState, BattleEventState

// Because of EditorMapEventController (Remove ?)
class Camera;
class Tilemap;
class Tileset;

class EventController
{
    protected:
        std::vector<SDL_Event> m_events;
        
    public:
        EventController() = default;
        virtual ~EventController() = default;
        virtual void HandleStateEvents() = 0; 
        virtual void HandlePollEvents() = 0;
        bool HandleWindowEvents() const;
        void PollAllEvents();
};

class MainMenuEventController : public EventController, public EventStateHolder<MainMenuEventState>
{
    private:
        std::unique_ptr<ActionController> m_actionController; // Should be in EventController ?

    public:
        MainMenuEventController();

        void HandleStateEvents() override;
        void HandlePollEvents() override;
};

class GameMapEventController : public EventController, public EventStateHolder<GameMapEventState>
{
    private:
        std::unique_ptr<ActionController> m_actionController; // Should be in EventController ? (but EditorMapEventController should not have it)

    public:
        GameMapEventController();

        void HandleStateEvents() override;
        void HandlePollEvents() override;
};

class EditorMapEventController : public EventController, public EventStateHolder<EditorMapEventState>
{
    private:
        ScreenPosition GetMouseScreenPosition() const;
        ScenePosition GetMouseScenePosition() const;
        
        // Rename ?
        ScenePosition m_lastKnownPosition; // Tileset screen position when m_tileset.GetShouldDraw(), otherwise m_camera scene position 

        // Will be removed ?
        Tileset& m_tileset; // Should be a UiElement ?
        Tilemap& m_tilemap;
        Camera& m_camera;
        size_t m_layerCount; // Remove ?

    public:
        EditorMapEventController(Tileset& tileset, Camera& camera, Tilemap& tilemap);

        void HandleStateEvents() override;
        void HandlePollEvents() override;
};

class BattleEventController : public EventController, public EventStateHolder<BattleEventState>
{
    private:
        std::unique_ptr<ActionController> m_actionController; // Should be in EventController ?

    public:
        BattleEventController();

        void HandleStateEvents() override;
        void HandlePollEvents() override;
};