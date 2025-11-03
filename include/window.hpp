#pragma once

#include <iostream>

#include <SDL2/SDL.h>

class Window
{
    private:
        void CreateWindow();
        void InitSdl();

        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        const SDL_Color m_bg_color;
        const unsigned int m_width;
        const unsigned int m_height;
        const char* m_title;

    public:
        Window(const char* title, const unsigned int width, const unsigned int height, const SDL_Color bg_color);
        ~Window();

        SDL_Renderer* GetRenderer();
        bool HasError();
        void ClearRenderer(const SDL_Color);
        void UpdateRender();
};