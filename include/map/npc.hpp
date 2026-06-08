#pragma once

#include <memory>

#include "ai_map/map_behaviour.hpp"
#include "map/map_entity.hpp"

// Forward declaration of Tilemap is already in map_entity.hpp

class NPC : public MapEntity
{
    private:
        std::unique_ptr<MapEntityBehaviour> m_behaviour;

    public:
        NPC(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController, const MapEntity* trackedEntity, // Remove tracked entity
            const std::string& spriteFilepath, Camera& camera, const MapPosition position, const float walkSpeed, const float runSpeed);

        void Update(const float deltaTime) override;
};