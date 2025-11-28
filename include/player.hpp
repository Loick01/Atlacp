#pragma once

#include <iostream>

#include "drawable.hpp"
#include "element.hpp"
#include "event.hpp"

class Player : public Drawable, public MapElement
{
    private:
        const MapEventController* m_event_controller;

    public:
        Player(Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
            const std::string& sprite_filepath, Camera* camera, const float speed);
        ~Player();

        void DrawTexture() const override;
        void Update(const float delta_time) override;
};