#pragma once

#include <iostream>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

struct Boxing
{
    SDL_Rect rect_a; // Left or Top rectangle
    SDL_Rect rect_b; // Right or Bottom rectangle
};

class Window
{
    private:
        void InitSdl() const;
        void CreateWindow();

        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        SDL_Color m_bg_color;
        Boxing m_box;
        int m_width;
        int m_height;
        const std::string m_title;

    public:
        Window(const std::string& title, const SDL_Color bg_color);
        ~Window();

        SDL_Renderer* GetRenderer() const;
        int GetWidth() const;
        int GetHeight() const;
        bool HasError() const;
        void SetBoxing(const int x_b, const int y_b, const int w, const int h);
        void ClearRenderer() const;
        void UpdateRender() const;
        void SetBackgroundColor(const SDL_Color bg_color);
        void DrawBoxing();
        void HideCursor();
};