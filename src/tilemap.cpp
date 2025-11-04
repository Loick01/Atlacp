#include "tilemap.hpp"

Tilemap::Tilemap(const TextureController* texture_controller, FileReader* file_reader, const std::string& map_filepath, const std::string& tileset_filepath) : // Must initialize a map from a file
    m_file_reader(file_reader), m_tileset_filepath(tileset_filepath), m_texture_controller(texture_controller)
{
    LoadMap(map_filepath);
}

Tilemap::~Tilemap()
{

}

void Tilemap::LoadMap(const std::string& filepath)
{
    m_map = m_file_reader->GetMapFromFile(filepath, m_map_width, m_map_height, m_tile_size); 
    // Later, this function will build a vector<Tile>
}

void Tilemap::DrawMap() const
{
    for (unsigned short i = 0 ; i < m_map.size() ; i++) // maximum map size (unsigned char * unsigned char) can't exceed the range of unsigned short
    {
        const SDL_Rect src{(m_map[i]%3)*m_tile_size,(m_map[i]/3)*m_tile_size,m_tile_size,m_tile_size};
        const SDL_Rect dst{(i%m_map_width)*m_tile_size,(i/m_map_width)*m_tile_size,m_tile_size,m_tile_size};
        m_texture_controller->RenderTexture(m_tileset_filepath, src, dst);
    }
}