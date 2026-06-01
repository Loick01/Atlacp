#pragma once

#include <memory>
#include <vector>

#include <SDL2/SDL.h> // <SDL.h> ?

#include "system/type.hpp"

#define JOYSTICK_DEAD_ZONE 15000 // Must be a positive value between 0 and 32767 (should be a member in JoystickActionController to verify ?)

class ActionController
{
    public:
        virtual ~ActionController() = default;
        
        virtual void GetStateActions() = 0;
        
        virtual bool IsLeftActionState() = 0;
        virtual bool IsRightActionState() = 0;
        virtual bool IsUpActionState() = 0;
        virtual bool IsDownActionState() = 0;
        virtual bool IsPrimaryActionState() = 0; // Unused ?
        virtual bool IsSecondaryActionState() = 0;

        virtual bool IsPressedPoll(const SDL_Event& event) = 0;
        virtual bool IsMotionPoll(const SDL_Event& event) = 0;
        
        virtual bool IsLeftActionPoll(const SDL_Event& event) = 0; // Should I avoid having a SDL_Event parameter ?
        virtual bool IsRightActionPoll(const SDL_Event& event) = 0;
        virtual bool IsUpActionPoll(const SDL_Event& event) = 0;
        virtual bool IsDownActionPoll(const SDL_Event& event) = 0;
        virtual bool IsPrimaryActionPoll(const SDL_Event& event) = 0; 
        virtual bool IsSecondaryActionPoll(const SDL_Event& event) = 0;
};

class KeyboardActionController : public ActionController
{
    private:
        const Uint8* m_state;

    public: 
        void GetStateActions() override;

        bool IsLeftActionState() override;
        bool IsRightActionState() override;
        bool IsUpActionState() override;
        bool IsDownActionState() override;
        bool IsPrimaryActionState() override;
        bool IsSecondaryActionState() override;

        bool IsPressedPoll(const SDL_Event& event) override;
        bool IsMotionPoll(const SDL_Event& event) override;

        bool IsLeftActionPoll(const SDL_Event& event) override;
        bool IsRightActionPoll(const SDL_Event& event) override;
        bool IsUpActionPoll(const SDL_Event& event) override;
        bool IsDownActionPoll(const SDL_Event& event) override;
        bool IsPrimaryActionPoll(const SDL_Event& event) override;
        bool IsSecondaryActionPoll(const SDL_Event& event) override;
};

enum class JoystickState
{
    Active, Neutral
};

class JoystickActionController : public ActionController
{
    private:
        SDL_Joystick* m_joystick;
        JoystickState m_joystickState;
        int m_axisX;
        int m_axisY;

    public:
        JoystickActionController();
        static bool IsJoystickAvailable();
        
        void GetStateActions() override; // Don't need to call it for joystick button events
        
        bool IsLeftActionState() override;
        bool IsRightActionState() override;
        bool IsUpActionState() override;
        bool IsDownActionState() override;
        bool IsPrimaryActionState() override;
        bool IsSecondaryActionState() override;

        bool IsPressedPoll(const SDL_Event& event) override;
        bool IsMotionPoll(const SDL_Event& event) override;
        
        bool IsLeftActionPoll(const SDL_Event& event) override;
        bool IsRightActionPoll(const SDL_Event& event) override;
        bool IsUpActionPoll(const SDL_Event& event) override;
        bool IsDownActionPoll(const SDL_Event& event) override;
        bool IsPrimaryActionPoll(const SDL_Event& event) override;
        bool IsSecondaryActionPoll(const SDL_Event& event) override;
};