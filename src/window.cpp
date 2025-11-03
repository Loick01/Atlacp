#include "window.hpp"

Window::Window(const char* title, const unsigned int width, const unsigned int height, const SDL_Color bg_color) :
    m_title(title), m_width(width), m_height(height), m_window(nullptr), m_renderer(nullptr), m_bg_color(bg_color)
{
    InitSdl();
    CreateWindow();
    ClearRenderer(m_bg_color);
}

Window::~Window()
{
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::CreateWindow()
{
    m_window = SDL_CreateWindow(m_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, 0);
    if(!m_window) std::cout << "Failed to initialize SDL window\n";
    
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if(!m_renderer) std::cout << "Failed to create a renderer for the window\n";
}

void Window::InitSdl()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) std::cout << "Failed to initialize SDL library\n";
}

bool Window::HasError() 
{ 
    return !m_window || !m_renderer;
}

SDL_Renderer* Window::GetRenderer()
{
    return m_renderer;
}

void Window::ClearRenderer(const SDL_Color rgb)
{
    SDL_SetRenderDrawColor(m_renderer, rgb.r, rgb.g, rgb.b, 255);
    SDL_RenderClear(m_renderer);
}

void Window::UpdateRender()
{
    SDL_RenderPresent(m_renderer);
}