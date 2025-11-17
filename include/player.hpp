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
        Tilemap* m_tilemap; // Try to make it const ?
        const int m_tile_size;

    public:
        Player(Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
            const std::string& sprite_filepath, Camera* camera);
        ~Player();

        void Update() override;
        void GetNewPosition(const MapPosition movement);
};