#include "window.hpp"

Window::Window(const std::string& title, const unsigned int width, const unsigned int height, const SDL_Color bg_color) :
    m_title(title), m_width(width), m_height(height), m_window(nullptr), m_renderer(nullptr), m_bg_color(bg_color)
{
    InitSdl();
    CreateWindow();
    ClearRenderer();
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
    m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, 0);
    if(!m_window) std::cout << "Failed to initialize SDL window\n";
    
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if(!m_renderer) std::cout << "Failed to create a renderer for the window\n";
}

SDL_Renderer* Window::GetRenderer() const
{
    return m_renderer;
}

bool Window::HasError() const
{ 
    return !m_window || !m_renderer;
}

void Window::ClearRenderer() const
{
    SDL_SetRenderDrawColor(m_renderer, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
    SDL_RenderClear(m_renderer);
}

void Window::UpdateRender() const
{
    SDL_RenderPresent(m_renderer);
}