#pragma once

#include "core/notifier.hpp"
#include "map/player.hpp" // EntityEvent comes from map_entity.hpp <- player.hpp

struct DataMapElement;

class Camera;
class FileReader;
class MapEntity;
class TextureController;
class Tilemap;

class MapEntityController : public Notifier<EntityEvent>
{
    private:
        const FileReader& m_fileReader;
        Camera& m_camera;
        TextureController& m_textureController;
        Tilemap& m_tilemap;
        
        Player m_player;
        
        // TODO++ : Will have a std::vector<MapEntity>
        // m_renderedEntities and m_updatedEntities both include the Player and every NPC
        std::vector<MapEntity*> m_renderedEntities; // Sorted by y position
        // A specific order could be necessary for MapEntity updating (for example with MapFollowBehaviour), I use a second vector of MapEntity*
        std::vector<MapEntity*> m_updatedEntities;

        MapEntity* m_currentMapEntityUpdated;

        bool m_shouldUpdateNpc;
        
    public:
        MapEntityController(const FileReader& fileReader, TextureController& textureController,
            Camera& camera, Tilemap& tilemap, const std::string& spritePlayerPath);
        ~MapEntityController();
        
        std::vector<MapEntity*>& GetEntities();
        
        MapEntity* GetMapEntityFromId(const unsigned int id);
        MapEntity* GetCurrentMapEntityUpdated() const; // TODO : const ?
        
        void Draw() const;
        void Update(const GameMapEventState& playerEventState, const float deltaTime); // GameMapEventState or call SetEventState in GameMapScene (before MapEntityController::Update()) 
        void SortRenderedEntities();
        void HandleEntityEvent(const EntityEvent e);

        void CreateEntity(const std::string& npcFilepath, const unsigned int idEntity, const MapPosition spawnPosition); // TODO : npcFilepath will be removed ?
        void DeleteEntity(const unsigned int idEntity);
        void DeleteNPCs();
        void LoadNPCsForMap(const std::string& filepath, const unsigned int mapIndex);
};