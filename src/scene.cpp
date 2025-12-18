#include "scene.hpp"

Scene::Scene():
    m_window("Atlacp", {25,25,25}),
    m_camera(m_window, ScenePosition{16, 9}, 32),
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
    m_tileset(m_texture_controller, m_camera, m_file_reader),
    m_tilemap(m_texture_controller, m_file_reader, m_tileset, "../assets/worlds/ff_world", m_camera)
{
    
}

GameplayTilemapScene::GameplayTilemapScene():
    m_player(m_file_reader, m_tilemap, m_texture_controller, m_game_event_controller, "../assets/sprites/character", m_camera, 5.0f)
{
    m_window.HideCursor();
    m_tilemap.SetShouldCulling(true);
    // Layer 1
    m_drawables = {&m_tilemap};
    // Layer 2
    m_entities = {&m_player};
    // Testing my NPC, will be remove (they will be load from the tilemap header)
    for (unsigned int i = 0 ; i < 10 ; i++){
        NPC* npc = new NPC(m_file_reader, m_tilemap, m_texture_controller, nullptr, "../assets/sprites/npc", m_camera, 5.0f);
        // NPC are not yet deleted + their texture is destroy by TextureController::~TextureController
        m_entities.push_back(npc);
    }
    /*
    // Testing follow behaviour (tracked_entity parameter will be remove from NPC constructor)
    Entity* tracked_entity = &m_player;
    for (unsigned int i = 0 ; i < 14 ; i++){
        NPC* npc = new NPC(m_file_reader, m_tilemap, m_texture_controller, tracked_entity, "../assets/sprites/npc", m_camera, 3.0f);
        // NPC are not yet deleted + their texture is destroy by TextureController::~TextureController
        m_entities.push_back(npc);
        tracked_entity = npc;
    }*/
}

void GameplayTilemapScene::Gameloop()
{
    m_time.Update();
    m_window.ClearRenderer();
    m_game_event_controller.PollAllEvents();
    
    m_gameloop = m_game_event_controller.HandleWindowEvents();
    
    // Remove (should sort only at the end of any movement, or even better --> remove then insert the moving npc at the correct index)
    std::sort(m_entities.begin(), m_entities.end(),
        [](Entity* a, Entity* b){
            return a->GetMapPosition().y < b->GetMapPosition().y; 
        });
    const float delta_time = m_time.GetDeltaTime();
    for (const Drawable* d : m_drawables) d->DrawTexture();
    for (Entity* e : m_entities){
        e->DrawTexture();
        e->Update(delta_time);
    }

    m_window.DrawBoxing();
    m_window.UpdateRender();
}

EditorTilemapScene::EditorTilemapScene():
    m_editor_event_controller(m_tileset)
{
    m_tilemap.SetShouldCulling(false);
    m_drawables = {&m_tilemap, &m_tileset};
}

void EditorTilemapScene::Gameloop()
{
    m_window.ClearRenderer();
    m_editor_event_controller.PollAllEvents();
    
    m_gameloop = m_editor_event_controller.HandleWindowEvents();
    m_editor_event_controller.HandleEditorEvent(m_tilemap, m_camera);

    for (const Drawable* d : m_drawables) d->DrawTexture();
    
    m_window.UpdateRender();    
}