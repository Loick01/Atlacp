#include "animation/sprite_animation.hpp"

#include "core/file/file.hpp"

SpriteAnimation::SpriteAnimation(const FileReader& fileReader, const std::string& animationFilepath):
    m_fileReader(fileReader), m_currentSpriteIndex(0), m_count(0.f), m_isDone(false)
{
    UpdateAnimationData(animationFilepath);
}

Vec2 SpriteAnimation::GetCurrentSprite() const
{
    return m_animationData.sprites[m_currentSpriteIndex];
}

AreaSize SpriteAnimation::GetSpriteSize() const
{
    return m_animationData.spriteSize;
}

bool SpriteAnimation::IsDone() const
{
    return m_isDone;
}

void SpriteAnimation::UpdateAnimationData(const std::string& animationFilepath)
{
    m_animationData = m_fileReader.ReadAnimationFile(animationFilepath);
}

void SpriteAnimation::Continue(const float deltaTime)
{
    m_count += deltaTime;
    if (m_count >= m_animationData.frameDuration){
        m_count -= m_animationData.frameDuration;
        ++m_currentSpriteIndex;
        if (m_currentSpriteIndex == m_animationData.step) m_isDone = true;
    }
}