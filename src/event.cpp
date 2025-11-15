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

MapPosition MapEventController::HandlePlayerEvent() const
{
    MapPosition pos{0, 0};
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_W])
        pos.y = -1; 
    if (state[SDL_SCANCODE_A])
        pos.x = -1;
    if (state[SDL_SCANCODE_S])
        pos.y = 1;
    if (state[SDL_SCANCODE_D])
        pos.x = 1;
    return pos;
}

EditorEventController::EditorEventController(Tileset* tileset):
    EventController(), m_tileset(tileset)
{
    m_selected_tile = 0;
    m_selected_tileset = 0;
    m_tileset->SetDisplayedTileset(m_selected_tileset);
}

EditorEventController::~EditorEventController()
{

}

ScreenPosition EditorEventController::GetMousePosition() const
{
    ScreenPosition p;
    SDL_GetMouseState(&p.x, &p.y);
    return p;
}

void EditorEventController::HandleEditorEvent(Tileset* tileset, Tilemap* tilemap)
{
    for (SDL_Event event : m_events){
        switch (event.type){
            case SDL_KEYDOWN: {
                if (event.key.repeat != 0) break;
                SDL_Scancode event_scancode = event.key.keysym.scancode;

                switch(event_scancode){
                    case SDL_SCANCODE_SPACE:
                        tileset->InvertShouldDraw();
                        tileset->SetScreenPosition(GetMousePosition());
                        break;
                    case SDL_SCANCODE_S:
                        tilemap->SaveMap("../map2.txt");
                        std::cout << "Map saved in map2.txt\n";
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
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT){
                    if (tileset->GetShouldDraw()){
                        const ScreenPosition normalized_mouse_position = GetMousePosition()-tileset->GetScreenPosition();
                        tileset->UpdateSelectedTile(normalized_mouse_position, m_selected_tileset, m_selected_tile);
                    }else{
                        const ScreenPosition normalized_mouse_position = GetMousePosition()-tilemap->GetScreenPosition();
                        tilemap->ReplaceTileAt(normalized_mouse_position, m_selected_tile);
                    }
                }
                break;
            case SDL_MOUSEWHEEL:
                if (tileset->GetShouldDraw()){
                    if (event.wheel.y > 0){
                        m_selected_tileset = ++m_selected_tileset%tileset->GetTilesetsSize();
                        tileset->SetDisplayedTileset(m_selected_tileset);
                    }
                }
                break;
        }
    }
}