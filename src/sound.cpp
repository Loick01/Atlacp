#include "sound.hpp"

SoundController::SoundController()
{
    // SDL_mixer initialization will be here
    m_background_music = nullptr;
}

SoundController::~SoundController()
{
    DeleteBackgroundMusic();
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