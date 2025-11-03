#include "tilemap.hpp"

Tilemap::Tilemap(FileReader* file_reader, const char* map_filepath, char* tileset_filepath) :// Must initialize a map from a file
    m_file_reader(file_reader), m_tileset_filepath(tileset_filepath)
{
    LoadMap(map_filepath);
}

Tilemap::~Tilemap()
{

}

void Tilemap::LoadMap(const char* filepath)
{
    m_map = m_file_reader->GetMapFromFile(filepath, m_map_width, m_map_height, m_tile_size); 
    // Later, this function will build a vector<Tile>
}

void Tilemap::DrawMap(const TextureController* texture_controller)
{
    for (unsigned int i = 0 ; i < m_map.size() ; i++)
    {
        const SDL_Rect src{(m_map[i]%3)*m_tile_size,(m_map[i]/3)*m_tile_size,m_tile_size,m_tile_size};
        const SDL_Rect dst{(i%m_map_width)*m_tile_size,(i/m_map_width)*m_tile_size,m_tile_size,m_tile_size};
        texture_controller->RenderTexture(m_tileset_filepath, src, dst);
    }
}