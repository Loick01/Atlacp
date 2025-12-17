#include <iostream>
#include <vector>

#include "event.hpp"
#include "file.hpp"
#include "texture.hpp"
#include "tilemap.hpp"
#include "window.hpp"
#include "tileset.hpp"

int main(){
    Window window("Map Editor - Atlacp", {25,25,25}, false);
    if (window.HasError()){
        return -1;
    }
    TextureController texture_controller(window.GetRenderer());
    FileReader file_reader;
    Camera camera(window, ScenePosition{16, 12}, 32);
    Tileset tileset(texture_controller, camera, file_reader);
    Tilemap tilemap(texture_controller, file_reader, tileset, "../assets/worlds/ff_world", camera, false);
    EditorEventController event_controller(tileset);
    std::vector<Drawable*> drawables = {&tilemap, &tileset}; // Must be a vector of Drawable* because we need dynamic dispatch

    bool gameloop = true;
    while(gameloop){
        window.ClearRenderer();
        event_controller.PollAllEvents();
        
        gameloop = event_controller.HandleWindowEvents();
        event_controller.HandleEditorEvent(tilemap, camera);

        for (const Drawable* d : drawables) d->DrawTexture();
        
        window.UpdateRender();     
    }

    // Because texture_controller is created before every Drawable, its destructor is called after every Drawable destructor
    // It means that there should no longer be any textures to SDL_Destroy. 
    // If that's not the case, ~TextureController can avoid memory leaks, but it would be better not to use it (I put a warning message there)
    return 0;
}