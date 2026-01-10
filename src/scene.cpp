#include "scene.hpp"

Scene::Scene():
    m_window("Atlacp", {25,25,25}),
    m_camera(m_window, GridSize{16, 9}, 16), // Don't forget to adapt tile_size when using a new world 
    m_texture_controller(m_window.GetRenderer()),
    m_gameloop(true)
{
    if (m_window.HasError())
        std::cout << "SDL window was not initialized\n"; // Will throw an error
}

bool Scene::GetGameloop() const
{
    return m_gameloop;
}

TilemapScene::TilemapScene():
    m_tileset(m_texture_controller),
    m_tilemap(m_texture_controller, m_file_reader, m_tileset, "../assets/worlds/z_world", m_camera)
{
    UpdateTilemapLayer();
    m_tilemap.AddListener([this](){UpdateTilemapLayer();});
}

void TilemapScene::UpdateTilemapLayer()
{
    m_layers.clear(); // Forget the previous TileLayer
    const std::vector<TileLayer>& layers = m_tilemap.GetLayers();
    for (const TileLayer& l : layers)
        m_layers.push_back(&l);
}

GameplayTilemapScene::GameplayTilemapScene():
    m_player(m_file_reader, m_tilemap, m_texture_controller, m_event_controller, "../assets/sprites/character16", m_camera, 4.0f),
    m_ui_controller(m_texture_controller, m_camera, "PixelOperator8"), m_layers_split_index(1) 
{
    m_window.HideCursor();
    m_tilemap.SetLayerCulling(true);

    m_rendered_entities = {&m_player};

    // Testing my NPC, will be remove (they will be load from the tilemap header)
    for (unsigned int i = 0 ; i < 1 ; i++){
        NPC* npc = new NPC(m_file_reader, m_tilemap, m_texture_controller, nullptr, "../assets/sprites/npc16", m_camera, 4.0f);
        m_rendered_entities.push_back(npc);
    }
    // Testing follow behaviour (tracked_entity parameter will be remove from NPC constructor)
    /*
    Entity* tracked_entity = &m_player;
    for (unsigned int i = 0 ; i < 10 ; i++){
        NPC* npc = new NPC(m_file_reader, m_tilemap, m_texture_controller, tracked_entity, "../assets/sprites/npc16", m_camera, 4.0f);
        m_rendered_entities.push_back(npc);
        tracked_entity = npc;
    }
    */
    m_updated_entities = m_rendered_entities;
}

GameplayTilemapScene::~GameplayTilemapScene()
{
    // Do not try to delete the player (first element in m_updated_entities, be sure to don't modify the order)
    for (unsigned int i = 1 ; i < m_updated_entities.size() ; i++)
        delete m_updated_entities[i];
}

void GameplayTilemapScene::Gameloop()
{
    m_time.Update();
    m_window.ClearRenderer();
    m_event_controller.PollAllEvents();
    
    m_gameloop = m_event_controller.HandleWindowEvents();
    
    // Remove (should sort only at the end of any movement, or even better --> remove then insert the moving npc at the correct index)
    std::sort(m_rendered_entities.begin(), m_rendered_entities.end(),
        [](Entity* a, Entity* b){
            return a->GetMapPosition().y < b->GetMapPosition().y; 
        });
    const float delta_time = m_time.GetDeltaTime();
    
    for (size_t i=0 ; i<m_layers_split_index ; i++)
        m_layers[i]->DrawTexture();
    for (Entity* e : m_rendered_entities)
        e->DrawTexture();
    for (size_t i=m_layers_split_index ; i<m_layers.size() ; i++)
        m_layers[i]->DrawTexture();

    for (Entity* e : m_updated_entities)
        e->Update(delta_time);

    m_ui_controller.Draw();
    
    m_window.DrawBoxing();
    m_window.UpdateRender();
}

EditorTilemapScene::EditorTilemapScene():
    m_event_controller(m_tileset, m_tilemap.GetLayerCount()), m_ui_controller(m_texture_controller, m_camera, "NormalFont", m_event_controller.GetSelectedLayer())
{
    m_tilemap.SetLayerCulling(false);
    m_drawables.push_back(&m_tileset);
}

void EditorTilemapScene::Gameloop()
{
    m_window.ClearRenderer();
    m_event_controller.PollAllEvents();
    
    m_gameloop = m_event_controller.HandleWindowEvents();
    m_event_controller.HandleEditorEvent(m_tilemap, m_camera);

    for (const TileLayer* l : m_layers) l->DrawTexture(); // Unlike GameplayTilemapScene, TileLayer are rendered all at once
    for (const Drawable* d : m_drawables) d->DrawTexture(); // Will be removed if m_tileset become a UiElement (drawed by UiController::Draw)
    
    m_ui_controller.UpdateState(m_event_controller.GetSelectedLayer());
    m_ui_controller.Draw();
    m_window.UpdateRender();    
}

FightScene::FightScene()
{

}

void FightScene::Gameloop()
{
    m_window.ClearRenderer();
    m_event_controller.PollAllEvents();
    
    m_gameloop = m_event_controller.HandleWindowEvents();
    
    m_window.DrawBoxing();
    m_window.UpdateRender();
}