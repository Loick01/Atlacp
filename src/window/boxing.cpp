#include "window/boxing.hpp"

Boxing::Boxing(const SDL_Color& color):
    m_color(color), m_renderer(nullptr), m_state(BoxingAnimationState::Idle)
{}

void Boxing::SetRenderer(SDL_Renderer* renderer)
{
    m_renderer = renderer;
}

void Boxing::SetRect(const int x, const int y, const int w, const int h)
{
    m_rectF = SDL_Rect{0, 0, w, h};
    m_rectS = SDL_Rect{x, y, w, h};
}

void Boxing::Draw() const
{
    SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, 255);
    SDL_RenderFillRect(m_renderer, &m_rectF);
    SDL_RenderFillRect(m_renderer, &m_rectS);
}

// void Boxing::Update()
// {
    
// }