#pragma once

#include <iostream>

#include "drawable.hpp"
#include "element.hpp"
#include "event.hpp"
#include "tilemap.hpp"

class Player : public Drawable, public MapElement
{
    private:
        const EventController* m_event_controller;
        const Tilemap* m_tilemap;

    public:
        Player(const Tilemap* tilemap, TextureController* texture_controller, const EventController* event_controller,
            const std::string& sprite_filepath, const ScreenPosition position={0,0});
        ~Player();

        void LoadSprite(const std::string& sprite_filepath);
        void DrawTexture() const override;
        void Update() override;
};