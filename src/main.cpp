#include <iostream>
#include <vector>

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
    const Offset drawing_offset{static_cast<int>(window->GetWidth()/2-tilemap->GetTotalWidth()/2),
                                static_cast<int>(window->GetHeight()/2-tilemap->GetTotalHeight()/2)};
    tilemap->SetOffset(drawing_offset);
    Player* player = new Player(tilemap, texture_controller, event_controller, "../cpp.png", drawing_offset);

    std::vector<Drawable*> drawables = {tilemap, player}; // Rendering order must be respected
    std::vector<MapElement*> elements = {player};

    bool gameloop = true;
    while(gameloop){
        window->ClearRenderer();
        event_controller->PollAllEvents();
        
        if (event_controller->HandleWindowEvents()==-1) gameloop=false;
        
        for (MapElement* e : elements) e->Update();
        for (const Drawable* d : drawables) d->DrawTexture();
        
        window->UpdateRender();     
        SDL_Delay(48);   
    }

    delete tilemap;
    delete player;
    delete file_reader;
    delete event_controller;
    delete window;
    delete texture_controller; // This instance must be delete after deleting every Drawable instance 
    return 0;
}