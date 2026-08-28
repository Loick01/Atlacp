#include "scene/scene.hpp"

#include <stdexcept>

#include "sound/sound.hpp"

// This controller is called only when the first Scene is loaded. Thus, the same Window is used for every Scene
SceneController::SceneController(const int mode):
    m_window("Atlacp", {25,25,25}), m_fontController("PixelOperator8.ttf", m_window.GetSize().x*0.388), m_textureController(m_fontController, m_window.GetRenderer()), 
    m_uiController(m_fileReader, m_textureController, "PixelOperator8"), m_uiComponentController(m_time, m_uiController),
    m_context{m_window, m_time, m_fontController, m_textureController, m_fileReader, m_uiController, m_uiComponentController},
    m_pendingSwitch(std::nullopt)
{
    const SwitchEvent e = GetSwitchEventFromMode(mode);
    SetCurrentScene(e);
}

SwitchEvent SceneController::GetSwitchEventFromMode(const int mode) const
{
    switch (mode){
        case 0:
            return SwitchEvent::ToMainMenu;
        case 1:
            return SwitchEvent::ToGameMap;
        case 2:
            return SwitchEvent::ToBattle;
        case 3:
            return SwitchEvent::ToEditorMap;
        default:
            throw std::invalid_argument("Unknown mode");
    };
}

