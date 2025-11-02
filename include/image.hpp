#include <iostream>

#include <SDL2/SDL.h>

// TODO : Do not let this struct, m_image and Draw in this file
struct Image
{
    SDL_Surface* surface;
    unsigned int pos_x;
    unsigned int pos_y;
};

class ImageController
{
    private:
        static void LoadImageFromFile(Image& image, const char* filepath);

        Image m_image;

    public:
        ImageController();
        ~ImageController();

        void Draw(SDL_Surface* window_surface);
};