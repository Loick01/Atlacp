#pragma once

#include <SDL2/SDL.h>

#include "core/core_types.hpp" // AreaSize
#include "image/image_types.hpp" // ScreenPosition

enum class BoxingAnimationState
{
    Idle, Animated
};

struct BarAnimation
{
    Vec2f direction;
    float speed;
};

struct Bar
{
    SDL_Rect sdlRect;

    AreaSize initialSize;
    Vec2f currentSize;
    
    BarAnimation anim;

    Bar() = default;

    Bar(const SDL_Rect& rect, const AreaSize size):
        sdlRect(rect), initialSize(size)
    {
        currentSize = Vec2f(sdlRect.w, sdlRect.h);
    }

    void UpdateSize(const float deltaTime) // TODO : Rename
    {
        currentSize += anim.direction*anim.speed*deltaTime;
        sdlRect.w = currentSize.x;
        sdlRect.h = currentSize.y;
    }
};

class Boxing // TODO : public Animation
{
    private:
        // TODO : std::vector<Bar> ? (Maybe I will use more than two Bar) 
        Bar m_barF; // First bar : Left or Top 
        Bar m_barS; // Second bar : Right or Bottom
        BoxingAnimationState m_state;
        ScreenPosition m_windowCenter;

        SDL_Renderer* m_renderer;
        const SDL_Color m_color;
    
    public:
        Boxing(const SDL_Color& color);

        void SetRenderer(SDL_Renderer* renderer);
        void SetBars(const AreaSize windowSize, const AreaSize barsSize);
        void SetWindowCenter(const ScreenPosition windowCenter);
        
        void Draw() const;
        void Update(const float deltaTime);
};