void SceneController::SetCurrentScene(const SwitchEvent e)
{
    m_uiController.ClearAll();
    switch(e){
        case SwitchEvent::ToGameMap: {
            m_currentScene = std::make_unique<GameMapScene>(m_context);
            break;
        } 
        case SwitchEvent::ToEditorMap: {
            m_currentScene = std::make_unique<EditorMapScene>(m_context);
            break;
        }
        case SwitchEvent::ToBattle: {
            m_currentScene = std::make_unique<BattleScene>(m_context);
            break;
        }
        case SwitchEvent::ToMainMenu: {
            m_currentScene = std::make_unique<MainMenuScene>(m_context);
            break;
        }
        default:{
            throw std::invalid_argument("Unknown value");
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
{}

bool Scene::GetGameloop() const
{
    return m_gameloop;
}

MainMenuScene::MainMenuScene(GameContext& context):
    Scene(context), 
    m_selector(m_context.uiController, "selector.uit"), m_staticList(m_context.uiController, "main_menu_mode_selection.uif")
{
    m_camera.ComputeViewport(m_context.window, GridSize{16, 9}, 1); // Camera::m_screenOffset and Camera::m_viewport must be defined when drawing ui elements, but this line should not be here 
    // if I put camera in GameContext, I could avoid calling these setters ?
    m_context.uiController.SetSize(m_camera.GetViewport());
    m_context.uiController.SetPosition(m_camera.GetScreenOffset());
    
    m_context.eventController = std::make_unique<MainMenuEventController>();
   
    m_context.uiController.BuildUiFile("main_menu_scene.uif");
    // SoundController::GetInstance().SetBackgroundMusic("spirits.ogg"); // Background music will not be started from here

    m_context.window.HideCursor();

    m_staticList.Open();
    m_selector.Open();
    m_selector.SetOptionKeys(m_staticList.GetItemsKey(), Axis::Height, Axis::Width, 1.f, -0.05f);
}

void MainMenuScene::Gameloop()
{
    m_context.window.ClearRenderer();
    
    m_context.eventController->PollAllEvents();
    m_gameloop = m_context.eventController->HandleWindowEvents();
    m_context.eventController->HandlePollEvents();
    // m_context.eventController->HandleStateEvents();

    const MainMenuEventState eventState = static_cast<MainMenuEventController*>(m_context.eventController.get())->GetEventState();

    if (m_selector.VerticalNavigation(eventState.uiDirection, eventState.isAction)) {
        SwitchEvent se;
        switch(m_selector.GetOptionIndex()) { // Must be the same order as in main_menu_mode_selection.uif
            case 0:
                se = SwitchEvent::ToGameMap;
                break;
            case 1:
                se = SwitchEvent::ToEditorMap;
                break;
            case 2:
                se = SwitchEvent::ToBattle;
                break;
            case 3:
                m_gameloop = false;
                return;
            default:
                throw std::runtime_error("Selector is not supposed to be on this index : " + m_selector.GetOptionIndex());
        }
        Notify(se);
    }
    SoundController::GetInstance().PlayRequestedChunk(); 
    
    m_context.uiController.Draw();
    m_context.window.DrawBoxing();
    m_context.window.UpdateRender();
}

TilemapScene::TilemapScene(GameContext& context, const bool shouldCulling):
    Scene(context), m_tileset(m_context.textureController),
    m_tilemap(m_context.textureController, m_context.fileReader, m_tileset, "tx_world", m_camera, shouldCulling) // Replace world file here to try other worlds
{
    m_camera.ComputeViewport(m_context.window, GridSize{16, 9}, m_tileset.GetTileSize());
    m_camera.SetTilemapInfo(m_tilemap.GetLayerSize()*m_tileset.GetTileSize());
    m_tilemap.AddCallback([this](TilemapEvent e){HandleTilemapEvent(e);});

    // if I put camera in GameContext, I could avoid calling these setters ?
    m_context.uiController.SetSize(m_camera.GetViewport());
    m_context.uiController.SetPosition(m_camera.GetScreenOffset());

    // UpdateTilemapLayer() must be called when initializing TilemapScene
    // Because it's a virtual function, it would only call TilemapScene::UpdateTilemapLayer() if called here
    // That's why it has to be called in the constructors of TilemapScene subclasses (GameMapScene and EditorMapScene)
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

GameMapScene::GameMapScene(GameContext& context):
    TilemapScene(context, true), m_elementsController(m_context.fileReader, m_context.textureController, m_camera, m_tilemap, m_tilemap.GetWorldData().spritePlayerPath),
    m_orderController(m_camera, m_context.fileReader, m_elementsController, m_tilemap, m_context.uiComponentController), 
    m_interactionController(m_orderController), m_triggerController(m_orderController)
{
    UpdateTilemapLayer();
    m_context.eventController = std::make_unique<GameMapEventController>();
    
    m_elementsController.LoadNPCs(m_context.textureController, m_camera, m_tilemap, 
                m_tilemap.GetWorldData().npcsFile, m_tilemap.GetCurrentMapIndex());
    m_elementsController.LoadElements(m_tilemap.GetElementsData(), m_tilemap);
    m_triggerController.SetTriggers(m_tilemap.GetTriggersData(), m_tilemap);
    
    SoundController::GetInstance().SetBackgroundMusic("forest.ogg"); // Will be removed (read from a file)
    m_context.window.HideCursor();

    m_elementsController.AddCallback([this](EntityEvent e){HandleEntityEvent(e);});
}


void GameMapScene::HandleEntityEvent(const EntityEvent e)
{
    switch (e) {
        case EntityEvent::HasMoved : {
            MapEntity* currentEntity = m_elementsController.GetCurrentMapEntityUpdated();
            if (currentEntity->GetId() != 0) break; // Remove --> I use this because I only want triggers with the player for now (Player Id is 0) 
            m_triggerController.LookForTrigger(currentEntity);
            break;
        }
        case EntityEvent::EnterInteraction : {
            // Should use GetCurrentMapEntityUpdated() to get interacting entity ?
            m_interactionController.InitializeInteraction(m_elementsController.GetEntities(), m_elementsController.GetElements());
            m_interactionController.StartInteraction();
            break;
        }
        case EntityEvent::ContinueInteraction : {
            m_interactionController.ContinueInteraction();
            break;
        }
        case EntityEvent::ContinueTrigger : {
            m_triggerController.ContinueTrigger();
            break;
        }
        default:
            throw std::runtime_error("GameMapScene::HandleEntityEvent() : Unknown EntityEvent value");
    }
}

void GameMapScene::Gameloop()
{
    m_context.time.Update();
    const float deltaTime = m_context.time.GetDeltaTime();
    m_context.window.ClearRenderer();
    m_context.eventController->PollAllEvents();
    m_gameloop = m_context.eventController->HandleWindowEvents();
    
    m_context.eventController->HandlePollEvents(); 
    m_context.eventController->HandleStateEvents(); 
    
    m_camera.Update(deltaTime); // Here ?
    m_camera.ComputeMapCulling(m_tilemap.GetLayerSize(), m_tileset.GetTileSize());
    const size_t layerSplitIndex = m_tilemap.GetLayerSplitIndex();
    for (size_t i = 0 ; i < layerSplitIndex ; i++)
        m_layers[i]->DrawTexture();
    
    m_elementsController.Draw();
    
    for (size_t i = layerSplitIndex ; i < m_layers.size() ; i++)
        m_layers[i]->DrawTexture();

    m_elementsController.Update(static_cast<GameMapEventController*>(m_context.eventController.get())->GetEventState(), deltaTime);

    SoundController::GetInstance().PlayRequestedChunk();
    m_context.uiController.Draw();
    
    m_context.window.DrawBoxing();
    m_context.window.UpdateRender();
}

void GameMapScene::HandleTilemapEvent(const TilemapEvent e)
{
    switch(e) {
        case TilemapEvent::LoadingMap : {
            UpdateTilemapLayer();
            m_elementsController.LoadNPCs(m_context.textureController, m_camera, m_tilemap, 
                m_tilemap.GetWorldData().npcsFile, m_tilemap.GetCurrentMapIndex());
            m_elementsController.LoadElements(m_tilemap.GetElementsData(), m_tilemap);
            m_triggerController.SetTriggers(m_tilemap.GetTriggersData(), m_tilemap);
            break;
        }
        default:
            break;
    }
}

EditorMapScene::EditorMapScene(GameContext& context):
    TilemapScene(context, false), m_layerSelection(nullptr), m_selector(nullptr), m_lastLayer(-1), m_isFirstLoad(true) // m_lastLayer should be initialized with EditorMapEventState::selectedLayer ?
{
    m_context.eventController = std::make_unique<EditorMapEventController>(m_tileset, m_camera, m_tilemap);
    
    m_context.uiController.BuildUiFile("editor_scene.uif");
    m_layerSelection = m_context.uiComponentController.CreateDynamicList("layerSelection", "layer_selectable.uit");
    m_layerSelection->SetFirstItemParams(
        UiParams(m_context.uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Width, 0.5f)), Axis::Width, // Scale
        Anchor::LeftIn, Anchor::Center, // Anchor BottomIn ?
        m_context.uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Height, 0.2f)), // Padding
        m_context.uiController.GetResultFromPartialSize(PartialSize("frame", Axis::Height, 0.2f))));
    m_selector = m_context.uiComponentController.CreateSelector("selector", "selector.uit");
    
    UpdateTilemapLayer();

    m_drawables.push_back(&m_tileset);
    m_context.window.ShowCursor();
    // SoundController::GetInstance().DeleteBackgroundMusic(); // ?

    m_isLayerRendered.assign(m_layers.size(), true); // Do not use m_tilemap.GetLayerCount(), it doesn't include ExtraTileLayer
}


