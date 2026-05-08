#pragma once

#include "drawable.hpp"
#include "type.hpp"

class FileReader;

class Tileset : public ScreenDrawable // Could be a UiElement
{
    private:
        std::vector<TilesetData> m_tilesets;
        TilesetNormalizationInfo m_normalizationInfo;
        int m_tileSize; // All loaded file in a Tileset instance must have the same tileSize
        size_t m_indexTileset;

        TilesetData GetTilesetData() const;
        
    public:
        Tileset(TextureController& textureController);
        ~Tileset();
        
        TextureKey GetTextureKey() const override;
        int GetTilesetsSize() const;
        int GetTileSize() const;
        int GetTilesetWidth() const;
        int GetTilesetWidth(const int selectedTileset) const;
        int GetTilesetHeight() const;
        Tile GetNormalizedTile(const Tile tile);
        bool IsEmptyTile(const Tile tile);
        void UpdateSelectedTile(const ScreenPosition sp, const int selectedTileset, Tile& tile) const;
        void LoadTileset(const FileReader& fileReader, const std::string& path);
        void SetDisplayedTileset(const int selectedTileset);
        // if shouldDeleteCurrent is true, the texture with key = m_textureKey is Deleted,
        // otherwise this texture must be deleted by ~Drawable()
        void CleanTilesets(const bool shouldDeleteCurrent);
};