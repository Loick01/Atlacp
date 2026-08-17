#pragma once

#include "core/notifier.hpp"
#include "map/player.hpp" // EntityEvent comes from map_entity.hpp <- player.hpp

struct DataMapElement;

class Camera;
class FileReader;
class MapEntity;
class Tilemap;

class MapElementController : public Notifier<EntityEvent>
{
    private:
        const FileReader& m_fileReader;
        Player m_player; // ?
        
        // Will use unique_ptr<MapEntity> ?
        std::vector<MapEntity*> m_renderedEntities; // Sorted by y position
        // A specific order could be necessary for MapEntity updating (for example with MapFollowBehaviour), I use a second vector of MapEntity*
        std::vector<MapEntity*> m_updatedEntities;

        std::vector<MapElement*> m_mapElements; // Remove ? + Do not contains MapEntity (Player and NPCs), so the name is not really correct
    
    public:
        MapElementController(const FileReader& fileReader, TextureController& textureController,
            Camera& camera, Tilemap& tilemap);
        ~MapElementController();
        
        std::vector<MapEntity*>& GetEntities();
        std::vector<MapElement*>& GetElements();
        
        MapEntity* GetMapEntityFromId(const unsigned int id);

        void Draw() const;
        void Update(const GameMapEventState& playerEventState, const float deltaTime); // GameMapEventState or call SetEventState in GameMapScene (before MapElementController::Update()) 
        void SortRenderedEntities();
        void HandleEntityEvent(const EntityEvent e);

        void DeleteNPCs();
        void LoadNPCs(TextureController& textureController, Camera& camera, Tilemap& tilemap, // These 3 parameters should not be here ?
            const std::string& filepath, const unsigned int mapIndex);
        void LoadElements(const std::vector<DataMapElement>& elements, Tilemap& tilemap);
};