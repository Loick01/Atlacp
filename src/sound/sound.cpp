#include "sound/sound.hpp"

#include <stdexcept>

SoundController::SoundController()
{
    // SDL_mixer initialization
    // https://lazyfoo.net/SDL_tutorials/lesson11/index.php
    int mixFlags = MIX_INIT_OGG;
    if ((Mix_Init(mixFlags) & mixFlags) != mixFlags)
        throw std::runtime_error("Failed to initialize SDL_mixer\n" + std::string(Mix_GetError()));   

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
        throw std::runtime_error("Failed to open audio\n" + std::string(Mix_GetError()));

    m_backgroundMusic = nullptr;
}

SoundController::~SoundController()
{
    DeleteBackgroundMusic();
    Mix_CloseAudio();
    Mix_Quit(); // Optionnal if use only Mix_OpenAudio, but required if use Mix_Init()
}

void SoundController::DeleteBackgroundMusic()
{
    Mix_HaltMusic();
    Mix_FreeMusic(m_backgroundMusic);
    m_backgroundMusic = nullptr; // Be sure to not play music that has been freed
}

void SoundController::SetBackgroundMusic(const std::string& filepath)
{
    if (m_backgroundMusic != nullptr)
        DeleteBackgroundMusic();
    
    m_backgroundMusic = Mix_LoadMUS(("../assets/musics/"+filepath).c_str()); // Use Mix_Chunk for sound effects
    if (Mix_PlayMusic(m_backgroundMusic, -1) < 0) 
        throw std::runtime_error("Failed to play audio\n" + std::string(Mix_GetError()));
}