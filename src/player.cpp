#include "player.hpp"

Player::Player(TextureController* texture_controller, EventController* event_controller, const std::string& sprite_filepath):
    Drawable(texture_controller, sprite_filepath), m_event_controller(event_controller)
{
    m_player_position = {10, 1};
    //m_texture_controller->LoadTextureFromFile(m_sprite_texture); // Only for constructor, then use LoadSprite to change the sprite texture
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