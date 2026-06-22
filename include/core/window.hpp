#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "core/core_types.hpp" // AreaSize

struct Boxing
{
    SDL_Rect rectF; // First rectangle : Left or Top 
    SDL_Rect rectS; // Second rectangle : Right or Bottom
};

class Window
{
    private:
        void CreateWindow();

        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        SDL_Color m_bgColor;
        Boxing m_box;
        AreaSize m_size;
        const std::string m_title;

    public:
        Window(const std::string& title, const SDL_Color bgColor);
        ~Window();

        SDL_Renderer* GetRenderer() const;
        AreaSize GetSize() const;
        void SetBoxing(const int x_b, const int y_b, const int w, const int h);
        void ClearRenderer() const;
        void UpdateRender() const;
        void SetBackgroundColor(const SDL_Color bgColor);
        void DrawBoxing();
        void ShowCursor();
        void HideCursor();
};