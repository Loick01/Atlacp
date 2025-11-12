#include "tilemap.hpp"

Tilemap::Tilemap(TextureController* texture_controller, const FileReader* file_reader, Tileset* tileset, 
    const std::string& map_filepath, const ScreenPosition screen_position, const bool should_draw) :
    Drawable(texture_controller, screen_position, should_draw), m_file_reader(file_reader), m_tileset(tileset)
{
    LoadMap(map_filepath);
    //m_texture_key = tileset->GetTextureKey();
}

Tilemap::~Tilemap()
{

}

unsigned int Tilemap::GetTileSize() const
{
    return m_tileset->GetTileSize();
}

unsigned char Tilemap::GetTileAt(const MapPosition p) const
{
    return m_map_data.map[p.y*m_map_data.width+p.x];
}

void Tilemap::SetTileAt(const unsigned char new_tile, const MapPosition p)
{
    m_map_data.map[p.y*m_map_data.width+p.x] = new_tile;
}

unsigned int Tilemap::GetTextureWidth() const
{
    return m_map_data.width*m_tileset->GetTileSize();
}

unsigned int Tilemap::GetTextureHeight() const
{
    return m_map_data.height*m_tileset->GetTileSize();
}

bool Tilemap::IsOutOfMap(const MapPosition p) const
{
    return p.x < 0 || p.x >= m_map_data.width || p.y < 0 || p.y >= m_map_data.height;
}

bool Tilemap::IsMapPositionEmpty(const MapPosition p) const
{
    if (IsOutOfMap(p)) return false;
    const unsigned char tile = GetTileAt(p);
    return m_tileset->IsEmptyTile(tile);
}

void Tilemap::LoadMap(const std::string& filepath)
{
    // When a new map will be load, useless tilesets will have to be remove

    m_file_reader->GetMapFromFile(filepath, m_map_data);

    // Load tilesets read in the header of the map file
    for (const std::string& tileset_filepath : m_map_data.tilesets)
        m_tileset->LoadTileset(tileset_filepath);
}

void Tilemap::DrawTexture() const
{
    std::vector<unsigned char> map = m_map_data.map;
    int tile_size = static_cast<int>(m_tileset->GetTileSize());
    int map_width = static_cast<int>(m_map_data.width);
    for (unsigned int i = 0 ; i < map.size() ; i++){
        int tile = m_tileset->GetNormalizedTile(map[i]);
        int tileset_width = static_cast<int>(m_tileset->GetTilesetWidth());
        const SDL_Rect src{(tile%tileset_width)*tile_size, (tile/tileset_width)*tile_size, tile_size, tile_size};
        const SDL_Rect dst{static_cast<int>(m_screen_position.x+(i%map_width)*tile_size),
                           static_cast<int>(m_screen_position.y+(i/map_width)*tile_size), tile_size, tile_size};
        m_texture_controller->RenderTexture(m_tileset->GetTextureKey(), src, dst);
    }
}

void Tilemap::ReplaceTileAt(const ScreenPosition position, const unsigned char new_tile)
{
    if (IsPositionInTexture(position)){
        const unsigned int tile_size = m_tileset->GetTileSize();
        int c = position.x/tile_size;
        int l = position.y/tile_size;
        SetTileAt(new_tile,{c,l});
    }
}