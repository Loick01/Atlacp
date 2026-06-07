#include "map/entity_controller.hpp"

#include "map/map_entity.hpp"
#include "map/npc.hpp"
#include "system/camera.hpp"
#include "system/file.hpp"
#include "tile/tilemap.hpp"

EntityController::EntityController(const FileReader& fileReader, UiController& uiController, TextureController& textureController,
Camera& camera, Tilemap& tilemap):
    m_player(fileReader, tilemap, textureController, "../assets/sprites/character16", camera, 4.f, 6.f), // Will be removed (player's sprite path will be read from a file)
    m_fileReader(fileReader), m_interactionController(uiController)
{
    m_player.AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
    
    m_renderedEntities = {&m_player}; // ?
    
    // Testing my NPC, will be removed (they will be load from the tilemap header)
    // for (unsigned int i = 0 ; i < 10 ; i++){
    //     NPC* npc = new NPC(m_fileReader, tilemap, textureController, nullptr, "../assets/sprites/npc16_inverted", camera, 4.f, 6.f);
    //     npc->AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
    //     m_renderedEntities.push_back(npc);
    // }
    
    // Testing follow behaviour (trackedEntity parameter will be remove from NPC constructor)
    // MapEntity* trackedEntity = &m_player;
    // for (unsigned int i = 0 ; i < 10 ; i++){
    //     NPC* npc = new NPC(m_context.m_fileReader, tilemap, m_context.textureController, trackedEntity, "../assets/sprites/npc16", camera, 4.f, 6.f);
    //     npc->AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
    //     m_renderedEntities.push_back(npc);
    //     trackedEntity = npc;
    // }

    m_updatedEntities = m_renderedEntities;
    SortRenderedEntities(); // ?
}

EntityController::~EntityController()
{
    DeleteNPCs();
}

void EntityController::DeleteNPCs()
{
    // Do not try to delete the player (first element in m_updatedEntities, be sure to don't modify the order --> player must always be updated before every NPC)
    for (unsigned int i = 1 ; i < m_updatedEntities.size() ; i++) {
        delete m_updatedEntities[i];
        m_updatedEntities[i] = nullptr; // Fix segfault on MapEntity::Update() when loading a new map
    }
    
    // Warning : NPC adress are still in m_renderedEntities
}

void EntityController::Draw() const
{
    for (MapEntity* e : m_renderedEntities)
        e->DrawTexture();
}

void EntityController::Update(const GameplayEventState& playerEventState, const float deltaTime)
{
    m_player.SetEventState(playerEventState);
    
    for (MapEntity* e : m_updatedEntities) {
        if (e == nullptr) break; // I don't like that
        e->Update(deltaTime);
    }
}

void EntityController::SortRenderedEntities()
{
    // m_renderedEntities is sorted each time an MapEntity ends its movement (remove then insert the moving entity at the correct index instead ?)
    // It would be even better to sort only once when several entities end their movement in the same frame
    std::sort(m_renderedEntities.begin(), m_renderedEntities.end(),
        [](MapEntity* a, MapEntity* b){
            return a->GetMapPosition().y < b->GetMapPosition().y; 
        });
}

void EntityController::HandleEntityEvent(const EntityEvent e)
{
    switch(e) {
        case EntityEvent::SortEntity : {
            SortRenderedEntities();
            break;
        }
        case EntityEvent::EnterInteraction : {
            m_interactionController.StartInteraction(m_updatedEntities); // m_updatedEntities ?
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

void EntityController::LoadNPCs(TextureController& textureController, Camera& camera, Tilemap& tilemap,
    const std::string& filepath, const unsigned int mapIndex)
{
    DeleteNPCs();

    // Same code in EntityController constructor, try to merge it
    m_renderedEntities = {&m_player}; // Clear this vector and keep only the player

    const std::vector<DataNPC> npcsData = m_fileReader.ReadNPCsFile(filepath, mapIndex);

    for (unsigned int i = 0 ; i < npcsData.size() ; i++) {
        DataNPC currentData = npcsData[i];
        const MapPosition npcPosition = currentData.position;
        if (!tilemap.IsFreePosition(npcPosition))
            throw std::runtime_error("NPC can only spawn on free position"); 
        
        NPC* npc = new NPC(
            m_fileReader, 
            tilemap, 
            textureController, 
            nullptr,
            "../assets/sprites/" + currentData.sprite, // TODO
            camera, npcPosition,
            currentData.walkSpeed, currentData.runSpeed
        );
        npc->AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
        m_renderedEntities.push_back(npc);
    }
    
    m_updatedEntities = m_renderedEntities;
    SortRenderedEntities();
}