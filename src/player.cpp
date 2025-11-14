#include "player.hpp"

Player::Player(Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
    const std::string& sprite_filepath, const ScreenPosition screen_position, const bool should_draw) :
    Drawable(texture_controller, sprite_filepath, screen_position, should_draw), MapElement({10,1}), m_event_controller(event_controller),
    m_tilemap(tilemap), m_tile_size(tilemap->GetTileSize())
{

}

Player::~Player()
{
    m_texture_controller->DeleteTexture(m_texture_key);
}

void Player::DrawTexture() const
{
    const SDL_Rect src{0,0,m_texture_width,m_texture_height};
    // Should use a scene graph instead of keeping screen position for each Drawable
    const SDL_Rect dst{m_screen_position.x+m_map_position.x*m_tile_size,m_screen_position.y+m_map_position.y*m_tile_size,m_texture_width,m_texture_height};
    m_texture_controller->RenderTexture(m_texture_key, src, dst);
}

void Player::Update()
{
    MapPosition new_pos = m_map_position + m_event_controller->HandlePlayerEvent();
    if (m_tilemap->CheckNewPosition(new_pos)){
        m_map_position = new_pos;
    }
}