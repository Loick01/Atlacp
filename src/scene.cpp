#include "scene.hpp"

// This controller is called only when the first Scene is loaded. Thus, the same Window is used for every Scene
SceneController::SceneController(const int mode):
    m_window("Atlacp", {25,25,25}), m_textureController(m_window.GetRenderer()),
    m_context{m_window, m_textureController, m_soundController, m_fileReader}
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
            throw std::invalid_argument("Unknown mode\n");
    };
}

void SceneController::SetCurrentScene(const SwitchEvent e)
{
    switch(e){
        case SwitchEvent::ToGameplay: {
            m_currentScene = std::make_unique<GameplayTilemapScene>(m_context);
            break;
        } 
        case SwitchEvent::ToEditor: {
            m_currentScene = std::make_unique<EditorTilemapScene>(m_context);
            break;
        }
        case SwitchEvent::ToBattle: {
            m_currentScene = std::make_unique<BattleScene>(m_context);
            break;
        }
        default:{
            throw std::invalid_argument("Unknown value\n");
        }
    }
    m_currentScene->AddCallback([this](SwitchEvent e){SetCurrentScene(e);});
}

void SceneController::StartGameloop()
{
    while(m_currentScene->GetGameloop()){
        m_currentScene->Gameloop();
    }
} 

Scene::Scene(GameContext& context):
    m_context(context), m_gameloop(true)
{

}

bool Scene::GetGameloop() const
{
    return m_gameloop;
}

TilemapScene::TilemapScene(GameContext& context, const bool shouldCulling):
    Scene(context), m_tileset(m_context.textureController),
    m_tilemap(m_context.textureController, m_context.fileReader, m_tileset, "../assets/worlds/z_world", m_camera, shouldCulling)
{
    m_camera.ComputeViewport(m_context.window, GridSize{16, 9}, m_tileset.GetTileSize());
    m_camera.SetTilemapInfo(m_tilemap.GetLayerSize()*m_tileset.GetTileSize());
    UpdateTilemapLayer();
    m_tilemap.AddCallback([this](TilemapEvent e){HandleTilemapEvent(e);}); // TilemapEvent is unused for now

    // m_context.soundController.SetBackgroundMusic("forest.ogg"); // Will be removed (read from a file)
}

void TilemapScene::UpdateTilemapLayer()
{
    m_layers.clear(); // Forget the previous TileLayer
    const std::vector<TileLayer>& layers = m_tilemap.GetLayers();
    for (const TileLayer& l : layers)
        m_layers.push_back(&l);
}

void TilemapScene::HandleTilemapEvent(const TilemapEvent e)
{
    switch(e) {
        case TilemapEvent::LoadingMap : {
            UpdateTilemapLayer();
            break;
        }
        default:
            break;
    }
}


GameplayTilemapScene::GameplayTilemapScene(GameContext& context):
    TilemapScene(context, true), m_entities(m_context.fileReader, m_context.textureController, m_camera, m_tilemap),
    m_layersSplitIndex(1) 
{
    m_context.eventController = std::make_unique<GameplayEventController>();
    m_context.uiController = std::make_unique<GameplayUiController>(m_context.textureController, m_camera, "PixelOperator8");

    m_context.window.HideCursor();
}

void GameplayTilemapScene::Gameloop()
{
    m_time.Update();
    const float deltaTime = m_time.GetDeltaTime();
    m_context.window.ClearRenderer();
    m_context.eventController->PollAllEvents();
    m_gameloop = m_context.eventController->HandleWindowEvents();
    
    m_context.eventController->HandleEvents(); 
    
    m_camera.ComputeMapCulling(m_tilemap.GetLayerSize(), m_tileset.GetTileSize());
    for (size_t i=0 ; i<m_layersSplitIndex ; i++)
        m_layers[i]->DrawTexture();
    
    m_entities.Draw();
    
    for (size_t i=m_layersSplitIndex ; i<m_layers.size() ; i++)
        m_layers[i]->DrawTexture();

    m_entities.Update(static_cast<GameplayEventController*>(m_context.eventController.get())->GetEventInfo(), deltaTime);

    m_context.uiController->Draw();
    
    m_context.window.DrawBoxing();
    m_context.window.UpdateRender();
}

void GameplayTilemapScene::HandleTilemapEvent(const TilemapEvent e)
{
    switch(e) {
        case TilemapEvent::LoadingMap : {
            UpdateTilemapLayer();
            m_entities.LoadNPCs();
            break;
        }
        default:
            break;
    }
}

EditorTilemapScene::EditorTilemapScene(GameContext& context):
    TilemapScene(context, false)
{
    m_context.eventController = std::make_unique<EditorEventController>(m_tileset, m_camera, m_tilemap);
    m_context.uiController = std::make_unique<EditorUiController>(m_context.textureController, m_camera, "NormalFont");
    m_drawables.push_back(&m_tileset);
}

void EditorTilemapScene::Gameloop()
{
    m_context.window.ClearRenderer();
    m_context.eventController->PollAllEvents();
    
    m_gameloop = m_context.eventController->HandleWindowEvents();
    m_context.eventController->HandleEvents();
    m_camera.ComputeMapCulling(m_tilemap.GetLayerSize(), m_tileset.GetTileSize());
    
    const EditorEventInfo eventInfo = static_cast<EditorEventController*>(m_context.eventController.get())->GetEventInfo(); // Need dynamic_cast ?
    
    for (unsigned int i = 0 ; i < m_layers.size() ; i++){ // Unlike GameplayTilemapScene, TileLayer are rendered all at once
        if (eventInfo.isLayerRendered[i])
            m_layers[i]->DrawTexture();
    }
    for (const Drawable* d : m_drawables) d->DrawTexture(); // Will be removed if m_tileset become a UiElement (drawed by UiController::Draw)
    
    static_cast<EditorUiController*>(m_context.uiController.get())->SetEventInfo(eventInfo); // Need dynamic_cast ?
    m_context.uiController->Update();
    m_context.uiController->Draw();
    m_context.window.UpdateRender();    
}

BattleScene::BattleScene(GameContext& context):
    Scene(context)
{
    m_camera.ComputeViewport(m_context.window, GridSize{16, 9}, 1); // Camera::m_screenOffset and Camera::m_viewport must be defined when drawing ui elements, but this line should not be here 
    m_context.eventController = std::make_unique<EventController>(); // Will use BattleEventController
    m_context.uiController = std::make_unique<BattleUiController>(m_context.textureController, m_camera, "PixelOperator8");
    m_context.soundController.SetBackgroundMusic("battle.ogg"); // Will be removed
}

void BattleScene::Gameloop()
{
    m_context.window.ClearRenderer();
    m_context.eventController->PollAllEvents();
    
    m_gameloop = m_context.eventController->HandleWindowEvents();
    
    m_context.uiController->Draw();
    m_context.window.DrawBoxing();
    m_context.window.UpdateRender();
}