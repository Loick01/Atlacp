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
    MapEventController* event_controller = new MapEventController();
    TextureController* texture_controller = new TextureController(window->GetRenderer());
    FileReader* file_reader = new FileReader();
    Tileset* tileset = new Tileset(texture_controller, file_reader);
    Tilemap* tilemap = new Tilemap(texture_controller, file_reader, tileset, "../map1.txt");
    const ScreenPosition drawing_offset{window->GetWidth()/2-tilemap->GetTextureWidth()/2, window->GetHeight()/2-tilemap->GetTextureHeight()/2};
    tilemap->SetScreenPosition(drawing_offset);
    Player* player = new Player(tilemap, texture_controller, event_controller, "../cpp.png", drawing_offset);

    std::vector<Drawable*> drawables = {tilemap, player}; // Rendering order must be respected
    std::vector<MapElement*> elements = {player};

    bool gameloop = true;
    while(gameloop){
        window->ClearRenderer();
        event_controller->PollAllEvents();
        
        gameloop = event_controller->HandleWindowEvents();
        
        for (MapElement* e : elements) e->Update();
        for (const Drawable* d : drawables) d->DrawTexture();
        
        window->UpdateRender();     
        SDL_Delay(48);   
    }

    delete tileset;
    delete tilemap;
    delete player;
    delete file_reader;
    delete event_controller;
    delete window;
    delete texture_controller; // This instance must be delete after deleting every Drawable instance 
    return 0;
}