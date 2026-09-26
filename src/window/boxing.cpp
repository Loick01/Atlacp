#include "window/boxing.hpp"

Boxing::Boxing(const SDL_Color& color):
    m_color(color), m_renderer(nullptr), m_state(BoxingAnimationState::Animated)
{}

void Boxing::SetRenderer(SDL_Renderer* renderer)
{
    m_renderer = renderer;
}

void Boxing::SetBars(const AreaSize windowSize, const AreaSize barsSize)
{
    m_barF = Bar({0, 0, barsSize.x, barsSize.y}, barsSize);
    m_barS = Bar({windowSize.x, windowSize.y, -barsSize.x, -barsSize.y}, barsSize);
    
    m_barF.anim = {{1.f, 0.f}, 500.f}; // TODO : Remove
    m_barS.anim = {{-1.f, 0.f}, 500.f}; // TODO : Remove
}

void Boxing::SetWindowCenter(const ScreenPosition windowCenter)
{
    m_windowCenter = windowCenter;
}

void Boxing::Draw() const
{
    SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, 255);
    SDL_RenderFillRect(m_renderer, &m_barF.sdlRect);
    SDL_RenderFillRect(m_renderer, &m_barS.sdlRect);
}

void Boxing::Update(const float deltaTime)
{
    switch (m_state) {
        case BoxingAnimationState::Idle :
            break;
        case BoxingAnimationState::Animated :
            m_barF.UpdateSize(deltaTime);
            m_barS.UpdateSize(deltaTime);

            // TODO : Should not be here (Each Bar should check independently in UpdateSize() ?)
            // TODO : Should also check y axis
            // TODO : Do not use -currentSize for m_barS
            if (m_barF.currentSize.x > m_windowCenter.x)
                m_barF.anim.direction *= -1.f;
            else if (m_barF.currentSize.x < m_barF.initialSize.x)
                m_state = BoxingAnimationState::Idle;

            if (-m_barS.currentSize.x > m_windowCenter.x)
                m_barS.anim.direction *= -1.f;
            else if (-m_barS.currentSize.x < m_barS.initialSize.x)
                m_state = BoxingAnimationState::Idle;
            
            break;
    }
}