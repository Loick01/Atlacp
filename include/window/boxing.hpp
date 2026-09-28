#pragma once

#include <SDL2/SDL.h>

#include "core/core_types.hpp" // AreaSize
#include "core/notifier.hpp"
#include "image/image_types.hpp" // ScreenPosition

enum class BoxingType
{
    Letterboxing, Pillarboxing
};

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

class Bar
{
    private:
        SDL_Rect m_sdlRect;

        AreaSize m_initialSize;
        Vec2f m_currentSize;
        
        BarAnimation m_barAnim;
        BoxingState m_state;

        Direction m_resizeDirection;
        BoxingType m_boxType;
    
        void UpdateSize(const float deltaTime0); // TODO : Rename ?
        
    public:
        Bar(const SDL_Rect& rect, const AreaSize size, const Vec2f& dir, const BoxingType boxType, const Direction resizeDirection);
        
        const SDL_Rect* GetRectPtr() const;
        BarAnimation& GetBarAnimation();
        BoxingState GetState() const;

        bool Check(const ScreenPosition windowCenter);

        void SetState(const BoxingState state);

        void Update(const ScreenPosition windowCenter, const float deltaTime);
};

class Boxing : public Notifier<UselessEvent> // Will Notify() for Order execution // TODO : public Animation ?
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
        void SetBars(const AreaSize windowSize, const AreaSize barsSize, const BoxingType boxType);
        void SetWindowCenter(const ScreenPosition windowCenter);
        
        void StartAnimation(const float speed);
        void Draw() const;
        void Update(const float deltaTime);
};