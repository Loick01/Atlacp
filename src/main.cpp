#include <iostream>

#include "scene.hpp"

int main(int argc, char* argv[]){

    const int mode = argc == 1 ? 0 : std::stoi(argv[1]);

    switch (mode){
        case 0:
        {
            GameplayTilemapScene gts;
            while(gts.GetGameloop()){
                gts.Gameloop();
            }
            break;
        }
        case 1:
        {
            EditorTilemapScene ets;
            while(ets.GetGameloop()){
                ets.Gameloop();
            }
            break;
        }
        default:
        {
            std::cout << "Undefined mode\n";
            break;
        }
    }

    // Because texture_controller is created before every Drawable, its destructor is called after every Drawable destructor
    // It means that there should no longer be any textures to SDL_Destroy. 
    // If that's not the case, ~TextureController can avoid memory leaks, but it would be better not to use it (I put a warning message there)
    return 0;
}