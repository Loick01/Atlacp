#pragma once

#include <iostream>

#include <SDL2/SDL.h>

class Window
{
    private:
        void CreateWindow();
        void InitSdl();

        SDL_Window* m_window;
        SDL_Surface* m_surface;
        const unsigned int m_width;
        const unsigned int m_height;
        const char* m_title;

    public:
        Window(const char* title, const unsigned int width, const unsigned int height);
        ~Window();

        SDL_Surface* GetSurface();
        bool HasError();
        void UpdateSurface();
};