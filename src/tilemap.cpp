#include "tilemap.hpp"

Tilemap::Tilemap(TextureController* texture_controller, const FileReader* file_reader, const std::string& map_filepath, const std::string& tileset_filepath) :
    Drawable(texture_controller, tileset_filepath), m_file_reader(file_reader)
{
    LoadMap(map_filepath);
}

Tilemap::~Tilemap()
{

}

unsigned char Tilemap::GetTileFromPosition(const Position p) const
{
    return m_map.map[p.y*m_map.width+p.x];
}

bool Tilemap::IsPositionEmpty(const Position p) const
{
    const unsigned char tile = GetTileFromPosition(p);
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
    unsigned char tile_size = m_map.tile_size;
    for (unsigned short i = 0 ; i < map.size() ; i++) // maximum map size (unsigned char * unsigned char) can't exceed the range of unsigned short
    {
        const SDL_Rect src{(map[i]%3)*tile_size,(map[i]/3)*tile_size,tile_size,tile_size};
        const SDL_Rect dst{(i%m_map.width)*tile_size,(i/m_map.width)*tile_size,tile_size,tile_size};
        m_texture_controller->RenderTexture(m_texture_filepath, src, dst);
    }
}