#pragma once

#include "type.hpp"
#include "texture.hpp" // Remove

// Rename this class
class Animation
{
    private:
        int m_step;
        int m_sprite_index;
        bool m_is_first_movement;
        uint32_t m_last_time;
        float m_count;
        const float m_frame_duration;
        const Pair<int> m_spritesheet_size;
        const int m_sprite_size;
        Pair<int> m_current_sprite;

        void UpdateCurrentSprite();

    public:
        Animation(const int step, const float frame_duration, const int sprite_size, const Pair<int> spritesheet_size);
        ~Animation();

        Pair<int> GetCurrentSprite() const;
        int GetSpriteSize() const;
        void Initialize();
        void Reset();
        void ContinueAnimation();
};