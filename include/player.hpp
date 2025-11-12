#pragma once

#include <iostream>

#include "drawable.hpp"
#include "element.hpp"
#include "event.hpp"
#include "tilemap.hpp"

class Player : public Drawable, public MapElement
{
    private:
        const MapEventController* m_event_controller;
        const Tilemap* m_tilemap;
        const int m_tile_size;

    public:
        Player(const Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
            const std::string& sprite_filepath, const ScreenPosition position={0,0}, const bool should_draw=true);
        ~Player();

        void DrawTexture() const override;
        void Update() override;
};