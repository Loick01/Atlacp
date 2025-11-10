#include "player.hpp"

Player::Player(const Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
    const std::string& sprite_filepath, const unsigned int tile_size, const ScreenPosition screen_position) :
    Drawable(texture_controller, sprite_filepath, screen_position), MapElement({10,1}), m_event_controller(event_controller),
    m_tilemap(tilemap), m_tile_size(tile_size)
{

}

Player::~Player()
{

}

void Player::DrawTexture() const
{
    int tile_size = static_cast<int>(m_tile_size);
    int texture_width = static_cast<int>(m_texture_width);
    int texture_height = static_cast<int>(m_texture_height);
    const SDL_Rect src{0,0,texture_width,texture_height};
    // Should use a scene graph instead of keeping screen position for each Drawable
    const SDL_Rect dst{m_screen_position.x+m_map_position.x*tile_size,m_screen_position.y+m_map_position.y*tile_size,texture_width,texture_height};
    m_texture_controller->RenderTexture(m_texture_key, src, dst);
}

void Player::Update()
{
    MapPosition new_pos = m_map_position + m_event_controller->HandlePlayerEvent();
    if (m_tilemap->IsMapPositionEmpty(new_pos)){
        m_map_position = new_pos;
    }
}