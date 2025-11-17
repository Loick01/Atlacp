#pragma once

#include <iostream>

#include "camera.hpp"
#include "texture.hpp"

class Drawable
{
    protected:
        Drawable(TextureController* texture_controller, const std::string& texture_filepath, Camera* camera, const ScreenPosition position,
            const bool should_draw);
        Drawable(TextureController* texture_controller, Camera* camera, const ScreenPosition position, const bool should_draw);
        ~Drawable(); // If this class doesn't stay abstract, will need to delete m_texture_key in its destructor (texture_controller->DeleteTexture(texture_key))
        
        void LoadTexture(const std::string& texture_filepath);

        TextureController* m_texture_controller;
        Camera* m_camera;
        TextureKey m_texture_key;
        ScreenPosition m_position; // Position in 2D space
        int m_texture_width;
        int m_texture_height;
        bool m_should_draw;
    
    public:
        virtual TextureKey GetTextureKey() const;
        ScreenPosition GetScreenPosition() const;
        virtual int GetTextureWidth() const;
        virtual int GetTextureHeight() const;
        bool GetShouldDraw() const;
        bool IsPositionInTexture(const ScreenPosition sp) const;
        virtual void DrawTexture() const;
        void SetScreenPosition(const ScreenPosition sp);
        void AddScreenPosition(const ScreenPosition sp);
        void InvertShouldDraw();
};