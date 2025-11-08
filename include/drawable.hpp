#pragma once

#include <iostream>

#include "texture.hpp"

class Drawable
{
    protected:
        Drawable(TextureController* texture_controller, const std::string& texture_filepath, const ScreenPosition position={0,0});
        ~Drawable();

        void LoadTexture(const std::string& texture_filepath);

        TextureController* m_texture_controller;
        TextureKey m_texture_key;
        ScreenPosition m_screen_position;
        unsigned int m_texture_width;
        unsigned int m_texture_height;
    
    public:
        virtual void DrawTexture() const;
        void SetScreenPosition(const ScreenPosition screen_position);
};