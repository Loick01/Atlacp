#include "animation/animation.hpp"

#include "core/file.hpp"

Animation::Animation(const FileReader& fileReader, const std::string& path):
    m_currentSpriteIndex(0), m_count(0.f), m_isDone(false)
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

bool Animation::IsDone() const
{
    return m_isDone;
}

void Animation::Continue(const float deltaTime)
{
    m_count += deltaTime;
    if (m_count >= m_animationData.frameDuration){
        m_count -= m_animationData.frameDuration;
        ++m_currentSpriteIndex;
        if (m_currentSpriteIndex == m_animationData.step) m_isDone = true;
    }
}