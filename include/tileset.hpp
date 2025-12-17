#pragma once

#include <iostream>

#include "drawable.hpp"
#include "file.hpp"
#include "type.hpp"

// Tileset is also a Drawable, but it uses screen coordinates (m_screen_position) instead of Drawable::m_scene_position to render
class Tileset : public Drawable
{
    private:
        std::vector<TilesetData> m_tilesets;
        TilesetNormalizationInfo m_normalization_info;
        const FileReader& m_file_reader;
        ScreenPosition m_screen_position;
        int m_tile_size; // All loaded file in a Tileset instance must have the same tile_size
        int m_index_tileset;
        bool m_should_draw; // Used in editor only

        TilesetData GetTilesetData() const;
        
    public:
        Tileset(TextureController& texture_controller, Camera& camera, const FileReader& file_reader, const bool should_draw=false);
        ~Tileset();
        
        TextureKey GetTextureKey() const override;
        ScreenPosition GetScreenPosition() const; // Could be in a ScreenDrawable class
        int GetTilesetsSize() const;
        int GetTileSize() const;
        int GetTilesetWidth() const;
        int GetTilesetWidth(const int selected_tileset) const;
        int GetTilesetHeight() const;
        Tile GetNormalizedTile(const Tile tile);
        bool IsEmptyTile(const Tile tile);
        bool GetShouldDraw() const;
        void InvertShouldDraw();
        void UpdateSelectedTile(const ScreenPosition sp, const int selected_tileset, Tile& tile) const;
        void LoadTileset(const std::string& path);
        void SetDisplayedTileset(const int selected_tileset);
        void CleanTilesets();
        void DrawTexture() const override;
        void SetScreenPosition(const ScreenPosition sp);
};