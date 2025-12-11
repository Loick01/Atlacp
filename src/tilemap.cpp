#include "tilemap.hpp"

Tilemap::Tilemap(TextureController* texture_controller, const FileReader* file_reader, Tileset* tileset, 
    const std::string& world_filepath, Camera* camera, const bool should_culling) :
    Drawable(texture_controller, camera, ScenePosition{0,0}), m_file_reader(file_reader), m_tileset(tileset), m_should_culling(should_culling)
{
    m_world_data = m_file_reader->ReadWorldFile(world_filepath);
    m_current_map = 0; // Load the first map write in the world file (should be specified in the world file ?)
    LoadMap(m_world_data.maps[m_current_map]); 
}

Tilemap::~Tilemap()
{

}

MapPosition Tilemap::GetSpawnPosition() const
{
    return m_map_data.spawn_position; // Return the spawn position of the loaded map (could be -1 if undefined)
}

int Tilemap::GetTileSize() const
{
    return m_tileset->GetTileSize();
}

unsigned int Tilemap::GetTileIndex(const MapPosition p) const
{
    return p.y*m_map_data.width+p.x;
}

void Tilemap::SetTileAt(const Tile new_tile, const MapPosition p)
{
    m_map_data.map[GetTileIndex(p)] = new_tile;
}

int Tilemap::GetTextureWidth() const
{
    return m_map_data.width*m_tileset->GetTileSize()*m_camera->GetZoom();
}

int Tilemap::GetTextureHeight() const
{
    return m_map_data.height*m_tileset->GetTileSize()*m_camera->GetZoom();
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
    // When loading a new map, no verifications are made to check if the code tries to reach an out-of-world map.
    // Maps are supposed to be designed in such a way the player can't get out of the world.
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
    LoadAdjacentMap(bound);

    MapPosition projected_position = p;
    switch (bound){
        case MapBound::OutUp:
            projected_position.y = m_map_data.height-1;
            break;
        case MapBound::OutDown:
            projected_position.y = 0;
            break;
        case MapBound::OutRight:
            projected_position.x = 0;
            break;
        case MapBound::OutLeft:
            projected_position.x = m_map_data.width-1;
            break;
    }
    return projected_position;
}

void Tilemap::TakePosition(const MapPosition p)
{
    m_map_data.occupancy_grid[GetTileIndex(p)] = false;
}

void Tilemap::FreePosition(const MapPosition p)
{
    m_map_data.occupancy_grid[GetTileIndex(p)] = true;
}

bool Tilemap::IsFreePosition(MapPosition& p)
{
    MapBound is_in_bound = IsOutOfMap(p);
    if (is_in_bound != MapBound::Inside)
        p = GetProjectedPosition(p, is_in_bound);
    else
        return m_map_data.occupancy_grid[GetTileIndex(p)];
    return true;
}

bool Tilemap::CanMoveCamera(const int axis_position, const int axis_new_position) const
{
    const int m_min_limit = 5, m_max_limit = 15; // Should be member of a Camera class
    return (axis_position > m_min_limit || axis_new_position == m_min_limit+1)
        && (axis_position < m_max_limit || axis_new_position == m_max_limit-1);
}

void Tilemap::LoadMap(const std::string& path)
{
    m_tileset->CleanTilesets(); // Delete tilesets used for the previous map
    m_map_data = m_file_reader->GetMapFromFile(path);

    // Load tilesets read in the header of the map file
    for (const std::string& p : m_map_data.tilesets)
        m_tileset->LoadTileset(p);

    for (Tile t : m_map_data.map)
        m_map_data.occupancy_grid.push_back(m_tileset->IsEmptyTile(t));

    m_camera->SetTilemapInfo(ScenePosition{GetTextureWidth(),GetTextureHeight()}, m_tileset->GetTileSize());
}

void Tilemap::DrawTexture() const
{
    std::vector<Tile> map = m_map_data.map;
    int tile_size = m_tileset->GetTileSize();
    int map_width = m_map_data.width;
    int map_height = m_map_data.height;
    const ScenePosition camera_position = m_camera->GetCameraPosition();
    const float zoom = m_camera->GetZoom();

    // Culling
    // While animating a movement, end_index could not be enough to fill the window with the map
    // So I add 1 to end_index, and check if it becomes greater than map size
    Pair<int> start_index = Pair<int>{0, 0};
    Pair<int> end_index = Pair<int>{map_width, map_height};

    if (m_should_culling){ // No map culling in editor (find better way than just use a bool ?)
        // Should be in a function in Camera ?
        if (m_camera->GetIsOffScreen().x){
            start_index.x = camera_position.x/(tile_size*zoom);
            end_index.x = std::min(end_index.x, start_index.x + m_camera->GetRangeTile().x + 1);
        }
        if (m_camera->GetIsOffScreen().y){
            start_index.y = camera_position.y/(tile_size*zoom);
            end_index.y = std::min(end_index.y, start_index.y + m_camera->GetRangeTile().y + 1);
        }
    }

    for (int j = start_index.y ; j < end_index.y ; j++){
        for (int i = start_index.x ; i < end_index.x ; i++){
            int tile = m_tileset->GetNormalizedTile(map[j*map_width+i]); // Should use Tile type ?
            int tileset_width = m_tileset->GetTilesetWidth();
            const SDL_Rect src{(tile%tileset_width)*tile_size, (tile/tileset_width)*tile_size, tile_size, tile_size};
            const int tile_screen_size = static_cast<int>(tile_size*zoom+1);
            const Pair<int> dst_position = (Vec2{i,j}*tile_size)*zoom-camera_position;
            const SDL_Rect dst{dst_position.x, dst_position.y, tile_screen_size, tile_screen_size};
            m_texture_controller->RenderTexture(m_tileset->GetTextureKey(), src, dst);
        }
    }
}

void Tilemap::ReplaceTileAt(const ScenePosition sp, const Tile new_tile)
{
    if (IsPositionInTexture(sp)){ // sp must be normalized (with scene position)
        const int tile_size = m_tileset->GetTileSize();
        int c = sp.x/tile_size;
        int l = sp.y/tile_size;
        SetTileAt(new_tile,{c,l});
    }
}

void Tilemap::SaveMap(const std::string &map_filepath) const
{
    m_file_reader->SaveMapFile(map_filepath, m_map_data);
}