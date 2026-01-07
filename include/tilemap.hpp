#pragma once 

#include <functional> // Should not be here
#include <iostream>
#include <string>
#include <vector>

#include "drawable.hpp"
#include "file.hpp"
#include "layer.hpp"
#include "tileset.hpp"
#include "type.hpp"

class Tilemap
{
    private:
        // Should not be here ? (For now it's only used by Tilemap, to notify Scene when TileLayer in Scene::m_drawables must be updated)
        using Callback = std::function<void()>;
        std::vector<Callback> m_listeners;
        void Notify();

        const FileReader& m_file_reader;
        Tileset& m_tileset;
        TextureController& m_texture_controller;
        Camera& m_camera;
        WorldData m_world_data;
        MapData m_map_data;
        bool m_should_culling; // Const ?

        void LoadMap(const std::string& path);
        size_t m_current_map;

    public:
        Tilemap(TextureController& texture_controller, const FileReader& file_reader, Tileset& tileset, 
            const std::string& world_filepath, Camera& camera);
        
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
        void SetShouldCulling(const bool should_culling);

        void AddListener(Callback c); // Should not be here ? (Same for Tilemap::Notify)
};