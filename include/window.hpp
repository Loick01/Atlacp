#pragma once

#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Window
{
    private:
        void CreateWindow();
        void InitSdl() const;

        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        const SDL_Color m_bg_color;
        const unsigned int m_width;
        const unsigned int m_height;
        const std::string m_title;

    public:
        Window(const std::string& title, const unsigned int width, const unsigned int height, const SDL_Color bg_color);
        ~Window();

        SDL_Renderer* GetRenderer() const;
        bool HasError() const;
        void ClearRenderer(const SDL_Color rgb) const;
        void UpdateRender() const;
};