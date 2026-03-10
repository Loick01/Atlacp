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
    return m_startMapPosition;
}

ScenePosition EntityMovement::GetScenePosition() const
{
    return m_startPosition + (m_endPosition - m_startPosition) * m_progress; // Should be in Interpolation struct ?
}

EntityState EntityMovement::UpdateProgress(const float speed, const float deltaTime)
{
    m_progress += speed * deltaTime;
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

void EntityMovement::Initialize(const int tileSize, const MapPosition startPosition, const MapPosition endPosition)
{
    m_startMapPosition = startPosition;
    m_startPosition = startPosition.ToScenePosition(tileSize);
    m_endPosition = endPosition.ToScenePosition(tileSize);
    m_progress = 0.;
}

Entity::Entity(TextureController& textureController, const std::string& spriteFilepath, Camera& camera, const FileReader& fileReader,
    Tilemap& tilemap, const float speed):
    SceneDrawable(textureController, spriteFilepath+".png", camera, ScenePosition{0,0}), MapElement(tilemap),
    m_speed(speed), m_state(EntityState::Free), m_animation(fileReader, spriteFilepath)
    // Remove +".png" if I create RessourceFile struct ?
{
    const AreaSize spriteSize = m_animation.GetSpriteSize();
    m_textureWidth = spriteSize.x;
    m_textureHeight = spriteSize.y;
    // Sprites could have a different size than tiles
    SetDisplayOffset(ScenePosition{(m_textureWidth-tilemap.GetTileSize())/2, m_textureHeight-tilemap.GetTileSize()});
}

EntityState Entity::GetState() const
{
    return m_state;
}

EntityMovement Entity::GetCurrentMovement() const
{
    return m_currentMovement;
}

void Entity::Reset(const MapDirection direction)
{
    m_animation.Reset(direction);
    m_state = EntityState::Free;
}

void Entity::TryStartMovement(const EntityMovement movement, const bool isFirstMovement, const bool canExitMap)
{
    const MapPosition currentPosition = GetMapPosition();
    MapPosition next_position = currentPosition + movement.GetMove();
    const MapBound bound = m_tilemap.IsOutOfMap(next_position);

    if (canExitMap && bound != MapBound::Inside){
        SetMapPosition(m_tilemap.GetProjectedPosition(next_position, bound));
        const ScenePosition new_position = GetMapPosition().ToScenePosition(m_tilemap.GetTileSize());
        m_position = GetFinalDrawingPosition(new_position);
        // Reset(); ? Will also reset the sprite animation when loading a new map, maybe I don't want that
    }else if (bound == MapBound::Inside && m_tilemap.IsFreePosition(next_position)){
        m_currentMovement = movement;
        m_state = EntityState::Moving;

        int tileSize = m_tilemap.GetTileSize();
        m_animation.Initialize(movement.GetDirection(), isFirstMovement);
        m_currentMovement.Initialize(tileSize, currentPosition, next_position);
        
        m_tilemap.FreePosition(currentPosition);
        m_tilemap.TakePosition(next_position);
        SetMapPosition(next_position);
    }else{
        Reset(movement.GetDirection()); // Reset animation to idle + state to Free
    }
}

ScenePosition Entity::ContinueMovement(const float deltaTime)
{
    m_state = m_currentMovement.UpdateProgress(m_speed, deltaTime);
    m_animation.ContinueAnimation(deltaTime);
    return m_currentMovement.GetScenePosition();
}

ScenePosition Entity::GetFinalDrawingPosition(const ScenePosition sp) const
{
    return (sp-GetDisplayOffset())*m_camera.GetZoom();
}

void Entity::DrawTexture() const
{
    const Vec2 sprite = m_animation.GetCurrentSprite(); 
    const SDL_Rect src{sprite.x, sprite.y, m_textureWidth, m_textureHeight};
    const ScenePosition cameraPosition = m_camera.GetPosition()-m_camera.GetScreenOffset();
    const float zoom = m_camera.GetZoom();
    const SDL_Rect dst{m_position.x-cameraPosition.x, m_position.y-cameraPosition.y,
                       static_cast<int>(m_textureWidth*zoom), static_cast<int>(m_textureHeight*zoom)};
    m_textureController.RenderTexture(m_textureKey, src, dst);
}

void Entity::OrderStartMovement(const MapDirection direction, const bool isFirstMovement, const bool canExitMap)
{
    EntityMovement movement;
    movement.DefineMovement(direction);
    TryStartMovement(movement, isFirstMovement, canExitMap);
}

void Entity::OrderUpdateMovement(const float deltaTime)
{
    m_position = GetFinalDrawingPosition(ContinueMovement(deltaTime));
}

float Entity::GetSpeed() const
{
    return m_speed;
}