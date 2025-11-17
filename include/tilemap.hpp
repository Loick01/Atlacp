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
        const FileReader* m_file_reader;
        Tileset* m_tileset;
        WorldData m_world_data;
        MapData m_map_data;

        MapPosition GetProjectedPosition(const MapPosition p, const MapBound bound); // Try to make it const ?
        MapBound IsOutOfMap(const MapPosition p) const;
        unsigned char GetTileAt(const MapPosition p) const;
        int m_current_map;

    public:
        Tilemap(TextureController* texture_controller, const FileReader* file_reader, Tileset* tileset, 
            const std::string& world_filepath, Camera* camera);
        ~Tilemap();

        int GetTextureWidth() const override;
        int GetTextureHeight() const override;
        int GetTileSize() const;
        bool CheckNewPosition(MapPosition& p);
        bool CanMoveCamera(const int axis_position, const int axis_new_position) const;
        void LoadAdjacentMap(const MapBound bound);
        void SetTileAt(const unsigned char new_tile, const MapPosition p);
        void LoadMap(const std::string& filepath);
        void DrawTexture() const override;
        void ReplaceTileAt(const ScenePosition position, const unsigned char new_tile);
        void SaveMap(const std::string &map_filepath) const;
};