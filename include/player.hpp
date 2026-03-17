#pragma once

#include <iostream>

#include "entity.hpp"

class Player : public Entity
{
    private:
        PlayerEventInfo m_eventInfo;

    public:
        Player(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController,
            const std::string& spriteFilepath, Camera& camera, const float walkSpeed, const float runSpeed);
        
        void SetEventInfo(const PlayerEventInfo eventInfo);
        void Update(const float deltaTime) override;
};