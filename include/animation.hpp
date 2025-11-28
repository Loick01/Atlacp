#pragma once

#include "type.hpp"
#include "texture.hpp" // Remove

// Rename this class
class Animation
{
    private:
        int m_animation_step;
        int m_sprite_index;
        float m_count; // Elapsed time since the last sprite update
        const float m_frame_duration;
        const Pair<int> m_spritesheet_size;
        const int m_sprite_size;
        Pair<int> m_current_sprite; // Remove (will use m_sprite_index instead) 
        // std::vector<Pair<int>> m_sprites; // Position (column, line) of each sprites in spritesheet

        void UpdateCurrentSprite();

    public:
        Animation(const int step, const float frame_duration, const int sprite_size, const Pair<int> spritesheet_size);
        ~Animation();

        Pair<int> GetCurrentSprite() const;
        int GetSpriteSize() const;
        void Initialize();
        void Reset();
        void ContinueAnimation(const float delta_time);
};