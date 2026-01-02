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

MapPosition EntityMovement::GetStartPosition() const
{
    return m_start_map_position;
}

ScenePosition EntityMovement::GetScenePosition() const
{
    return m_start_position + (m_end_position - m_start_position) * m_progress; // Should be in Interpolation struct ?
}

EntityState EntityMovement::UpdateProgress(const float speed, const float delta_time)
{
    m_progress += speed * delta_time;
    m_progress = std::min(1.0f, m_progress); 
    EntityState new_state = m_progress == 1.f ? EntityState::OnStop : EntityState::Moving;
    return new_state;
}

MapPosition EntityMovement::GetMoveFromDirection(const MapDirection direction) const
{
    switch(direction){
        case MapDirection::Up:
            return MapPosition{0, -1};
        case MapDirection::Down:
            return MapPosition{0, 1};
        case MapDirection::Left:
            return MapPosition{-1, 0};
        case MapDirection::Right:
            return MapPosition{1, 0};
        default: // Should not happen ? (Maybe for MapDirection::None)
            return MapPosition{0, 0}; 
    }
}

MapDirection EntityMovement::GetDirectionFromMove(const MapPosition move) const
{
    if (move.x==0){
        if (move.y==-1) return MapDirection::Up;
        else if (move.y==1) return MapDirection::Down;
    }else{
        if (move.x==-1) return MapDirection::Left;
        else if (move.x==1) return MapDirection::Right;
    }
    return MapDirection::None; // Should not happen ?
}

void EntityMovement::DefineMovement(const MapDirection direction)
{
    m_direction = direction;
    m_move = GetMoveFromDirection(direction);
}

void EntityMovement::Initialize(const int tile_size, const MapPosition start_position, const MapPosition end_position)
{
    m_start_map_position = start_position;
    m_start_position = start_position.ToScenePosition(tile_size);
    m_end_position = end_position.ToScenePosition(tile_size);
    m_progress = 0.;
}

Entity::Entity(TextureController& texture_controller, const std::string& sprite_filepath, Camera& camera, const FileReader& file_reader,
    Tilemap& tilemap, const float speed):
    SceneDrawable(texture_controller, sprite_filepath+".png", camera, ScenePosition{0,0}), MapElement(tilemap),
    m_speed(speed), m_state(EntityState::Free), m_animation(file_reader, sprite_filepath)
    // Remove +".png" if I create RessourceFile struct ?
{
    const AreaSize sprite_size = m_animation.GetSpriteSize();
    m_texture_width = sprite_size.x;
    m_texture_height = sprite_size.y;
    // Sprites could have a different size than tiles
    SetDisplayOffset(ScenePosition{(m_texture_width-tilemap.GetTileSize())/2, m_texture_height-tilemap.GetTileSize()});
}

EntityState Entity::GetState() const
{
    return m_state;
}

EntityMovement Entity::GetCurrentMovement() const
{
    return m_current_movement;
}

void Entity::Reset()
{
    m_animation.Reset();
    m_state = EntityState::Free;
}

void Entity::TryStartMovement(const EntityMovement movement, const bool is_first_movement, const bool can_exit_map)
{
    const MapPosition current_position = GetMapPosition();
    MapPosition next_position = current_position + movement.GetMove();
    const MapBound bound = m_tilemap.IsOutOfMap(next_position);

    if (can_exit_map && bound != MapBound::Inside){
        SetMapPosition(m_tilemap.GetProjectedPosition(next_position, bound));
        const ScenePosition new_position = GetMapPosition().ToScenePosition(m_tilemap.GetTileSize());
        m_position = GetFinalDrawingPosition(new_position);
        // Reset(); ? Will also reset the sprite animation when loading a new map, maybe I don't want that
    }else if (bound == MapBound::Inside && m_tilemap.IsFreePosition(next_position)){
        m_current_movement = movement;
        m_state = EntityState::Moving;

        int tile_size = m_tilemap.GetTileSize();
        m_animation.Initialize(movement.GetDirection(), is_first_movement);
        m_current_movement.Initialize(tile_size, current_position, next_position);
        
        m_tilemap.FreePosition(current_position);
        m_tilemap.TakePosition(next_position);
        SetMapPosition(next_position);
    }else{
        Reset(); // Reset animation to idle + state to Free
    }
}

ScenePosition Entity::ContinueMovement(const float delta_time)
{
    m_state = m_current_movement.UpdateProgress(m_speed, delta_time);
    m_animation.ContinueAnimation(delta_time);
    return m_current_movement.GetScenePosition();
}

ScenePosition Entity::GetFinalDrawingPosition(const ScenePosition sp) const
{
    return (sp-GetDisplayOffset())*m_camera.GetZoom();
}

void Entity::DrawTexture() const
{
    const Vec2 sprite = m_animation.GetCurrentSprite(); 
    const SDL_Rect src{sprite.x, sprite.y, m_texture_width, m_texture_height};
    const ScenePosition camera_position = m_camera.GetPosition()-m_camera.GetScreenOffset();
    const float zoom = m_camera.GetZoom();
    const SDL_Rect dst{m_position.x-camera_position.x, m_position.y-camera_position.y,
                       static_cast<int>(m_texture_width*zoom), static_cast<int>(m_texture_height*zoom)};
    m_texture_controller.RenderTexture(m_texture_key, src, dst);
}

void Entity::OrderStartMovement(const MapDirection direction, const bool is_first_movement, const bool can_exit_map)
{
    EntityMovement movement;
    movement.DefineMovement(direction);
    TryStartMovement(movement, is_first_movement, can_exit_map);
}

void Entity::OrderUpdateMovement(const float delta_time)
{
    m_position = GetFinalDrawingPosition(ContinueMovement(delta_time));
}

float Entity::GetSpeed() const
{
    return m_speed;
}