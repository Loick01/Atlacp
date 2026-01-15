#include "tilemap.hpp"

Tilemap::Tilemap(TextureController& texture_controller, const FileReader& file_reader, Tileset& tileset, 
    const std::string& world_filepath, Camera& camera, const bool should_culling) :
    m_camera(camera), m_texture_controller(texture_controller), m_file_reader(file_reader), m_tileset(tileset), m_should_culling(should_culling)
{
    m_world_data = m_file_reader.ReadWorldFile(world_filepath);
    m_current_map = m_world_data.start_map; // The first loaded map is specified in the world file
    LoadMap(m_world_data.maps[m_current_map]); 
}

const std::vector<TileLayer>& Tilemap::GetLayers() const
{
    return m_map_data.map;
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

size_t Tilemap::GetLayerCount() const
{
    return m_map_data.layer_count;
}

void Tilemap::LoadMap(const std::string& path)
{
    // I don't think I should delete all tilesets when loading a new map
    // Instead, I could only delete the unused one
    m_tileset.CleanTilesets(); // Delete tilesets used for the previous map

    // TileLayer are created in GetMapFromFile. Because they are SceneDrawable, they need camera and texture controller
    // TileLayer also need a Tileset to be rendered
    m_map_data = m_file_reader.GetMapFromFile(path, m_camera, m_texture_controller, m_tileset, m_should_culling);
    Notify(TilemapEvent::TileLayerUpdate); // Update TileLayer used for rendering in TilemapScene
    
    // Load tilesets read in the header of the map file
    for (const std::string& p : m_map_data.tilesets)
        m_tileset.LoadTileset(m_file_reader, p);

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
    
    m_camera.SetTilemapInfo(m_map_data.size*m_tileset.GetTileSize());
}

bool Tilemap::IsPositionInTexture(const Vec2 sp) const
{
    const AreaSize as = m_map_data.size*m_tileset.GetTileSize(); // Do not use zoom here
    return sp.x >= 0 && sp.y >= 0 && sp.x <= as.x && sp.y <= as.y;
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