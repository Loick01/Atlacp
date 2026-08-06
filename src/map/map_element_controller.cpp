#include "map/map_element_controller.hpp"

#include "map/map_entity.hpp"
#include "map/npc.hpp"
#include "core/camera.hpp"
#include "core/file.hpp"
#include "tile/tilemap.hpp"

MapElementController::MapElementController(const FileReader& fileReader, UiController& uiController, TextureController& textureController,
Camera& camera, Tilemap& tilemap):
    m_player(fileReader, tilemap, textureController, "map_entity/character16", camera, 4.f, 6.f), // Will be removed (player's sprite path will be read from a file) ?
    m_fileReader(fileReader), m_interactionController(uiController)
{
    m_player.AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
    
    m_renderedEntities = {&m_player}; // ?

    m_updatedEntities = m_renderedEntities;
    SortRenderedEntities(); // ?
}

MapElementController::~MapElementController()
{
    DeleteNPCs();
}

void MapElementController::DeleteNPCs()
{
    // Do not try to delete the player (first element in m_updatedEntities, be sure to don't modify the order --> player must always be updated before every NPC)
    for (unsigned int i = 1 ; i < m_updatedEntities.size() ; i++) {
        delete m_updatedEntities[i];
        m_updatedEntities[i] = nullptr; // Fix segfault on MapEntity::Update() when loading a new map
    }
    
    // Warning : NPC adress are still in m_renderedEntities
}

void MapElementController::Draw() const
{
    for (MapEntity* e : m_renderedEntities)
        e->DrawTexture();
}

void MapElementController::Update(const GameMapEventState& playerEventState, const float deltaTime)
{
    m_player.SetEventState(playerEventState);
    
    for (MapEntity* e : m_updatedEntities) {
        if (e == nullptr) break; // I don't like that
        e->Update(deltaTime);
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
        case EntityEvent::SortEntity : {
            SortRenderedEntities();
            break;
        }
        case EntityEvent::EnterInteraction : {
            m_interactionController.StartInteraction(m_updatedEntities, m_mapElements); // m_updatedEntities ?
            m_interactionController.ProcessInteraction(); // Will check if a MapEntity/MapElement has been found
            break;
        }
        case EntityEvent::LeaveInteraction : {
            m_interactionController.EndInteraction();
            break;
        }
        default:
            break;
    }
}

void MapElementController::LoadNPCs(TextureController& textureController, Camera& camera, Tilemap& tilemap,
    const std::string& filepath, const unsigned int mapIndex)
{
    DeleteNPCs();

    // Same code in MapElementController constructor, try to merge it
    m_renderedEntities = {&m_player}; // Clear this vector and keep only the player

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
            camera, npcPosition,
            data.walkSpeed, data.runSpeed
        );
        npc->AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
        m_renderedEntities.push_back(npc);
    }
    
    // Testing follow behaviour (trackedEntity parameter will be remove from NPC constructor)
    // MapEntity* trackedEntity = &m_player;
    // for (unsigned int i = 0 ; i < 10 ; i++){
    //     NPC* npc = new NPC(m_fileReader, tilemap, textureController, trackedEntity, "map_entity/" + data.sprite, camera, 4.f, 6.f);
    //     npc->AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
    //     m_renderedEntities.push_back(npc);
    //     trackedEntity = npc;
    // }
    
    m_updatedEntities = m_renderedEntities;
    SortRenderedEntities();
}

void MapElementController::LoadElements(const std::vector<DataMapElement>& elements, Tilemap& tilemap)
{
    for (const DataMapElement& data : elements) {
        MapElement* e = new MapElement(tilemap); // TODO
        e->SetMapPosition(data.position);
        m_mapElements.push_back(e);
    }
}