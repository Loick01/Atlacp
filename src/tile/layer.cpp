#include "tile/layer.hpp"

#include "image/texture.hpp"
#include "core/camera.hpp"
#include "tile/tileset.hpp"

TileLayer::TileLayer(const GridSize layerSize, Camera& camera, TextureController& textureController, Tileset& tileset):
    m_textureController(textureController), m_camera(camera), m_tileset(tileset), m_layerSize(layerSize)
{
    m_tiles.reserve(layerSize.x*layerSize.y);
}

std::vector<Tile> TileLayer::GetTiles() const
{
    return m_tiles;
}

Tile TileLayer::GetTile(const size_t index) const
{
    return m_tiles[index];
}

void TileLayer::DrawTexture() const
{
    int tileSize = m_tileset.GetTileSize();
    const float zoom = m_camera.GetZoom();

    Pair<int> startIndex = m_camera.GetStartIndex();
    Pair<int> endIndex = m_camera.GetEndIndex();

    ScenePosition cameraPosition = m_camera.GetPosition()-m_camera.GetScreenOffset(); // Do not forget, even if the culling is disabled

    for (int j = startIndex.y ; j < endIndex.y ; j++){
        for (int i = startIndex.x ; i < endIndex.x ; i++){
            int tile = m_tileset.GetNormalizedTile(m_tiles[j*m_layerSize.x+i]); // Should use Tile type ?
            int tilesetWidth = m_tileset.GetTilesetWidth(); // Keep this line inside for-loop (selected tileset can be modified by Tileset::GetNormalizedTileset)
            const SDL_Rect src{(tile%tilesetWidth)*tileSize, (tile/tilesetWidth)*tileSize, tileSize, tileSize};
            const int tileScreenSize = static_cast<int>(tileSize*zoom+1);
            const ScreenPosition dstPosition = (Vec2{i,j}*tileSize)*zoom-cameraPosition;
            const SDL_Rect dst{dstPosition.x, dstPosition.y, tileScreenSize, tileScreenSize};
            m_textureController.RenderTexture(m_tileset.GetTextureKey(), src, dst);
        }
    }
}

void TileLayer::AddTile(const Tile t)
{
    m_tiles.push_back(t);
}

void TileLayer::SetTile(const size_t index, const Tile t)
{
    m_tiles[index] = t;
}

ExtraTileLayer::ExtraTileLayer(const GridSize layerSize, Camera& camera, TextureController& textureController, Tileset& tileset, const ExtraTileType tileType):
    TileLayer(layerSize, camera, textureController, tileset), m_tileType(tileType)
{
    m_textureController.LoadTextureFromFile("../assets/tileset/tile_border16.png", "tile_border16", m_tileSize, m_tileSize);
}

void ExtraTileLayer::DrawTexture() const
{
    const float zoom = m_camera.GetZoom();

    Pair<int> startIndex = m_camera.GetStartIndex();
    Pair<int> endIndex = m_camera.GetEndIndex();

    ScenePosition cameraPosition = m_camera.GetPosition()-m_camera.GetScreenOffset(); // Do not forget, even if the culling is disabled

    for (int j = startIndex.y ; j < endIndex.y ; j++){
        for (int i = startIndex.x ; i < endIndex.x ; i++){
            const SDL_Rect src{0, 0, m_tileSize, m_tileSize};
            const int tileScreenSize = static_cast<int>(m_tileSize*zoom+1);
            const ScreenPosition dstPosition = (Vec2{i,j}*m_tileSize)*zoom-cameraPosition;
            const SDL_Rect dst{dstPosition.x, dstPosition.y, tileScreenSize, tileScreenSize};
            m_textureController.RenderTexture("tile_border16", src, dst);
        }
    }
}