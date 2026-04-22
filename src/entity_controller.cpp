#include "entity_controller.hpp"

EntityController::EntityController(const FileReader& fileReader, TextureController& textureController, Camera& camera, Tilemap& tilemap):
    m_player(fileReader, tilemap, textureController, "../assets/sprites/character16", camera, 4.f, 6.f) // Will be removed
{
    m_player.AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
    // m_interactionController.SetUiController(m_context.uiController.get());
    
    m_renderedEntities = {&m_player}; // ?
    
    // Testing my NPC, will be removed (they will be load from the tilemap header)
    for (unsigned int i = 0 ; i < 10 ; i++){
        NPC* npc = new NPC(fileReader, tilemap, textureController, nullptr, "../assets/sprites/npc16", camera, 4.f, 6.f);
        npc->AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
        m_renderedEntities.push_back(npc);
    }
    
    // Testing follow behaviour (trackedEntity parameter will be remove from NPC constructor)
    // Entity* trackedEntity = &m_player;
    // for (unsigned int i = 0 ; i < 10 ; i++){
    //     NPC* npc = new NPC(m_context.fileReader, tilemap, m_context.textureController, trackedEntity, "../assets/sprites/npc16", camera, 4.f, 6.f);
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
        m_updatedEntities[i] = nullptr; // Fix segfault on Entity::Update() when loading a new map
    }
    
    // Warning : NPC adress are still in m_updatedEntities and m_renderedEntities
}

void EntityController::Draw() const
{
    for (Entity* e : m_renderedEntities)
        e->DrawTexture();
}

void EntityController::Update(const GameplayEventInfo playerEventInfo, const float deltaTime)
{
    m_player.SetEventInfo(playerEventInfo);
    
    for (Entity* e : m_updatedEntities) {
        if (e == nullptr) break; // I don't like that
        e->Update(deltaTime);
    }
}

void EntityController::SortRenderedEntities()
{
    // m_renderedEntities is sorted each time an Entity ends its movement (remove then insert the moving entity at the correct index instead ?)
    // It would be even better to sort only once when several entities end their movement in the same frame
    std::sort(m_renderedEntities.begin(), m_renderedEntities.end(),
        [](Entity* a, Entity* b){
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
        case EntityEvent::Interaction : {
            m_interactionController.StartInteraction(m_updatedEntities); // m_updatedEntities ?
            break;
        }
        default:
            break;
    }
}

void EntityController::LoadNPCs()
{
    DeleteNPCs();

    // Same code in EntityController constructor, try to merge it
    m_renderedEntities = {&m_player}; // Clear this vector and keep only the player

    // Load NPCs from the map here
    m_updatedEntities = m_renderedEntities;
    SortRenderedEntities();
}