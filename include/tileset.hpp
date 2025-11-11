#pragma once

#include <algorithm>
#include <iostream>

#include "drawable.hpp"
#include "file.hpp"
#include "type.hpp"

// Should have a TilesetEditor subclass
class Tileset : public Drawable
{
    private:
        std::vector<TilesetData> m_tilesets;
        const FileReader* m_file_reader;
        unsigned int m_tile_size; // All loaded file in a Tileset instance must have the same tile_size
        unsigned int m_current_tileset;

        TilesetData GetTilesetData() const;
        std::string GetHeaderForTileset(const std::string& tileset_filepath) const;
        void LoadTilesetHeader(const std::string& tileset_header, TilesetData& data);

    public:
        Tileset(TextureController* texture_controller, const FileReader* file_reader, const std::string& tileset_filepath,
            const ScreenPosition position={0,0}, const bool should_draw=false);
        ~Tileset();
        
        unsigned int GetTileSize() const;
        unsigned int GetTilesetWidth() const;
        unsigned int GetTilesetHeight() const;
        bool IsEmptyTile(const unsigned char tile) const;
        void UpdateSelectedTile(const ScreenPosition position, unsigned char& tile) const;
        void LoadTileset(const std::string& texture_filepath);
        void NextTileset();
};