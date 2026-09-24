#pragma once

#include <SDL2/SDL.h>

enum class BoxingAnimationState
{
    Idle, Animated
};

class Boxing
{
    private:
        SDL_Rect m_rectF; // First rectangle : Left or Top 
        SDL_Rect m_rectS; // Second rectangle : Right or Bottom
    
        BoxingAnimationState m_state;
        const SDL_Color m_color;
        SDL_Renderer* m_renderer;
    
    public:
        Boxing(const SDL_Color& color);

        void SetRenderer(SDL_Renderer* renderer);
        void SetRect(const int x, const int y, const int w, const int h);

        void Draw() const;
        // void Update();
};