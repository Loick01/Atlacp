#pragma once

#include <memory>

#include "ai_map/map_behaviour.hpp"
#include "map/map_entity.hpp"

// Forward declaration of Tilemap is already in map_entity.hpp

class NPC : public MapEntity // Should have a Tilemap& member ?
{
    private:
        std::unique_ptr<MapEntityBehaviour> m_behaviour;

    public:
        NPC(const FileReader& fileReader, Tilemap& tilemap, TextureController& textureController, const std::string& spriteFilepath,
            Camera& camera, const MapPosition position, const float walkSpeed, const float runSpeed, const unsigned int id);

        const MapEntityBehaviour* GetMapBehaviour() const;
        void Update(const float deltaTime) override;
        
        void SetRandomBehaviour();
        void SetGoToBehaviour(const Tilemap& tilemap, const MapPosition target);
};