#pragma once

#include <string>
#include <unordered_map>

#include <SDL2/SDL_mixer.h>
// Mix_FadeOutMusic(int ms), Mix_FadeInMusic(Mix_Music *music, int loops, int ms)

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
        std::unordered_map<std::string, Mix_Chunk*> m_chunks; // TODO : string as key ?
        Mix_Music* m_backgroundMusic;
        std::string m_requestedChunk;

        SoundController();
        ~SoundController();
        
        void LoadBaseSfx();
        void DeleteBackgroundMusic();

    public:
        static SoundController& GetInstance();

        void SetBackgroundMusic(const std::string& filepath);

        void ClearChunk();
        void RequestChunk(const std::string& path);
        void PlayRequestedChunk();
        
        void PlayChunk(const std::string& path); // TODO : private ?
        void LoadChunk(const std::string& path);
        void DeleteChunk(const std::string& path); // Used only in BattleController::PlayNextTurn (TurnState::End) for now 
};
