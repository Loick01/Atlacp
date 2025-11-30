#pragma once

#include "type.hpp"

// Rename this class
class Animation
{
    private:
        int m_animation_step;
        int m_current_sprite_index;
        float m_count; // Elapsed time since the last sprite update
        const float m_frame_duration;
        std::vector<Pair<int>> m_sprites; // Position (column, line) of each sprites in spritesheet
        Pair<int> m_sprite_size;

    public:
        Animation(const int step, const float frame_duration);
        ~Animation();

        Pair<int> GetCurrentSprite() const;
        Pair<int> GetSpriteSize() const;
        void Initialize();
        void Reset();
        void ContinueAnimation(const float delta_time);
};