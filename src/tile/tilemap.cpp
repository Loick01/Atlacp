#include "tile/tilemap.hpp"

#include "core/camera.hpp"
#include "core/file/file.hpp"
#include "tile/layer.hpp" // TileLayer
#include "tile/tileset.hpp"

Tilemap::Tilemap(TextureController& textureController, const FileReader& fileReader, Tileset& tileset, 
const std::string& worldFilepath, Camera& camera, const bool shouldCulling) :
    m_camera(camera), m_textureController(textureController), m_fileReader(fileReader), m_tileset(tileset), 
    m_shouldCulling(shouldCulling), m_currentBound(MapBound::Inside)
{
    m_camera.SetShouldCulling(m_shouldCulling);
    std::optional<DataWorld> optDataWorld = m_fileReader.ReadWorldFile(worldFilepath);
    if (!optDataWorld.has_value())
        return;
    m_dataWorld = optDataWorld.value();
    m_currentMap = m_dataWorld.startMap; // The first loaded map is specified in the world file
    LoadCurrentMap();
}

const std::vector<DataMapElement>& Tilemap::GetElementsData() const
{
    return m_dataMap.elements;
}

const std::vector<DataMapElement>& Tilemap::GetTriggersData() const
{
    return m_dataMap.triggers;
}

const std::vector<TileLayer>& Tilemap::GetLayers() const
{
    return m_dataMap.layers;
}

const std::vector<bool>& Tilemap::GetOccupancyGrid() const
{
    return m_dataMap.occupancyGrid;
}

const DataWorld& Tilemap::GetDataWorld() const
{
    return m_dataWorld;
}

MapBound Tilemap::IsOutOfMap(const MapPosition p) const
{
    if (p.x < 0) return MapBound::OutLeft;
    if (p.x >= m_dataMap.size.x) return MapBound::OutRight;
    if (p.y < 0) return MapBound::OutUp;
    if (p.y >= m_dataMap.size.y) return MapBound::OutDown;
    return MapBound::Inside;
}

MapPosition Tilemap::GetSpawnPosition() const
{
    return m_dataMap.spawnPosition; // Return the spawn position of the loaded map (could be -1 if undefined)
}

MapPosition Tilemap::GetProjectedPosition(const MapPosition p) const
{
    MapPosition projectedPosition = p;
    switch (m_currentBound){
        case MapBound::OutUp:
            projectedPosition.y = m_dataMap.size.y-1;
            break;
        case MapBound::OutDown:
            projectedPosition.y = 0;
            break;
        case MapBound::OutRight:
            projectedPosition.x = 0;
            break;
        case MapBound::OutLeft:
            projectedPosition.x = m_dataMap.size.x-1;
            break;
        default:
            Logger::GetInstance().AddLog(Level::L4, "Unhandled MapBound (MapBound::Inside ?) used in Tilemap::GetProjectedPosition"); // TODO : Log
            return {0,0};
    }
    return projectedPosition;
}

GridSize Tilemap::GetLayerSize() const
{
    return m_dataMap.size;
}

size_t Tilemap::GetLayerCount() const
{
    return m_dataMap.layerCount;
}

size_t Tilemap::GetCurrentMapIndex() const
{
    return m_currentMap;
}

size_t Tilemap::GetLayerSplitIndex() const
{
    return m_dataMap.layerSplitIndex;
}

int Tilemap::GetTileSize() const
{
    return m_tileset.GetTileSize();
}

int Tilemap::GetGridSize() const
{
    return m_dataMap.size.x*m_dataMap.size.y;
}

unsigned int Tilemap::GetTileIndex(const MapPosition p) const
{
    return p.y*m_dataMap.size.x+p.x;
}

bool Tilemap::IsPositionInTexture(const Vec2 sp) const
{
    const AreaSize as = m_dataMap.size*m_tileset.GetTileSize(); // Do not use zoom here
    return sp.x >= 0 && sp.y >= 0 && sp.x <= as.x && sp.y <= as.y;
}

bool Tilemap::IsFreePosition(const MapPosition p)
{
    if (m_dataMap.occupancyGrid.size() <= GetTileIndex(p)) {
        Logger::GetInstance().AddLog(Level::L4, "Invalid MapPosition used in Tilemap::IsFreePosition"); // TODO : Level
        return false;
    }
    return m_dataMap.occupancyGrid.at(GetTileIndex(p));
}

