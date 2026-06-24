#pragma once

#include "map/interaction.hpp"
#include "map/player.hpp"

class Camera;
class FileReader;
class MapEntity;
class Tilemap;

class EntityController
{
    private:
        const FileReader& m_fileReader;
        Player m_player; // ?
        
        // Will use unique_ptr<MapEntity> ?
        std::vector<MapEntity*> m_renderedEntities; // Sorted by y position
        // A specific order could be necessary for MapEntity updating (for example with MapFollowBehaviour), I use a second vector of MapEntity*
        std::vector<MapEntity*> m_updatedEntities;

        InteractionController m_interactionController;
    
    public:
        EntityController(const FileReader& fileReader, UiController& uiController, TextureController& textureController,
            Camera& camera, Tilemap& tilemap);
        ~EntityController();

        void Draw() const;
        void Update(const GameMapEventState& playerEventState, const float deltaTime); // GameMapEventState or call SetEventState in GameMapScene (before EntityController::Update()) 
        void SortRenderedEntities();
        void HandleEntityEvent(const EntityEvent e);

        void DeleteNPCs(); // Rename DeleteNPCs
        void LoadNPCs(TextureController& textureController, Camera& camera, Tilemap& tilemap, // These 3 paramaters should not be here ?
            const std::string& filepath, const unsigned int mapIndex); // Rename LoadNPCs
};