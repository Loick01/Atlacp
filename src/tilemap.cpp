#include "tilemap.hpp"

Tilemap::Tilemap(TextureController* texture_controller, const FileReader* file_reader, Tileset* tileset, 
    const std::string& map_filepath, const ScreenPosition screen_position, const bool should_draw) :
    Drawable(texture_controller, screen_position, should_draw), m_file_reader(file_reader), m_tileset(tileset)
{
    LoadMap(map_filepath);
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
    if (p.y < 0) return MapBound::OutTop;
    if (p.y >= m_map_data.height) return MapBound::OutBottom;
    return MapBound::Inside;
}

MapPosition Tilemap::GetProjectedPosition(const MapPosition p, const MapBound bound) const
{
    switch (bound){
        case MapBound::OutTop:
            return {p.x,m_map_data.height-1};
        case MapBound::OutBottom:
            return {p.x,0};
        case MapBound::OutRight:
            return {0,p.y};
        case MapBound::OutLeft:
            return {m_map_data.width-1,p.y};
    }
    return p;
}

bool Tilemap::CheckNewPosition(MapPosition& p)
{
    MapBound is_in_bound = IsOutOfMap(p);
    if (is_in_bound != MapBound::Inside){
        p = GetProjectedPosition(p, is_in_bound);
        LoadMap("../map2.txt");
    }else{
        const unsigned char tile = GetTileAt(p);
        return m_tileset->IsEmptyTile(tile);
    }
    return true;
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