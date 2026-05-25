#pragma once

#include "map/entity.hpp"

class Player : public Entity, public EventStateHolder<GameplayEventState>
{
    public:
        Player(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController,
            const std::string& spriteFilepath, Camera& camera, const float walkSpeed, const float runSpeed);
        
        void Update(const float deltaTime) override;
};