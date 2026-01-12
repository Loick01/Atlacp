#include "window.hpp"

Window::Window(const std::string& title, const SDL_Color bg_color) :
    m_title(title), m_window(nullptr), m_renderer(nullptr), m_bg_color(bg_color)
{
    InitSdl();
    CreateWindow();
    SDL_SetRenderDrawColor(m_renderer, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
}

Window::~Window()
{
    IMG_Quit(); // In TextureController
    TTF_Quit(); // In TextureController
    Mix_CloseAudio(); // In SoundController
    Mix_Quit(); // InSoundController (Optionnal if use only Mix_OpenAudio, but required if use Mix_Init())
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

void Window::InitSdl() const
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) std::cout << "Failed to initialize SDL library\n";
    int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
    // SDL_image initialization will be in TextureController
    if ((IMG_Init(img_flags) & img_flags) != img_flags) std::cout << "Failed to initialize SDL image library\n";
    if (TTF_Init() < 0) std::cout << "Failed to initialize SDL font library\n"; // Will use TTF_GetError()

    // SDL_mixer initialization will be in SoundController
    // https://lazyfoo.net/SDL_tutorials/lesson11/index.php
    int mix_flags = MIX_INIT_OGG;
    if ((Mix_Init(mix_flags) & mix_flags) != mix_flags) std::cout << "Can't initialize SDL_mixer\n"; // Mix_GetError()

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
        std::cout << "Failed to initialize SDL2_mixer library\n";
}

void Window::CreateWindow()
{
    m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(!m_window) std::cout << "Failed to initialize SDL window\n";
    SDL_GetWindowSize(m_window, &m_width, &m_height);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if(!m_renderer) std::cout << "Failed to create a renderer for the window\n";
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

void Window::SetBoxing(const int x_b, const int y_b, const int w, const int h)
{
    m_box.rect_a = SDL_Rect{0, 0, w, h};
    m_box.rect_b = SDL_Rect{x_b, y_b, w, h};
}

void Window::DrawBoxing()
{
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(m_renderer, &m_box.rect_a);
    SDL_RenderFillRect(m_renderer, &m_box.rect_b);
    SDL_SetRenderDrawColor(m_renderer, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
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

void Window::HideCursor()
{
    SDL_ShowCursor(SDL_DISABLE); 
}