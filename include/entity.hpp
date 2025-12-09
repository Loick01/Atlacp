#pragma once

#include <iostream>

#include "drawable.hpp"
#include "element.hpp"

class Entity : public Drawable, public MapElement
{
    private:
        ScenePosition m_display_offset;

    protected:
        Entity(TextureController* texture_controller, const std::string& sprite_filepath, Camera* camera, const FileReader* file_reader,
            Tilemap* tilemap, const float speed);
        ~Entity();
        
        ScenePosition GetFinalDrawingPosition(const ScenePosition sp) const;
        void SetMapPosition(const MapPosition mp); // Should be in MapElement ?
        void DrawTexture() const override;
};