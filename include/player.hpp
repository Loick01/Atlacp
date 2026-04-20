#pragma once

#include "entity.hpp"

class Player : public Entity
{
    private:
        GameplayEventInfo m_eventInfo;

    public:
        Player(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController,
            const std::string& spriteFilepath, Camera& camera, const float walkSpeed, const float runSpeed);
        
        void SetEventInfo(const GameplayEventInfo eventInfo);
        void Update(const float deltaTime) override;
};