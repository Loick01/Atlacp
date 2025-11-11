#pragma once 

#include <iostream>
#include <string>
#include <vector>

#include "drawable.hpp"
#include "file.hpp"
#include "tileset.hpp"
#include "type.hpp"

class Tilemap : public Drawable
{
    private:
        const FileReader* m_file_reader;
        const Tileset* m_tileset;
        MapData m_map_data;

        unsigned char GetTileAt(const MapPosition p) const;
        bool IsOutOfMap(const MapPosition p) const;

    public:
        Tilemap(TextureController* texture_controller, const FileReader* file_reader, const Tileset* tileset, 
            const std::string& map_filepath, const ScreenPosition position={0,0}, const bool should_draw=true);
        ~Tilemap();

        unsigned int GetTextureWidth() const override;
        unsigned int GetTextureHeight() const override;
        unsigned int GetTileSize() const;
        bool IsMapPositionEmpty(const MapPosition p) const;
        void SetTileAt(const unsigned char new_tile, const MapPosition p);
        void LoadMap(const std::string& filepath);
        void DrawTexture() const override;
        void ReplaceTileAt(const ScreenPosition position, const unsigned char new_tile);
};