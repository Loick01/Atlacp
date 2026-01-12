#pragma once

#include <iostream>
#include <string>

#include <SDL2/SDL_mixer.h>
// Mix_FadeOutMusic(int ms), Mix_FadeInMusic(Mix_Music *music, int loops, int ms)

class SoundController
{
    private:
        Mix_Music* m_background_music;
        void DeleteBackgroundMusic();

    public:
        SoundController();
        ~SoundController();
        void SetBackgroundMusic(const std::string& filepath);
};
