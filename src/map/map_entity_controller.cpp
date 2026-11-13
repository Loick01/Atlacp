#include "map/map_entity_controller.hpp"

#include "map/map_entity.hpp"
#include "map/npc.hpp"
#include "core/camera.hpp"
#include "core/file/file.hpp" // DataMapElement
#include "tile/tilemap.hpp"

MapEntityController::MapEntityController(const FileReader& fileReader, TextureController& textureController,
Camera& camera, Tilemap& tilemap, const std::string& spritePlayerPath):
    m_fileReader(fileReader), m_camera(camera), m_textureController(textureController), m_tilemap(tilemap), 
    m_player(fileReader, m_tilemap, m_textureController, spritePlayerPath, 4.f, 6.f, m_camera.GetZoom()),
    m_currentMapEntityUpdated(nullptr), m_shouldUpdateNpcs(true)
{
    m_player.AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
    m_camera.SetAnchoredEntity(&m_player);
}

MapEntityController::~MapEntityController()
{
    DeleteNPCs();
}

std::vector<MapEntity*>& MapEntityController::GetEntities()
{
    return m_entities;
}

MapEntity* MapEntityController::GetMapEntityFromId(const unsigned int id)
{
    for (MapEntity* m : m_entities) {
        if (m->GetId() == id)
            return m;
    }
    Logger::GetInstance().AddLog(Level::L4, "The MapEntity " + std::to_string(id) + " was not found in MapEntityController::m_entities"); // TODO : Log
    return nullptr;
}

MapEntity* MapEntityController::GetCurrentMapEntityUpdated() const
{
    return m_currentMapEntityUpdated;
}

void MapEntityController::RefreshRenderedEntities()
{
    m_renderedEntities = m_entities;
    SortRenderedEntities();
}

void MapEntityController::AddNpc(const DataNPC& data, const MapPosition spawn)
{
    std::unique_ptr<NPC> npc = std::make_unique<NPC>(
        m_fileReader, 
        m_tilemap, 
        m_textureController,
        "map_entity/" + data.sprite, // TODO : "map_entity" should be in data.sprite ?
        spawn,
        data.walkSpeed, data.runSpeed,
        m_camera.GetZoom(),
        data.id
    );
    npc->SetOrders(data.orders);
    npc->AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
    m_entities.push_back(npc.get());
    m_npcs.push_back(std::move(npc));
}

void MapEntityController::CreateEntity(const std::string& npcFilepath, const unsigned int idEntity, const MapPosition spawnPosition)
{
    if (idEntity == 0) {
        Logger::GetInstance().AddLog(Level::L4, "MapEntityController::CreateEntity -> idEntity = 0 is reserved for Player"); // TODO : Level
        return; 
    }

    const std::optional<DataNPC> optData = m_fileReader.GetDataNPCFromId(npcFilepath, idEntity);
    
    if (!optData.has_value()) {
        Logger::GetInstance().AddLog(Level::L4, "Failed to create the MapEntity with id = " + idEntity); // TODO : Level
        return;
    }

    if (!m_tilemap.IsFreePosition(spawnPosition)) { // Do not use data.position, but the MapPosition given as a parameter 
        Logger::GetInstance().AddLog(Level::L4, "MapEntity can only spawn on free position"); // TODO : Level
        return;
    }
    
    const DataNPC data = optData.value();
    AddNpc(data, spawnPosition);
    
    RefreshRenderedEntities();
}

void MapEntityController::DeleteEntity(const unsigned int idEntity)
{
    if (idEntity == 0) { 
        Logger::GetInstance().AddLog(Level::L4, "MapEntityController::DeleteEntity -> Player (idEntity = 0) should not be deleted"); // TODO : Level
        return;
    }
    
    std::vector<std::unique_ptr<NPC>>::iterator itNPC = std::find_if(m_npcs.begin(), m_npcs.end(), 
        [idEntity](const std::unique_ptr<NPC>& npc) {
            return npc->GetId() == idEntity;
        }
    );
    std::vector<MapEntity*>::iterator itEntities = std::find_if(m_entities.begin(), m_entities.end(), 
        [idEntity](const MapEntity* entity) {
            return entity->GetId() == idEntity;
        }
    );
    std::vector<MapEntity*>::iterator itRendered = std::find_if(m_renderedEntities.begin(), m_renderedEntities.end(), 
        [idEntity](const MapEntity* entity) {
            return entity->GetId() == idEntity;
        }
    );

    if (itNPC == m_npcs.end() || itRendered == m_renderedEntities.end() || itEntities == m_entities.end()) { // TODO ?
        Logger::GetInstance().AddLog(Level::L4, "This MapEntity was not found in MapEntityController : " + idEntity); // TODO : Level
        return;
    }

    (*itNPC)->FreePosition();
    
    m_npcs.erase(itNPC);
    m_entities.erase(itEntities);
    m_renderedEntities.erase(itRendered);
}

