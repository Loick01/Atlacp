#include <iostream>

#include "event.hpp"
#include "texture.hpp"
#include "window.hpp"

int main(){
    Window* window = new Window("Atlacp", 1600, 900);
    if (window->HasError()){
        return -1;
    }
    EventController* events = new EventController();
    TextureController* textures = new TextureController();
    bool gameloop = true;
    SDL_Rect image_position{100, 50, 0, 0}; // For a tile, this SDL_Rect for position will be in a struct with its SDL_Surface

    while(gameloop){
        if (events->HandleEvents()==-1) gameloop=false;
        textures->DrawTexture("../cpp.bmp", window->GetSurface(), image_position);
        window->UpdateSurface();
    }

    delete textures;
    delete events;
    delete window;
    return 0;
}