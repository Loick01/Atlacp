#pragma once 

#include <iostream>
#include <string>
#include <vector>

#include "drawable.hpp"
#include "file.hpp"
#include "tileset.hpp"
#include "type.hpp"

class Tilemap : public Drawable
{
    private:
        const FileReader& m_file_reader;
        Tileset& m_tileset;
        WorldData m_world_data;
        MapData m_map_data;
        bool m_should_culling; // Const ?

        MapPosition GetProjectedPosition(const MapPosition p, const MapBound bound); // Try to make it const ?
        void LoadMap(const std::string& path);
        int m_current_map;

    public:
        Tilemap(TextureController& texture_controller, const FileReader& file_reader, Tileset& tileset, 
            const std::string& world_filepath, Camera& camera);
        
        MapPosition GetSpawnPosition() const;
        std::vector<bool> GetOccupancyGrid() const;
        int GetTextureWidth() const override;
        int GetTextureHeight() const override;
        int GetTileSize() const;
        int GetGridSize() const;
        MapBound IsOutOfMap(const MapPosition p) const;
        unsigned int GetTileIndex(const MapPosition p) const;
        void TakePosition(const MapPosition p);
        void FreePosition(const MapPosition p);
        bool IsFreePosition(MapPosition& p, const bool can_exit_map);
        bool CanMoveCamera(const int axis_position, const int axis_new_position) const;
        void LoadAdjacentMap(const MapBound bound);
        void SetTileAt(const Tile new_tile, const MapPosition p);
        void DrawTexture() const override;
        void ReplaceTileAt(const ScenePosition position, const Tile new_tile);
        void SaveMap(const std::string &map_filepath) const;
        void SetShouldCulling(const bool should_culling);
};