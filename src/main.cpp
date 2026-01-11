#include <iostream>
#include <memory> // Remove

#include "scene.hpp"

int main(int argc, char* argv[]){

    const int mode = argc == 1 ? 0 : std::stoi(argv[1]);
    SceneController scenes(mode);
    scenes.StartGameloop();

    // Because texture_controller is created before every Drawable, its destructor is called after every Drawable destructor
    // It means that there should no longer be any textures to SDL_Destroy. 
    // If that's not the case, ~TextureController can avoid memory leaks, but it would be better not to use it (I put a warning message there)
    return 0;
}