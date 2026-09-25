#pragma once

#include <SDL2/SDL.h>

#include "core/core_types.hpp" // AreaSize
#include "image/image_types.hpp" // ScreenPosition

enum class BoxingAnimationState
{
    Idle, Animated
};

struct Bar
{
    SDL_Rect rect;
    AreaSize size;
};

class Boxing // TODO : public Animation
{
    private:
        Bar m_barF; // First bar : Left or Top 
        Bar m_barS; // Second bar : Right or Bottom
    
        BoxingAnimationState m_state;
        const SDL_Color m_color;
        SDL_Renderer* m_renderer;
    
    public:
        Boxing(const SDL_Color& color);

        void SetRenderer(SDL_Renderer* renderer);
        void SetBars(const ScreenPosition positionBarS, const AreaSize barsSize); // I don't need positionBarF

        void Draw() const;
        void Update(const float deltaTime);
};