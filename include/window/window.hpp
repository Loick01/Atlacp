#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "core/core_types.hpp" // AreaSize
#include "window/boxing.hpp"

class Window
{
    private:
        void CreateWindow();

        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        SDL_Color m_bgColor;
        Boxing m_boxing;
        AreaSize m_size;
        const std::string m_title;

    public:
        Window(const std::string& title, const SDL_Color bgColor);
        ~Window();

        SDL_Renderer* GetRenderer() const;
        AreaSize GetSize() const;
        void SetBoxing(const ScreenPosition positionBarS, const AreaSize barsSize);
        void ClearRenderer() const;
        void UpdateRender() const;
        void SetBackgroundColor(const SDL_Color bgColor);
        void FrameBoxing(const float deltaTime);
        void ShowCursor();
        void HideCursor();
};