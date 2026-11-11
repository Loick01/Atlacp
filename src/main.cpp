#include <exception>

#include "core/logger.hpp"
#include "scene/scene.hpp"

int main(int argc, char* argv[])
{
    try {
        // Run with ./atlacp n 
        // n = 0 : Game mode, n = 1 : Editor mode, n = 2 : Battle mode
        const int mode = argc == 1 ? 0 : std::stoi(argv[1]);
        SceneController scenes(mode);
        scenes.StartGameloop();

        // Because textureController is created before every Drawable, its destructor is called after every Drawable destructor
        // It means that there should no longer be any textures to SDL_Destroy. 
        // If that's not the case, ~TextureController can avoid memory leaks, but it would be better not to use it (I put a warning message there)
    } catch (const std::exception& e) {
        const std::string exception = std::string("[Unhandled exception] ") + e.what();
        Logger::GetInstance().AddLog(Level::L4, exception); // TODO : Level
    }
    
    Logger::GetInstance().WriteLogsInFile();
    for (const Log& log : Logger::GetInstance().GetLogs())
        std::cerr << log.label << " " << log.message << "\n";

    return 0;
}