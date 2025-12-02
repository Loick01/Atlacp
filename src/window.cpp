#include "window.hpp"

Window::Window(const std::string& title, const int grid_width, const int grid_height, const SDL_Color bg_color) :
    m_title(title), m_window(nullptr), m_renderer(nullptr), m_bg_color(bg_color)
{
    m_width = grid_width*32;
    m_height = grid_height*32;
    InitSdl();
    CreateWindow();
    SDL_SetRenderDrawColor(m_renderer, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
    // SDL_ShowCursor(SDL_DISABLE); 
}

Window::~Window()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::InitSdl() const
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) std::cout << "Failed to initialize SDL library\n";
    int flags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(flags) & flags) != flags) std::cout << "Failed to initialize SDL image library\n";
}

void Window::CreateWindow()
{
    m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(!m_window) std::cout << "Failed to initialize SDL window\n";
    
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if(!m_renderer) std::cout << "Failed to create a renderer for the window\n";

    SDL_RenderSetLogicalSize(m_renderer, m_width, m_height);
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

bool Window::HasError() const
{ 
    return !m_window || !m_renderer;
}

void Window::ClearRenderer() const
{
    SDL_RenderClear(m_renderer);
}

void Window::UpdateRender() const
{
    SDL_RenderPresent(m_renderer);
}

void Window::SetBackgroundColor(const SDL_Color bg_color)
{
    m_bg_color = bg_color;
    SDL_SetRenderDrawColor(m_renderer, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
}