void EditorMapScene::UpdateTilemapLayer()
{    
    TilemapScene::UpdateTilemapLayer();

    TileLayer* borderLayer = new ExtraTileLayer(m_tilemap.GetLayerSize(), m_camera, m_context.textureController, m_tileset, 
        ExtraLayerType::LayerBorder, m_tilemap.GetOccupancyGrid()); // Try to remove occupancyGrid parameter (used only for ExtraLayerType::LayerCollision)
    m_layers.push_back(borderLayer);
    TileLayer* collisionLayer = new ExtraTileLayer(m_tilemap.GetLayerSize(), m_camera, m_context.textureController, m_tileset, 
        ExtraLayerType::LayerCollision, m_tilemap.GetOccupancyGrid());
    m_layers.push_back(collisionLayer);

    EditorMapEventController* eventController = static_cast<EditorMapEventController*>(m_context.eventController.get());
    eventController->SetLayerCount(m_tilemap.GetLayerCount());

    BuildUILayerSelection();
}

void EditorMapScene::BuildUILayerSelection()
{
    if (!m_isFirstLoad) {
        m_layerSelection->Close();
        // m_selector->Close(); // TODO : Not working, there is a problem with the key
    } else {
        m_isFirstLoad = false;
    }
    m_layerSelection->SetNrItem(m_layers.size()); // Do not use m_tilemap.GetLayerCount(), it doesn't count ExtraTileLayer
    m_layerSelection->Open();

    const std::vector<UiKey> keys = m_layerSelection->GetItemsKey();
    for (unsigned int i = 0 ; i < m_layers.size() ; i++) { // m_layers.size() = keys.size()
        std::string layerDisplay = std::to_string(i);
        
        if (const ExtraTileLayer* etl = dynamic_cast<const ExtraTileLayer*>(m_layers[i]))
            layerDisplay = etl->GetUiDisplay();

        if (i != m_layers.size()-1) layerDisplay += " | ";

        m_context.uiController.UpdateText(keys[i], layerDisplay);
    }
    m_selector->Open();
    m_selector->SetOptionKeys(keys, Axis::Height, Axis::Width, 1.f, -0.05f);
}

