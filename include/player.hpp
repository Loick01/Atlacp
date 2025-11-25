#pragma once

#include <iostream>

#include "drawable.hpp"
#include "element.hpp"
#include "event.hpp"

class Player : public Drawable, public MapElement
{
    private:
        const MapEventController* m_event_controller;

        // Class Animated
        int m_anim_step;
        int m_anim_index;
        bool m_is_first_movement;
        uint32_t m_last_time;
        float m_count;
        float m_frame_duration; // Const
        Pair<int> m_spritesheet_size; // Const
        int m_sprite_size; // Const
        Pair<int> m_src;

    public:
        Player(Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
            const std::string& sprite_filepath, Camera* camera, const float speed);
        ~Player();

        void DrawTexture() const override;
        void Update() override;
};