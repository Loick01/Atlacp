#include "layer.hpp"

TileLayer::TileLayer(const GridSize layerSize, Camera& camera, TextureController& textureController, Tileset& tileset):
    m_textureController(textureController), m_camera(camera), m_tileset(tileset), m_size(layerSize)
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
            int tile = m_tileset.GetNormalizedTile(m_tiles[j*m_size.x+i]); // Should use Tile type ?
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