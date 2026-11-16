#pragma once 

#include <memory>
#include <vector>

#include <SDL2/SDL.h>

#include "event/action.hpp"
#include "event/event_types.hpp" // GameMapEventState, EditorMapEventState, BattleEventState

class Camera;
class Tilemap;
class Tileset;

class EventController
{
    protected:
        std::vector<SDL_Event> m_events;

        std::unique_ptr<ActionController> m_actionController;
        
        void InitActionController();
        
    public:
        EventController();
        virtual ~EventController() = default;
        virtual void HandleStateEvents() = 0; 
        virtual void HandlePollEvents() = 0;
        bool HandleWindowEvents() const;
        void PollAllEvents();
};

class MainMenuEventController : public EventController, public EventStateHolder<MainMenuEventState>
{
    public:
        MainMenuEventController();

        void HandleStateEvents() override;
        void HandlePollEvents() override;
};

class GameMapEventController : public EventController, public EventStateHolder<GameMapEventState>
{
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
        
        ScenePosition m_lastPosition; // Tileset screen position when m_tileset.GetShouldDraw(), otherwise m_camera scene position

        Tileset& m_tileset;
        Tilemap& m_tilemap;
        Camera& m_camera;
        
        size_t m_layerCount;

    public:
        EditorMapEventController(Tileset& tileset, Camera& camera, Tilemap& tilemap);

        void HandleStateEvents() override;
        void HandlePollEvents() override;

        void SetLayerCount(const unsigned int layerCount);
        void Reset();
};

class BattleEventController : public EventController, public EventStateHolder<BattleEventState>
{
    public:
        BattleEventController();

        void HandleStateEvents() override;
        void HandlePollEvents() override;
};