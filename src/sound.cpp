#include "sound.hpp"

SoundController::SoundController()
{
    // SDL_mixer initialization
    // https://lazyfoo.net/SDL_tutorials/lesson11/index.php
    int mix_flags = MIX_INIT_OGG;
    if ((Mix_Init(mix_flags) & mix_flags) != mix_flags) std::cout << "Can't initialize SDL_mixer\n"; // Mix_GetError()

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
        std::cout << "Failed to initialize SDL2_mixer library\n";

    m_background_music = nullptr;
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
    Mix_FreeMusic(m_background_music);
    m_background_music = nullptr; // Be sure to not play music that has been freed
}

void SoundController::SetBackgroundMusic(const std::string& filepath)
{
    if (m_background_music != nullptr)
        DeleteBackgroundMusic();
    
    m_background_music = Mix_LoadMUS(("../assets/musics/"+filepath).c_str()); // Use Mix_Chunk for sound effects
    if (Mix_PlayMusic(m_background_music, -1) < 0) std::cout << "Can't play music\n"; // Will throw error
}