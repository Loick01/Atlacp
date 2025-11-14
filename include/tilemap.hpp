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
        Tileset* m_tileset;
        MapData m_map_data;

        MapPosition GetProjectedPosition(const MapPosition p, const MapBound bound) const;
        MapBound IsOutOfMap(const MapPosition p) const;
        unsigned char GetTileAt(const MapPosition p) const;

    public:
        Tilemap(TextureController* texture_controller, const FileReader* file_reader, Tileset* tileset, 
            const std::string& map_filepath, const ScreenPosition position={0,0}, const bool should_draw=true);
        ~Tilemap();

        int GetTextureWidth() const override;
        int GetTextureHeight() const override;
        int GetTileSize() const;
        bool CheckNewPosition(MapPosition& p);
        void SetTileAt(const unsigned char new_tile, const MapPosition p);
        void LoadMap(const std::string& filepath);
        void DrawTexture() const override;
        void ReplaceTileAt(const ScreenPosition position, const unsigned char new_tile);
        void SaveMap(const std::string &map_filepath) const;
};