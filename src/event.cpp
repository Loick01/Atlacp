#include "event.hpp"

void KeyboardActionController::GetActions()
{
    m_state = SDL_GetKeyboardState(NULL);
}

bool KeyboardActionController::IsLeftAction()
{
    return m_state[SDL_SCANCODE_A];
}

bool KeyboardActionController::IsRightAction()
{
    return m_state[SDL_SCANCODE_D];
}

bool KeyboardActionController::IsUpAction()
{
    return m_state[SDL_SCANCODE_W];
}

bool KeyboardActionController::IsDownAction()
{
    return m_state[SDL_SCANCODE_S];
}

JoystickActionController::JoystickActionController() :
    m_joystick(nullptr)
{
    m_joystick = SDL_JoystickOpen(0);
    if (m_joystick == nullptr)
        std::cout << "Error while opening the joystick: " << SDL_GetError() << "\n"; // Will throw error
    else
        std::cout << "Joystick connected\n";
}

/* If I need to know when the joystick is removed

for (SDL_Event event : m_events){
    switch (event.type){
        case SDL_JOYDEVICEREMOVED: {
            //m_joystick = nullptr;
            std::cout << "Joystick with index " << event.jdevice.which << " was removed.\n";
            break;
        }
    }
}
*/

void JoystickActionController::GetActions()
{
    m_axisX = SDL_JoystickGetAxis(m_joystick, 0);
    m_axisY = SDL_JoystickGetAxis(m_joystick, 1);
}

bool JoystickActionController::IsLeftAction()
{
    return m_axisX < -JOYSTICK_DEAD_ZONE;
}

bool JoystickActionController::IsRightAction()
{
    return m_axisX > JOYSTICK_DEAD_ZONE;
}

bool JoystickActionController::IsUpAction()
{
    return m_axisY < -JOYSTICK_DEAD_ZONE;
}

bool JoystickActionController::IsDownAction()
{
    return m_axisY > JOYSTICK_DEAD_ZONE;
}

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

// Will be remove
void EventController::HandleEvents()
{

}

GameplayEventController::GameplayEventController():
    EventController()
{
    const int joystick = SDL_Init(SDL_INIT_JOYSTICK);
    if (joystick==0){
        if (SDL_NumJoysticks() != 0)
            m_actionController = new JoystickActionController();
        else 
            m_actionController = new KeyboardActionController();
    }else{
        std::cout << "Unable to initialize joystick system\n"; // Throw error ? or use KeyboardActionController
    }   
}

void GameplayEventController::HandleEvents()
{
    m_actionController->GetActions();
    if (m_actionController->IsLeftAction())
        m_eventDirection = MapDirection::Left;
    else if (m_actionController->IsRightAction())
        m_eventDirection = MapDirection::Right;
    else if (m_actionController->IsUpAction())
        m_eventDirection = MapDirection::Up;
    else if (m_actionController->IsDownAction())
        m_eventDirection = MapDirection::Down;
    else
        m_eventDirection = MapDirection::None;
}

EditorEventController::EditorEventController(Tileset& tileset, Camera& camera, Tilemap& tilemap):
    m_tileset(tileset), m_tilemap(tilemap), m_camera(camera), m_layerCount(m_tilemap.GetLayerCount())
{
    m_selectedTile = 0;
    m_selectedTileset = 0;
    m_selectedLayer = 0;
    m_tileset.SetDisplayedTileset(m_selectedTileset);
    m_isCameraMoving = false;
    m_isReplacingTile = false;
}

ScreenPosition EditorEventController::GetMouseScreenPosition() const
{
    ScreenPosition sp;
    SDL_GetMouseState(&sp.x, &sp.y);
    return sp;
}

ScenePosition EditorEventController::GetMouseScenePosition() const
{
    return (m_camera.GetPosition()-m_camera.GetScreenOffset()+GetMouseScreenPosition())/m_camera.GetZoom();
}

int EditorEventController::GetSelectedLayer() const
{
    return m_selectedLayer;
}

void EditorEventController::HandleEvents()
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
                        std::cin >> savefile;
                        m_tilemap.SaveMap(savefile);
                        std::cout << "Map saved in assets/maps/" << savefile << "\n";
                        break;
                    }
                    case SDL_SCANCODE_W:
                        m_selectedLayer = (m_selectedLayer+1)%m_layerCount;
                        break;
                    case SDL_SCANCODE_S:
                        m_selectedLayer = (m_selectedLayer-1+m_layerCount)%m_layerCount;
                        break;
                    case SDL_SCANCODE_R:
                        m_camera.Reset();
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
                m_tileset.SetDisplayedTileset(m_selectedTileset);
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT){
                    if (m_tileset.GetShouldDraw()){
                        const ScreenPosition normScreenPos = (GetMouseScreenPosition()-m_tileset.GetScreenPosition())/m_tileset.GetZoom();
                        m_tileset.UpdateSelectedTile(normScreenPos, m_selectedTileset, m_selectedTile);
                    }else{
                        m_isReplacingTile = true;
                    }
                }else if (event.button.button == SDL_BUTTON_MIDDLE){
                    m_lastCameraOrigin = m_camera.GetPosition() + GetMouseScreenPosition(); // Do not use the zoom here
                    m_isCameraMoving = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_MIDDLE)
                    m_isCameraMoving = false;
                else if (event.button.button == SDL_BUTTON_LEFT)
                    m_isReplacingTile = false;
                break;
            case SDL_MOUSEWHEEL:
                if (m_tileset.GetShouldDraw()){
                    const int tilesetSize = m_tileset.GetTilesetsSize();
                    if (event.wheel.y > 0) // Should use (event.wheel.y > 0) - (event.wheel.y < 0) to know direction ?
                        m_selectedTileset = (m_selectedTileset+1)%tilesetSize;
                    else
                        m_selectedTileset = (m_selectedTileset-1+tilesetSize)%tilesetSize;
                    m_tileset.SetDisplayedTileset(m_selectedTileset);
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

    if (m_isCameraMoving){
        const ScreenPosition mousePosition = GetMouseScreenPosition();
        m_camera.SetCameraPosition(m_lastCameraOrigin-mousePosition);
    } else if (m_isReplacingTile){
        const ScenePosition normScenePos = GetMouseScenePosition();
        m_tilemap.ReplaceTileAt(normScenePos, m_selectedLayer, m_selectedTile);
    }
}