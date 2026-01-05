#pragma once 

#include <iostream>
#include <string>
#include <vector>

#include "drawable.hpp"
#include "file.hpp"
#include "layer.hpp"
#include "tileset.hpp"
#include "type.hpp"

class Tilemap : public SceneDrawable // Because Tilemap is a Drawable, its destructor try to DeleteTexture with texture key = "" (nothing happen)
{
    private:
        const FileReader& m_file_reader;
        Tileset& m_tileset;
        WorldData m_world_data;
        MapData m_map_data;
        bool m_should_culling; // Const ?

        void LoadMap(const std::string& path);
        size_t m_current_map;

    public:
        Tilemap(TextureController& texture_controller, const FileReader& file_reader, Tileset& tileset, 
            const std::string& world_filepath, Camera& camera);
        
        MapPosition GetSpawnPosition() const;
        std::vector<bool> GetOccupancyGrid() const;
        int GetTextureWidth() const override;
        int GetTextureHeight() const override;
        int GetTileSize() const;
        int GetGridSize() const;
        MapPosition GetProjectedPosition(const MapPosition p, const MapBound bound);
        MapBound IsOutOfMap(const MapPosition p) const;
        unsigned int GetTileIndex(const MapPosition p) const;
        void TakePosition(const MapPosition p);
        void FreePosition(const MapPosition p);
        bool IsFreePosition(MapPosition& p);
        void LoadAdjacentMap(const MapBound bound);
        void SetTileAt(const size_t layer, const Tile new_tile, const MapPosition p);
        void DrawTexture() const override;
        void ReplaceTileAt(const ScenePosition position, const size_t layer, const Tile new_tile);
        void SaveMap(const std::string &map_filepath) const;
        void SetShouldCulling(const bool should_culling);
};