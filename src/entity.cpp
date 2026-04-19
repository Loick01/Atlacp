#include "entity.hpp"

EntityMovement::EntityMovement():
    m_direction(MapDirection::None), m_move(MapPosition{0, 0})
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

MapDirection EntityMovement::GetOppositeDirection() const
{
    return static_cast<MapDirection>(((int)m_direction+2)%4);
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
    EntityState newState = m_progress == 1.f ? EntityState::OnStop : EntityState::Moving;
    return newState;
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
        default: // Will throw error ? (for MapDirection::None)
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
    return MapDirection::None; // Will throw error ?
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
    Tilemap& tilemap, const MapDirection initialDirection, const float walkSpeed, const float runSpeed):
    SceneDrawable(textureController, spriteFilepath+".png", camera, ScenePosition{0,0}), MapElement(tilemap),
    m_walkSpeed(walkSpeed), m_runSpeed(runSpeed), m_isRunning(false), m_state(EntityState::Free), m_animation(fileReader, spriteFilepath)
    // Remove +".png" if I create RessourceFile struct ?
{
    const AreaSize spriteSize = m_animation.GetSpriteSize();
    m_textureWidth = spriteSize.x;
    m_textureHeight = spriteSize.y;
    // Sprites could have a different size than tiles
    SetDisplayOffset(ScenePosition{(m_textureWidth-tilemap.GetTileSize())/2, m_textureHeight-tilemap.GetTileSize()});
    Reset(initialDirection);
}

EntityState Entity::GetState() const
{
    return m_state;
}

EntityMovement Entity::GetCurrentMovement() const
{
    return m_currentMovement;
}

void Entity::SetOrientation(const MapDirection direction)
{
    m_animation.Reset(direction);
    m_currentMovement.DefineMovement(direction);
}

void Entity::Reset(const MapDirection direction)
{
    if (direction == MapDirection::None) 
        throw std::invalid_argument("Direction should not be None\n");
    SetOrientation(direction);
    m_state = EntityState::Free;
}

void Entity::TryStartMovement(const EntityMovement movement, const bool isFirstMovement, const bool canExitMap)
{
    const MapPosition currentPosition = GetMapPosition();
    MapPosition targetPosition = currentPosition + movement.GetMove();
    const MapBound bound = m_tilemap.IsOutOfMap(targetPosition); // Rename

    if (canExitMap && bound != MapBound::Inside){
        m_tilemap.LoadAdjacentMap(bound);
        SetMapPosition(m_tilemap.GetProjectedPosition(targetPosition, bound));
        const ScenePosition newPosition = GetMapPosition().ToScenePosition(m_tilemap.GetTileSize());
        m_position = GetFinalDrawingPosition(newPosition);
        // Reset(); ? Will also reset the sprite animation when loading a new map, maybe I don't want that
    }else if (bound == MapBound::Inside && m_tilemap.IsFreePosition(targetPosition)){
        m_currentMovement = movement;
        m_state = EntityState::Moving;

        int tileSize = m_tilemap.GetTileSize();
        m_animation.Initialize(movement.GetDirection(), isFirstMovement);
        m_currentMovement.Initialize(tileSize, currentPosition, targetPosition);
        
        m_tilemap.FreePosition(currentPosition);
        m_tilemap.TakePosition(targetPosition);
        SetMapPosition(targetPosition);
    }else{
        Reset(movement.GetDirection()); // Reset animation to idle + state to Free
    }
}

void Entity::TryStartInteraction(const MapPosition targetPosition)
{
    const MapBound bound = m_tilemap.IsOutOfMap(targetPosition);
    if (bound == MapBound::Inside){
        m_state = EntityState::Interacting;
        SetTargetPosition(targetPosition);
        Notify(EntityEvent::Interaction);
    }
}

ScenePosition Entity::ContinueMovement(const float deltaTime)
{
    m_state = m_currentMovement.UpdateProgress(GetCurrentSpeed(), deltaTime);
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
    // Should berify if direction != MapDirection::None ?
    EntityMovement movement;
    movement.DefineMovement(direction);
    TryStartMovement(movement, isFirstMovement, canExitMap);
}

void Entity::OrderUpdateMovement(const float deltaTime)
{
    m_position = GetFinalDrawingPosition(ContinueMovement(deltaTime));
}

void Entity::OrderInteraction(const MapDirection direction)
{
    EntityMovement movement; // To get targeted position, I need to initialize a EntityMovement (to use GetMoveFromDirection)
    movement.DefineMovement(direction);
    const MapPosition targetPosition = GetMapPosition() + movement.GetMove();
    TryStartInteraction(targetPosition);
}

float Entity::GetWalkSpeed() const
{
    return m_walkSpeed;
}

float Entity::GetRunSpeed() const
{
    return m_runSpeed;
}

float Entity::GetCurrentSpeed() const
{
    return m_isRunning ? m_runSpeed : m_walkSpeed;
}

bool Entity::GetIsRunning() const
{
    return m_isRunning;
}

void Entity::SetState(const EntityState state)
{
    m_state = state;
}

void Entity::SetIsRunning(const bool isRunning)
{
    m_isRunning = isRunning;
}
