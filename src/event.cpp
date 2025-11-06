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

Position EventController::HandlePlayerEvent() const
{
    Position pos{0, 0};
    for (SDL_Event event : m_events){
        switch (event.type){
            case SDL_KEYDOWN:
                SDL_Scancode event_scancode = event.key.keysym.scancode;
                if (event_scancode == SDL_SCANCODE_W)
                    pos.y = -1; 
                if (event_scancode == SDL_SCANCODE_A)
                    pos.x = -1;
                if (event_scancode == SDL_SCANCODE_S)
                    pos.y = 1;
                if (event_scancode == SDL_SCANCODE_D)
                    pos.x = 1;
                break;
        }
    }
    return pos;
}