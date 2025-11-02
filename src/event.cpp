#include "event.hpp"

EventController::EventController()
{

}

EventController::~EventController()
{

}

int EventController::HandleEvents()
{
    while (SDL_PollEvent(&m_event) > 0){
        switch (m_event.type){
            case SDL_QUIT:
                return -1;
        }
    }
    return 0;
}