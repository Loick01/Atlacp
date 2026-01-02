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
        int m_index_offset;

    public:
        Animation(const FileReader& file_reader, const std::string& path);

        Vec2 GetCurrentSprite() const;
        AreaSize GetSpriteSize() const;
        void Initialize(const MapDirection direction, const bool is_first_movement);
        void Reset();
        void ContinueAnimation(const float delta_time);
};