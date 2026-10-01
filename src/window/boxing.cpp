#include "window/boxing.hpp"

Bar::Bar(const SDL_FRect& fRect, const Vec2f& dir, const BoxingType boxType, const Direction resizeDirection):
    m_sdlFRect(fRect), m_initialFRect(fRect), m_barAnim(dir), m_barState(BarState::Open), m_boxingState(BoxingState::Fixed),
    m_boxType(boxType), m_resizeDirection(resizeDirection)
{}

const SDL_FRect* Bar::GetFRectPtr() const
{
    return &m_sdlFRect;
}

BarAnimation& Bar::GetBarAnimation()
{
    return m_barAnim;
}

BarState Bar::GetBarState() const
{
    return m_barState;
}

BoxingState Bar::GetBoxingState() const
{
    return m_boxingState;
}

void Bar::SetBarState(const BarState state)
{
    m_barState = state;
}

void Bar::SetBoxingState(const BoxingState state)
{
    m_boxingState = state;
}

void Bar::UpdateSize(const float deltaTime)
{   
    const Vec2f deltaSize = m_barAnim.direction*m_barAnim.speed*deltaTime;

    switch (m_resizeDirection) {
        case Direction::Right : {
            m_sdlFRect.w += deltaSize.x;
            break;
        }
        case Direction::Left : {
            m_sdlFRect.w -= deltaSize.x;
            m_sdlFRect.x += deltaSize.x;
            break; 
        }
        case Direction::Down : {
            m_sdlFRect.h += deltaSize.y;
            break;
        }
        case Direction::Up : {
            m_sdlFRect.h -= deltaSize.y;
            m_sdlFRect.y += deltaSize.y;
            break;
        }
    }
}

bool Bar::Check(const ScreenPosition windowCenter)
{
    switch (m_boxType) {
        case BoxingType::Letterboxing :
            return m_sdlFRect.h > windowCenter.y || m_sdlFRect.h < m_initialFRect.h;
        case BoxingType::Pillarboxing :
            return m_sdlFRect.w > windowCenter.x || m_sdlFRect.w < m_initialFRect.w;
        default:
            throw std::runtime_error("Unknown BoxingType value");
    }
}

void Bar::Update(const ScreenPosition windowCenter, const float deltaTime)
{
    switch (m_boxingState) {
        case BoxingState::Fixed : {
            break;
        }
        case BoxingState::Animated : {
            UpdateSize(deltaTime);

            if (Check(windowCenter)) {
                m_barAnim.direction *= -1.f;
                m_boxingState = BoxingState::Fixed;
                if (m_barState == BarState::Open)
                    m_sdlFRect = m_initialFRect;
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
        if (b.GetBoxingState() == BoxingState::Animated)
            return BoxingState::Animated;
        
    return BoxingState::Fixed;
}

void Boxing::SetRenderer(SDL_Renderer* renderer)
{
    m_renderer = renderer;
}

void Boxing::SetBars(const AreaSize windowSize, const Vec2f barsSize, const BoxingType boxType)
{
    m_bars.clear();
    switch (boxType) {
        case BoxingType::Letterboxing :
            m_bars.emplace_back(SDL_FRect{0, 0, barsSize.x, barsSize.y}, Vec2f{0.f, 1.f}, boxType, Direction::Down);
            m_bars.emplace_back(SDL_FRect{windowSize.x-barsSize.x, windowSize.y-barsSize.y, barsSize.x, barsSize.y}, Vec2f{0.f, -1.f}, boxType, Direction::Up);
            break;
        case BoxingType::Pillarboxing :
            m_bars.emplace_back(SDL_FRect{0, 0, barsSize.x, barsSize.y}, Vec2f{1.f, 0.f}, boxType, Direction::Right);
            m_bars.emplace_back(SDL_FRect{windowSize.x-barsSize.x, windowSize.y-barsSize.y, barsSize.x, barsSize.y}, Vec2f{-1.f, 0.f}, boxType, Direction::Left);
            break;
    }
}

void Boxing::SetWindowCenter(const ScreenPosition windowCenter)
{
    m_windowCenter = windowCenter;
}

void Boxing::StartAnimation(const float speed)
{
    for (Bar& b : m_bars) {
        BarAnimation& barAnim = b.GetBarAnimation();
        barAnim.speed = speed;
        b.SetBoxingState(BoxingState::Animated);
        if (b.GetBarState() == BarState::Open)
            b.SetBarState(BarState::Close);
        else
            b.SetBarState(BarState::Open);
    }
}

void Boxing::Draw() const
{
    SDL_SetRenderDrawColor(m_renderer, m_color.r, m_color.g, m_color.b, 255);

    for (const Bar& b : m_bars)
        SDL_RenderFillRectF(m_renderer, b.GetFRectPtr());
}

void Boxing::Update(const float deltaTime)
{
    for (Bar& b : m_bars)
        b.Update(m_windowCenter, deltaTime);
    
    if (GetAnimationState() == BoxingState::Fixed) // Each Bars have m_boxingState = Fixed
        Notify(UselessEvent::None);
}