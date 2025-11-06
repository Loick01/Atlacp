#pragma once

#include <iostream>

#include "drawable.hpp"
#include "element.hpp"
#include "event.hpp"
#include "tilemap.hpp"
#include "type.hpp"

class Player : public Drawable, public Element
{
    private:
        const EventController* m_event_controller;
        const Tilemap* m_tilemap;
        Position m_player_position; // Should be in Element class ?

    public:
        Player(const Tilemap* tilemap, TextureController* texture_controller, const EventController* event_controller, const std::string& sprite_filepath);
        ~Player();

        void LoadSprite(const std::string& sprite_filepath);
        void DrawTexture() const override;
        void Update() override;
};