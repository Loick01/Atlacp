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
        int m_tile_size; // All loaded file in a Tileset instance must have the same tile_size
        int m_index_tileset;

        TilesetData GetTilesetData() const;
        std::string GetHeaderForTileset(const std::string& tileset_filepath) const;
        void LoadTilesetHeader(const std::string& tileset_header, TilesetData& data);

    public:
        Tileset(TextureController* texture_controller, const FileReader* file_reader,
            const ScreenPosition position={0,0}, const bool should_draw=false);
        ~Tileset();
        
        TextureKey GetTextureKey() const override;
        int GetTilesetsSize() const;
        int GetTileSize() const;
        int GetTilesetWidth() const;
        int GetTilesetWidth(const int selected_tileset) const;
        int GetTilesetHeight() const;
        unsigned char GetNormalizedTile(const unsigned char tile);
        bool IsEmptyTile(const unsigned char tile);
        void UpdateSelectedTile(const ScreenPosition position, const int selected_tileset, unsigned char& tile) const;
        void LoadTileset(const std::string& texture_filepath);
        void SetDisplayedTileset(const int selected_tileset);
        void CleanTilesets();
};