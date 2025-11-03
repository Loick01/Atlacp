#include <iostream>
#include <vector>

#include "event.hpp"
#include "texture.hpp"
#include "window.hpp"
#include "file.hpp"
#include "tilemap.hpp"

int main(){
    Window* window = new Window("Atlacp", 1600, 900, {100,100,100});
    if (window->HasError()){
        return -1;
    }
    EventController* events = new EventController();
    TextureController* texture_controller = new TextureController(window->GetRenderer());
    FileReader* file_reader = new FileReader();
    Tilemap* tilemap = new Tilemap(file_reader, "../map.txt", "../tileset.png");
    
    bool gameloop = true;
    while(gameloop){
        if (events->HandleEvents()==-1) gameloop=false;
        tilemap->DrawMap(texture_controller);
        window->UpdateRender();        
    }

    delete tilemap;
    delete file_reader;
    delete texture_controller;
    delete events;
    delete window;
    return 0;
}