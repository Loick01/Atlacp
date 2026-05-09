#pragma once 

#include <memory>
#include <stdexcept>
#include <vector>

#include <SDL2/SDL.h>

#include "drawable.hpp"
#include "tilemap.hpp"
#include "type.hpp"

#define JOYSTICK_DEAD_ZONE 15000 // Must be a positive value between 0 and 32767 (should be a member in JoystickActionController to verify ?)

class ActionController
{
    public:
        virtual ~ActionController() = default;
        
        virtual void GetStateActions() = 0;
        
        // State Event only (I don't think I will need polled version)
        virtual bool IsLeftAction() = 0;
        virtual bool IsRightAction() = 0;
        virtual bool IsUpAction() = 0;
        virtual bool IsDownAction() = 0;
        
        virtual bool IsPrimaryActionStated() = 0;
        virtual bool IsPrimaryActionPolled(const SDL_Event& event) = 0; // Should I avoid having a SDL_Event parameter ?
        virtual bool IsSecondaryActionStated() = 0;
        // virtual bool IsSecondaryActionPolled() = 0; // ???

        virtual bool IsPressedPolledEvent(const Uint32 eventType) = 0;
};

class KeyboardActionController : public ActionController
{
    private:
        const Uint8* m_state;

    public: 
        void GetStateActions() override;

        bool IsLeftAction() override;
        bool IsRightAction() override;
        bool IsUpAction() override;
        bool IsDownAction() override;
        
        bool IsPrimaryActionStated() override;
        bool IsPrimaryActionPolled(const SDL_Event& event) override;
        bool IsSecondaryActionStated() override;
        
        bool IsPressedPolledEvent(const Uint32 eventType) override;
};

class JoystickActionController : public ActionController
{
    private:
        SDL_Joystick* m_joystick;
        int m_axisX;
        int m_axisY;

    public:
        JoystickActionController();
        static bool IsJoystickAvailable();
        
        void GetStateActions() override;
        
        bool IsLeftAction() override;
        bool IsRightAction() override;
        bool IsUpAction() override;
        bool IsDownAction() override;
        
        bool IsPrimaryActionStated() override;
        bool IsPrimaryActionPolled(const SDL_Event& event) override;
        bool IsSecondaryActionStated() override;

        bool IsPressedPolledEvent(const Uint32 eventType) override;
};

class EventController
{
    protected:
        std::vector<SDL_Event> m_events;
        
    public:
        EventController() = default;
        virtual ~EventController() = default;
        virtual void HandleStateEvents() = 0; 
        virtual void HandlePolledEvents() = 0;
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
        void HandlePolledEvents() override;
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
        void HandlePolledEvents() override;
};

class BattleEventController : public EventController, public EventStateHolder<BattleEventState>
{
    private:
        std::unique_ptr<ActionController> m_actionController; // Should be in EventController ?

    public:
        BattleEventController();

        void HandleStateEvents() override;
        void HandlePolledEvents() override;
};