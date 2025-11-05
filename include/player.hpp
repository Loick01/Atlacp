#pragma once

#include <iostream>

#include "drawable.hpp"
#include "event.hpp"
//#include "texture.hpp"
#include "type.hpp"

class Player : public Drawable
{
    private:
        EventController* m_event_controller; // Should be const
        Position m_player_position; 

    public:
        Player(TextureController* texture_controller, EventController* event_controller, const std::string& sprite_filepath);
        ~Player();

        void LoadSprite(const std::string& sprite_filepath);
        void DrawTexture() const override;
};