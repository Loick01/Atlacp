#include "map/map_element_controller.hpp"

#include "map/map_entity.hpp"
#include "map/npc.hpp"
#include "core/camera.hpp"
#include "core/file/file.hpp" // DataMapElement
#include "tile/tilemap.hpp"

MapElementController::MapElementController(const FileReader& fileReader, TextureController& textureController,
Camera& camera, Tilemap& tilemap, const std::string& spritePlayerPath):
    m_fileReader(fileReader), m_camera(camera), m_player(fileReader, tilemap, textureController, spritePlayerPath, 4.f, 6.f, m_camera.GetZoom()),
    m_currentMapEntityUpdated(nullptr), m_shouldUpdateNpc(true)
{
    m_player.AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
    m_camera.SetAnchoredEntity(&m_player);
    m_renderedEntities = {&m_player};
    m_updatedEntities = m_renderedEntities;
}

MapElementController::~MapElementController()
{
    DeleteNPCs();
}

std::vector<MapEntity*>& MapElementController::GetEntities()
{
    return m_updatedEntities; // m_updatedEntities ?
}

std::vector<MapElement*>& MapElementController::GetElements()
{
    return m_mapElements;
}

MapEntity* MapElementController::GetMapEntityFromId(const unsigned int id)
{
    // if (id == 0) throw std::runtime_error("MapElementController::GetMapEntity() --> Do not use id = 0, this is reserved for the player"); // Will be removed
    
    // Should return exclusively NPC ? For now GetMapEntity is used only for NpcGoToOrder, but maybe I will change it to EntityGoToOrder
    for (MapEntity* m : m_updatedEntities) {
        if (m->GetId() == id)
            return m;
    }
    throw std::runtime_error("This MapEntity was not found in MapElementController::m_updatedEntities : " + id);
}

MapEntity* MapElementController::GetCurrentMapEntityUpdated() const
{
    return m_currentMapEntityUpdated;
}

void MapElementController::DeleteNPCs()
{
    // Do not try to delete the player (first element in m_updatedEntities, be sure to don't modify the order --> player must always be updated before every NPC)
    for (unsigned int i = 1 ; i < m_updatedEntities.size() ; i++)
        delete m_updatedEntities[i]; // Because I use new to instantiate NPC, later I will use unique_ptr
    
    // Currently, DeleteNPCs is only called in LoadNPCs() and ~MapElementController(), so calling clear() on these vectors is useless because their elements are 
    // already overwriten in LoadNPCs(). I keep it anyway, in case DeleteNPCs() is used somewhere else later)
    m_updatedEntities.clear();
    m_renderedEntities.clear();
}

void MapElementController::Draw() const
{
    for (MapEntity* e : m_renderedEntities) {
        e->SetCameraPosition(m_camera.GetPosition() - m_camera.GetScreenOffset());
        e->SetCameraZoom(m_camera.GetZoom());
        e->DrawTexture();
    }
}

void MapElementController::Update(const GameMapEventState& playerEventState, const float deltaTime)
{
    m_player.SetEventState(playerEventState);
    
    m_currentMapEntityUpdated = &m_player;
    m_player.Update(deltaTime);
    
    // Do not update the NPCs on this frame if a new map has been loaded, when the Player exited the map or walked on a trigger with LoadMapOrder (not implemented yet)
    if (m_shouldUpdateNpc) {
        // Warning : If at some point a NPC could trigger a map loading, this for loop will crash 
        // Because Tilemap::LoadMap() -> Notify(TilemapEvent::LoadingMap) -> MapElementController::LoadNPCs() (which modifies m_updatedEntities) 
        for (unsigned int i = 1 ; i < m_updatedEntities.size() ; i++) {
            MapEntity* e = m_updatedEntities[i];
            m_currentMapEntityUpdated = e;
            e->Update(deltaTime);
        }
    } else {
        m_shouldUpdateNpc = true;
    }
}

void MapElementController::SortRenderedEntities()
{
    // m_renderedEntities is sorted each time an MapEntity ends its movement (remove then insert the moving entity at the correct index instead ?)
    // It would be even better to sort only once when several entities end their movement in the same frame
    std::sort(m_renderedEntities.begin(), m_renderedEntities.end(),
        [](MapEntity* a, MapEntity* b){
            return a->GetMapPosition().y < b->GetMapPosition().y; 
        });
}

void MapElementController::HandleEntityEvent(const EntityEvent e)
{
    switch(e) {
        case EntityEvent::HasMoved : {
            SortRenderedEntities();
            Notify(e);
            break;
        }
        case EntityEvent::EnterInteraction :
        case EntityEvent::ContinueInteraction :
        case EntityEvent::ContinueTrigger : {
            Notify(e);
            break;
        }
        default:
            break;
    }
}

void MapElementController::LoadNPCs(TextureController& textureController, Tilemap& tilemap,
    const std::string& filepath, const unsigned int mapIndex)
{
    DeleteNPCs();

    m_renderedEntities = {&m_player}; // Keep only the Player

    const std::vector<DataNPC> npcsData = m_fileReader.ReadNPCsFile(filepath, mapIndex);

    for (unsigned int i = 0 ; i < npcsData.size() ; i++) {
        DataNPC data = npcsData[i];
        const MapPosition npcPosition = data.position;
        if (!tilemap.IsFreePosition(npcPosition))
            throw std::runtime_error("NPC can only spawn on free position"); 
        
        NPC* npc = new NPC( // TODO
            m_fileReader, 
            tilemap, 
            textureController,
            "map_entity/" + data.sprite, // "map_entity" should be in data.sprite ?
            npcPosition,
            data.walkSpeed, data.runSpeed,
            m_camera.GetZoom(),
            data.id
        );
        npc->SetOrders(data.orders);
        npc->AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
        m_renderedEntities.push_back(npc);
    }
    
    // Testing follow behaviour (trackedEntity parameter will be remove from NPC constructor)
    // MapEntity* trackedEntity = &m_player;
    // for (unsigned int i = 0 ; i < 10 ; i++){
    //     NPC* npc = new NPC(m_fileReader, tilemap, textureController, trackedEntity, "map_entity/" + data.sprite, 4.f, 6.f);
    //     npc->AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
    //     m_renderedEntities.push_back(npc);
    //     trackedEntity = npc;
    // }
    
    m_updatedEntities = m_renderedEntities;
    SortRenderedEntities();
    m_shouldUpdateNpc = false;
}

void MapElementController::LoadElements(const std::vector<DataMapElement>& elementsData, Tilemap& tilemap) // Same than TriggerController::SetTriggers()
{
    for (MapElement* e : m_mapElements)
        delete e; // Because I use new to create my MapElement (below)
    m_mapElements.clear();

    for (const DataMapElement& data : elementsData) {
        MapElement* e = new MapElement(tilemap); // TODO
        e->SetMapPosition(data.position);
        e->SetOrders(data.orders);
        m_mapElements.push_back(e);
    }
}