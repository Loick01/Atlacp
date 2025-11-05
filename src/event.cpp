#include "event.hpp"

EventController::EventController()
{

}

EventController::~EventController()
{

}

int EventController::HandleWindowEvents()
{
    while (SDL_PollEvent(&m_event) > 0){
        switch (m_event.type){
            case SDL_QUIT:
                return -1;
            case SDL_KEYDOWN:
                if (m_event.key.keysym.sym == SDLK_ESCAPE)
                    return -1;
        }
    }
    return 0;
}