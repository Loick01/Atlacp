#include "scene.hpp"

// This controller is called only when the first Scene is loaded. Thus, the same Window is used for every Scene
SceneController::SceneController(const int mode):
    m_window("Atlacp", {25,25,25}), m_texture_controller(m_window.GetRenderer()),
    m_context{m_window, m_texture_controller, m_sound_controller, m_file_reader}
{
    const SwitchEvent e = GetSwitchEventFromMode(mode);
    SetCurrentScene(e);
}

SwitchEvent SceneController::GetSwitchEventFromMode(const int mode) const
{
    switch (mode){
        case 0:
            return SwitchEvent::ToGameplay;
        case 1:
            return SwitchEvent::ToEditor;
        case 2:
            return SwitchEvent::ToBattle;
        default:
            std::cout << "Unknown mode\n";
    };
    return SwitchEvent::ToGameplay; // Default initial Scene
}

void SceneController::SetCurrentScene(const SwitchEvent e)
{
    // current_scene.reset(); Delete the previous Scene before creating a new one
    switch(e){
        case SwitchEvent::ToGameplay: {
            m_context.event_controller = std::make_unique<GameplayEventController>();
            m_current_scene = std::make_unique<GameplayTilemapScene>(m_context);
            break;
        } 
        case SwitchEvent::ToEditor: {
            m_context.event_controller = std::make_unique<EditorEventController>();
            m_current_scene = std::make_unique<EditorTilemapScene>(m_context);
            break;
        }
        case SwitchEvent::ToBattle: {
            m_context.event_controller = std::make_unique<EventController>(); // Will use BattleEventController
            m_current_scene = std::make_unique<BattleScene>(m_context);
            break;
        }
        default:{
            std::cout << "Undefined mode\n"; // Will throw an error
            break;
        }
    }
    m_current_scene->AddCallback([this](SwitchEvent e){SetCurrentScene(e);});
}

void SceneController::StartGameloop()
{
    while(m_current_scene->GetGameloop()){
        m_current_scene->Gameloop();
    }
} 

Scene::Scene(GameContext& context):
    m_context(context), m_camera(m_context.window, GridSize{16, 9}, 16), // Don't forget to adapt tile_size when using a new world 
    m_gameloop(true)
{
    if (m_context.window.HasError()) std::cout << "SDL window was not initialized\n"; // Will throw an error
}

bool Scene::GetGameloop() const
{
    return m_gameloop;
}

TilemapScene::TilemapScene(GameContext& context, const bool should_culling):
    Scene(context), m_tileset(m_context.texture_controller),
    m_tilemap(m_context.texture_controller, m_context.file_reader, m_tileset, "../assets/worlds/z_world", m_camera, should_culling)
{
    UpdateTilemapLayer();
    m_tilemap.AddCallback([this](TilemapEvent e){UpdateTilemapLayer();}); // TilemapEvent is unused for now

    // m_context.sound_controller.SetBackgroundMusic("forest.ogg"); // Will be removed
}

void TilemapScene::UpdateTilemapLayer()
{
    m_layers.clear(); // Forget the previous TileLayer
    const std::vector<TileLayer>& layers = m_tilemap.GetLayers();
    for (const TileLayer& l : layers)
        m_layers.push_back(&l);
}

GameplayTilemapScene::GameplayTilemapScene(GameContext& context):
    TilemapScene(context, true), // For now, I don't know how to do without using a dynamic_cast for Player construction
    m_player(m_context.file_reader, m_tilemap, m_context.texture_controller, dynamic_cast<GameplayEventController*>(m_context.event_controller.get()),
    "../assets/sprites/character16", m_camera, 4.0f),
    m_ui_controller(m_context.texture_controller, m_camera, "PixelOperator8"), m_layers_split_index(1) 
{
    m_context.window.HideCursor();
    m_player.AddCallback([this](EntityEvent e){SortRenderedEntities();}); // EntityEvent is unused for now

    m_rendered_entities = {&m_player};

    // Testing my NPC, will be remove (they will be load from the tilemap header)
    for (unsigned int i = 0 ; i < 10 ; i++){
        NPC* npc = new NPC(m_context.file_reader, m_tilemap, m_context.texture_controller, nullptr, "../assets/sprites/npc16", m_camera, 4.0f);
        npc->AddCallback([this](EntityEvent e){SortRenderedEntities();}); // EntityEvent is unused for now
        m_rendered_entities.push_back(npc);
    }
    // Testing follow behaviour (tracked_entity parameter will be remove from NPC constructor)
    /*
    Entity* tracked_entity = &m_player;
    for (unsigned int i = 0 ; i < 10 ; i++){
        NPC* npc = new NPC(m_context.file_reader, m_tilemap, m_context.texture_controller, tracked_entity, "../assets/sprites/npc16", m_camera, 4.0f);
        npc->AddCallback([this](EntityEvent e){SortRenderedEntities();}); // EntityEvent is unused for now
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

void GameplayTilemapScene::SortRenderedEntities()
{
    // m_rendered_entities is sorted each time an Entity ends its movement (remove then insert the moving entity at the correct index instead ?)
    // It would be even better to sort only once when several entities end their movement in the same frame
    std::sort(m_rendered_entities.begin(), m_rendered_entities.end(),
        [](Entity* a, Entity* b){
            return a->GetMapPosition().y < b->GetMapPosition().y; 
        });
}

void GameplayTilemapScene::Gameloop()
{
    m_time.Update();
    m_context.window.ClearRenderer();
    m_context.event_controller->PollAllEvents();
    m_gameloop = m_context.event_controller->HandleWindowEvents();
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
    
    m_context.window.DrawBoxing();
    m_context.window.UpdateRender();
}

EditorTilemapScene::EditorTilemapScene(GameContext& context):
    TilemapScene(context, false), m_ui_controller(m_context.texture_controller, m_camera, "NormalFont", 0/*m_context.event_controller->GetSelectedLayer()*/)
{
    // These values must be set here because they don't exist when the EditorEventController is created
    // For now, I don't know how to do without using a dynamic_cast
    EditorEventController* eec = dynamic_cast<EditorEventController*>(m_context.event_controller.get()); 
    eec->SetValues(m_tileset, m_tilemap.GetLayerCount(), m_camera, m_tilemap);
    
    m_drawables.push_back(&m_tileset);
}

void EditorTilemapScene::Gameloop()
{
    m_context.window.ClearRenderer();
    m_context.event_controller->PollAllEvents();
    
    m_gameloop = m_context.event_controller->HandleWindowEvents();
    m_context.event_controller->HandleEvents();

    for (const TileLayer* l : m_layers) l->DrawTexture(); // Unlike GameplayTilemapScene, TileLayer are rendered all at once
    for (const Drawable* d : m_drawables) d->DrawTexture(); // Will be removed if m_tileset become a UiElement (drawed by UiController::Draw)
    
    // Will use Notifier to know selected layer from UiController
    // m_ui_controller.UpdateState(m_context.event_controller->GetSelectedLayer());
    m_ui_controller.Draw();
    m_context.window.UpdateRender();    
}

BattleScene::BattleScene(GameContext& context):
    Scene(context), m_ui_controller(m_context.texture_controller, m_camera, "PixelOperator8")
{
    m_context.sound_controller.SetBackgroundMusic("battle.ogg"); // Will be removed
}

void BattleScene::Gameloop()
{
    m_context.window.ClearRenderer();
    m_context.event_controller->PollAllEvents();
    
    m_gameloop = m_context.event_controller->HandleWindowEvents();
    
    m_ui_controller.Draw();
    m_context.window.DrawBoxing();
    m_context.window.UpdateRender();
}