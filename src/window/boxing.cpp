#include "window/boxing.hpp"

Boxing::Boxing(const SDL_Color& color):
    m_color(color), m_renderer(nullptr), m_state(BoxingState::Fixed)
{}

BoxingState Boxing::GetAnimationState() const
{
    return m_state;
}

void Boxing::SetRenderer(SDL_Renderer* renderer)
{
    m_renderer = renderer;
}

void Boxing::SetBars(const AreaSize windowSize, const AreaSize barsSize)
{
    m_bars.emplace_back(SDL_Rect{0, 0, barsSize.x, barsSize.y}, barsSize, Vec2f{-1.f, 0.f});
    m_bars.emplace_back(SDL_Rect{windowSize.x, windowSize.y, -barsSize.x, -barsSize.y}, barsSize, Vec2f{1.f, 0.f});
}

void Boxing::SetWindowCenter(const ScreenPosition windowCenter)
{
    m_windowCenter = windowCenter;
}

void Boxing::StartAnimation(const float speed)
{
    for (Bar& b : m_bars) {
        b.anim.direction *= -1.f; // TODO : ?
        b.anim.speed = speed;
    }
    m_state = BoxingState::Animated;
}

void Boxing::Draw() const
{
    SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, 255);

    for (const Bar& b : m_bars)
        SDL_RenderFillRect(m_renderer, &b.sdlRect);
}

void Boxing::Update(const float deltaTime)
{
    switch (m_state) {
        case BoxingState::Fixed : {
            break;
        }
        case BoxingState::Animated : {
            for (Bar& b : m_bars)
                b.UpdateSize(deltaTime);

            // TODO : Should not be here (Each Bar should check independently in UpdateSize() ?)
            // TODO : Should also check y axis
            // TODO : Do not use -currentSize for m_barS
            // TODO : Fix sdlRect size to initialSize
            Bar m_barF = m_bars[0];
            Bar m_barS = m_bars[1];
            if (m_barF.currentSize.x > m_windowCenter.x || m_barF.currentSize.x < m_barF.initialSize.x) {
                m_state = BoxingState::Fixed;
                Notify(UselessEvent::None);
                return;
            }

            if (-m_barS.currentSize.x > m_windowCenter.x || -m_barS.currentSize.x < m_barS.initialSize.x) {
                m_state = BoxingState::Fixed;
                Notify(UselessEvent::None);
                return;
            }
            
            break;
        }
    }
}