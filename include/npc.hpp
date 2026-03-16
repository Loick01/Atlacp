#pragma once

#include <memory>

#include "behaviour.hpp"
#include "entity.hpp"

class NPC : public Entity
{
    private:
        std::unique_ptr<EntityBehaviour> m_behaviour;

    public:
        NPC(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController, const Entity* trackedEntity, // Remove tracked entity
            const std::string& spriteFilepath, Camera& camera, const float walkSpeed, const float runSpeed);

        void Update(const float deltaTime) override;
};