void Tilemap::TakePosition(const MapPosition p)
{
    if (m_dataMap.occupancyGrid.size() <= GetTileIndex(p)) { // TODO : Should also be in FreePosition ?
        Logger::GetInstance().AddLog(Level::L4, "Invalid MapPosition used in Tilemap::TakePosition"); // TODO : Level
        return;
    }
    m_dataMap.occupancyGrid.at(GetTileIndex(p)) = false;
}

void Tilemap::FreePosition(const MapPosition p)
{
    m_dataMap.occupancyGrid.at(GetTileIndex(p)) = true;
}

void Tilemap::SetTileAt(const size_t layerIndex, const Tile newTile, const MapPosition p)
{
    m_dataMap.layers[layerIndex].SetTile(GetTileIndex(p), newTile);
}

void Tilemap::ReplaceTileAt(const ScenePosition sp, const size_t layerIndex, const Tile newTile)
{
    if (IsPositionInTexture(sp)){ // sp must be normalized (with scene position)
        const int tileSize = m_tileset.GetTileSize();
        int c = sp.x/tileSize;
        int l = sp.y/tileSize;
        SetTileAt(layerIndex, newTile, {c,l});
    }
}

void Tilemap::LoadMapByIndex(const unsigned int mapIndex)
{
    m_currentMap = mapIndex;
    LoadMap(m_dataWorld.mapDirectoryPath + m_dataWorld.maps[m_currentMap]);
}

void Tilemap::LoadCurrentMap()
{
    LoadMapByIndex(m_currentMap);
}

void Tilemap::UpdateMapIndex(const MapBound bound)
{
    // When loading a new map, no verifications are made to check if the code tries to reach an out-of-world map.
    // Maps are supposed to be designed in such a way the player can't get out of the world.
    switch (bound){
        case MapBound::OutUp:
            m_currentMap -= m_dataWorld.size.x;
            break;
        case MapBound::OutDown:
            m_currentMap += m_dataWorld.size.x;
            break;
        case MapBound::OutRight:
            m_currentMap += 1;
            break;
        case MapBound::OutLeft:
            m_currentMap -= 1;
            break;
    }

    m_currentBound = bound;
}

void Tilemap::LoadMap(const std::string& mapFilepath)
{
    // TODO : I don't think I should delete all tilesets when loading a new map
    // Instead, I could only delete the unused one
    m_tileset.CleanTilesets(true); // Delete tilesets used for the previous map

    // TileLayer are created in ReadMapFile. Because they are SceneDrawable, they need camera and texture controller
    // TileLayer also need a Tileset to be rendered
    std::optional<DataMap> optDataMap = m_fileReader.ReadMapFile(mapFilepath, m_camera, m_textureController, m_tileset);
    if (!optDataMap.has_value())
        return;
    m_dataMap = std::move(optDataMap.value());
    
    // Load tilesets read in the header of the map file
    for (const std::string& p : m_dataMap.tilesets)
        m_tileset.LoadTileset(m_fileReader, m_dataMap.tilesetDirectoryPath + p);

    const GridSize map_size = m_dataMap.size;
    for (size_t j=0 ; j<map_size.y ; j++){
        for (size_t i=0 ; i<map_size.x ; i++){
            bool is_free = true;
            for (size_t l = 0 ; l < m_dataMap.layerCount ; l++){
                const Tile t = m_dataMap.layers[l].GetTile(j*map_size.x+i);
                is_free = m_tileset.IsEmptyTile(t);
                if (!is_free) break;
            }
            m_dataMap.occupancyGrid.push_back(is_free);
        }
    }

    m_camera.SetTilemapInfo(m_dataMap.size*m_tileset.GetTileSize());
    
    Notify(TilemapEvent::LoadingMap); // Update TileLayer used for rendering in TilemapScene
}

void Tilemap::SaveMap(const std::string &mapFilepath) const
{
    m_fileReader.SaveMapFile(m_dataWorld.mapDirectoryPath + mapFilepath, m_dataMap);
}

void Tilemap::SetCurrentBound(const MapBound bound)
{
    m_currentBound = bound;
}