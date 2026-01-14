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
        const FileReader& m_file_reader;
        Tileset& m_tileset;
        TextureController& m_texture_controller;
        Camera& m_camera; // Could be removed and use a parameter in LoadMap ?
        WorldData m_world_data;
        MapData m_map_data;

        void LoadMap(const std::string& path);
        size_t m_current_map;
        const bool m_should_culling;

    public:
        Tilemap(TextureController& texture_controller, const FileReader& file_reader, Tileset& tileset, 
            const std::string& world_filepath, Camera& camera, const bool should_culling);
        
        const std::vector<TileLayer>& GetLayers() const;
        MapPosition GetSpawnPosition() const;
        std::vector<bool> GetOccupancyGrid() const;
        int GetTileSize() const;
        int GetGridSize() const;
        MapPosition GetProjectedPosition(const MapPosition p, const MapBound bound);
        MapBound IsOutOfMap(const MapPosition p) const;
        unsigned int GetTileIndex(const MapPosition p) const;
        size_t GetLayerCount() const;
        bool IsPositionInTexture(const Vec2 sp) const; // This function was in Drawable, but Tilemap is not anymore a SceneDrawable
        void TakePosition(const MapPosition p);
        void FreePosition(const MapPosition p);
        bool IsFreePosition(MapPosition& p);
        void LoadAdjacentMap(const MapBound bound);
        void SetTileAt(const size_t layer, const Tile new_tile, const MapPosition p);
        void ReplaceTileAt(const ScenePosition position, const size_t layer, const Tile new_tile);
        void SaveMap(const std::string &map_filepath) const;
};