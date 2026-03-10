#pragma once 

#include <functional> // Should not be here
#include <iostream>
#include <string>
#include <vector>

#include "drawable.hpp"
#include "file.hpp"
#include "layer.hpp"
#include "notifier.hpp"
#include "tileset.hpp"
#include "type.hpp"

enum class TilemapEvent
{
    TileLayerUpdate
};

class Tilemap : public Notifier<TilemapEvent>
{
    private:
        const FileReader& m_fileReader;
        Tileset& m_tileset;
        TextureController& m_textureController;
        Camera& m_camera; // Could be removed and use a parameter in LoadMap ?
        WorldData m_worldData;
        MapData m_mapData;

        void LoadMap(const std::string& path);
        size_t m_currentMap;
        const bool m_shouldCulling;

    public:
        Tilemap(TextureController& textureController, const FileReader& fileReader, Tileset& tileset, 
            const std::string& worldFilepath, Camera& camera, const bool shouldCulling);
        
        const std::vector<TileLayer>& GetLayers() const;
        MapPosition GetSpawnPosition() const;
        std::vector<bool> GetOccupancyGrid() const;
        GridSize GetLayerSize() const;
        int GetTileSize() const;
        int GetGridSize() const; // Should be removed and use instead GetLayerSize()
        MapPosition GetProjectedPosition(const MapPosition p, const MapBound bound);
        MapBound IsOutOfMap(const MapPosition p) const;
        unsigned int GetTileIndex(const MapPosition p) const;
        size_t GetLayerCount() const;
        bool IsPositionInTexture(const Vec2 sp) const; // This function was in Drawable, but Tilemap is not anymore a SceneDrawable
        void TakePosition(const MapPosition p);
        void FreePosition(const MapPosition p);
        bool IsFreePosition(MapPosition& p);
        void LoadAdjacentMap(const MapBound bound);
        void SetTileAt(const size_t layer, const Tile newTile, const MapPosition p);
        void ReplaceTileAt(const ScenePosition position, const size_t layer, const Tile newTile);
        void SaveMap(const std::string &mapFilepath) const;
};