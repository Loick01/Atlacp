#include "animation.hpp"

Animation::Animation(const int step, const float frame_duration):
    m_animation_step(step), m_frame_duration(frame_duration), m_current_sprite_index(0)
{
    // Will be read from header file
    m_sprite_size = Pair<int>{32,32};
    m_sprites.push_back(Pair<int>{0, 0}*m_sprite_size);
    m_sprites.push_back(Pair<int>{1, 0}*m_sprite_size);
    m_sprites.push_back(Pair<int>{0, 1}*m_sprite_size);
    m_sprites.push_back(Pair<int>{1, 1}*m_sprite_size);
}

Animation::~Animation()
{

}

Pair<int> Animation::GetCurrentSprite() const
{
    return m_sprites[m_current_sprite_index];
}

Pair<int> Animation::GetSpriteSize() const
{
    return m_sprite_size;
}

void Animation::Initialize() // Initialize only when this is the first movement
{
    m_count = 0.f;
    m_current_sprite_index = 1; // Important --> Force to don't use the idle sprite 
}

void Animation::Reset()
{
    m_current_sprite_index = 0; // Set to idle sprite
}

void Animation::ContinueAnimation(const float delta_time)
{
    m_count += delta_time;
    if (m_count >= m_frame_duration){
        m_count -= m_frame_duration;
        m_current_sprite_index = (m_current_sprite_index+1)%m_animation_step;
    }
}