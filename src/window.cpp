#include "window.hpp"

Window::Window(const char* title, const unsigned int width, const unsigned int height) :
    m_title(title), m_width(width), m_height(height), m_window(nullptr), m_surface(nullptr)
{
    InitSdl();
    CreateWindow();
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
    
    m_surface = SDL_GetWindowSurface(m_window);
    if(!m_surface) std::cout << "Failed to get the surface from the window\n";
}

void Window::InitSdl()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0) std::cout << "Failed to initialize SDL library\n";
}

SDL_Surface* Window::GetSurface()
{
    return m_surface;
}

bool Window::HasError() 
{ 
    return !m_window || !m_surface;
}

void Window::UpdateSurface()
{
    SDL_UpdateWindowSurface(m_window);
}