#pragma once

#include <iostream>

#include "entity.hpp"

class NPC : public Entity
{
    private:

    public:
        NPC(const FileReader* file_reader, Tilemap* tilemap, TextureController* texture_controller,
            const std::string& sprite_filepath, Camera* camera, const float speed);
        ~NPC();

        void Update(const float delta_time) override;
};