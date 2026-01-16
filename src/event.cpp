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
    m_axis_x = SDL_JoystickGetAxis(m_joystick, 0);
    m_axis_y = SDL_JoystickGetAxis(m_joystick, 1);
}

bool JoystickActionController::IsLeftAction()
{
    return m_axis_x < -JOYSTICK_DEAD_ZONE;
}

bool JoystickActionController::IsRightAction()
{
    return m_axis_x > JOYSTICK_DEAD_ZONE;
}

bool JoystickActionController::IsUpAction()
{
    return m_axis_y < -JOYSTICK_DEAD_ZONE;
}

bool JoystickActionController::IsDownAction()
{
    return m_axis_y > JOYSTICK_DEAD_ZONE;
}

EventController::EventController()
{

}

bool EventController::HandleWindowEvents() const
{
    for (SDL_Event event : m_events){
        switch (event.type){
            case SDL_QUIT:
                return false;
            
            case SDL_KEYDOWN:
                SDL_Scancode event_scancode = event.key.keysym.scancode;
                if (event_scancode == SDL_SCANCODE_ESCAPE)
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
            m_action_controller = new JoystickActionController();
        else 
            m_action_controller = new KeyboardActionController();
    }else{
        std::cout << "Unable to initialize joystick system\n"; // Throw error ? or use KeyboardActionController
    }   
}

void GameplayEventController::HandleEvents()
{
    m_action_controller->GetActions();
    if (m_action_controller->IsLeftAction())
        m_event_direction = MapDirection::Left;
    else if (m_action_controller->IsRightAction())
        m_event_direction = MapDirection::Right;
    else if (m_action_controller->IsUpAction())
        m_event_direction = MapDirection::Up;
    else if (m_action_controller->IsDownAction())
        m_event_direction = MapDirection::Down;
    else
        m_event_direction = MapDirection::None;
}

EditorEventController::EditorEventController(Tileset& tileset, Camera& camera, Tilemap& tilemap):
    m_tileset(tileset), m_tilemap(tilemap), m_camera(camera), m_layer_count(m_tilemap.GetLayerCount())
{
    m_selected_tile = 0;
    m_selected_tileset = 0;
    m_selected_layer = 0;
    m_tileset.SetDisplayedTileset(m_selected_tileset);
    m_is_camera_moving = false;
    m_is_replacing_tile = false;
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
    return m_selected_layer;
}

void EditorEventController::HandleEvents()
{
    for (SDL_Event event : m_events){
        switch (event.type){
            case SDL_KEYDOWN: {
                if (event.key.repeat != 0) break;
                SDL_Scancode event_scancode = event.key.keysym.scancode;

                switch(event_scancode){
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
                        m_selected_layer = (m_selected_layer+1)%m_layer_count;
                        break;
                    case SDL_SCANCODE_S:
                        m_selected_layer = (m_selected_layer-1+m_layer_count)%m_layer_count;
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
                m_tileset.SetDisplayedTileset(m_selected_tileset);
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT){
                    if (m_tileset.GetShouldDraw()){
                        const ScreenPosition norm_screen_pos = (GetMouseScreenPosition()-m_tileset.GetScreenPosition())/m_tileset.GetZoom();
                        m_tileset.UpdateSelectedTile(norm_screen_pos, m_selected_tileset, m_selected_tile);
                    }else{
                        m_is_replacing_tile = true;
                    }
                }else if (event.button.button == SDL_BUTTON_MIDDLE){
                    m_last_camera_origin = m_camera.GetPosition() + GetMouseScreenPosition(); // Do not use the zoom here
                    m_is_camera_moving = true;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_MIDDLE)
                    m_is_camera_moving = false;
                else if (event.button.button == SDL_BUTTON_LEFT)
                    m_is_replacing_tile = false;
                break;
            case SDL_MOUSEWHEEL:
                if (m_tileset.GetShouldDraw()){
                    const int tileset_size = m_tileset.GetTilesetsSize();
                    if (event.wheel.y > 0) // Should use (event.wheel.y > 0) - (event.wheel.y < 0) to know direction ?
                        m_selected_tileset = (m_selected_tileset+1)%tileset_size;
                    else
                        m_selected_tileset = (m_selected_tileset-1+tileset_size)%tileset_size;
                    m_tileset.SetDisplayedTileset(m_selected_tileset);
                }else{ // If the tileset is not opened, the mouse wheel control the camera zoom (also )
                    const ScenePosition mouse_before_zoom = GetMouseScenePosition();
                    if (event.wheel.y > 0)
                        m_camera.AddZoom(0.1f);
                    else
                        m_camera.AddZoom(-0.1f);
                    const ScenePosition mouse_after_zoom = GetMouseScenePosition();
                    const ScenePosition delta = (mouse_before_zoom-mouse_after_zoom)*m_camera.GetZoom();
                    m_camera.MoveCameraPosition(delta); // Will zoom on mouse cursor
                }
                break;
        }
    }

    if (m_is_camera_moving){
        const ScreenPosition mouse_position = GetMouseScreenPosition();
        m_camera.SetCameraPosition(m_last_camera_origin-mouse_position);
    } else if (m_is_replacing_tile){
        const ScenePosition norm_scene_pos = GetMouseScenePosition();
        m_tilemap.ReplaceTileAt(norm_scene_pos, m_selected_layer, m_selected_tile);
    }
}