#pragma once

#include <vector>

#include "image/drawable.hpp"
#include "tile/tile_types.hpp" // Tile, DataTileset, TilesetNormalizationInfo

class FileReader;
class TextureController;

class Tileset : public ScreenDrawable // TODO : Could be a UiElement instead of ScreenDrawable ?
{
    private:
        std::vector<DataTileset> m_tilesets;
        TilesetNormalizationInfo m_normalizationInfo;
        int m_tileSize; // All loaded file in a Tileset instance must have the same tileSize
        size_t m_indexTileset;

        DataTileset GetDataTileset() const;
        
    public:
        Tileset(TextureController& textureController);
        ~Tileset();
        
        TextureKey GetTextureKey() const override;
        int GetTilesetsSize() const;
        int GetTileSize() const;
        int GetTilesetWidth() const;
        int GetTilesetWidth(const int tilesetIndex) const;
        int GetTilesetHeight() const;
        Tile GetNormalizedTile(const Tile tile); // TODO++ : Rename (it also update m_indexTileset)
        bool IsEmptyTile(const Tile tile);
        void UpdateSelectedTile(const ScreenPosition sp, const int tilesetIndex, Tile& tile) const;
        void LoadTileset(const FileReader& fileReader, const std::string& path);
        void SetDisplayedTileset(const int tilesetIndex);
        // If shouldDeleteCurrent is true, the texture with key = m_textureKey is Deleted,
        // otherwise this texture must be deleted by ~Drawable()
        void CleanTilesets(const bool shouldDeleteCurrent);
};