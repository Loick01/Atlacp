#include "tilemap.hpp"

Tilemap::Tilemap(TextureController* texture_controller, const FileReader* file_reader, const std::string& map_filepath, const std::string& tileset_filepath, const Offset offset) :
    Drawable(texture_controller, tileset_filepath, offset), m_file_reader(file_reader)
{
    LoadMap(map_filepath);
}

Tilemap::~Tilemap()
{

}

unsigned char Tilemap::GetTileFromMapPosition(const MapPosition p) const
{
    return m_map.map[p.y*m_map.width+p.x];
}

unsigned int Tilemap::GetTotalWidth() const
{
    return m_map.width*m_map.tile_size;
}

unsigned int Tilemap::GetTotalHeight() const
{
    return m_map.height*m_map.tile_size;
}

bool Tilemap::IsMapPositionEmpty(const MapPosition p) const
{
    if (p.x < 0 || p.x >= m_map.width || p.y < 0 || p.y >= m_map.height) // This should not be here, but in GetTileFromMapPosition
        return false;
    const unsigned char tile = GetTileFromMapPosition(p);
    static const std::set<unsigned char> solid = {1, 3}; // Will be read in a txt file + could use a unordered set
    return solid.find(tile) == solid.end();
}

void Tilemap::LoadTileset(const std::string& tileset_filepath)
{
   Drawable::LoadTexture(tileset_filepath);
}

void Tilemap::LoadMap(const std::string& filepath)
{
    m_map = m_file_reader->GetMapFromFile(filepath); 
}

void Tilemap::DrawTexture() const
{
    std::vector<unsigned char> map = m_map.map;
    int tile_size = static_cast<int>(m_map.tile_size);
    for (unsigned int i = 0 ; i < map.size() ; i++)
    {
        const SDL_Rect src{(map[i]%3)*tile_size, (map[i]/3)*tile_size, tile_size, tile_size};
        const SDL_Rect dst{static_cast<int>(m_offset.d_x+(i%m_map.width)*tile_size),
                           static_cast<int>(m_offset.d_y+(i/m_map.width)*tile_size), tile_size, tile_size};
        m_texture_controller->RenderTexture(m_texture_key, src, dst);
    }
}