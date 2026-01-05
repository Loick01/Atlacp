#pragma once

#include "camera.hpp" // Will be removed ?
#include "drawable.hpp"
// #include "tileset.hpp"
#include "texture.hpp" // Will be removed ?
#include "type.hpp" 

class TileLayer : public SceneDrawable
{
    private:
        std::vector<Tile> m_tiles;
        // Tileset& m_tileset;
    
    public:
        TileLayer(const size_t layer_size, Camera& camera, TextureController& texture_controller);
        std::vector<Tile> GetTiles() const;
        Tile GetTile(const size_t index) const;
        void DrawTexture() const override;
        // void SetTileset(Tileset& tileset);
        void AddTile(const Tile t);
        void SetTile(const size_t index, const Tile t);
};

struct MapData
{
    size_t layer_count;
    std::vector<TileLayer> map;
    std::vector<bool> occupancy_grid; // Could be a TileLayer ?
    std::vector<TextureKey> tilesets; // Tileset keys (read from the header of the map file) that will be used to build the map
    MapPosition spawn_position; // Position where the player will start when loading the associated map (-1 if no specific position)
    GridSize size;
};