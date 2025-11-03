#include <iostream>
#include <vector>

#include "event.hpp"
#include "texture.hpp"
#include "window.hpp"
#include "file.hpp"

int main(){
    Window* window = new Window("Atlacp", 1600, 900, {100,100,100});
    if (window->HasError()){
        return -1;
    }
    EventController* events = new EventController();
    TextureController* textures = new TextureController(window->GetRenderer());
    FileReader* file = new FileReader();
    std::vector<unsigned char> map = file->GetMapFromFile("../map.txt");

    // For a tile, this SDL_Rect for position will be in a struct with its SDL_Texture
    const unsigned int image_width = 212;
    const unsigned int image_height = 238;

    const SDL_Rect src{0,0,image_width,image_height};
    const SDL_Rect dst{50,50,image_width,image_height};
    
    bool gameloop = true;
    while(gameloop){
        if (events->HandleEvents()==-1) gameloop=false;
        textures->RenderTexture("../cpp.png", src, dst);
        window->UpdateRender();        
    }

    delete textures;
    delete events;
    delete window;
    return 0;
}