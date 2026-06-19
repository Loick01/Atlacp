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
    
    m_backgroundMusic = Mix_LoadMUS(("../assets/sound/music/"+filepath).c_str());
    if (Mix_PlayMusic(m_backgroundMusic, -1) == -1) 
        throw std::runtime_error("Failed to play audio\n" + std::string(Mix_GetError()));
}

void SoundController::PlayChunk(const std::string& path)
{
    if (m_chunks.find(path) == m_chunks.end())
        throw std::runtime_error("Try to play chunk not in SoundController : " + path + "\n" + std::string(Mix_GetError()));
    if (Mix_PlayChannel(-1, m_chunks[path], 0) == -1)
        throw std::runtime_error("Failed to play chunk : " + path + "\n" + std::string(Mix_GetError()));
}

void SoundController::LoadChunk(const std::string& path)
{
    if (m_chunks.find(path) == m_chunks.end()) {
        Mix_Chunk* chunk = Mix_LoadWAV(("../assets/sound/sfx/"+path).c_str());
        if (chunk != nullptr) 
            m_chunks[path] = chunk;
        else
            throw std::runtime_error("Failed to load this audio file : " + path + "\n" + std::string(Mix_GetError()));
    }
}

void SoundController::DeleteChunk(const std::string& path)
{
    if (m_chunks.find(path) != m_chunks.end())
        Mix_FreeChunk(m_chunks[path]);
    else
        throw std::runtime_error("Try to delete a chunk not in SoundController : " + path + "\n" + std::string(Mix_GetError()));
}
