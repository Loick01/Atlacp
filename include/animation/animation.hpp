#pragma once

#include "system/file.hpp"
#include "system/type.hpp"

// Rename this class (SpriteAnimation ?)
class Animation // Must have sprites for 4 directions
{
    private:
        AnimationData m_animationData;
        int m_currentSpriteIndex;
        float m_count; // Elapsed time since the last sprite update
        int m_indexOffset;

    public:
        Animation(const FileReader& fileReader, const std::string& path);

        Vec2 GetCurrentSprite() const;
        AreaSize GetSpriteSize() const;
        void Initialize(const Direction direction, const bool isFirstMovement);
        void Reset(const Direction direction);
        void ContinueAnimation(const float deltaTime);
};