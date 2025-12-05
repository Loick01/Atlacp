#include "event.hpp"

EventController::EventController()
{

}

EventController::~EventController()
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

GameplayEventController::GameplayEventController():
    EventController(), m_joystick(nullptr)
{
    // Controller can be use only for gameplay mode
    const int joystick = SDL_Init(SDL_INIT_JOYSTICK);
    if (joystick==0){
        if (SDL_NumJoysticks() != 0){
            m_joystick = SDL_JoystickOpen(0);
            if (m_joystick == nullptr)
                std::cout << "Error while opening the joystick: " << SDL_GetError() << "\n";
            else
                std::cout << "Joystick connected\n";
        }else{
            std::cout << "No joystick connected\n";
        }
    }
    else{
        std::cout << "Unable to initialize joystick system\n";
    }
}

GameplayEventController::~GameplayEventController()
{

}

MapMovement GameplayEventController::HandlePlayerEvent() const
{
    // Will find better solution than just testing is_joystick_connected
    MapMovement movement; // direction is None by default
    if (m_joystick != nullptr){
        /* // Remove ?
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
        const int axis_x = SDL_JoystickGetAxis(m_joystick, 0);
        const int axis_y = SDL_JoystickGetAxis(m_joystick, 1);
        if (std::abs(axis_x) > JOYSTICK_DEAD_ZONE){
            if (axis_x < 0)
                movement.DefineMovement(MapDirection::Left);
            else
                movement.DefineMovement(MapDirection::Right);
        }else if (std::abs(axis_y) > JOYSTICK_DEAD_ZONE){
            if (axis_y < 0)
                movement.DefineMovement(MapDirection::Up);
            else
                movement.DefineMovement(MapDirection::Down);
        }
    }else{
        const Uint8* state = SDL_GetKeyboardState(NULL);
        if (state[SDL_SCANCODE_W])
            movement.DefineMovement(MapDirection::Up);
        else if (state[SDL_SCANCODE_A])
            movement.DefineMovement(MapDirection::Left);
        else if (state[SDL_SCANCODE_S])
            movement.DefineMovement(MapDirection::Down);
        else if (state[SDL_SCANCODE_D])
            movement.DefineMovement(MapDirection::Right);
        }
    return movement;
}

EditorEventController::EditorEventController(Tileset* tileset):
    EventController(), m_tileset(tileset)
{
    m_selected_tile = 0;
    m_selected_tileset = 0;
    m_tileset->SetDisplayedTileset(m_selected_tileset);
    m_is_camera_moving = false;
    m_is_replacing_tile = false;
}

EditorEventController::~EditorEventController()
{

}

ScreenPosition EditorEventController::GetMouseScreenPosition() const
{
    ScreenPosition sp;
    SDL_GetMouseState(&sp.x, &sp.y);
    return sp;
}

ScenePosition EditorEventController::GetMouseScenePosition(const Camera* camera) const
{
    return (camera->GetCameraPosition()+GetMouseScreenPosition())/camera->GetZoom();
}

void EditorEventController::HandleEditorEvent(Tileset* tileset, Tilemap* tilemap, Camera* camera)
{
    for (SDL_Event event : m_events){
        switch (event.type){
            case SDL_KEYDOWN: {
                if (event.key.repeat != 0) break;
                SDL_Scancode event_scancode = event.key.keysym.scancode;

                switch(event_scancode){
                    case SDL_SCANCODE_SPACE:
                        tileset->InvertShouldDraw();
                        tileset->SetScreenPosition(GetMouseScreenPosition());
                        break;
                    case SDL_SCANCODE_L:
                        tilemap->SaveMap("ff_map");
                        std::cout << "Map saved in assets/maps/ff_map\n";
                        break;
                    case SDL_SCANCODE_R:
                        camera->Reset();
                        break;
                    case SDL_SCANCODE_UP:
                        tilemap->LoadAdjacentMap(MapBound::OutUp);
                        break;
                    case SDL_SCANCODE_DOWN:
                        tilemap->LoadAdjacentMap(MapBound::OutDown);
                        break;
                    case SDL_SCANCODE_RIGHT:
                        tilemap->LoadAdjacentMap(MapBound::OutRight);
                        break;
                    case SDL_SCANCODE_LEFT:
                        tilemap->LoadAdjacentMap(MapBound::OutLeft);
                        break;
                    default:
                        break;
                }
                tileset->SetDisplayedTileset(m_selected_tileset);
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT){
                    if (tileset->GetShouldDraw()){
                        const ScreenPosition norm_screen_pos = GetMouseScreenPosition()-tileset->GetScreenPosition();
                        tileset->UpdateSelectedTile(norm_screen_pos, m_selected_tileset, m_selected_tile);
                    }else{
                        m_is_replacing_tile = true;
                    }
                }else if (event.button.button == SDL_BUTTON_MIDDLE){
                    m_last_camera_origin = GetMouseScreenPosition();
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
                if (tileset->GetShouldDraw()){
                    const int tileset_size = tileset->GetTilesetsSize();
                    if (event.wheel.y > 0) // Should use (event.wheel.y > 0) - (event.wheel.y < 0) to know direction ?
                        m_selected_tileset = (m_selected_tileset+1)%tileset_size;
                    else
                        m_selected_tileset = (m_selected_tileset-1+tileset_size)%tileset_size;
                    tileset->SetDisplayedTileset(m_selected_tileset);
                }else{ // If the tileset is not opened, the mouse control the camera zoom
                    const ScenePosition mouse_before_zoom = GetMouseScenePosition(camera);
                    if (event.wheel.y > 0)
                        camera->AddZoom(0.05f);
                    else
                        camera->AddZoom(-0.05f);
                    const ScenePosition mouse_after_zoom = GetMouseScenePosition(camera);
                    camera->MoveCameraPosition(mouse_before_zoom-mouse_after_zoom); // Will zoom on mouse cursor
                }
                break;
        }
    }

    if (m_is_camera_moving){
        const ScreenPosition mouse_position = GetMouseScreenPosition();
        const ScreenPosition new_camera_position = m_last_camera_origin-mouse_position;
        camera->SetCameraPosition(ScenePosition{new_camera_position.x, new_camera_position.y});
    } else if (m_is_replacing_tile){
        const ScenePosition norm_scene_pos = GetMouseScenePosition(camera)-tilemap->GetScenePosition();
        tilemap->ReplaceTileAt(norm_scene_pos, m_selected_tile);
    }
}