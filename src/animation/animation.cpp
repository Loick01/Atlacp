#include "animation/animation.hpp"

Animation::Animation(const FileReader& fileReader, const std::string& path):
    m_currentSpriteIndex(0), m_indexOffset(0) // useless to initialize index offset here ?
{
    m_animationData = fileReader.GetAnimationFromFile(path);
}

Vec2 Animation::GetCurrentSprite() const
{
    return m_animationData.sprites[m_currentSpriteIndex];
}

AreaSize Animation::GetSpriteSize() const
{
    return m_animationData.spriteSize;
}

void Animation::Initialize(const Direction direction, const bool isFirstMovement)
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

void Animation::Reset(const Direction direction)
{
    // Even if the entity movement isn't valid, the sprite must be updated to face the correct direction
    if (direction != Direction::None) // If direction is None, static_cast<int>(direction) would be 4
        m_indexOffset = static_cast<int>(direction)*m_animationData.step; 
    m_currentSpriteIndex = m_indexOffset; // Set to idle sprite (index offset+0)
}

void Animation::ContinueAnimation(const float deltaTime)
{
    m_count += deltaTime;
    if (m_count >= m_animationData.frameDuration){
        m_count -= m_animationData.frameDuration;
        m_currentSpriteIndex = m_indexOffset + (m_currentSpriteIndex+1)%m_animationData.step;
    }
}