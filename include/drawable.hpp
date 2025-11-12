#pragma once

#include <iostream>

#include "texture.hpp"

class Drawable
{
    protected:
        Drawable(TextureController* texture_controller, const std::string& texture_filepath, const ScreenPosition position, const bool should_draw);
        Drawable(TextureController* texture_controller, const ScreenPosition position, const bool should_draw);
        ~Drawable(); // If this class doesn't stay abstract, will need to delete m_texture_key in its destructor (texture_controller->DeleteTexture(texture_key))
        
        void LoadTexture(const std::string& texture_filepath);

        TextureController* m_texture_controller;
        TextureKey m_texture_key;
        ScreenPosition m_screen_position;
        unsigned int m_texture_width;
        unsigned int m_texture_height;
        bool m_should_draw;
    
    public:
        virtual TextureKey GetTextureKey() const;
        ScreenPosition GetScreenPosition() const;
        virtual unsigned int GetTextureWidth() const;
        virtual unsigned int GetTextureHeight() const;
        bool GetShouldDraw() const;
        bool IsPositionInTexture(const ScreenPosition screen_position) const;
        virtual void DrawTexture() const;
        void SetScreenPosition(const ScreenPosition screen_position);
        void InvertShouldDraw();
};