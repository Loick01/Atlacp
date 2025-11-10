#pragma once

#include <iostream>

#include "drawable.hpp"
#include "file.hpp"
#include "type.hpp"

// Should have a TilesetEditor subclass
class Tileset : public Drawable
{
    private:
        const FileReader* m_file_reader;
        std::string m_tileset_filepath;
        TilesetData m_tileset_data;
        unsigned int m_tile_size; // All loaded file in a Tileset instance must have the same tile_size
        unsigned char m_selected_tile; // Could be in EditorEventController ?

        std::string GetHeaderFromFilepath() const;
        void LoadTexture(const std::string& texture_filepath) override;
        void LoadTilesetHeader(const std::string& tileset_header);

    public:
        Tileset(TextureController* texture_controller, const FileReader* file_reader, const std::string& tileset_filepath="",
            const ScreenPosition position={0,0}, const bool should_draw=false);
        ~Tileset();
        
        std::string GetTilesetFilepath() const;
        unsigned int GetTileSize() const;
        unsigned int GetSelectedTile() const;
        unsigned int GetTilesetWidth() const;
        unsigned int GetTilesetHeight() const;
        bool IsEmptyTile(const unsigned char tile) const;
        void SetSelectedTile(unsigned int selected_tile);
        void UpdateSelectedTile(const ScreenPosition position);
};