void EditorMapScene::Gameloop()
{
    m_context.window.ClearRenderer();
    
    m_context.eventController->PollAllEvents();
    m_gameloop = m_context.eventController->HandleWindowEvents();
    m_context.eventController->HandlePollEvents(); 
    // m_context.eventController->HandleStateEvents();

    m_camera.ComputeMapCulling(m_tilemap.GetLayerSize(), m_tileset.GetTileSize());
    
    EditorMapEventController* eventController = static_cast<EditorMapEventController*>(m_context.eventController.get());
    const EditorMapEventState eventState = eventController->GetEventState(); 
    
    for (unsigned int i = 0 ; i < m_layers.size() ; i++){ // Unlike GameMapScene, TileLayer are rendered all at once
        if (m_isLayerRendered[i])
            m_layers[i]->DrawTexture();
    }
    for (const Drawable* d : m_drawables) d->DrawTexture(); // Will be removed if m_tileset become a UiElement (drawed by UiController::Draw)
    
    if (eventState.selectedLayer != m_lastLayer){
        m_lastLayer = eventState.selectedLayer;
        m_context.uiController.UpdateText("editedLayerText", "Edited layer:" + std::to_string(m_lastLayer));
    } 

    if (m_selector->HorizontalNavigation(eventState.uiDirection, eventState.isAction)) {
        const unsigned int selectorIndex = m_selector->GetOptionIndex();
        m_isLayerRendered[selectorIndex] = !m_isLayerRendered[selectorIndex];     
    }

    SoundController::GetInstance().PlayRequestedChunk(); 
    
    m_context.uiController.Draw();
    m_context.window.UpdateRender();

    // Need to reset eventState.uiDirection to Direction::None and eventState.isAction to false
    eventController->Reset(); // Will be removed ?
}

BattleScene::BattleScene(GameContext& context):
    Scene(context), 
    m_battleController(m_context.fileReader, m_context.uiComponentController, m_context.uiController)
{
    m_camera.ComputeViewport(m_context.window, GridSize{16, 9}, 1); // Camera::m_screenOffset and Camera::m_viewport must be defined when drawing ui elements, but this line should not be here 
    // if I put camera in GameContext, I could avoid calling these setters ?
    m_context.uiController.SetSize(m_camera.GetViewport());
    m_context.uiController.SetPosition(m_camera.GetScreenOffset());
    
    m_context.eventController = std::make_unique<BattleEventController>();
   
    m_context.uiController.BuildUiFile("battle_scene.uif");
    SoundController::GetInstance().SetBackgroundMusic("battle.ogg"); // Background music will not be started from here

    m_battleController.InitializeActors("2p2ai_test");
    
    m_battleController.AddCallback([this](ExitEvent e){Exit(e);});
    m_context.window.HideCursor();
}

void BattleScene::Gameloop()
{
    m_context.time.Update();
    m_context.window.ClearRenderer();
    
    m_context.eventController->PollAllEvents();
    m_gameloop = m_context.eventController->HandleWindowEvents();
    m_context.eventController->HandlePollEvents();
    // m_context.eventController->HandleStateEvents();

    const BattleEventState eventState = static_cast<BattleEventController*>(m_context.eventController.get())->GetEventState();

    m_battleController.SetEventState(eventState);
    m_battleController.PlayNextTurn();

    m_context.uiController.Draw();
    m_context.window.DrawBoxing();
    m_context.window.UpdateRender();
}

void BattleScene::Exit(const ExitEvent e) {
    switch(e) {
        case ExitEvent::ExitWin : {
            Notify(SwitchEvent::ToGameMap);
            break;
        }
        case ExitEvent::ExitLost : {
            Notify(SwitchEvent::ToMainMenu);
            break;
        }
    }
}