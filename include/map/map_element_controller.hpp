#pragma once

#include "map/interaction/interaction.hpp"
#include "map/player.hpp"

class Camera;
class FileReader;
class MapEntity;
class Tilemap;

class MapElementController
{
    private:
        const FileReader& m_fileReader;
        Player m_player; // ?
        
        // Will use unique_ptr<MapEntity> ?
        std::vector<MapEntity*> m_renderedEntities; // Sorted by y position
        // A specific order could be necessary for MapEntity updating (for example with MapFollowBehaviour), I use a second vector of MapEntity*
        std::vector<MapEntity*> m_updatedEntities;

        std::vector<MapElement*> m_mapElements; // ?

        InteractionController m_interactionController;
    
    public:
        MapElementController(const FileReader& fileReader, OrderController& orderController, TextureController& textureController,
            Camera& camera, Tilemap& tilemap);
        ~MapElementController();

        void Draw() const;
        void Update(const GameMapEventState& playerEventState, const float deltaTime); // GameMapEventState or call SetEventState in GameMapScene (before MapElementController::Update()) 
        void SortRenderedEntities();
        void HandleEntityEvent(const EntityEvent e);

        void DeleteNPCs();
        void LoadNPCs(TextureController& textureController, Camera& camera, Tilemap& tilemap, // These 3 parameters should not be here ?
            const std::string& filepath, const unsigned int mapIndex);
        void LoadElements(const std::vector<DataMapElement>& elements, Tilemap& tilemap);
};