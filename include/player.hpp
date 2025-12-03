#pragma once

#include <iostream>

#include "drawable.hpp"
#include "element.hpp"
#include "event.hpp"

class Player : public Drawable, public MapElement
{
    private:
        const GameplayEventController* m_event_controller;
        ScenePosition m_display_offset; // Should be in MapElement or Drawable ? (NPC sprites could also be greater than tile size)

    public:
        Player(const FileReader* file_reader, Tilemap* tilemap, TextureController* texture_controller, const GameplayEventController* event_controller,
            const std::string& sprite_filepath, Camera* camera, const float speed);
        ~Player();

        void DrawTexture() const override;
        void Update(const float delta_time) override;
};