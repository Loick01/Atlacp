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

    // Volume setting will not be here
    Mix_VolumeMusic(50 * MIX_MAX_VOLUME / 100); // For the background music (played with Mix_PlayMusic())
    Mix_Volume(-1, MIX_MAX_VOLUME); // For the sound effects (played with Mix_PlayChannel())

    LoadBaseSfx();
    m_requestedChunk.clear();
}

SoundController::~SoundController()
{
    DeleteBackgroundMusic();

    std::unordered_map<std::string, Mix_Chunk*>::iterator it;
    for (it = m_chunks.begin() ; it != m_chunks.end() ; it++)
        Mix_FreeChunk(it->second);
    m_chunks.clear();

    Mix_CloseAudio();
    Mix_Quit(); // Optionnal if use only Mix_OpenAudio, but required if use Mix_Init()
}

SoundController& SoundController::GetInstance() {
    static SoundController instance; // Local static object, create an instance only when this is the first call
    return instance;
}

void SoundController::LoadBaseSfx()
{
    // ?
    LoadChunk(BaseSfx::Accept);
    LoadChunk(BaseSfx::Open);
    LoadChunk(BaseSfx::Close);
    LoadChunk(BaseSfx::Move);
    LoadChunk(BaseSfx::Next);
    LoadChunk(BaseSfx::Death);
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
    
    m_backgroundMusic = Mix_LoadMUS(("../assets/sound/music/"+filepath).c_str()); // TODO Filepath
    if (Mix_PlayMusic(m_backgroundMusic, -1) == -1) 
        throw std::runtime_error("Failed to play audio\n" + std::string(Mix_GetError()));
}

void SoundController::RequestChunk(const std::string& path)
{
    m_requestedChunk = path;
}

void SoundController::PlayRequestedChunk()
{
    if (!m_requestedChunk.empty()) {
        PlayChunk(m_requestedChunk);
        m_requestedChunk.clear();
    }
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
        Mix_Chunk* chunk = Mix_LoadWAV(("../assets/sound/sfx/"+path+".wav").c_str()); // TODO Filepath
        if (chunk != nullptr) 
            m_chunks[path] = chunk;
        else
            throw std::runtime_error("Failed to load this audio file : " + path + "\n" + std::string(Mix_GetError()));
    }
}

void SoundController::DeleteChunk(const std::string& path)
{
    std::unordered_map<std::string, Mix_Chunk*>::iterator it = m_chunks.find(path);
    if (it != m_chunks.end()) {
        Mix_FreeChunk(it->second);
        m_chunks.erase(it);
    } 
    // else throw std::runtime_error("Try to delete a chunk not in SoundController : " + path + "\n" + std::string(Mix_GetError()));
}
