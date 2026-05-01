#pragma once

#include "entity.hpp"

class Player : public Entity
{
    private:
        GameplayEventState m_eventState;

    public:
        Player(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController,
            const std::string& spriteFilepath, Camera& camera, const float walkSpeed, const float runSpeed);
        
        void SetEventState(const GameplayEventState eventState);
        void Update(const float deltaTime) override;
};