#pragma once 

#include <string>
#include <vector>

#include "core/notifier.hpp"
#include "tile/tile_types.hpp" // Tile
#include "tile/layer.hpp"

enum class TilemapEvent
{
    LoadingMap
};

class Camera;
class FileReader;
class Tileset;

class Tilemap : public Notifier<TilemapEvent>
{
    private:
        const FileReader& m_fileReader;
        Tileset& m_tileset;
        TextureController& m_textureController;
        Camera& m_camera; // Could be removed and use a parameter in LoadMap ?
        WorldData m_worldData;
        MapData m_mapData;
        size_t m_currentMap;
        const bool m_shouldCulling;

        void LoadMap(const std::string& mapFilepath);

    public:
        Tilemap(TextureController& textureController, const FileReader& fileReader, Tileset& tileset, 
            const std::string& worldFilepath, Camera& camera, const bool shouldCulling);
        
        const std::vector<TileLayer>& GetLayers() const;
        const std::vector<bool>& GetOccupancyGrid() const;
        
        MapBound IsOutOfMap(const MapPosition p) const;
        MapPosition GetSpawnPosition() const;
        MapPosition GetProjectedPosition(const MapPosition p, const MapBound bound) const;
        GridSize GetLayerSize() const;
        
        size_t GetLayerCount() const;
        size_t GetCurrentMapIndex() const;
        int GetTileSize() const;
        int GetGridSize() const; // Should be removed and use instead GetLayerSize()
        unsigned int GetTileIndex(const MapPosition p) const;

        bool IsPositionInTexture(const Vec2 sp) const; // This function was in Drawable, but Tilemap is not anymore a SceneDrawable
        bool IsFreePosition(const MapPosition p);
        void TakePosition(const MapPosition p);
        void FreePosition(const MapPosition p);

        void SetTileAt(const size_t layer, const Tile newTile, const MapPosition p);
        void ReplaceTileAt(const ScenePosition position, const size_t layer, const Tile newTile);
        
        void LoadAdjacentMap(const MapBound bound);
        void SaveMap(const std::string &mapFilepath) const;
};