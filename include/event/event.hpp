#pragma once 

#include <memory>
#include <vector>

#include <SDL2/SDL.h>

#include "event/action.hpp"
#include "event/event_types.hpp" // GameplayEventState, EditorEventState, BattleEventState

// Because of EditorEventController (Remove ?)
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

class GameplayEventController : public EventController, public EventStateHolder<GameplayEventState>
{
    private:
        std::unique_ptr<ActionController> m_actionController; // Should be in EventController ? (but EditorEventController should not have it)

    public:
        GameplayEventController();

        void HandleStateEvents() override;
        void HandlePollEvents() override;
};

class EditorEventController : public EventController, public EventStateHolder<EditorEventState>
{
    private:
        ScreenPosition GetMouseScreenPosition() const;
        ScenePosition GetMouseScenePosition() const;
        
        ScenePosition m_lastCameraOrigin;

        // Will be removed ?
        Tileset& m_tileset; // Should be a UiElement ?
        Tilemap& m_tilemap;
        Camera& m_camera;
        size_t m_layerCount; // Remove ?

    public:
        EditorEventController(Tileset& tileset, Camera& camera, Tilemap& tilemap);

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