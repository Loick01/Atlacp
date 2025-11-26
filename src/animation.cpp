#include "animation.hpp"

Animation::Animation(const int step, const float frame_duration, const int sprite_size, const Pair<int> spritesheet_size):
    m_step(step), m_frame_duration(frame_duration), m_is_first_movement(true), m_sprite_index(0), m_sprite_size(sprite_size),
    m_spritesheet_size(spritesheet_size)
{
    // Will use the first element in std::vector<Pair<int>> to initialize the first sprite
    m_current_sprite = Pair<int>{0, 0};
}

Animation::~Animation()
{

}

Pair<int> Animation::GetCurrentSprite() const
{
    return m_current_sprite;
}

int Animation::GetSpriteSize() const
{
    return m_sprite_size;
}

void Animation::Initialize()
{
    if (m_is_first_movement){ // When this is the first movement since the player take the control
        m_is_first_movement = false;
        m_count = 0.f;
        m_sprite_index = 1; // Important --> Force to don't use the idle sprite 
        m_last_time = SDL_GetTicks();
    }
}

void Animation::Reset()
{
    m_is_first_movement = true; 
    m_sprite_index = 0;
    UpdateCurrentSprite();
}

void Animation::UpdateCurrentSprite() // Use this function after setting m_sprite_index
{
    m_current_sprite = Pair<int>{(m_sprite_index%m_spritesheet_size.x)*m_sprite_size, (m_sprite_index/m_spritesheet_size.x)*m_sprite_size};
}

void Animation::ContinueAnimation()
{
    uint32_t current_time = SDL_GetTicks(); // Should use a Time class (same code in MapMovement::UpdateProgress)
    float deltaTime = (current_time - m_last_time) / 1000.f;
    m_last_time = current_time;
    m_count += deltaTime;
    if (m_count >= m_frame_duration){
        m_count -= m_frame_duration;
        m_sprite_index = (m_sprite_index+1)%m_step;
    }
    UpdateCurrentSprite();
}