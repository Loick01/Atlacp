#include "window/boxing.hpp"

Boxing::Boxing(const SDL_Color& color):
    m_color(color), m_renderer(nullptr)
{}

BoxingState Boxing::GetAnimationState() const
{
    for (const Bar& b : m_bars)
        if (b.state == BoxingState::Animated)
            return BoxingState::Animated;
        
    return BoxingState::Fixed;
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
        b.state = BoxingState::Animated;
    }
}

void Boxing::Draw() const
{
    SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, 255);

    for (const Bar& b : m_bars)
        SDL_RenderFillRect(m_renderer, &b.sdlRect);
}

void Boxing::Update(const float deltaTime)
{
    for (Bar& b : m_bars)
        b.Update(m_windowCenter, deltaTime);
    
    if (GetAnimationState() == BoxingState::Fixed) { // Each Bars have state = Fixed
        // for (Bar& b : m_bars)
            // TODO : Reset the SDL_Rect size to b.initialSize
        Notify(UselessEvent::None);
    }
}