#include "tileset.hpp"

Tileset::Tileset(TextureController* texture_controller, const FileReader* file_reader, const std::string& tileset_filepath, 
    const ScreenPosition position, const bool should_draw) :
    Drawable(texture_controller, tileset_filepath, position, should_draw), m_file_reader(file_reader)
{
    LoadTilesetHeader("../tileset.txt");
    m_selected_tile = 0;
}

Tileset::~Tileset()
{

}

unsigned int Tileset::GetTileSize() const
{
    return m_tile_size;
}

unsigned int Tileset::GetSelectedTile() const
{
    return m_selected_tile;
}

unsigned int Tileset::GetTilesetWidth() const
{
    return m_tileset_data.width;
}

unsigned int Tileset::GetTilesetHeight() const
{
    return m_tileset_data.height;
}

bool Tileset::IsEmptyTile(const unsigned char tile) const
{
    return m_tileset_data.solid_tiles.find(tile) == m_tileset_data.solid_tiles.end();
}

void Tileset::SetSelectedTile(unsigned int selected_tile)
{
    m_selected_tile = selected_tile;
}

void Tileset::LoadTilesetHeader(const std::string& tileset_header)
{
    m_file_reader->ReadTilesetHeader(tileset_header, m_tileset_data);
    m_tile_size = m_tileset_data.tile_size; // Use m_tileset_data.tile_size for m_tile_size only when loading the first tileset
    /*
    if (m_tile_size != m_tileset_data.tile_size)
    {
        std::cout << "All tilesets must have the same tile_size\n";
    }
    */
}