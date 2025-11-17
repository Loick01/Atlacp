#include "tilemap.hpp"

Tilemap::Tilemap(TextureController* texture_controller, const FileReader* file_reader, Tileset* tileset, 
    const std::string& world_filepath, const ScreenPosition screen_position, const bool should_draw) :
    Drawable(texture_controller, screen_position, should_draw), m_file_reader(file_reader), m_tileset(tileset)
{
    m_world_data.maps = m_file_reader->ReadWorldFile(world_filepath, m_world_data.width, m_world_data.height);
    m_current_map = 0; // Load the first map write in the world file (should be specified in the world file ?)
    LoadMap(m_world_data.maps[m_current_map]); 
}

Tilemap::~Tilemap()
{

}

int Tilemap::GetTileSize() const
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

int Tilemap::GetTextureWidth() const
{
    return m_map_data.width*m_tileset->GetTileSize();
}

int Tilemap::GetTextureHeight() const
{
    return m_map_data.height*m_tileset->GetTileSize();
}

MapBound Tilemap::IsOutOfMap(const MapPosition p) const
{
    if (p.x < 0) return MapBound::OutLeft;
    if (p.x >= m_map_data.width) return MapBound::OutRight;
    if (p.y < 0) return MapBound::OutUp;
    if (p.y >= m_map_data.height) return MapBound::OutDown;
    return MapBound::Inside;
}

void Tilemap::LoadAdjacentMap(const MapBound bound) // This function is used only in editor
{
    // Should I merge it with GetProjectedPosition ?
    switch (bound){
        case MapBound::OutUp:
            m_current_map -= m_world_data.width;
            break;
        case MapBound::OutDown:
            m_current_map += m_world_data.width;
            break;
        case MapBound::OutRight:
            m_current_map += 1;
            break;
        case MapBound::OutLeft:
            m_current_map -= 1;
            break;
    }
    // Error if out of range for world ?
    LoadMap(m_world_data.maps[m_current_map]);
}

MapPosition Tilemap::GetProjectedPosition(const MapPosition p, const MapBound bound)
{
    // When loading a new map, no verifications are made to check if the code tries to reach an out-of-world map.
    // Maps are supposed to be designed in such a way the player can't get out of the world.
    MapPosition projected_position = p;
    switch (bound){
        case MapBound::OutUp:
            m_current_map -= m_world_data.width;
            projected_position.y = m_map_data.height-1;
            break;
        case MapBound::OutDown:
            m_current_map += m_world_data.width;
            projected_position.y = 0;
            break;
        case MapBound::OutRight:
            m_current_map += 1;
            projected_position.x = 0;
            break;
        case MapBound::OutLeft:
            m_current_map -= 1;
            projected_position.x = m_map_data.width-1;
            break;
    }
    LoadMap(m_world_data.maps[m_current_map]);
    return projected_position;
}

bool Tilemap::CheckNewPosition(MapPosition& p)
{
    MapBound is_in_bound = IsOutOfMap(p);
    if (is_in_bound != MapBound::Inside){
        p = GetProjectedPosition(p, is_in_bound);
    }else{
        const unsigned char tile = GetTileAt(p);
        return m_tileset->IsEmptyTile(tile);
    }
    return true;
}

bool Tilemap::CanMoveCamera(const int axis_position, const int axis_new_position) const
{
    const int m_min_limit = 5, m_max_limit = 15; // Should be member of a Camera class
    return (axis_position > m_min_limit || axis_new_position == m_min_limit+1)
        && (axis_position < m_max_limit || axis_new_position == m_max_limit-1);
}

void Tilemap::LoadMap(const std::string& filepath)
{
    m_tileset->CleanTilesets(); // Delete tilesets used for the previous map
    m_file_reader->GetMapFromFile(filepath, m_map_data);
    
    // Load tilesets read in the header of the map file
    for (const std::string& tileset_filepath : m_map_data.tilesets)
        m_tileset->LoadTileset(tileset_filepath);
}

void Tilemap::DrawTexture() const
{
    std::vector<unsigned char> map = m_map_data.map;
    int tile_size = m_tileset->GetTileSize();
    int map_width = m_map_data.width;
    for (int i = 0 ; i < map.size() ; i++){ // i must be an int
        int tile = m_tileset->GetNormalizedTile(map[i]);
        int tileset_width = m_tileset->GetTilesetWidth();
        const SDL_Rect src{(tile%tileset_width)*tile_size, (tile/tileset_width)*tile_size, tile_size, tile_size};
        const SDL_Rect dst{m_screen_position.x+(i%map_width)*tile_size, m_screen_position.y+(i/map_width)*tile_size, tile_size, tile_size};
        m_texture_controller->RenderTexture(m_tileset->GetTextureKey(), src, dst);
    }
}

void Tilemap::ReplaceTileAt(const ScreenPosition position, const unsigned char new_tile)
{
    if (IsPositionInTexture(position)){
        const int tile_size = m_tileset->GetTileSize();
        int c = position.x/tile_size;
        int l = position.y/tile_size;
        SetTileAt(new_tile,{c,l});
    }
}

void Tilemap::SaveMap(const std::string &map_filepath) const
{
    m_file_reader->SaveMapFile(map_filepath, m_map_data);
}