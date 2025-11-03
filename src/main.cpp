#include <iostream>

#include "event.hpp"
#include "texture.hpp"
#include "window.hpp"

int main(){
    Window* window = new Window("Atlacp", 1600, 900, {100,100,100});
    if (window->HasError()){
        return -1;
    }
    EventController* events = new EventController();
    TextureController* textures = new TextureController(window->GetRenderer());
    bool gameloop = true;
    // For a tile, this SDL_Rect for position will be in a struct with its SDL_Texture
    const SDL_Rect image_position{100,50,212,238};

    while(gameloop){
        if (events->HandleEvents()==-1) gameloop=false;
        textures->RenderTexture("../cpp.bmp", image_position);
        window->UpdateRender();        
    }

    delete textures;
    delete events;
    delete window;
    return 0;
}