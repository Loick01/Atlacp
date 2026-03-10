#pragma once

#include <iostream>

#include "entity.hpp"
#include "event.hpp"

class Player : public Entity
{
    private:
        GameplayEventController* m_eventController;

    public:
        Player(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController,
            const std::string& spriteFilepath, Camera& camera, const float speed);
        
        void SetEventController(GameplayEventController* eventController);
        void Update(const float deltaTime) override;
};