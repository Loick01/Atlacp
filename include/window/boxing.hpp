#pragma once

#include <SDL2/SDL.h>

#include "core/core_types.hpp" // AreaSize
#include "core/notifier.hpp"
#include "image/image_types.hpp" // ScreenPosition

enum class BoxingType
{
    Letterboxing, Pillarboxing
};

enum class BarState // TODO : Rename
{
    Open, Close
};

enum class BoxingState // TODO : Rename
{
    Fixed, Animated
};

struct BarAnimation
{
    static constexpr float defaultSpeed = 50.f;

    Vec2f direction;
    float speed;

    BarAnimation(const Vec2f& dir):
        direction(dir), speed(defaultSpeed)
    {}
};

class Bar
{
    private:
        SDL_FRect m_sdlFRect;
        const SDL_FRect m_initialFRect;
        
        BarAnimation m_barAnim;
        BarState m_barState;
        BoxingState m_boxingState;

        Direction m_resizeDirection;
        BoxingType m_boxType;
    
        void UpdateSize(const float deltaTime0); // TODO : Rename ?
        
    public:
        Bar(const SDL_FRect& fRect, const Vec2f& dir, const BoxingType boxType, const Direction resizeDirection);
        
        const SDL_FRect* GetFRectPtr() const;
        BarAnimation& GetBarAnimation();
        BarState GetBarState() const;
        BoxingState GetBoxingState() const;

        bool Check(const ScreenPosition windowCenter);

        void SetBarState(const BarState state);
        void SetBoxingState(const BoxingState state);

        void Update(const ScreenPosition windowCenter, const float deltaTime);
};

class Boxing : public Notifier<UselessEvent> // Will Notify() for Order execution
{
    private:
        std::vector<Bar> m_bars;
        ScreenPosition m_windowCenter;

        SDL_Renderer* m_renderer;
        const SDL_Color m_color;
    
    public:
        Boxing(const SDL_Color& color);

        BoxingState GetAnimationState() const;
        
        void SetRenderer(SDL_Renderer* renderer);
        void SetBars(const AreaSize windowSize, const Vec2f barsSize, const BoxingType boxType);
        void SetWindowCenter(const ScreenPosition windowCenter);
        
        void StartAnimation(const float speed);
        void Draw() const;
        void Update(const float deltaTime);
};