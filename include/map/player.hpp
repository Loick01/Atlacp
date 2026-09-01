#pragma once

#include "event/event_types.hpp" // GameMapEventState
#include "map/map_entity.hpp"

// Forward declaration of Tilemap is already in map_entity.hpp

class Player : public MapEntity, public EventStateHolder<GameMapEventState>
{
    public:
        Player(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController,
            const std::string& spriteFilepath, const float walkSpeed, const float runSpeed, const float cameraZoom);
        
        void Update(const float deltaTime) override;
};