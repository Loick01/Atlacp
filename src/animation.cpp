#include "animation.hpp"

Animation::Animation(const FileReader& file_reader, const std::string& path):
    m_current_sprite_index(0), m_index_offset(0) // useless to initialize index offset here ?
{
    m_animation_data = file_reader.GetAnimationFromFile(path);
}

Vec2 Animation::GetCurrentSprite() const
{
    return m_animation_data.sprites[m_current_sprite_index];
}

AreaSize Animation::GetSpriteSize() const
{
    return m_animation_data.sprite_size;
}

void Animation::Initialize(const MapDirection direction, const bool is_first_movement)
{
    m_index_offset = static_cast<int>(direction)*m_animation_data.step; // Set the offset for sprite animation BEFORE initialize it
    if (is_first_movement){
        m_count = 0.f;
        m_current_sprite_index = m_index_offset+1; // Force to don't use the idle sprite as first animation frame  
    }else{
        // Fix the bug for sprite direction (Need to update current sprite index immediately after changing direction)
        // Remove the previous offset (with %) and add the new one (each direction must have the same number of step)
        m_current_sprite_index = m_index_offset + m_current_sprite_index%m_animation_data.step;
    }
}

void Animation::Reset()
{
    m_current_sprite_index = m_index_offset; // Set to idle sprite (index offset+0)
}

void Animation::ContinueAnimation(const float delta_time)
{
    m_count += delta_time;
    if (m_count >= m_animation_data.frame_duration){
        m_count -= m_animation_data.frame_duration;
        m_current_sprite_index = m_index_offset + (m_current_sprite_index+1)%m_animation_data.step;
    }
}