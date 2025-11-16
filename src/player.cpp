#include "player.hpp"

Player::Player(Tilemap* tilemap, TextureController* texture_controller, const MapEventController* event_controller,
    const std::string& sprite_filepath, const int window_width, const int window_height, const ScreenPosition screen_position, const bool should_draw) :
    Drawable(texture_controller, sprite_filepath, screen_position, should_draw), MapElement({10,10}), m_event_controller(event_controller),
    m_tilemap(tilemap), m_tile_size(tilemap->GetTileSize()), m_window_width(window_width), m_window_height(window_height)
{
    // Set the "camera" position to have the player in the middle of screen (without checking the tilemap bound)
    const ScreenPosition sp = m_map_position.ToScreenPosition(m_tile_size);
    const ScreenPosition tilemap_center = {m_window_width/2,m_window_height/2};
    tilemap->SetScreenPosition(tilemap_center-sp); // Tilemap and player will have to share somehow this screen position
    SetScreenPosition(tilemap_center-sp);
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
    const MapPosition movement = m_event_controller->HandlePlayerEvent();
    MapPosition new_pos = m_map_position + movement;
    if (m_tilemap->CheckNewPosition(new_pos)){
        m_map_position = new_pos;
        const ScreenPosition sp = (movement*-1).ToScreenPosition(m_tile_size); // m_tile_size must be equal to the same tile_size in Tilemap, if not use tilemap->GetTileSize()
        if (m_tilemap->CanMoveCamera(sp,m_window_width,m_window_height)){
            m_tilemap->AddScreenPosition(sp); // Tilemap and player will have to share somehow this screen position
            AddScreenPosition(sp);
        }
    }
}