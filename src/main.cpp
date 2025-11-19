#include <iostream>
#include <vector>

#include "camera.hpp"
#include "event.hpp"
#include "file.hpp"
#include "player.hpp"
#include "texture.hpp"
#include "tilemap.hpp"
#include "window.hpp"

int main(){
    Window* window = new Window("Atlacp", 640, 640, {100,100,100});
    if (window->HasError()){
        return -1;
    }
    MapEventController* event_controller = new MapEventController();
    TextureController* texture_controller = new TextureController(window->GetRenderer());
    FileReader* file_reader = new FileReader();
    Camera* camera = new Camera(ScenePosition{window->GetWidth()/2, window->GetHeight()/2});

    Tileset* tileset = new Tileset(texture_controller, camera, file_reader);
    Tilemap* tilemap = new Tilemap(texture_controller, file_reader, tileset, "../world.txt", camera);
    Player* player = new Player(tilemap, texture_controller, event_controller, "../cpp.png", camera);

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

    delete camera;
    delete tileset;
    delete tilemap;
    delete player;
    delete file_reader;
    delete event_controller;
    delete window;
    delete texture_controller; // This instance must be delete after deleting every Drawable instance 
    return 0;
}