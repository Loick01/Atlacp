#pragma once

#include <string>
#include <unordered_map>

#include <SDL2/SDL_mixer.h>
// Mix_FadeOutMusic(int ms), Mix_FadeInMusic(Mix_Music *music, int loops, int ms)

class SoundController
{
    private:
        std::unordered_map<std::string, Mix_Chunk*> m_chunks;
        Mix_Music* m_backgroundMusic;

        void DeleteBackgroundMusic();

    public:
        SoundController();
        ~SoundController();

        void SetBackgroundMusic(const std::string& filepath);

        void PlayChunk(const std::string& path);
        void LoadChunk(const std::string& path);
        void DeleteChunk(const std::string& path);
};
