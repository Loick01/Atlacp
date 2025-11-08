#include <iostream>
#include <vector>

#include "event.hpp"
#include "file.hpp"
#include "player.hpp"
#include "texture.hpp"
#include "tilemap.hpp"
#include "window.hpp"

int main(){
    Window* window = new Window("Map Editor - Atlacp", 1600, 900, {100,100,100});
    if (window->HasError()){
        return -1;
    }
    EventController* event_controller = new EventController();
    TextureController* texture_controller = new TextureController(window->GetRenderer());
    FileReader* file_reader = new FileReader();
    Tilemap* tilemap = new Tilemap(texture_controller, file_reader, "../map.txt", "../tileset.png");
    Drawable* tileset = new Drawable(texture_controller, "../tileset.png", {0,0}, false);
    std::vector<Drawable*> drawables = {tilemap, tileset}; // Rendering order must be respected

    bool gameloop = true;
    while(gameloop){
        window->ClearRenderer();
        event_controller->PollAllEvents();
        
        if (event_controller->HandleWindowEvents()==-1) gameloop=false;
        event_controller->HandleEditorEvent(tileset, tilemap);

        for (const Drawable* d : drawables) d->DrawTexture();
        
        window->UpdateRender();     
        //SDL_Delay(48);
    }

    delete tilemap;
    delete file_reader;
    delete event_controller;
    delete window;
    delete texture_controller; // This instance must be delete after deleting every Drawable instance 
    return 0;
}