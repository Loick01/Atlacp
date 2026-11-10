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
    m_currentMapEntityUpdated(nullptr), m_shouldUpdateNpc(true)
{
    m_player.AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
    m_camera.SetAnchoredEntity(&m_player);
    m_renderedEntities = {&m_player};
    m_updatedEntities = m_renderedEntities;
}

MapEntityController::~MapEntityController()
{
    DeleteNPCs();
}

std::vector<MapEntity*>& MapEntityController::GetEntities()
{
    return m_updatedEntities; // TODO : m_renderedEntities ?
}

MapEntity* MapEntityController::GetMapEntityFromId(const unsigned int id)
{
    for (MapEntity* m : m_updatedEntities) {
        if (m->GetId() == id)
            return m;
    }
    Logger::GetInstance().AddLog(Level::L4, "The MapEntity " + std::to_string(id) + " was not found in MapEntityController::m_updatedEntities"); // TODO : Log
    return nullptr;
}

MapEntity* MapEntityController::GetCurrentMapEntityUpdated() const
{
    return m_currentMapEntityUpdated;
}

void MapEntityController::CreateEntity(const std::string& npcFilepath, const unsigned int idEntity, const MapPosition spawnPosition)
{
    if (idEntity == 0) { // TODO : idEntity == -1 ?
        Logger::GetInstance().AddLog(Level::L4, "MapEntityController::CreateEntity -> idEntity = 0 is already use for Player"); // TODO : Log
        return; // TODO : Maybe I could still create the NPC but with a new id (but how I will retrieve the NPC from data file ?)
    }

    const std::optional<DataNPC> optData = m_fileReader.GetDataNPCFromId(npcFilepath, idEntity);
    
    if (!optData.has_value()) {
        Logger::GetInstance().AddLog(Level::L4, "Failed to create the MapEntity with id = " + idEntity); // TODO : Log
        return;
    }

    if (!m_tilemap.IsFreePosition(spawnPosition)) { // Do not use data.position, but the MapPosition given as a parameter 
        Logger::GetInstance().AddLog(Level::L4, "MapEntity can only spawn on free position"); // TODO : Log
        return;
    }
    
    const DataNPC data = optData.value();
        
    // TODO : Merge with the same code in MapEntityController::LoadNPCsForMap 
    // TODO : Do not use new
    NPC* npc = new NPC(
        m_fileReader, 
        m_tilemap, 
        m_textureController,
        "map_entity/" + data.sprite, // TODo : "map_entity" should be in data.sprite ?
        spawnPosition,
        data.walkSpeed, data.runSpeed,
        m_camera.GetZoom(),
        data.id
    );
    npc->SetOrders(data.orders);
    npc->AddCallback([this](EntityEvent e){HandleEntityEvent(e);});

    m_updatedEntities.push_back(npc);
    m_renderedEntities.push_back(npc);
    SortRenderedEntities();
}

void MapEntityController::DeleteEntity(const unsigned int idEntity)
{
    if (idEntity == 0) { 
        Logger::GetInstance().AddLog(Level::L4, "MapEntityController::DeleteEntity -> Player (idEntity = 0) should not be deleted"); // TODO : Log
        return;
    }

    // TODO : I will not use both vectors to retrieve the MapEntity
    
    std::vector<MapEntity*>::iterator itRendered = std::find_if(m_renderedEntities.begin(), m_renderedEntities.end(), 
        [idEntity](MapEntity* entity) {
            return entity->GetId() == idEntity;
        }
    );
    std::vector<MapEntity*>::iterator itUpdated = std::find_if(m_updatedEntities.begin(), m_updatedEntities.end(), 
        [idEntity](MapEntity* entity) {
            return entity->GetId() == idEntity;
        }
    );

    if (itRendered == m_renderedEntities.end() || itUpdated == m_updatedEntities.end()) {
        Logger::GetInstance().AddLog(Level::L4, "This MapEntity was not found in MapEntityController : " + idEntity); // TODO : Log
        return;
    }

    // TODo : Or itUpdated
    (*itRendered)->FreePosition();
    delete *itRendered;
    
    m_renderedEntities.erase(itRendered);
    m_updatedEntities.erase(itUpdated);
}

void MapEntityController::DeleteNPCs()
{
    // Do not try to delete the player (first element in m_updatedEntities, be sure to don't modify the order --> player must always be updated before every NPC)
    for (unsigned int i = 1 ; i < m_updatedEntities.size() ; i++)
        delete m_updatedEntities[i]; // TODO : Because I use new to instantiate NPC, later I will use unique_ptr
    
    // Currently, DeleteNPCs is only called in LoadNPCsForMap() and ~MapEntityController(), so calling clear() on these vectors is useless because their elements are 
    // already overwriten in LoadNPCsForMap(). I keep it anyway, in case DeleteNPCs() is used somewhere else later)
    m_updatedEntities.clear();
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
    if (m_shouldUpdateNpc) {
        // Warning : If at some point a NPC could trigger a map loading, this for loop will crash 
        // Because Tilemap::LoadMap() -> Notify(TilemapEvent::LoadingMap) -> MapEntityController::LoadNPCsForMap() (which modifies m_updatedEntities) 
        for (unsigned int i = 1 ; i < m_updatedEntities.size() ; i++) {
            MapEntity* e = m_updatedEntities[i];
            m_currentMapEntityUpdated = e;
            e->Update(deltaTime);
        }
    } else {
        m_shouldUpdateNpc = true;
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

    m_renderedEntities = {&m_player}; // Keep only the Player

    const std::vector<DataNPC> npcsData = m_fileReader.GetDataNPCsForMap(filepath, mapIndex);

    for (unsigned int i = 0 ; i < npcsData.size() ; i++) {
        DataNPC data = npcsData[i];
        const MapPosition npcPosition = data.position;
        if (!m_tilemap.IsFreePosition(npcPosition)) {
            Logger::GetInstance().AddLog(Level::L4, "NPC can only spawn on free position"); // TODO : Log
            continue; // TODO : Do something else than skipping the NPC ?
        }
        
        NPC* npc = new NPC(
            m_fileReader, 
            m_tilemap, 
            m_textureController,
            "map_entity/" + data.sprite, // TODO : "map_entity" should be in data.sprite ?
            npcPosition,
            data.walkSpeed, data.runSpeed,
            m_camera.GetZoom(),
            data.id
        );
        npc->SetOrders(data.orders);
        npc->AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
        m_renderedEntities.push_back(npc);
    }
    
    m_updatedEntities = m_renderedEntities;
    SortRenderedEntities();
    m_shouldUpdateNpc = false;
}