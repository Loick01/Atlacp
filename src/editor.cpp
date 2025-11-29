#include <iostream>
#include <vector>

#include "event.hpp"
#include "file.hpp"
#include "texture.hpp"
#include "tilemap.hpp"
#include "window.hpp"
#include "tileset.hpp"

int main(){
    Window* window = new Window("Map Editor - Atlacp", 640, 640, {50,50,50});
    if (window->HasError()){
        return -1;
    }
    TextureController* texture_controller = new TextureController(window->GetRenderer());
    FileReader* file_reader = new FileReader();
    Camera* camera = new Camera(ScenePosition{window->GetWidth(), window->GetHeight()});
    Tileset* tileset = new Tileset(texture_controller, camera, file_reader);
    Tilemap* tilemap = new Tilemap(texture_controller, file_reader, tileset, "../asset/world.txt", camera, false);
    EditorEventController* event_controller = new EditorEventController(tileset);
    std::vector<Drawable*> drawables = {tilemap, tileset}; // Rendering order must be respected

    bool gameloop = true;
    while(gameloop){
        window->ClearRenderer();
        event_controller->PollAllEvents();
        
        gameloop = event_controller->HandleWindowEvents();
        event_controller->HandleEditorEvent(tileset, tilemap, camera);

        for (const Drawable* d : drawables) d->DrawTexture();
        
        window->UpdateRender();     
    }

    delete tileset;
    delete tilemap;
    delete file_reader;
    delete event_controller;
    delete window;
    delete texture_controller; // This instance must be delete after deleting every Drawable instance 
    return 0;
}