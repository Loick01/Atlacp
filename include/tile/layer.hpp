#pragma once

#include "core/file/file.hpp" // DataMapElement
#include "map/map_types.hpp" // MapPosition
#include "tile/tile_types.hpp" // Tile

class Camera;
class TextureController;
class Tileset;

// Rename ?
enum class ExtraTileType
{
    TileBorder //, TileCollision
};

// No longer inherits from SceneDrawable --> Too many issues caused by the fact 
// that m_textureKey was unused (problematic calls of TextureController::DeleteTexture() with empty key) 
class TileLayer
{
    private:
        std::vector<Tile> m_tiles;
        Tileset& m_tileset;
        const GridSize m_layerSize;
    
    protected:
        TextureController& m_textureController;
        Camera& m_camera;

    public:
        TileLayer(const GridSize layerSize, Camera& camera, TextureController& textureController, Tileset& tileset);

        std::vector<Tile> GetTiles() const;
        Tile GetTile(const size_t index) const;
        virtual void DrawTexture() const;
        void AddTile(const Tile t);
        void SetTile(const size_t index, const Tile t);
};

// Rename ?
class ExtraTileLayer : public TileLayer // Used for tile border layer + will be used for tile collision layer
{
    private:
        ExtraTileType m_tileType;
        int m_tileSize;

    public:
        // Tileset parameter is useless, but I can't remove because of TileLayer constructor
        ExtraTileLayer(const GridSize layerSize, Camera& camera, TextureController& textureController, Tileset& tileset, const ExtraTileType tileType);

        void DrawTexture() const override;
};

struct MapData // Should be in map/map_types.hpp ?
{
    size_t layerCount;
    size_t layerSplitIndex; // TODO : Later, each MapEntity will have its own value so that it can be drawn independently 
    std::vector<TileLayer> map; // Rename layers ?
    std::vector<bool> occupancyGrid; // Could be a TileLayer ?
    std::vector<TextureKey> tilesets; // Tileset keys (read from the header of the map file) that will be used to build the map
    std::vector<DataMapElement> elements;
    std::vector<DataMapElement> triggers;
    MapPosition spawnPosition; // Position where the player will start when loading the associated map (-1 if no specific position)
    GridSize size; // Should not be here, and only in TileLayer ?
    std::string tilesetDirectoryPath;
};