#pragma once

#include <iostream>

#include "drawable.hpp"
#include "type.hpp"

class FileReader;

class Tileset : public ScreenDrawable // Could be a UiElement
{
    private:
        std::vector<TilesetData> m_tilesets;
        TilesetNormalizationInfo m_normalization_info;
        int m_tile_size; // All loaded file in a Tileset instance must have the same tile_size
        size_t m_index_tileset;

        TilesetData GetTilesetData() const;
        
    public:
        Tileset(TextureController& texture_controller);
        ~Tileset();
        
        TextureKey GetTextureKey() const override;
        int GetTilesetsSize() const;
        int GetTileSize() const;
        int GetTilesetWidth() const;
        int GetTilesetWidth(const int selected_tileset) const;
        int GetTilesetHeight() const;
        Tile GetNormalizedTile(const Tile tile);
        bool IsEmptyTile(const Tile tile);
        void UpdateSelectedTile(const ScreenPosition sp, const int selected_tileset, Tile& tile) const;
        void LoadTileset(const FileReader& file_reader, const std::string& path);
        void SetDisplayedTileset(const int selected_tileset);
        void CleanTilesets();
};