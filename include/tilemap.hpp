#pragma once 

#include <iostream>
#include <string>
#include <vector>

#include "drawable.hpp"
#include "file.hpp"
#include "type.hpp"

class Tilemap : public Drawable
{
    private:
        const FileReader* m_file_reader;
        MapData m_map_data;
        TilesetData m_tileset_data;

        unsigned char GetTileFromMapPosition(const MapPosition p) const;

    public:
        Tilemap(TextureController* texture_controller, const FileReader* file_reader, const std::string& map_filepath, 
            const std::string& tileset_filepath, const ScreenPosition position={0,0});
        ~Tilemap();

        TilesetData GetTilesetData() const;
        unsigned int GetTotalWidth() const;
        unsigned int GetTotalHeight() const;
        unsigned int GetTileSize() const;
        bool IsMapPositionEmpty(const MapPosition p) const;
        void LoadTileset(const std::string& tileset_filepath);
        void LoadTilesetHeader(const std::string& tileset_header);
        void LoadMap(const std::string& filepath);
        void DrawTexture() const override;
};