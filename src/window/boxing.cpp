#include "window/boxing.hpp"

Boxing::Boxing(const SDL_Color& color):
    m_color(color), m_renderer(nullptr), m_state(BoxingAnimationState::Idle)
{}

void Boxing::SetRenderer(SDL_Renderer* renderer)
{
    m_renderer = renderer;
}

void Boxing::SetBars(const ScreenPosition positionBarS, const AreaSize barsSize)
{
    m_barF = {{0, 0, barsSize.x, barsSize.y}, barsSize};
    m_barS = {{positionBarS.x, positionBarS.y, barsSize.x, barsSize.y}, barsSize};
}

void Boxing::Draw() const
{
    SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, 255);
    SDL_RenderFillRect(m_renderer, &m_barF.rect);
    SDL_RenderFillRect(m_renderer, &m_barS.rect);
}

void Boxing::Update(const float deltaTime)
{
    switch (m_state) {
        case BoxingAnimationState::Idle :
            break;
        case BoxingAnimationState::Animated :
            break;
    }
}