#include "tile/tilemap.hpp"

#include "core/camera.hpp"
#include "core/file/file.hpp"
#include "tile/tileset.hpp"

Tilemap::Tilemap(TextureController& textureController, const FileReader& fileReader, Tileset& tileset, 
    const std::string& worldFilepath, Camera& camera, const bool shouldCulling) :
    m_camera(camera), m_textureController(textureController), m_fileReader(fileReader), m_tileset(tileset), m_shouldCulling(shouldCulling)
{
    m_camera.SetShouldCulling(m_shouldCulling);
    m_worldData = m_fileReader.ReadWorldFile(worldFilepath);
    m_currentMap = m_worldData.startMap; // The first loaded map is specified in the world file
    LoadMap(m_worldData.mapDirectoryPath + m_worldData.maps[m_currentMap]); 
}

const std::vector<DataMapElement>& Tilemap::GetElementsData() const
{
    return m_mapData.elements;
}

const std::vector<TileLayer>& Tilemap::GetLayers() const
{
    return m_mapData.map;
}

const std::vector<bool>& Tilemap::GetOccupancyGrid() const
{
    return m_mapData.occupancyGrid;
}

const WorldData& Tilemap::GetWorldData() const
{
    return m_worldData;
}

MapBound Tilemap::IsOutOfMap(const MapPosition p) const
{
    if (p.x < 0) return MapBound::OutLeft;
    if (p.x >= m_mapData.size.x) return MapBound::OutRight;
    if (p.y < 0) return MapBound::OutUp;
    if (p.y >= m_mapData.size.y) return MapBound::OutDown;
    return MapBound::Inside;
}

MapPosition Tilemap::GetSpawnPosition() const
{
    return m_mapData.spawnPosition; // Return the spawn position of the loaded map (could be -1 if undefined)
}

MapPosition Tilemap::GetProjectedPosition(const MapPosition p, const MapBound bound) const
{
    MapPosition projectedPosition = p;
    switch (bound){
        case MapBound::OutUp:
            projectedPosition.y = m_mapData.size.y-1;
            break;
        case MapBound::OutDown:
            projectedPosition.y = 0;
            break;
        case MapBound::OutRight:
            projectedPosition.x = 0;
            break;
        case MapBound::OutLeft:
            projectedPosition.x = m_mapData.size.x-1;
            break;
    }
    return projectedPosition;
}

GridSize Tilemap::GetLayerSize() const
{
    return m_mapData.size;
}

size_t Tilemap::GetLayerCount() const
{
    return m_mapData.layerCount;
}

size_t Tilemap::GetCurrentMapIndex() const
{
    return m_currentMap;
}

size_t Tilemap::GetLayerSplitIndex() const
{
    return m_mapData.layerSplitIndex;
}

int Tilemap::GetTileSize() const
{
    return m_tileset.GetTileSize();
}

int Tilemap::GetGridSize() const
{
    return m_mapData.size.x*m_mapData.size.y;
}

unsigned int Tilemap::GetTileIndex(const MapPosition p) const
{
    return p.y*m_mapData.size.x+p.x;
}

bool Tilemap::IsPositionInTexture(const Vec2 sp) const
{
    const AreaSize as = m_mapData.size*m_tileset.GetTileSize(); // Do not use zoom here
    return sp.x >= 0 && sp.y >= 0 && sp.x <= as.x && sp.y <= as.y;
}

bool Tilemap::IsFreePosition(const MapPosition p)
{
    return m_mapData.occupancyGrid[GetTileIndex(p)];
}

void Tilemap::TakePosition(const MapPosition p)
{
    m_mapData.occupancyGrid[GetTileIndex(p)] = false;
}

void Tilemap::FreePosition(const MapPosition p)
{
    m_mapData.occupancyGrid[GetTileIndex(p)] = true;
}

void Tilemap::SetTileAt(const size_t layer, const Tile newTile, const MapPosition p)
{
    m_mapData.map[layer].SetTile(GetTileIndex(p), newTile);
}

void Tilemap::ReplaceTileAt(const ScenePosition sp, const size_t layer, const Tile newTile)
{
    if (IsPositionInTexture(sp)){ // sp must be normalized (with scene position)
        const int tileSize = m_tileset.GetTileSize();
        int c = sp.x/tileSize;
        int l = sp.y/tileSize;
        SetTileAt(layer,newTile,{c,l});
    }
}

void Tilemap::LoadAdjacentMap(const MapBound bound) // This function is used only in editor
{
    // When loading a new map, no verifications are made to check if the code tries to reach an out-of-world map.
    // Maps are supposed to be designed in such a way the player can't get out of the world.
    switch (bound){
        case MapBound::OutUp:
            m_currentMap -= m_worldData.size.x;
            break;
        case MapBound::OutDown:
            m_currentMap += m_worldData.size.x;
            break;
        case MapBound::OutRight:
            m_currentMap += 1;
            break;
        case MapBound::OutLeft:
            m_currentMap -= 1;
            break;
    }
    LoadMap(m_worldData.mapDirectoryPath + m_worldData.maps[m_currentMap]);
    m_camera.SetTilemapInfo(m_mapData.size*m_tileset.GetTileSize());
}

void Tilemap::LoadMap(const std::string& mapFilepath)
{
    // I don't think I should delete all tilesets when loading a new map
    // Instead, I could only delete the unused one
    m_tileset.CleanTilesets(true); // Delete tilesets used for the previous map

    // TileLayer are created in ReadMapFile. Because they are SceneDrawable, they need camera and texture controller
    // TileLayer also need a Tileset to be rendered
    m_mapData = m_fileReader.ReadMapFile(mapFilepath, m_camera, m_textureController, m_tileset);
    
    // Load tilesets read in the header of the map file
    for (const std::string& p : m_mapData.tilesets)
        m_tileset.LoadTileset(m_fileReader, m_mapData.tilesetDirectoryPath + p);

    const GridSize map_size = m_mapData.size;
    for (size_t j=0 ; j<map_size.y ; j++){
        for (size_t i=0 ; i<map_size.x ; i++){
            bool is_free = true;
            for (size_t layer=0 ; layer<m_mapData.layerCount ; layer++){
                const Tile t = m_mapData.map[layer].GetTile(j*map_size.x+i);
                is_free = m_tileset.IsEmptyTile(t);
                if (!is_free) break;
            }
            m_mapData.occupancyGrid.push_back(is_free);
        }
    }
    Notify(TilemapEvent::LoadingMap); // Update TileLayer used for rendering in TilemapScene
}

void Tilemap::SaveMap(const std::string &mapFilepath) const
{
    m_fileReader.SaveMapFile(m_worldData.mapDirectoryPath + mapFilepath, m_mapData);
}