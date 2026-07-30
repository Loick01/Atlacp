#include "animation/map_entity_animation.hpp"

#include "core/file.hpp"

MapEntityAnimation::MapEntityAnimation(const FileReader& fileReader, const std::string& path):
    Animation(fileReader, path)
{}

void MapEntityAnimation::Initialize(const Direction direction, const bool isFirstMovement)
{
    m_indexOffset = static_cast<int>(direction)*m_animationData.step; // Set the offset for sprite animation BEFORE initialize it
    if (isFirstMovement){
        m_count = 0.f;
        m_currentSpriteIndex = m_indexOffset+1; // Force to don't use the idle sprite as first animation frame  
    }else{
        // Fix the bug for sprite direction (Need to update current sprite index immediately after changing direction)
        // Remove the previous offset (with %) and add the new one (each direction must have the same number of step)
        m_currentSpriteIndex = m_indexOffset + m_currentSpriteIndex%m_animationData.step;
    }
}

void MapEntityAnimation::Reset(const Direction direction)
{
    // Even if the entity movement isn't valid, the sprite must be updated to face the correct direction
    if (direction != Direction::None) // If direction is None, static_cast<int>(direction) would be 4
        m_indexOffset = static_cast<int>(direction)*m_animationData.step; 
    m_currentSpriteIndex = m_indexOffset; // Set to idle sprite (index offset+0)
}

void MapEntityAnimation::Continue(const float deltaTime)
{
    m_count += deltaTime;
    if (m_count >= m_animationData.frameDuration){
        m_count -= m_animationData.frameDuration;
        m_currentSpriteIndex = m_indexOffset + (m_currentSpriteIndex+1)%m_animationData.step;
    }
}