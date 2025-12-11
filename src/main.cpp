#include <iostream>
#include <vector>

#include "camera.hpp"
#include "event.hpp"
#include "file.hpp"
#include "npc.hpp"
#include "player.hpp"
#include "texture.hpp"
#include "tilemap.hpp"
#include "time.hpp"
#include "window.hpp"

int main(){
    Window* window = new Window("Atlacp", {25,25,25});
    if (window->HasError()){
        return -1;
    }
    
    Time time;
    GameplayEventController* event_controller = new GameplayEventController();
    TextureController* texture_controller = new TextureController(window->GetRenderer());
    FileReader* file_reader = new FileReader();
    Camera* camera = new Camera(window, ScenePosition{16, 12}, 32);

    Tileset* tileset = new Tileset(texture_controller, camera, file_reader);
    Tilemap* tilemap = new Tilemap(texture_controller, file_reader, tileset, "../assets/worlds/ff_world", camera);
    Player* player = new Player(file_reader, tilemap, texture_controller, event_controller, "../assets/sprites/character", camera, 3.0f);

    // Layer 1
    std::vector<Drawable*> drawables = {tilemap};
    // Layer 2
    std::vector<Entity*> entities = {player};
    
    std::vector<Entity*> elements = {player};
    for (unsigned int i = 0 ; i < 10 ; i++){
        NPC* npc = new NPC(file_reader, tilemap, texture_controller, "../assets/sprites/npc", camera, 3.0f);
        entities.push_back(npc);
        elements.push_back(npc);
    }

    bool gameloop = true;
    while(gameloop){
        time.Update();
        window->ClearRenderer();
        event_controller->PollAllEvents();
        
        gameloop = event_controller->HandleWindowEvents();
        
        const float delta_time = time.GetDeltaTime();
        for (const Drawable* d : drawables) d->DrawTexture();
        for (const Entity* e : entities) e->DrawTexture();
        for (Entity* e : elements) e->Update(delta_time);
        
        window->DrawBoxing();
        window->UpdateRender();       
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