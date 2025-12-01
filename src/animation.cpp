#include "animation.hpp"

Animation::Animation(const FileReader* file_reader, const std::string& path):
    m_current_sprite_index(0)
{
    m_animation_data = file_reader->GetAnimationFromFile(path);
}

Animation::~Animation()
{

}

Pair<int> Animation::GetCurrentSprite() const
{
    return m_animation_data.sprites[m_current_sprite_index];
}

Pair<int> Animation::GetSpriteSize() const
{
    return m_animation_data.sprite_size;
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
    if (m_count >= m_animation_data.frame_duration){
        m_count -= m_animation_data.frame_duration;
        m_current_sprite_index = (m_current_sprite_index+1)%m_animation_data.step;
    }
}