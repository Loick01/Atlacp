#include "window/window.hpp"

#include <stdexcept>

Window::Window(const std::string& title, const SDL_Color bgColor) :
    m_title(title), m_window(nullptr), m_renderer(nullptr), m_bgColor(bgColor), m_boxing({0, 0, 0})
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) 
        throw std::runtime_error("Failed to initialize SDL library\n" + std::string(SDL_GetError()));
    CreateWindow();

    m_boxing.SetRenderer(m_renderer);
    m_boxing.SetWindowCenter(GetSize()/2);
    SDL_SetRenderDrawColor(m_renderer, m_bgColor.r, m_bgColor.g, m_bgColor.b, 255); // TODO : Remove
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
    SDL_GetWindowSize(m_window, &m_size.x, &m_size.y);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!m_renderer) 
        throw std::runtime_error("Failed to create a SDL renderer\n" + std::string(SDL_GetError()));
}

SDL_Renderer* Window::GetRenderer() const
{
    return m_renderer;
}

AreaSize Window::GetSize() const
{
    return m_size;
}

void Window::SetBoxing(const ScreenPosition positionBarS, const AreaSize barsSize)
{
    m_boxing.SetBars(GetSize(), barsSize);
}

void Window::FrameBoxing(const float deltaTime)
{
    m_boxing.Draw();
    m_boxing.Update(deltaTime);
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

void Window::ShowCursor()
{
    SDL_ShowCursor(SDL_ENABLE); 
}

void Window::HideCursor()
{
    SDL_ShowCursor(SDL_DISABLE); 
}