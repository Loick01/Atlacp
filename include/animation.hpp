#pragma once

#include "file.hpp"
#include "type.hpp"

// Rename this class
class Animation
{
    private:
        int m_current_sprite_index;
        float m_count; // Elapsed time since the last sprite update
        AnimationData m_animation_data;

    public:
        Animation(const FileReader* file_reader, const std::string& path);
        ~Animation();

        Pair<int> GetCurrentSprite() const;
        Pair<int> GetSpriteSize() const;
        void Initialize();
        void Reset();
        void ContinueAnimation(const float delta_time);
};