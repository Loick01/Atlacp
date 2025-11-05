#pragma once

#include <iostream>

#include "texture.hpp"

class Drawable
{
    protected:
        Drawable(TextureController* texture_controller, const std::string& texture_filepath);
        ~Drawable();

        void LoadTexture(const std::string& texture_filepath);

        TextureController* m_texture_controller; // Should be const
        std::string m_texture_filepath; // Could be private
    
    public:
        virtual void DrawTexture() const = 0;
};