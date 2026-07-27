#pragma once

#include <string>
#include <unordered_map>

#include <SDL2/SDL_mixer.h>
// Mix_FadeOutMusic(int ms), Mix_FadeInMusic(Mix_Music *music, int loops, int ms)

namespace BaseSfx{
    // Should be inline constexpr std::string_view ?
    inline const std::string Accept = "Accept";
    inline const std::string Open = "Open";
    inline const std::string Close = "Close";
    inline const std::string Move = "Move";
    inline const std::string Next = "Next";
    inline const std::string Death = "Death"; // Should not be here (only used for BattleScene) ?
}

class SoundController // Singleton
{
    private:
        std::unordered_map<std::string, Mix_Chunk*> m_chunks; // string as key ?
        Mix_Music* m_backgroundMusic;
        std::string m_requestedChunk;

        SoundController();
        ~SoundController();
        
        void LoadBaseSfx();
        void DeleteBackgroundMusic();

    public:
        static SoundController& GetInstance();

        void SetBackgroundMusic(const std::string& filepath);

        void RequestChunk(const std::string& path);
        void PlayRequestedChunk();
        
        void PlayChunk(const std::string& path); // private ?
        void LoadChunk(const std::string& path);
        void DeleteChunk(const std::string& path); // Used only in BattleController::PlayNextTurn (TurnState::End) for now 
};
