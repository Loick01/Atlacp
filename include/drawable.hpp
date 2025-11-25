#pragma once

#include <iostream>

#include "camera.hpp"
#include "texture.hpp"

class Drawable
{
    protected:
        Drawable(TextureController* texture_controller, const std::string& texture_filepath, Camera* camera, const ScenePosition position);
        Drawable(TextureController* texture_controller, Camera* camera, const ScenePosition position);
        ~Drawable(); // If this class doesn't stay abstract, will need to delete m_texture_key in its destructor (texture_controller->DeleteTexture(texture_key))
        
        void LoadTexture(const std::string& texture_filepath);

        TextureController* m_texture_controller;
        Camera* m_camera;
        TextureKey m_texture_key;
        ScenePosition m_position;
        int m_texture_width;
        int m_texture_height;
    
    public:
        virtual TextureKey GetTextureKey() const;
        ScenePosition GetScenePosition() const;
        virtual int GetTextureWidth() const;
        virtual int GetTextureHeight() const;
        bool IsPositionInTexture(const ScreenPosition sp) const; 
        bool IsPositionInTexture(const ScenePosition sp) const;
        virtual void DrawTexture() const = 0;
        void SetScenePosition(const ScenePosition sp);
        void AddScenePosition(const ScenePosition sp);
        void LookMe();
};