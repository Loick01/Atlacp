#include <iostream>
#include <vector>
#include <list>
#include <set>

#include "event.hpp"
#include "file.hpp"
#include "player.hpp"
#include "texture.hpp"
#include "tilemap.hpp"
#include "window.hpp"

int main(){
    Window* window = new Window("Atlacp", 1600, 900, {100,100,100});
    if (window->HasError()){
        return -1;
    }
    EventController* event_controller = new EventController();
    TextureController* texture_controller = new TextureController(window->GetRenderer());
    FileReader* file_reader = new FileReader();
    Tilemap* tilemap = new Tilemap(texture_controller, file_reader, "../map.txt", "../tileset.png");
    Player* player = new Player(tilemap, texture_controller, event_controller, "../cpp.png");

    std::list<Drawable*> drawables = {tilemap, player}; // Rendering order must be respected
    std::set<Element*> elements = {player};

    bool gameloop = true;
    while(gameloop){
        window->ClearRenderer();
        event_controller->PollAllEvents();
        
        if (event_controller->HandleWindowEvents()==-1) gameloop=false;
        
        for (std::set<Element*>::iterator it = elements.begin() ; it != elements.end() ; it++){
            (*it)->Update();
        }
        
        for (std::list<Drawable*>::iterator it = drawables.begin() ; it != drawables.end() ; it++){
            (*it)->DrawTexture();
        }
        
        window->UpdateRender();        
    }

    delete tilemap;
    delete player;
    delete file_reader;
    delete event_controller;
    delete window;
    delete texture_controller; // This instance must be delete last
    return 0;
}