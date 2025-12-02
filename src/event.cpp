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

MapEventController::MapEventController():
    EventController()
{

}

MapEventController::~MapEventController()
{

}

MapMovement MapEventController::HandlePlayerEvent() const
{
    MapMovement movement; // direction is None by default
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_W])
        movement.DefineMovement(MapDirection::Up);
    else if (state[SDL_SCANCODE_A])
        movement.DefineMovement(MapDirection::Left);
    else if (state[SDL_SCANCODE_S])
        movement.DefineMovement(MapDirection::Down);
    else if (state[SDL_SCANCODE_D])
        movement.DefineMovement(MapDirection::Right);
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
    return camera->GetCameraPosition()+GetMouseScreenPosition();
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
                        camera->ResetCameraPosition();
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
                    m_last_camera_origin = camera->GetCameraPosition() + GetMouseScreenPosition();
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
                    if (event.wheel.y != 0){
                        const int tileset_size = tileset->GetTilesetsSize();
                        if (event.wheel.y > 0) // Should use (event.wheel.y > 0) - (event.wheel.y < 0) to know direction ?
                            m_selected_tileset = (m_selected_tileset+1)%tileset_size;
                        else
                            m_selected_tileset = (m_selected_tileset-1+tileset_size)%tileset_size;
                    }
                    tileset->SetDisplayedTileset(m_selected_tileset);
                }
                break;
        }
    }

    if (m_is_camera_moving){
        const ScreenPosition mouse_position = GetMouseScreenPosition();
        camera->SetCameraPosition(m_last_camera_origin-mouse_position);
    } else if (m_is_replacing_tile){
        const ScenePosition norm_scene_pos = GetMouseScenePosition(camera)-tilemap->GetScenePosition();
        tilemap->ReplaceTileAt(norm_scene_pos, m_selected_tile);
    }
}