#pragma once

#include <iostream>

#include "texture.hpp"

class Drawable
{
    protected:
        void LoadTexture(const std::string& texture_filepath);

        TextureController* m_texture_controller;
        TextureKey m_texture_key;
        ScreenPosition m_screen_position;
        unsigned int m_texture_width;
        unsigned int m_texture_height;
        bool m_should_draw;
    
    public:
        Drawable(TextureController* texture_controller, const std::string& texture_filepath, const ScreenPosition position={0,0}, const bool should_draw=true);
        ~Drawable();

        ScreenPosition GetScreenPosition() const;
        bool GetShouldDraw() const;
        virtual void DrawTexture() const;
        void SetScreenPosition(const ScreenPosition screen_position);
        void InvertShouldDraw();
};