#include "event/event.hpp"

// Because of EditorMapEventController
#include "core/camera.hpp"
#include "tile/tilemap.hpp"
#include "tile/tileset.hpp"

bool EventController::HandleWindowEvents() const
{
    for (SDL_Event event : m_events){
        switch (event.type){
            case SDL_QUIT:
                return false;
            
            case SDL_KEYDOWN:
                SDL_Scancode eventScancode = event.key.keysym.scancode;
                if (eventScancode == SDL_SCANCODE_ESCAPE)
                    return false;
                break;
        }
    }
    return true;
}

void EventController::PollAllEvents()
{
    m_events.clear();
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        m_events.push_back(event);
    }
}

MainMenuEventController::MainMenuEventController() 
{
    if (JoystickActionController::IsJoystickAvailable())
        m_actionController = std::make_unique<JoystickActionController>();
    else
        m_actionController = std::make_unique<KeyboardActionController>();
}

void MainMenuEventController::HandleStateEvents()
{}

void MainMenuEventController::HandlePollEvents()
{
    for (SDL_Event event : m_events){
        if (m_actionController->IsPressedPoll(event)) {
            if (m_actionController->IsPrimaryActionPoll(event)) {
                m_eventState.isAction = true;
                return;
            }
        } 
        if (m_actionController->IsMotionPoll(event)) {
            if (m_actionController->IsUpActionPoll(event)) {
                m_eventState.uiDirection = Direction::Up;
                return;
            } else if (m_actionController->IsDownActionPoll(event)) {
                m_eventState.uiDirection = Direction::Down;
                return;
            } else if (m_actionController->IsRightActionPoll(event)) {
                m_eventState.uiDirection = Direction::Right;
                return;
            } else if (m_actionController->IsLeftActionPoll(event)) {
                m_eventState.uiDirection = Direction::Left;
                return;
            }
        }
    }
    m_eventState.isAction = false;
    m_eventState.uiDirection = Direction::None;
}

GameMapEventController::GameMapEventController():
    EventController()
{
    if (JoystickActionController::IsJoystickAvailable())
        m_actionController = std::make_unique<JoystickActionController>();
    else
        m_actionController = std::make_unique<KeyboardActionController>();
}

void GameMapEventController::HandleStateEvents()
{
    m_actionController->GetStateActions();

    m_eventState.isRunning = m_actionController->IsSecondaryActionState();
    if (m_actionController->IsLeftActionState())
        m_eventState.mapDirection = Direction::Left;
    else if (m_actionController->IsRightActionState())
        m_eventState.mapDirection = Direction::Right;
    else if (m_actionController->IsUpActionState())
        m_eventState.mapDirection = Direction::Up;
    else if (m_actionController->IsDownActionState())
        m_eventState.mapDirection = Direction::Down;
    else
        m_eventState.mapDirection = Direction::None;
}

void GameMapEventController::HandlePollEvents()
{
    for (SDL_Event event : m_events){
        if (m_actionController->IsPressedPoll(event)) {
            if (m_actionController->IsPrimaryActionPoll(event)) {
                m_eventState.isInteracting = true;
                return;
            }
        }
    }
    m_eventState.isInteracting = false;
}

EditorMapEventController::EditorMapEventController(Tileset& tileset, Camera& camera, Tilemap& tilemap):
    m_tileset(tileset), m_tilemap(tilemap), m_camera(camera)
{
    m_tileset.SetDisplayedTileset(m_eventState.selectedTileset);
}

ScreenPosition EditorMapEventController::GetMouseScreenPosition() const
{
    ScreenPosition sp;
    SDL_GetMouseState(&sp.x, &sp.y);
    return sp;
}

ScenePosition EditorMapEventController::GetMouseScenePosition() const
{
    return (m_camera.GetPosition()-m_camera.GetScreenOffset()+GetMouseScreenPosition())/m_camera.GetZoom();
}

void EditorMapEventController::HandleStateEvents()
{}

