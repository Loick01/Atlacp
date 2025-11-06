#include "player.hpp"

Player::Player(const Tilemap* tilemap, TextureController* texture_controller, const EventController* event_controller, const std::string& sprite_filepath):
    Drawable(texture_controller, sprite_filepath), m_event_controller(event_controller), m_tilemap(tilemap)
{
    m_player_position = {10, 1};
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
    unsigned char tile_size = 32; // Will be read from tilemap file, through tilemap instance
    const SDL_Rect src{0,0,tile_size,tile_size};
    const SDL_Rect dst{m_player_position.x*tile_size,m_player_position.y*tile_size,tile_size,tile_size};
    m_texture_controller->RenderTexture(m_texture_filepath, src, dst);
}

void Player::Update()
{
    Position new_pos = m_player_position + m_event_controller->HandlePlayerEvent();
    if (m_tilemap->IsPositionEmpty(new_pos)){
        m_player_position = new_pos;
    }
}