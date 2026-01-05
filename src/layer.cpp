#include "layer.hpp"

TileLayer::TileLayer(const GridSize layer_size, Camera& camera, TextureController& texture_controller, Tileset& tileset):
    SceneDrawable(texture_controller, camera, ScenePosition{0,0}), m_tileset(tileset), m_size(layer_size) // I could remove the ScenePosition argument ?
{
    m_tiles.reserve(layer_size.x*layer_size.y);
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
    int tile_size = m_tileset.GetTileSize();
    ScenePosition camera_position = m_camera.GetPosition();
    const float zoom = m_camera.GetZoom();

    // Culling
    // While animating a movement, end_index could not be enough to fill the window with the map
    // So I add 1 to end_index, and check if it becomes greater than map size
    Pair<int> start_index = GridSize{0, 0};
    Pair<int> end_index = m_size; // Same
    /*
    if (m_should_culling){ // No map culling in editor (find better way than just use a bool ?)
        // Should be in a function in Camera ?
        if (m_camera.GetIsOffScreen().x){
            start_index.x = camera_position.x/(tile_size*zoom);
            end_index.x = std::min(end_index.x, start_index.x + m_camera.GetRangeTile().x + 1);
        }
        if (m_camera.GetIsOffScreen().y){
            start_index.y = camera_position.y/(tile_size*zoom);
            end_index.y = std::min(end_index.y, start_index.y + m_camera.GetRangeTile().y + 1);
        }
    }
    camera_position = camera_position-m_camera.GetScreenOffset();
    */

    for (int j = start_index.y ; j < end_index.y ; j++){
        for (int i = start_index.x ; i < end_index.x ; i++){
            int tile = m_tileset.GetNormalizedTile(m_tiles[j*m_size.x+i]); // Should use Tile type ?
            int tileset_width = m_tileset.GetTilesetWidth(); // Keep this line inside for-loop (selected tileset can be modified by Tileset::GetNormalizedTileset)
            const SDL_Rect src{(tile%tileset_width)*tile_size, (tile/tileset_width)*tile_size, tile_size, tile_size};
            const int tile_screen_size = static_cast<int>(tile_size*zoom+1);
            const ScreenPosition dst_position = (Vec2{i,j}*tile_size)*zoom-camera_position;
            const SDL_Rect dst{dst_position.x, dst_position.y, tile_screen_size, tile_screen_size};
            m_texture_controller.RenderTexture(m_tileset.GetTextureKey(), src, dst);
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