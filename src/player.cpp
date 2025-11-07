#include "player.hpp"

Player::Player(const Tilemap* tilemap, TextureController* texture_controller, const EventController* event_controller, const std::string& sprite_filepath, const Offset offset):
    Drawable(texture_controller, sprite_filepath, offset), MapElement({10,1}), m_event_controller(event_controller), m_tilemap(tilemap)
{

}

Player::~Player()
{

}

void Player::LoadSprite(const std::string& sprite_filepath)
{
    Drawable::LoadTexture(sprite_filepath);
}

void Player::DrawTexture() const
{
    unsigned char tile_size = m_tilemap->GetTileSize();
    const SDL_Rect src{0,0,tile_size,tile_size};
    const SDL_Rect dst{m_offset.d_x+m_position.x*tile_size,m_offset.d_y+m_position.y*tile_size,tile_size,tile_size};
    m_texture_controller->RenderTexture(m_texture_key, src, dst);
}

void Player::Update()
{
    MapPosition new_pos = m_position + m_event_controller->HandlePlayerEvent();
    if (m_tilemap->IsMapPositionEmpty(new_pos)){
        m_position = new_pos;
    }
}