#pragma once

#include "tile/tile_types.hpp" // Tile

class Camera;
class TextureController;
class Tileset;

enum class ExtraLayerType
{
    Border, Collision
};

// No longer inherits from SceneDrawable --> Too many issues caused by the fact 
// that m_textureKey was unused (problematic calls of TextureController::DeleteTexture() with empty key) 
class TileLayer
{
    private:
        Tileset& m_tileset;
    
    protected:
        TextureController& m_textureController;
        Camera& m_camera;

        std::vector<Tile> m_tiles;
        const GridSize m_layerSize;

    public:
        TileLayer(const GridSize layerSize, Camera& camera, TextureController& textureController, Tileset& tileset);
        virtual ~TileLayer() = default;
        
        std::vector<Tile> GetTiles() const;
        Tile GetTile(const size_t index) const;
        virtual void DrawTexture() const;
        void AddTile(const Tile t);
        void SetTile(const size_t index, const Tile t);
};

class ExtraTileLayer : public TileLayer // Used for tile border layer + will be used for tile collision layer
{
    private:
        ExtraLayerType m_layerType;
        TextureKey m_tileKey;
        std::string m_uiDisplay; // How this ExtraTileLayer will be displayed in the UI. For TileLayer, it only uses 0,1,2,etc.
        int m_tileSize;

    public:
        // Tileset parameter is useless, but I can't remove because of TileLayer constructor
        ExtraTileLayer(const GridSize layerSize, Camera& camera, TextureController& textureController, Tileset& tileset, 
            const ExtraLayerType layerType, const std::vector<bool>& occupancyGrid); // TODO : occupancyGrid should not be in constructor ?
        ~ExtraTileLayer() override;
        
        std::string GetUiDisplay() const;
        void BuildBorderLayer();
        void BuildCollisionLayer(const std::vector<bool>& occupancyGrid);
        void DrawTexture() const override;
};