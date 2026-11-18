#pragma once

#include <string>
#include <unordered_map>

#include <SDL2/SDL_mixer.h>
// Mix_FadeOutMusic(int ms), Mix_FadeInMusic(Mix_Music *music, int loops, int ms)

using ChunkKey = std::string;

namespace BaseSfx{
    inline constexpr std::string_view Accept = "Accept";
    inline constexpr std::string_view Open = "Open";
    inline constexpr std::string_view Close = "Close";
    inline constexpr std::string_view Move = "Move";
    inline constexpr std::string_view Next = "Next";
    inline constexpr std::string_view Death = "Death"; // TODO : Should not be here (only used for BattleScene) ?
}

class SoundController
{
    private:
        std::unordered_map<ChunkKey, Mix_Chunk*> m_chunks;
        Mix_Music* m_bgm;
        ChunkKey m_requestedChunk;

        SoundController();
        ~SoundController();
        
        void LoadBaseSfx();
        void DeleteBackgroundMusic();

    public:
        static SoundController& GetInstance();

        void SetBackgroundMusic(const std::string& filename);

        void ClearChunk();
        void RequestChunk(const ChunkKey& key);
        void PlayRequestedChunk();
        
        void PlayChunk(const ChunkKey& key); // TODO : private ?
        void LoadChunk(const ChunkKey& key);
        void DeleteChunk(const ChunkKey& key); // Used only in BattleController::PlayNextTurn (TurnState::End) for now 
};
