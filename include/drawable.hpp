#pragma once

#include <iostream>

#include "camera.hpp"
#include "texture.hpp"

class Drawable
{
    protected:
        // string texture_filepath could be replace by a RessourceFile struct, which contains the path without its extension
        Drawable(TextureController& texture_controller, const std::string& texture_filepath);
        Drawable(TextureController& texture_controller);
        // If this class doesn't stay abstract, will need to delete m_texture_key in its destructor (texture_controller->DeleteTexture(texture_key))
        // If that happens, beware of every destructor of derivated class from Drawable
        
        void LoadTexture(const std::string& texture_filepath);

        TextureController& m_texture_controller;
        TextureKey m_texture_key;
        int m_texture_width;
        int m_texture_height;
    
    public:
        ~Drawable();
        virtual TextureKey GetTextureKey() const;
        virtual int GetTextureWidth() const; // Remove virtual ?
        virtual int GetTextureHeight() const; // Remove virtual ?
        bool IsPositionInTexture(const Vec2 sp) const; // Should not use Vec2 for the parameter ?
        virtual void DrawTexture() const = 0;
};

class SceneDrawable : public Drawable
{
    protected:
        ScenePosition m_position;
        ScenePosition m_display_offset; // Should be in Entity
        Camera& m_camera;

        ScenePosition GetDisplayOffset() const;
        void SetDisplayOffset(const ScenePosition offset);
    
    public:
        SceneDrawable(TextureController& texture_controller, const std::string& texture_filepath, Camera& camera, const ScenePosition position);
        SceneDrawable(TextureController& texture_controller, Camera& camera, const ScenePosition position);
        void LookMe();
};

class ScreenDrawable : public Drawable
{
    protected:
        ScreenPosition m_position; // For class that inherit from ScreenDrawable and have a scene graph concept (ex: UiElement), this is the global position 
        float m_zoom; // Don't use camera zoom, but this value instead. Could be private

        // Will be removed ?
        bool m_should_draw; // Could be private

    public:
        ScreenDrawable(TextureController& texture_controller, const std::string& texture_filepath, const ScreenPosition position={0, 0}, const bool should_draw=true);
        ScreenDrawable(TextureController& texture_controller, const ScreenPosition position={0, 0}, const bool should_draw=true);

        ScreenPosition GetScreenPosition() const;
        ScreenPosition GetSize() const;
        float GetZoom() const;
        bool GetShouldDraw() const;
        void SetZoom(const float zoom);
        void DrawTexture() const override;
        void SetScreenPosition(const ScreenPosition position);
        void InvertShouldDraw();
};