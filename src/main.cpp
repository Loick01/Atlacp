#include <iostream>

#include "event.hpp"
#include "image.hpp"
#include "window.hpp"

int main(){
    
    Window* window = new Window("Atlacp", 1600, 900);
    if (window->HasError()){
        return -1;
    }
    EventController* events = new EventController();
    ImageController* images = new ImageController();
    bool gameloop = true;

    while(gameloop){
        if (events->HandleEvents()==-1) gameloop=false;
        images->Draw(window->GetSurface());
        window->UpdateSurface();
    }

    delete images;
    delete events;
    delete window;
    return 0;
}