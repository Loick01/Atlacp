#include "layer.hpp"

TileLayer::TileLayer(const size_t layer_size, Camera& camera, TextureController& texture_controller):
    SceneDrawable(texture_controller, camera, ScenePosition{0,0}) // I could remove the ScenePosition argument ?
{
    m_tiles.reserve(layer_size);
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
    // for (int j = start_index.y ; j < end_index.y ; j++){
    //     for (int i = start_index.x ; i < end_index.x ; i++){
    //         int tile = m_tileset.GetNormalizedTile(m_tiles[j*map_size.x+i]); // Should use Tile type ?
    //         int tileset_width = m_tileset.GetTilesetWidth();
    //         const SDL_Rect src{(tile%tileset_width)*tile_size, (tile/tileset_width)*tile_size, tile_size, tile_size};
    //         const int tile_screen_size = static_cast<int>(tile_size*zoom+1);
    //         const ScreenPosition dst_position = (Vec2{i,j}*tile_size)*zoom-camera_position;
    //         const SDL_Rect dst{dst_position.x, dst_position.y, tile_screen_size, tile_screen_size};
    //         m_texture_controller.RenderTexture(m_tileset.GetTextureKey(), src, dst);
    //     }
    // }
}

// void TileLayer::SetTileset(Tileset& tileset)
// {
//     m_tileset = tileset;
// }

void TileLayer::AddTile(const Tile t)
{
    m_tiles.push_back(t);
}

void TileLayer::SetTile(const size_t index, const Tile t)
{
    m_tiles[index] = t;
}