#include "sound/sound.hpp"

#include "core/logger.hpp"
#include "core/path.hpp"

SoundController::SoundController():
    m_bgm(nullptr), m_requestedChunk()
{
    // https://lazyfoo.net/SDL_tutorials/lesson11/index.php
    int mixFlags = MIX_INIT_OGG;
    if ((Mix_Init(mixFlags) & mixFlags) != mixFlags)
        Logger::GetInstance().AddLog(Level::L4, "Failed to initialize SDL_mixer: " + std::string(Mix_GetError())); // TODO : Level

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        Logger::GetInstance().AddLog(Level::L4, "Failed to open Audio: " + std::string(Mix_GetError())); // TODO : Level
        return;
    }

    // TODO : Volume setting will not be here ?
    Mix_VolumeMusic(50 * MIX_MAX_VOLUME / 100); // For the background music (played with Mix_PlayMusic())
    Mix_Volume(-1, MIX_MAX_VOLUME); // For the sound effects (played with Mix_PlayChannel())

    LoadBaseSfx();
}

SoundController::~SoundController()
{
    DeleteBackgroundMusic();

    for (std::unordered_map<ChunkKey, Mix_Chunk*>::const_iterator it = m_chunks.begin() ; it != m_chunks.end() ; it++)
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
    // TODO : ?
    LoadChunk(std::string(BaseSfx::Accept));
    LoadChunk(std::string(BaseSfx::Open));
    LoadChunk(std::string(BaseSfx::Close));
    LoadChunk(std::string(BaseSfx::Move));
    LoadChunk(std::string(BaseSfx::Next));
    LoadChunk(std::string(BaseSfx::Flee));
    LoadChunk(std::string(BaseSfx::Death));
}

void SoundController::DeleteBackgroundMusic()
{
    Mix_HaltMusic();
    Mix_FreeMusic(m_bgm);
    m_bgm = nullptr; // Be sure to not play music that has been freed
}

void SoundController::SetBackgroundMusic(const std::string& filename)
{
    if (m_bgm != nullptr)
        DeleteBackgroundMusic();
    
    m_bgm = Mix_LoadMUS((std::string(AssetDirectory::Music)+filename).c_str());
    
    if (m_bgm == nullptr) {
        Logger::GetInstance().AddLog(Level::L4, "Failed to load the BGM " + filename + ": " + std::string(Mix_GetError())); // TDOO : Level
        return;
    }

    if (Mix_PlayMusic(m_bgm, -1) == -1) {
        Logger::GetInstance().AddLog(Level::L4, "Failed to play the BGM " + filename + ": " + std::string(Mix_GetError())); // TODO : Level
        DeleteBackgroundMusic();
        return;
    }
}

void SoundController::ClearChunk()
{
    m_requestedChunk.clear();
}

void SoundController::RequestChunk(const ChunkKey& key)
{
    m_requestedChunk = key;
}

void SoundController::PlayRequestedChunk()
{
    if (!m_requestedChunk.empty()) {
        PlayChunk(m_requestedChunk);
        m_requestedChunk.clear();
    }
}   

void SoundController::PlayChunk(const ChunkKey& key)
{
    std::unordered_map<ChunkKey, Mix_Chunk*>::const_iterator chunkIt = m_chunks.find(key);

    if (chunkIt == m_chunks.end()) {
        Logger::GetInstance().AddLog(Level::L4, "(SoundController::PlayChunk) Chunk not in SoundController (" + key + ")"); // TODO : Level
        return;
    }

    if (Mix_PlayChannel(-1, chunkIt->second, 0) == -1)
        Logger::GetInstance().AddLog(Level::L4, "Failed to play Chunk (" + key + "): " + std::string(Mix_GetError())); // TODO : Level
}

void SoundController::LoadChunk(const ChunkKey& key)
{
    std::unordered_map<ChunkKey, Mix_Chunk*>::const_iterator chunkIt = m_chunks.find(key);

    if (chunkIt != m_chunks.end())
        return;

    Mix_Chunk* chunk = Mix_LoadWAV((std::string(AssetDirectory::Sfx)+key+std::string(FileExtension::Sfx)).c_str());
    if (chunk == nullptr) {
        Logger::GetInstance().AddLog(Level::L4, "Failed to load the Chunk (" + key + "): " + std::string(Mix_GetError())); // TODO : Level
        return;
    }

    m_chunks.emplace(key, chunk);
}

void SoundController::DeleteChunk(const ChunkKey& key)
{
    std::unordered_map<ChunkKey, Mix_Chunk*>::iterator chunkIt = m_chunks.find(key);
    
    if (chunkIt == m_chunks.end()) {
        Logger::GetInstance().AddLog(Level::L4, "Try to delete a Chunk not in SoundController (" + key + ")"); // TODO : Level
        return;
    }

    Mix_FreeChunk(chunkIt->second);
    m_chunks.erase(chunkIt);
}
