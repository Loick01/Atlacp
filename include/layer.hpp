#pragma once

#include "camera.hpp" // Will be removed ?
#include "drawable.hpp"
#include "tileset.hpp"
#include "texture.hpp" // Will be removed ?
#include "type.hpp" 

class TileLayer : public SceneDrawable
{
    private:
        std::vector<Tile> m_tiles;
        Tileset& m_tileset;
        const GridSize m_size;
    
    public:
        TileLayer(const GridSize layerSize, Camera& camera, TextureController& textureController, Tileset& tileset);
        std::vector<Tile> GetTiles() const;
        Tile GetTile(const size_t index) const;
        void DrawTexture() const override;
        void AddTile(const Tile t);
        void SetTile(const size_t index, const Tile t);
};

struct MapData
{
    size_t layerCount;
    std::vector<TileLayer> map; // Rename layers ?
    std::vector<bool> occupancyGrid; // Could be a TileLayer ?
    std::vector<TextureKey> tilesets; // Tileset keys (read from the header of the map file) that will be used to build the map
    MapPosition spawnPosition; // Position where the player will start when loading the associated map (-1 if no specific position)
    GridSize size; // Should not be here, and only in TileLayer ?
};