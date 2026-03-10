#pragma once

#include <iostream>

#include "behaviour.hpp"
#include "entity.hpp"

class NPC : public Entity
{
    private:
        EntityBehaviour* m_behaviour;

    public:
        NPC(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController, const Entity* trackedEntity, // Remove tracked entity
            const std::string& spriteFilepath, Camera& camera, const float speed);

        void Update(const float deltaTime) override;
};