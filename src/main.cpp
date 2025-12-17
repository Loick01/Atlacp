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

    FileReader file_reader;
    Time time;
    GameplayEventController event_controller;

    Window window("Atlacp", {25,25,25});
    if (window.HasError()){
        return -1;
    }

    TextureController texture_controller(window.GetRenderer());
    Camera camera(window, ScenePosition{16, 9}, 32);

    Tileset tileset(texture_controller, camera, file_reader);
    Tilemap tilemap(texture_controller, file_reader, tileset, "../assets/worlds/ff_world", camera);
    Player player(file_reader, tilemap, texture_controller, event_controller, "../assets/sprites/character", camera, 5.0f);

    // Layer 1
    std::vector<Drawable*> drawables = {&tilemap}; // Must be a vector of Drawable* because we need dynamic dispatch
    // Layer 2
    std::vector<Entity*> entities = {&player}; // Same
    
    /*
    for (unsigned int i = 0 ; i < 1 ; i++){
        NPC npc(file_reader, tilemap, texture_controller, "../assets/sprites/npc", camera, 10.0f);
        entities.push_back(&npc);
    }*/

    // Testing follow behaviour (will be remove)
    Entity* tracked_entity = &player;
    for (unsigned int i = 0 ; i < 10 ; i++){
        NPC* npc = new NPC(file_reader, tilemap, texture_controller, tracked_entity, "../assets/sprites/npc", camera, 5.0f);
        // NPC are not yet deleted + their texture is destroy by TextureController::~TextureController (as explained below)
        entities.push_back(npc);
        tracked_entity = npc;
    }

    bool gameloop = true;
    while(gameloop){
        time.Update();
        window.ClearRenderer();
        event_controller.PollAllEvents();
        
        gameloop = event_controller.HandleWindowEvents();
        
        // Remove (should sort only at the end of any movement, or even better --> remove then insert the moving npc at the correct index)
        std::sort(entities.begin(), entities.end(),
            [](Entity* a, Entity* b){
                return a->GetMapPosition().y < b->GetMapPosition().y; 
            });
        const float delta_time = time.GetDeltaTime();
        for (const Drawable* d : drawables) d->DrawTexture();
        for (Entity* e : entities){
            e->DrawTexture();
            e->Update(delta_time);
        }

        window.DrawBoxing();
        window.UpdateRender();       
    }

    // Because texture_controller is created before every Drawable, its destructor is called after every Drawable destructor
    // It means that there should no longer be any textures to SDL_Destroy. 
    // If that's not the case, ~TextureController can avoid memory leaks, but it would be better not to use it (I put a warning message there)
    return 0;
}