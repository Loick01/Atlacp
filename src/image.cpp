#include "image.hpp"

ImageController::ImageController()
{
    LoadImageFromFile(m_image,"../cpp.bmp");
    m_image.pos_x = 0;
    m_image.pos_y = 0;
}

ImageController::~ImageController()
{

}

void ImageController::LoadImageFromFile(Image& img, const char* filepath)
{
    SDL_Surface* s = SDL_LoadBMP(filepath);
    if (!s) std::cout << "Failed to load this image : " << filepath << "\n";
    img.surface = s;
}

void ImageController::Draw(SDL_Surface* window_surface)
{
    SDL_BlitSurface(m_image.surface, NULL, window_surface, NULL);
}