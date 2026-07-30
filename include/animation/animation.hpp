#pragma once

#include "animation/animation_types.hpp" // AnimationData
#include "core/core_types.hpp" // Vec2, AreaSize

class FileReader;

class Animation
{
    protected:
        const FileReader& m_fileReader;
        AnimationData m_animationData;
        float m_count; // Elapsed time since the last sprite update
        int m_currentSpriteIndex;
        bool m_isDone;

    public:
        Animation(const FileReader& fileReader, const std::string& path);
        Animation(const FileReader& fileReader);

        Vec2 GetCurrentSprite() const;
        AreaSize GetSpriteSize() const;
        bool IsDone() const;
        void GetAnimationData(const std::string& path); // Rename
        virtual void Continue(const float deltaTime);
};