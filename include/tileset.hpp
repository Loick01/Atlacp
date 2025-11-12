#pragma once

#include <iostream>

#include "drawable.hpp"
#include "file.hpp"
#include "type.hpp"

// Should have a TilesetEditor subclass
class Tileset : public Drawable
{
    private:
        std::vector<TilesetData> m_tilesets;
        TilesetNormalizationInfo m_normalization_info;
        const FileReader* m_file_reader;
        unsigned int m_tile_size; // All loaded file in a Tileset instance must have the same tile_size
        int m_index_tileset;

        TilesetData GetTilesetData() const;
        std::string GetHeaderForTileset(const std::string& tileset_filepath) const;
        void LoadTilesetHeader(const std::string& tileset_header, TilesetData& data);

    public:
        Tileset(TextureController* texture_controller, const FileReader* file_reader,
            const ScreenPosition position={0,0}, const bool should_draw=false);
        ~Tileset();
        
        TextureKey GetTextureKey() const override;
        unsigned int GetTileSize() const;
        unsigned int GetTilesetWidth() const;
        unsigned int GetTilesetHeight() const;
        bool IsEmptyTile(const unsigned char tile); // Should be const
        void UpdateSelectedTile(const ScreenPosition position, unsigned char& tile) const;
        void LoadTileset(const std::string& texture_filepath);
        void NextTileset();
        unsigned char GetNormalizedTile(const unsigned char tile);
};