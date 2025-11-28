#include "animation.hpp"

Animation::Animation(const int step, const float frame_duration, const int sprite_size, const Pair<int> spritesheet_size):
    m_animation_step(step), m_frame_duration(frame_duration), m_sprite_index(0), m_sprite_size(sprite_size),
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

void Animation::Initialize() // Initialize only when this is the first movement
{
    m_count = 0.f;
    m_sprite_index = 1; // Important --> Force to don't use the idle sprite 
}

void Animation::Reset()
{
    m_sprite_index = 0; // Set to idle sprite
    UpdateCurrentSprite();
}

void Animation::UpdateCurrentSprite() // Use this function after setting m_sprite_index
{
    m_current_sprite = Pair<int>{(m_sprite_index%m_spritesheet_size.x)*m_sprite_size, (m_sprite_index/m_spritesheet_size.x)*m_sprite_size};
}

void Animation::ContinueAnimation(const float delta_time)
{
    m_count += delta_time;
    if (m_count >= m_frame_duration){
        m_count -= m_frame_duration;
        m_sprite_index = (m_sprite_index+1)%m_animation_step;
    }
    UpdateCurrentSprite();
}