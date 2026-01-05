#include "tilemap.hpp"

Tilemap::Tilemap(TextureController& texture_controller, const FileReader& file_reader, Tileset& tileset, 
    const std::string& world_filepath, Camera& camera) :
    SceneDrawable(texture_controller, camera, ScenePosition{0,0}), m_file_reader(file_reader), m_tileset(tileset)
{
    m_world_data = m_file_reader.ReadWorldFile(world_filepath);
    m_current_map = m_world_data.start_map; // The first loaded map is specified in the world file
    LoadMap(m_world_data.maps[m_current_map]); 
}

MapPosition Tilemap::GetSpawnPosition() const
{
    return m_map_data.spawn_position; // Return the spawn position of the loaded map (could be -1 if undefined)
}

int Tilemap::GetTileSize() const
{
    return m_tileset.GetTileSize();
}

unsigned int Tilemap::GetTileIndex(const MapPosition p) const
{
    return p.y*m_map_data.size.x+p.x;
}

void Tilemap::SetTileAt(const size_t layer, const Tile new_tile, const MapPosition p)
{
    m_map_data.map[layer].SetTile(GetTileIndex(p), new_tile);
}

// Because of EditorEventController::GetMouseScenePosition, Tilemap::GetTextureWidth should not use camera zoom
int Tilemap::GetTextureWidth() const
{
    return m_map_data.size.x*m_tileset.GetTileSize();
}

int Tilemap::GetTextureHeight() const
{
    return m_map_data.size.y*m_tileset.GetTileSize();
}

MapBound Tilemap::IsOutOfMap(const MapPosition p) const
{
    if (p.x < 0) return MapBound::OutLeft;
    if (p.x >= m_map_data.size.x) return MapBound::OutRight;
    if (p.y < 0) return MapBound::OutUp;
    if (p.y >= m_map_data.size.y) return MapBound::OutDown;
    return MapBound::Inside;
}

void Tilemap::LoadAdjacentMap(const MapBound bound) // This function is used only in editor
{
    // When loading a new map, no verifications are made to check if the code tries to reach an out-of-world map.
    // Maps are supposed to be designed in such a way the player can't get out of the world.
    switch (bound){
        case MapBound::OutUp:
            m_current_map -= m_world_data.size.x;
            break;
        case MapBound::OutDown:
            m_current_map += m_world_data.size.x;
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

std::vector<bool> Tilemap::GetOccupancyGrid() const
{
    return m_map_data.occupancy_grid;
}

MapPosition Tilemap::GetProjectedPosition(const MapPosition p, const MapBound bound)
{
    LoadAdjacentMap(bound); // Should not be here

    MapPosition projected_position = p;
    switch (bound){
        case MapBound::OutUp:
            projected_position.y = m_map_data.size.y-1;
            break;
        case MapBound::OutDown:
            projected_position.y = 0;
            break;
        case MapBound::OutRight:
            projected_position.x = 0;
            break;
        case MapBound::OutLeft:
            projected_position.x = m_map_data.size.x-1;
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

int Tilemap::GetGridSize() const
{
    return m_map_data.size.x*m_map_data.size.y;
}

bool Tilemap::IsFreePosition(MapPosition& p)
{
    return m_map_data.occupancy_grid[GetTileIndex(p)];
}

void Tilemap::LoadMap(const std::string& path)
{
    // I don't think I should delete all tilesets when loading a new map
    // Instead, I could only delete the unused one
    m_tileset.CleanTilesets(); // Delete tilesets used for the previous map

    // TileLayer are created in GetMapFromFile. Because they are SceneDrawable, they need camera and texture controller
    m_map_data = m_file_reader.GetMapFromFile(path, m_camera, m_texture_controller);
    
    // Load tilesets read in the header of the map file
    for (const std::string& p : m_map_data.tilesets)
        m_tileset.LoadTileset(p);

    const GridSize map_size = m_map_data.size;
    for (size_t j=0 ; j<map_size.y ; j++){
        for (size_t i=0 ; i<map_size.x ; i++){
            bool is_free = true;
            for (size_t layer=0 ; layer<m_map_data.layer_count ; layer++){ // I think it would better if this loop was before for(i)/for(j)
                const Tile t = m_map_data.map[layer].GetTile(j*map_size.x+i);
                is_free = m_tileset.IsEmptyTile(t);
                if (!is_free) break;
            }
            m_map_data.occupancy_grid.push_back(is_free);
        }
    }
    
    m_camera.SetTilemapInfo(ScenePosition{GetTextureWidth(),GetTextureHeight()}, m_tileset.GetTileSize());
}

void Tilemap::DrawTexture() const
{
    int tile_size = m_tileset.GetTileSize();
    const GridSize map_size = m_map_data.size;
    ScenePosition camera_position = m_camera.GetPosition();
    const float zoom = m_camera.GetZoom();

    // Culling
    // While animating a movement, end_index could not be enough to fill the window with the map
    // So I add 1 to end_index, and check if it becomes greater than map size
    Pair<int> start_index = Pair<int>{0, 0}; // Should use something else than Pair<int>
    Pair<int> end_index = map_size; // Same

    if (m_should_culling){ // No map culling in editor (find better way than just use a bool ?)
        // Should be in a function in Camera ?
        if (m_camera.GetIsOffScreen().x){
            start_index.x = camera_position.x/(tile_size*zoom);
            end_index.x = std::min(end_index.x, start_index.x + m_camera.GetRangeTile().x + 1);
        }
        if (m_camera.GetIsOffScreen().y){
            start_index.y = camera_position.y/(tile_size*zoom);
            end_index.y = std::min(end_index.y, start_index.y + m_camera.GetRangeTile().y + 1);
        }
    }
    camera_position = camera_position-m_camera.GetScreenOffset();

    for (size_t layer=0 ; layer<m_map_data.layer_count ; layer++){
        std::vector<Tile> tiles = m_map_data.map[layer].GetTiles();
        for (int j = start_index.y ; j < end_index.y ; j++){
            for (int i = start_index.x ; i < end_index.x ; i++){
                int tile = m_tileset.GetNormalizedTile(tiles[j*map_size.x+i]); // Should use Tile type ?
                int tileset_width = m_tileset.GetTilesetWidth();
                const SDL_Rect src{(tile%tileset_width)*tile_size, (tile/tileset_width)*tile_size, tile_size, tile_size};
                const int tile_screen_size = static_cast<int>(tile_size*zoom+1);
                const ScreenPosition dst_position = (Vec2{i,j}*tile_size)*zoom-camera_position;
                const SDL_Rect dst{dst_position.x, dst_position.y, tile_screen_size, tile_screen_size};
                m_texture_controller.RenderTexture(m_tileset.GetTextureKey(), src, dst);
            }
        }
    }
}

void Tilemap::ReplaceTileAt(const ScenePosition sp, const size_t layer, const Tile new_tile)
{
    if (IsPositionInTexture(sp)){ // sp must be normalized (with scene position)
        const int tile_size = m_tileset.GetTileSize();
        int c = sp.x/tile_size;
        int l = sp.y/tile_size;
        SetTileAt(layer,new_tile,{c,l});
    }
}

void Tilemap::SaveMap(const std::string &map_filepath) const
{
    m_file_reader.SaveMapFile(map_filepath, m_map_data);
}

void Tilemap::SetShouldCulling(const bool should_culling)
{
    m_should_culling = should_culling;
}