void MapEntityController::DeleteNPCs()
{    
    // Currently, DeleteNPCs is only called in LoadNPCsForMap() and ~MapEntityController(), so calling clear() on these vectors is useless because their elements are 
    // already overwriten in LoadNPCsForMap(). I keep it anyway, in case DeleteNPCs() is used somewhere else later)
    m_npcs.clear();
    m_entities.clear();
    m_renderedEntities.clear();
}

void MapEntityController::Draw() const
{
    for (MapEntity* e : m_renderedEntities) {
        e->SetCameraPosition(m_camera.GetPosition() - m_camera.GetScreenOffset());
        e->SetCameraZoom(m_camera.GetZoom());
        e->DrawTexture();
    }
}

void MapEntityController::Update(const GameMapEventState& playerEventState, const float deltaTime)
{
    m_player.SetEventState(playerEventState);
    
    m_currentMapEntityUpdated = &m_player;
    m_player.Update(deltaTime);
    
    // Do not update the NPCs on this frame if a new map has been loaded, when the Player exited the map or walked on a trigger with LoadMapOrder
    if (m_shouldUpdateNpcs) {
        // Warning : If at some point a NPC could trigger a map loading, this for loop will crash 
        // Because Tilemap::LoadMap() -> Notify(TilemapEvent::LoadingMap) -> MapEntityController::LoadNPCsForMap() (which modifies m_entities) 
        for (unsigned int i = 1 ; i < m_entities.size() ; i++) {
            MapEntity* e = m_entities[i];
            m_currentMapEntityUpdated = e;
            e->Update(deltaTime);
        }
    } else {
        m_shouldUpdateNpcs = true;
    }
}

void MapEntityController::SortRenderedEntities()
{
    // m_renderedEntities is sorted each time an MapEntity ends its movement (remove then insert the moving entity at the correct index instead ?)
    // It would be even better to sort only once when several entities end their movement in the same frame
    std::sort(m_renderedEntities.begin(), m_renderedEntities.end(),
        [](MapEntity* a, MapEntity* b){
            return a->GetMapPosition().y < b->GetMapPosition().y; 
        });
}

void MapEntityController::HandleEntityEvent(const EntityEvent e)
{
    switch(e) {
        case EntityEvent::HasMoved : {
            SortRenderedEntities();
            Notify(e);
            break;
        }
        case EntityEvent::EnterInteraction :
        case EntityEvent::ContinueInteraction :
        case EntityEvent::ContinueTrigger : 
        case EntityEvent::RequestLoadMap : {
            Notify(e);
            break;
        }
        default:
            break;
    }
}

void MapEntityController::LoadNPCsForMap(const std::string& filepath, const unsigned int mapIndex)
{
    DeleteNPCs();

    m_entities = {&m_player}; // Keep only the Player

    const std::vector<DataNPC> npcsData = m_fileReader.GetDataNPCsForMap(filepath, mapIndex);

    for (unsigned int i = 0 ; i < npcsData.size() ; i++) {
        DataNPC data = npcsData[i];
        const MapPosition spawn = data.position;
        if (!m_tilemap.IsFreePosition(spawn)) {
            Logger::GetInstance().AddLog(Level::L4, "NPC can only spawn on free position"); // TODO : Log
            continue; // TODO : Do something else than skipping the NPC ?
        }
        
        AddNpc(data, spawn);
    }
    
    RefreshRenderedEntities();

    m_shouldUpdateNpcs = false;
}