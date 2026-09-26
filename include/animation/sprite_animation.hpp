#pragma once

#include "animation/animation_types.hpp" // SpriteAnimationData
#include "core/core_types.hpp" // Vec2, AreaSize

class FileReader;

class SpriteAnimation
{
    private:
        const FileReader& m_fileReader;
        bool m_isDone;

    protected:
        SpriteAnimationData m_animationData;
        float m_count; // Elapsed time since the last sprite update
        int m_currentSpriteIndex;

    public:
        SpriteAnimation(const FileReader& fileReader, const std::string& animationFilepath);

        Vec2 GetCurrentSprite() const;
        AreaSize GetSpriteSize() const;
        
        bool IsDone() const;
        void UpdateAnimationData(const std::string& animationFilepath);

        virtual void Continue(const float deltaTime);
};