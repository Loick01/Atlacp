#include "scene.hpp"

// This controller is called only when the first Scene is loaded. Thus, the same Window is used for every Scene
SceneController::SceneController(const int mode):
    m_window("Atlacp", {25,25,25}), m_textureController(m_window.GetRenderer()),
    m_context{m_window, m_textureController, m_soundController, m_fileReader},
    m_pendingSwitch(std::nullopt)
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
    m_currentScene->AddCallback([this](SwitchEvent e){RequestSwitchScene(e);});
}

void SceneController::RequestSwitchScene(const SwitchEvent e)
{
    m_pendingSwitch = e;
}

void SceneController::ApplySwitchScene()
{
    if (!m_pendingSwitch) return;
    SetCurrentScene(*m_pendingSwitch); // Use * to get SwitchEvent from std::optional<SwitchEvent>
    m_pendingSwitch.reset();
}

void SceneController::StartGameloop()
{
    while(m_currentScene->GetGameloop()){
        m_currentScene->Gameloop();
        ApplySwitchScene();
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
    m_tilemap.AddCallback([this](TilemapEvent e){HandleTilemapEvent(e);});

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
    m_entities.LoadNPCs(m_context.textureController, m_camera, m_tilemap, 
                "../assets/npcs/z_npcs", m_tilemap.GetCurrentMapIndex()); // NPC filepath will be read in WorldData
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

    m_entities.Update(static_cast<GameplayEventController*>(m_context.eventController.get())->GetEventState(), deltaTime);

    m_context.uiController->Draw();
    
    m_context.window.DrawBoxing();
    m_context.window.UpdateRender();
}

void GameplayTilemapScene::HandleTilemapEvent(const TilemapEvent e)
{
    switch(e) {
        case TilemapEvent::LoadingMap : {
            UpdateTilemapLayer();
            m_entities.LoadNPCs(m_context.textureController, m_camera, m_tilemap, 
                "../assets/npcs/z_npcs", m_tilemap.GetCurrentMapIndex()); // NPC filepath will be read in WorldData
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
    
    const EditorEventState eventState = static_cast<EditorEventController*>(m_context.eventController.get())->GetEventState(); 
    
    for (unsigned int i = 0 ; i < m_layers.size() ; i++){ // Unlike GameplayTilemapScene, TileLayer are rendered all at once
        if (eventState.isLayerRendered[i])
            m_layers[i]->DrawTexture();
    }
    for (const Drawable* d : m_drawables) d->DrawTexture(); // Will be removed if m_tileset become a UiElement (drawed by UiController::Draw)
    
    static_cast<EditorUiController*>(m_context.uiController.get())->SetEventState(eventState);
    m_context.uiController->Update();
    m_context.uiController->Draw();
    m_context.window.UpdateRender();    
}

BattleScene::BattleScene(GameContext& context):
    Scene(context), m_battleController(BattleActor("Howler", 100), BattleActor("Bone Appetit", 100)) // BattleActors will not be here
{
    m_camera.ComputeViewport(m_context.window, GridSize{16, 9}, 1); // Camera::m_screenOffset and Camera::m_viewport must be defined when drawing ui elements, but this line should not be here 
    m_context.eventController = std::make_unique<BattleEventController>();
    m_context.uiController = std::make_unique<BattleUiController>(m_context.textureController, m_camera, "PixelOperator8");
    // m_context.soundController.SetBackgroundMusic("battle.ogg"); // Background music will not be started from here

    BattleUiController* uiController = static_cast<BattleUiController*>(m_context.uiController.get());
    // Actor name and health will be defined here instead of BattleUiController (names will be retrivied from the BattleController)
    uiController->UpdateText("actorAName", "Howler"); 
    uiController->UpdateText("actorBName", "Bone Appetit");
    uiController->UpdateText("actorAHealth", "100 PV"); 
    uiController->UpdateText("actorBHealth", "100 PV");

    m_battleController.AddCallback([this](ExitEvent e){Exit(e);});
    m_context.window.HideCursor(); // Mouse will not be used for events
}

void BattleScene::Gameloop()
{
    m_context.window.ClearRenderer();
    m_context.eventController->PollAllEvents();
    
    m_gameloop = m_context.eventController->HandleWindowEvents();
    m_context.eventController->HandleEvents();

    const BattleEventState eventState = static_cast<BattleEventController*>(m_context.eventController.get())->GetEventState(); 
    // static_cast<BattleUiController*>(m_context.uiController.get())->SetEventState(eventState);

    m_battleController.SetEventState(eventState);
    m_battleController.PlayTurn();

    m_context.uiController->Draw();
    m_context.window.DrawBoxing();
    m_context.window.UpdateRender();
}

void BattleScene::Exit(const ExitEvent e) {
    switch(e) {
        case ExitEvent::ExitWin : {
            Notify(SwitchEvent::ToGameplay);
            break;
        }
        case ExitEvent::ExitLost : {
            throw std::runtime_error("ExitEvent::ExitLost is undefined for now\n"); 
            // Notify(SwitchEvent::ToMenu);
            // break;
        }
    }
}