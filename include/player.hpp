#pragma once

#include <iostream>

#include "entity.hpp"
#include "event.hpp"

class Player : public Entity
{
    private:
        GameplayEventController* m_event_controller;

    public:
        Player(const FileReader& file_reader, Tilemap& tilemap, TextureController& texture_controller, GameplayEventController* event_controller,
            const std::string& sprite_filepath, Camera& camera, const float speed);

        void Update(const float delta_time) override;
};