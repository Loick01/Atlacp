#pragma once

#include "file.hpp"
#include "type.hpp"

// Rename this class (SpriteAnimation ?)
class Animation // Must have sprites for 4 directions
{
    private:
        AnimationData m_animation_data;
        int m_current_sprite_index;
        float m_count; // Elapsed time since the last sprite update
        int m_offset;

    public:
        Animation(const FileReader* file_reader, const std::string& path);
        ~Animation();

        Pair<int> GetCurrentSprite() const;
        Pair<int> GetSpriteSize() const;
        void SetOffset(const MapDirection direction);
        void Initialize();
        void Reset();
        void ContinueAnimation(const float delta_time);
};