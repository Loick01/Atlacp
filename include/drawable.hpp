#pragma once

#include <iostream>

#include "texture.hpp"

class Drawable
{
    protected:
        Drawable(TextureController* texture_controller, const std::string& texture_filepath, const Offset offset={0,0});
        ~Drawable();

        void LoadTexture(const std::string& texture_filepath);

        TextureController* m_texture_controller;
        TextureKey m_texture_key;
        Offset m_offset;
    
    public:
        virtual void DrawTexture() const = 0;
        void SetOffset(const Offset offset);
};