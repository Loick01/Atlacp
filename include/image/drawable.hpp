#pragma once

#include "core/core_types.hpp" // Vec2
#include "image/image_types.hpp" // TextureKey, ScenePosition, ScreenPosition

class TextureController;

class Drawable
{
    protected:
        // textureFilepath could be replace by a RessourceFile struct, which contains the path without its extension
        Drawable(TextureController& textureController, const std::string& textureFilepath);
        Drawable(TextureController& textureController);
        // If this class doesn't stay abstract, will need to delete m_textureKey in its destructor (textureController->DeleteTexture(textureKey))
        // If that happens, beware of every destructor of derivated class from Drawable
        
        TextureController& m_textureController;
        TextureKey m_textureKey;
        int m_textureWidth;
        int m_textureHeight;
    
    public:
        virtual ~Drawable();
        virtual TextureKey GetTextureKey() const;
        virtual int GetTextureWidth() const; // Remove virtual ?
        virtual int GetTextureHeight() const; // Remove virtual ?
        virtual void DrawTexture() const = 0;

        bool IsPositionInTexture(const Vec2 sp) const; // Should not use Vec2 for the parameter ?
        void LoadTexture(const std::string& textureFilepath); // Load a new texture and update the m_textureKey
        void DeleteTexture();
};

class SceneDrawable : public Drawable
{
    protected:
        ScenePosition m_position;
        ScenePosition m_displayOffset; // Should be in MapEntity

        void SetDisplayOffset(const ScenePosition offset);
    
    public:
        SceneDrawable(TextureController& textureController, const std::string& textureFilepath, const ScenePosition position);
        
        ScenePosition GetDisplayOffset() const;
        ScenePosition GetScenePosition() const;
};

class ScreenDrawable : public Drawable
{
    protected:
        ScreenPosition m_position; // For class that inherit from ScreenDrawable and have a scene graph concept (ex: UiElement), this is the global position 
        float m_zoom; // Don't use camera zoom, but this value instead. Could be private

        // Will be removed ?
        bool m_shouldDraw; // Could be private

    public:
        ScreenDrawable(TextureController& textureController, const std::string& textureFilepath, const ScreenPosition position={0, 0}, const bool shouldDraw=true);
        ScreenDrawable(TextureController& textureController, const ScreenPosition position={0, 0}, const bool shouldDraw=true);

        ScreenPosition GetScreenPosition() const;
        ScreenPosition GetSize() const;
        float GetZoom() const;
        bool GetShouldDraw() const;
        void SetZoom(const float zoom);
        void DrawTexture() const override;
        void SetScreenPosition(const ScreenPosition position);
        void InvertShouldDraw();
};