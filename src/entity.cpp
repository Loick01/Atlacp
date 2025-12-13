#include "entity.hpp"

EntityMovement::EntityMovement():
    m_direction(MapDirection::None), m_move(MapPosition{0, 0})
{

}

EntityMovement::~EntityMovement()
{

}

MapPosition EntityMovement::GetMove() const
{
    return m_move;
}

MapDirection EntityMovement::GetDirection() const
{
    return m_direction;
}

ScenePosition EntityMovement::GetScenePosition() const
{
    return m_start_position + (m_end_position - m_start_position) * m_progress; // Should be in Interpolation struct ?
}

EntityState EntityMovement::UpdateProgress(const float speed, const float delta_time)
{
    m_progress += speed * delta_time;
    m_progress = std::min(1.0f, m_progress); 
    EntityState new_state = m_progress == 1.f ? EntityState::StopMoving : EntityState::Moving;
    return new_state;
}

void EntityMovement::ResetProgress()
{
    m_progress = 0.f;
}

void EntityMovement::DefineMovement(MapDirection direction)
{
    m_direction = direction;
    switch(direction){
        case MapDirection::Up:
            m_move = MapPosition{0, -1};
            break;
        case MapDirection::Down:
            m_move = MapPosition{0, 1};
            break;
        case MapDirection::Left:
            m_move = MapPosition{-1, 0};
            break;
        case MapDirection::Right:
            m_move = MapPosition{1, 0};
            break;
    }
}

void EntityMovement::Initialize(const int tile_size, const MapPosition start_position, const MapPosition end_position)
{
    m_start_position = start_position.ToScenePosition(tile_size);
    m_end_position = end_position.ToScenePosition(tile_size);
    m_progress = 0.;
}

Entity::Entity(TextureController* texture_controller, const std::string& sprite_filepath, Camera* camera, const FileReader* file_reader,
    Tilemap* tilemap, const float speed):
    Drawable(texture_controller, sprite_filepath+".png", camera, ScenePosition{0,0}), MapElement(tilemap),
    m_speed(speed), m_state(EntityState::Free), m_animation(file_reader, sprite_filepath)
    // Remove +".png" if I create RessourceFile struct ?
{
    const Pair<int> sprite_size = m_animation.GetSpriteSize();
    m_texture_width = sprite_size.x;
    m_texture_height = sprite_size.y;
    SetDisplayOffset(ScenePosition{(m_texture_width-tilemap->GetTileSize())/2, m_texture_height-tilemap->GetTileSize()});
}

Entity::~Entity()
{

}

EntityState Entity::GetState() const
{
    return m_state;
}

void Entity::SetState(const EntityState state)
{
    m_state = state;
}

void Entity::Reset()
{
    m_animation.Reset();
    m_state = EntityState::Free;
}

void Entity::StartMovement(const EntityMovement movement, const bool is_first_movement, const bool can_exit_map)
{
    const MapPosition current_position = GetMapPosition();
    MapPosition next_position = current_position + movement.GetMove();
    Tilemap* tilemap = GetTilemap();
    if (tilemap->IsFreePosition(next_position, can_exit_map)){
        m_current_movement = movement;
        m_animation.Initialize(movement.GetDirection(), is_first_movement);
        m_state = EntityState::Moving;
        int tile_size = tilemap->GetTileSize();
        m_current_movement.Initialize(tile_size, current_position, next_position);
        tilemap->FreePosition(current_position);
        tilemap->TakePosition(next_position);
        SetMapPosition(next_position);
    } // Should reset here the animation to use idle sprite when there is a collision ?
}

ScenePosition Entity::ContinueMovement(const float delta_time)
{
    m_state = m_current_movement.UpdateProgress(m_speed, delta_time);
    m_animation.ContinueAnimation(delta_time);
    return m_current_movement.GetScenePosition();
}

ScenePosition Entity::GetFinalDrawingPosition(const ScenePosition sp) const
{
    return (sp-GetDisplayOffset())*m_camera->GetZoom();
}

void Entity::DrawTexture() const
{
    const Pair<int> sprite = m_animation.GetCurrentSprite(); 
    const SDL_Rect src{sprite.x, sprite.y, m_texture_width, m_texture_height};
    const ScenePosition camera_position = m_camera->GetCameraPosition();
    const ScenePosition camera_offset = m_camera->GetOffset(); // Remove
    const float zoom = m_camera->GetZoom();
    const SDL_Rect dst{m_position.x-camera_position.x+camera_offset.x, m_position.y-camera_position.y+camera_offset.y,
                       static_cast<int>(m_texture_width*zoom), static_cast<int>(m_texture_height*zoom)};
    m_texture_controller->RenderTexture(m_texture_key, src, dst);
}