#pragma once

#include <SDL2/SDL.h>

#include "core/core_types.hpp" // AreaSize
#include "core/notifier.hpp"
#include "image/image_types.hpp" // ScreenPosition

enum class BoxingState
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

struct Bar
{
    SDL_Rect sdlRect;

    AreaSize initialSize;
    Vec2f currentSize;
    
    BarAnimation anim;

    Bar() = default;

    Bar(const SDL_Rect& rect, const AreaSize size, const Vec2f& dir):
        sdlRect(rect), initialSize(size), anim(dir)
    {
        currentSize = Vec2f(sdlRect.w, sdlRect.h);
    }

    void UpdateSize(const float deltaTime) // TODO : Rename ?
    {
        currentSize += anim.direction*anim.speed*deltaTime;
        sdlRect.w = currentSize.x;
        sdlRect.h = currentSize.y;
    }
};

class Boxing : public Notifier<UselessEvent> // Will Notify() for Order execution // TODO : public Animation ?
{
    private:
        std::vector<Bar> m_bars;
        BoxingState m_state;
        ScreenPosition m_windowCenter;

        SDL_Renderer* m_renderer;
        const SDL_Color m_color;
    
    public:
        Boxing(const SDL_Color& color);

        BoxingState GetAnimationState() const;
        
        void SetRenderer(SDL_Renderer* renderer);
        void SetBars(const AreaSize windowSize, const AreaSize barsSize);
        void SetWindowCenter(const ScreenPosition windowCenter);
        
        void StartAnimation(const float speed);
        void Draw() const;
        void Update(const float deltaTime);
};