#include "tilemap.hpp"

Tilemap::Tilemap(TextureController* texture_controller, const FileReader* file_reader, const std::string& map_filepath, 
    const std::string& tileset_filepath, const ScreenPosition screen_position) :
    Drawable(texture_controller, tileset_filepath, screen_position), m_file_reader(file_reader)
{
    LoadTilesetHeader("../tileset.txt");
    LoadMap(map_filepath);
}

Tilemap::~Tilemap()
{

}

TilesetData Tilemap::GetTilesetData() const
{
    return m_tileset_data;
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
    return m_map_data.width*m_tileset_data.tile_size;
}

unsigned int Tilemap::GetTextureHeight() const
{
    return m_map_data.height*m_tileset_data.tile_size;
}

unsigned int Tilemap::GetTileSize() const
{
    return m_tileset_data.tile_size;
}

bool Tilemap::IsMapPositionEmpty(const MapPosition p) const
{
    if (p.x < 0 || p.x >= m_map_data.width || p.y < 0 || p.y >= m_map_data.height) // This should not be here, but in GetTileAt
        return false;
    const unsigned char tile = GetTileAt(p);
    return m_tileset_data.solid_tiles.find(tile) == m_tileset_data.solid_tiles.end();
}

void Tilemap::LoadTileset(const std::string& tileset_filepath)
{
   Drawable::LoadTexture(tileset_filepath);
}

void Tilemap::LoadTilesetHeader(const std::string& tileset_header)
{
    m_file_reader->GetInfoFromTileset(tileset_header, m_tileset_data);
}

void Tilemap::LoadMap(const std::string& filepath)
{
    m_file_reader->GetMapFromFile(filepath, m_map_data); 
}

void Tilemap::DrawTexture() const
{
    std::vector<unsigned char> map = m_map_data.map;
    int tile_size = static_cast<int>(m_tileset_data.tile_size);
    int tileset_width = static_cast<int>(m_tileset_data.width);
    int map_width = static_cast<int>(m_map_data.width);
    for (unsigned int i = 0 ; i < map.size() ; i++){
        const SDL_Rect src{(map[i]%tileset_width)*tile_size, (map[i]/tileset_width)*tile_size, tile_size, tile_size};
        const SDL_Rect dst{static_cast<int>(m_screen_position.x+(i%map_width)*tile_size),
                           static_cast<int>(m_screen_position.y+(i/map_width)*tile_size), tile_size, tile_size};
        m_texture_controller->RenderTexture(m_texture_key, src, dst);
    }
}