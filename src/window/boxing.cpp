#include "window/boxing.hpp"

Bar::Bar(const SDL_Rect& rect, const AreaSize size, const Vec2f& dir, const BoxingType boxType, const Direction resizeDirection):
    m_sdlRect(rect), m_initialSize(size), m_barAnim(dir), m_state(BoxingState::Fixed),
    m_boxType(boxType), m_resizeDirection(resizeDirection)
{
    m_currentSize = Vec2f(m_sdlRect.w, m_sdlRect.h);
}

const SDL_Rect* Bar::GetRectPtr() const
{
    return &m_sdlRect;
}

BarAnimation& Bar::GetBarAnimation()
{
    return m_barAnim;
}

BoxingState Bar::GetState() const
{
    return m_state;
}

void Bar::SetState(const BoxingState state)
{
    m_state = state;
}

void Bar::UpdateSize(const float deltaTime)
{   
    const Vec2f deltaSize = m_barAnim.direction*m_barAnim.speed*deltaTime;
    m_currentSize += deltaSize;

    switch (m_resizeDirection) {
        case Direction::Right : {
            m_sdlRect.w = m_currentSize.x;
            break;
        }
        case Direction::Left : {
            m_sdlRect.w = m_currentSize.x;
            m_sdlRect.x -= deltaSize.x; // TODO : I need the window size instead of directly updating m_sdlRect
            break; 
        }
        case Direction::Down : {
            m_sdlRect.h = m_currentSize.y;
            break;
        }
        case Direction::Up : {
            m_sdlRect.h = m_currentSize.y;
            m_sdlRect.y -= deltaSize.y;
            break;
        }
    }
}

bool Bar::Check(const ScreenPosition windowCenter)
{
    switch (m_boxType) {
        case BoxingType::Letterboxing :
            return m_currentSize.y > windowCenter.y || m_currentSize.y < m_initialSize.y;
        case BoxingType::Pillarboxing :
            return m_currentSize.x > windowCenter.x || m_currentSize.x < m_initialSize.x;
        default:
            throw std::runtime_error("Unknown BoxingType value");
    }
}

void Bar::Update(const ScreenPosition windowCenter, const float deltaTime)
{
    switch (m_state) {
        case BoxingState::Fixed : {
            break;
        }
        case BoxingState::Animated : {
            UpdateSize(deltaTime);

            if (Check(windowCenter)) {
                m_barAnim.direction *= -1.f; // TODO : Should not be here ?
                m_state = BoxingState::Fixed;
            }
            
            break;
        }
    }
}

Boxing::Boxing(const SDL_Color& color):
    m_color(color), m_renderer(nullptr)
{}

BoxingState Boxing::GetAnimationState() const
{
    for (const Bar& b : m_bars)
        if (b.GetState() == BoxingState::Animated)
            return BoxingState::Animated;
        
    return BoxingState::Fixed;
}

void Boxing::SetRenderer(SDL_Renderer* renderer)
{
    m_renderer = renderer;
}

void Boxing::SetBars(const AreaSize windowSize, const AreaSize barsSize, const BoxingType boxType)
{
    m_bars.clear();
    switch (boxType) {
        case BoxingType::Letterboxing :
            m_bars.emplace_back(SDL_Rect{0, 0, barsSize.x, barsSize.y}, barsSize, Vec2f{0.f, 1.f}, boxType, Direction::Down);
            m_bars.emplace_back(SDL_Rect{windowSize.x-barsSize.x, windowSize.y-barsSize.y, barsSize.x, barsSize.y}, barsSize, Vec2f{0.f, -1.f}, boxType, Direction::Up);
            break;
        case BoxingType::Pillarboxing :
            m_bars.emplace_back(SDL_Rect{0, 0, barsSize.x, barsSize.y}, barsSize, Vec2f{1.f, 0.f}, boxType, Direction::Right);
            m_bars.emplace_back(SDL_Rect{windowSize.x-barsSize.x, windowSize.y-barsSize.y, barsSize.x, barsSize.y}, barsSize, Vec2f{1.f, 0.f}, boxType, Direction::Left);
            break;
    }
}

void Boxing::SetWindowCenter(const ScreenPosition windowCenter)
{
    m_windowCenter = windowCenter; // TODO : m_windowCenter is not the correct value to use, I need the distance from each Bar to windowCenter 
}

void Boxing::StartAnimation(const float speed)
{
    for (Bar& b : m_bars) {
        BarAnimation& barAnim = b.GetBarAnimation();
        barAnim.speed = speed;
        b.SetState(BoxingState::Animated);
    }
}

void Boxing::Draw() const
{
    SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, 255);

    for (const Bar& b : m_bars)
        SDL_RenderFillRect(m_renderer, b.GetRectPtr());
}

void Boxing::Update(const float deltaTime)
{
    for (Bar& b : m_bars)
        b.Update(m_windowCenter, deltaTime);
    
    if (GetAnimationState() == BoxingState::Fixed) { // Each Bars have m_state = Fixed
        // for (Bar& b : m_bars)
            // TODO : Reset the SDL_Rect size to b.m_initialSize
        Notify(UselessEvent::None);
    }
}