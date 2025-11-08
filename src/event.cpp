#include "event.hpp"

EventController::EventController()
{

}

EventController::~EventController()
{
    
}

void EventController::PollAllEvents()
{
    m_events.clear();
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        m_events.push_back(event);
    }
}

int EventController::HandleWindowEvents() const
{
    for (SDL_Event event : m_events){
        switch (event.type){
            case SDL_QUIT:
                return -1;
            
            case SDL_KEYDOWN:
                SDL_Scancode event_scancode = event.key.keysym.scancode;
                if (event_scancode == SDL_SCANCODE_ESCAPE)
                    return -1;
                break;
        }
    }
    return 0;
}

// Should create EditorEventController and GameEventControllr that both inherit from EventController ?
// EditorEventController will have a current_selected_tile member
void EventController::HandleEditorEvent(Drawable* tileset, const Tilemap* tilemap) const
{
    for (SDL_Event event : m_events){
        switch (event.type){
            case SDL_KEYDOWN: {
                SDL_Scancode event_scancode = event.key.keysym.scancode;
                if (event_scancode == SDL_SCANCODE_SPACE)
                    tileset->InvertShouldDraw();
                    tileset->SetScreenPosition(GetMousePosition());
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT){
                    if (tileset->GetShouldDraw()){
                        // Should not be here
                        const ScreenPosition normalize_mouse_position = GetMousePosition()-tileset->GetScreenPosition();
                        const TilesetData data = tilemap->GetTilesetData(); // Should create a Tileset class instead of using Tilemap to get tileset_data
                        int tile_size = static_cast<int>(data.tile_size);
                        int c = normalize_mouse_position.x/tile_size;
                        int l = normalize_mouse_position.y/tile_size;
                        std::cout << c << ", " << l << "\n";
                    }else{
                        // Put a new tile on the current map
                    }
                }
                break;
        }
    }
}

MapPosition EventController::HandlePlayerEvent() const
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

ScreenPosition EventController::GetMousePosition() const
{
    ScreenPosition p;
    SDL_GetMouseState(&p.x, &p.y);
    return p;
}