#pragma once

#include "file.hpp"
#include "type.hpp"

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
        void Initialize(const MapDirection direction, const bool isFirstMovement);
        void Reset(const MapDirection direction);
        void ContinueAnimation(const float deltaTime);
};