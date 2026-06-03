#pragma once

#include "map/map_entity.hpp"

class Player : public MapEntity, public EventStateHolder<GameplayEventState>
{
    public:
        Player(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController,
            const std::string& spriteFilepath, Camera& camera, const float walkSpeed, const float runSpeed);
        
        void Update(const float deltaTime) override;
};