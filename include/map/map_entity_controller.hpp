#pragma once

#include "core/notifier.hpp"
#include "map/npc.hpp"
#include "map/player.hpp" // EntityEvent comes from map_entity.hpp <- player.hpp

struct DataMapElement;
struct DataNPC;

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
        std::vector<std::unique_ptr<NPC>> m_npcs;

        std::vector<MapEntity*> m_entities;
        std::vector<MapEntity*> m_renderedEntities;
        MapEntity* m_currentMapEntityUpdated;

        bool m_shouldUpdateNpcs;

        void RefreshRenderedEntities();
        void AddNpc(const DataNPC& data, const MapPosition spawn);

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

        void CreateEntity(const std::string& npcFilepath, const unsigned int idEntity, const MapPosition spawnPosition);
        void DeleteEntity(const unsigned int idEntity);
        void DeleteNPCs();
        void LoadNPCsForMap(const std::string& filepath, const unsigned int mapIndex);
};