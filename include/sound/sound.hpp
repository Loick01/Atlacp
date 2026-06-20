#pragma once

#include <string>
#include <unordered_map>

#include <SDL2/SDL_mixer.h>
// Mix_FadeOutMusic(int ms), Mix_FadeInMusic(Mix_Music *music, int loops, int ms)

namespace {
    const std::string acceptSfx = "Accept";
    const std::string moveSfx = "Move";
    const std::string nextSfx = "Next";
}

class SoundController // Singleton
{
    private:
        std::unordered_map<std::string, Mix_Chunk*> m_chunks; // string as key ?
        Mix_Music* m_backgroundMusic;

        SoundController();
        ~SoundController();
        
        void LoadBasicSound();
        void DeleteBackgroundMusic();

    public:
        static SoundController& GetInstance();

        void SetBackgroundMusic(const std::string& filepath);

        void PlayChunk(const std::string& path);
        void LoadChunk(const std::string& path);
        void DeleteChunk(const std::string& path);
};