void EditorMapEventController::HandlePollEvents()
{
    for (SDL_Event event : m_events){
        switch (event.type){
            case SDL_KEYDOWN: {
                if (event.key.repeat != 0) break;
                SDL_Scancode eventScancode = event.key.keysym.scancode;

                switch(eventScancode){
                    case SDL_SCANCODE_SPACE:
                        m_tileset.InvertShouldDraw();
                        m_tileset.SetScreenPosition(GetMouseScreenPosition());
                        break;
                    case SDL_SCANCODE_L:
                    {
                        std::string savefile;
                        std::cout << "Enter the file name : \n";
                        std::cin >> savefile;
                        m_tilemap.SaveMap(savefile);
                        std::cout << "Map saved as " << savefile << "\n"; // Will be removed (avoid cout, should be displayed in the Ui frame)
                        break;
                    }
                    case SDL_SCANCODE_W:
                        m_eventState.selectedLayer = (m_eventState.selectedLayer+1)%m_layerCount;
                        break;
                    case SDL_SCANCODE_S:
                        m_eventState.selectedLayer = (m_eventState.selectedLayer-1+m_layerCount)%m_layerCount;
                        break;
                    case SDL_SCANCODE_A:
                        m_eventState.uiDirection = Direction::Left;
                        break;
                    case SDL_SCANCODE_D:
                        m_eventState.uiDirection = Direction::Right;
                        break;
                    case SDL_SCANCODE_T:
                        m_eventState.isAction = true;
                        break;
                    case SDL_SCANCODE_R:
                        m_camera.ResetPositionAndZoom();
                        break;
                    case SDL_SCANCODE_UP:
                        m_tilemap.LoadAdjacentMap(MapBound::OutUp);
                        break;
                    case SDL_SCANCODE_DOWN:
                        m_tilemap.LoadAdjacentMap(MapBound::OutDown);
                        break;
                    case SDL_SCANCODE_RIGHT:
                        m_tilemap.LoadAdjacentMap(MapBound::OutRight);
                        break;
                    case SDL_SCANCODE_LEFT:
                        m_tilemap.LoadAdjacentMap(MapBound::OutLeft);
                        break;
                    default:
                        break;
                }
                m_tileset.SetDisplayedTileset(m_eventState.selectedTileset);
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT){
                    if (m_tileset.GetShouldDraw()){
                        const ScreenPosition normScreenPos = (GetMouseScreenPosition()-m_tileset.GetScreenPosition())/m_tileset.GetZoom();
                        m_tileset.UpdateSelectedTile(normScreenPos, m_eventState.selectedTileset, m_eventState.selectedTile);
                    }else{
                        m_eventState.isReplacingTile = true;
                    }
                }else if (event.button.button == SDL_BUTTON_MIDDLE){
                    const ScreenPosition mousePosition = GetMouseScreenPosition();
                    if (m_tileset.GetShouldDraw()) {
                        // Do not m_tileset.SetScreenPosition on mousePosition here
                        m_lastKnownPosition = m_tileset.GetScreenPosition() - mousePosition;
                    } else {
                        m_lastKnownPosition = m_camera.GetPosition() + mousePosition; // Do not use the zoom here
                    }
                    m_eventState.isCameraMoving = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_MIDDLE)
                    m_eventState.isCameraMoving = false;
                else if (event.button.button == SDL_BUTTON_LEFT)
                    m_eventState.isReplacingTile = false;
                break;
            case SDL_MOUSEWHEEL:
                if (m_tileset.GetShouldDraw()){
                    const int tilesetSize = m_tileset.GetTilesetsSize();
                    if (event.wheel.y > 0) // Should use (event.wheel.y > 0) - (event.wheel.y < 0) to know direction ?
                        m_eventState.selectedTileset = (m_eventState.selectedTileset+1)%tilesetSize;
                    else
                        m_eventState.selectedTileset = (m_eventState.selectedTileset-1+tilesetSize)%tilesetSize;
                    m_tileset.SetDisplayedTileset(m_eventState.selectedTileset);
                }else{ // If the tileset is not opened, the mouse wheel control the camera zoom (also )
                    const ScenePosition mouseBeforeZoom = GetMouseScenePosition();
                    if (event.wheel.y > 0)
                        m_camera.AddZoom(0.1f);
                    else
                        m_camera.AddZoom(-0.1f);
                    const ScenePosition mouseAfterZoom = GetMouseScenePosition();
                    const ScenePosition delta = (mouseBeforeZoom-mouseAfterZoom)*m_camera.GetZoom();
                    m_camera.MoveCameraPosition(delta); // Will zoom on mouse cursor
                }
                break;
        }
    }

    if (m_eventState.isCameraMoving){
        if (m_tileset.GetShouldDraw()) {
            const ScreenPosition mousePosition = GetMouseScreenPosition();
            m_tileset.SetScreenPosition(m_lastKnownPosition+mousePosition);
        } else {
            const ScreenPosition mousePosition = GetMouseScreenPosition();
            m_camera.SetCameraPosition(m_lastKnownPosition-mousePosition);
        }
    } else if (m_eventState.isReplacingTile){
        const ScenePosition normScenePos = GetMouseScenePosition();
        m_tilemap.ReplaceTileAt(normScenePos, m_eventState.selectedLayer, m_eventState.selectedTile);
    }
}

void EditorMapEventController::SetLayerCount(const unsigned int layerCount)
{
    m_eventState.selectedLayer = 0;
    m_layerCount = layerCount;
}

void EditorMapEventController::Reset()
{
    m_eventState.uiDirection = Direction::None;
    m_eventState.isAction = false;
}

BattleEventController::BattleEventController() 
{
    if (JoystickActionController::IsJoystickAvailable())
        m_actionController = std::make_unique<JoystickActionController>();
    else
        m_actionController = std::make_unique<KeyboardActionController>();
}

void BattleEventController::HandleStateEvents()
{}

void BattleEventController::HandlePollEvents()
{
    for (SDL_Event event : m_events){
        if (m_actionController->IsPressedPoll(event)) {
            if (m_actionController->IsPrimaryActionPoll(event)) {
                m_eventState.isAction = true;
                return;
            }
        } 
        if (m_actionController->IsMotionPoll(event)) {
            if (m_actionController->IsUpActionPoll(event)) {
                m_eventState.uiDirection = Direction::Up;
                return;
            } else if (m_actionController->IsDownActionPoll(event)) {
                m_eventState.uiDirection = Direction::Down;
                return;
            } else if (m_actionController->IsRightActionPoll(event)) {
                m_eventState.uiDirection = Direction::Right;
                return;
            } else if (m_actionController->IsLeftActionPoll(event)) {
                m_eventState.uiDirection = Direction::Left;
                return;
            }
        }
    }
    m_eventState.isAction = false;
    m_eventState.uiDirection = Direction::None;
}