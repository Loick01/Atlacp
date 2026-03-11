#include "window.hpp"

Window::Window(const std::string& title, const SDL_Color bgColor) :
    m_title(title), m_window(nullptr), m_renderer(nullptr), m_bgColor(bgColor)
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) 
        throw std::runtime_error("Failed to initialize SDL library\n" + std::string(SDL_GetError()));
    CreateWindow();
    SDL_SetRenderDrawColor(m_renderer, m_bgColor.r, m_bgColor.g, m_bgColor.b, 255);
}

Window::~Window()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::CreateWindow()
{
    m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(!m_window)
        throw std::runtime_error("Failed to create SDL window\n" + std::string(SDL_GetError()));
    SDL_GetWindowSize(m_window, &m_width, &m_height);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if(!m_renderer) 
        throw std::runtime_error("Failed to create a SDL renderer\n" + std::string(SDL_GetError()));
}

SDL_Renderer* Window::GetRenderer() const
{
    return m_renderer;
}

int Window::GetWidth() const
{
    return m_width;
}

int Window::GetHeight() const
{
    return m_height;
}

void Window::SetBoxing(const int x_b, const int y_b, const int w, const int h)
{
    m_box.rectF = SDL_Rect{0, 0, w, h};
    m_box.rectS = SDL_Rect{x_b, y_b, w, h};
}

void Window::DrawBoxing()
{
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(m_renderer, &m_box.rectF);
    SDL_RenderFillRect(m_renderer, &m_box.rectS);
    SDL_SetRenderDrawColor(m_renderer, m_bgColor.r, m_bgColor.g, m_bgColor.b, 255);
}

void Window::ClearRenderer() const
{
    SDL_RenderClear(m_renderer);
}

void Window::UpdateRender() const
{
    SDL_RenderPresent(m_renderer);
}

void Window::SetBackgroundColor(const SDL_Color bgColor)
{
    m_bgColor = bgColor;
    SDL_SetRenderDrawColor(m_renderer, m_bgColor.r, m_bgColor.g, m_bgColor.b, 255);
}

void Window::HideCursor()
{
    SDL_ShowCursor(SDL_DISABLE); 
}