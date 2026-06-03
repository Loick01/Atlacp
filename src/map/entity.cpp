#include "map/map_entity.hpp"

#include <SDL2/SDL_rect.h>

#include "image/texture.hpp"
#include "system/camera.hpp"
#include "tile/tilemap.hpp"

MapEntity::MapEntity(TextureController& textureController, const std::string& spriteFilepath, Camera& camera, const FileReader& fileReader,
    Tilemap& tilemap, const Direction initialDirection, const float walkSpeed, const float runSpeed):
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

EntityState MapEntity::GetState() const
{
    return m_state;
}

MapMovement MapEntity::GetCurrentMovement() const
{
    return m_currentMovement;
}

void MapEntity::SetOrientation(const Direction direction)
{
    m_animation.Reset(direction);
    m_currentMovement.DefineMovement(direction);
}

void MapEntity::Reset(const Direction direction)
{
    if (direction == Direction::None) 
        throw std::invalid_argument("Direction should not be None\n");
    SetOrientation(direction);
    m_state = EntityState::Free;
}

void MapEntity::TryStartMovement(const MapMovement movement, const bool isFirstMovement, const bool canExitMap)
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

void MapEntity::TryStartInteraction(const MapPosition targetPosition)
{
    const MapBound bound = m_tilemap.IsOutOfMap(targetPosition);
    if (bound == MapBound::Inside){
        m_state = EntityState::Interacting;
        SetTargetPosition(targetPosition);
        Notify(EntityEvent::EnterInteraction);
    }
}

void MapEntity::LeaveInteraction()
{
    // SetState(Free) should be here rather than inside InteractionController::EndInteraction()
    Notify(EntityEvent::LeaveInteraction);
}

ScenePosition MapEntity::ContinueMovement(const float deltaTime)
{
    m_state = m_currentMovement.UpdateProgress(GetCurrentSpeed(), deltaTime);
    m_animation.ContinueAnimation(deltaTime);
    return m_currentMovement.GetScenePosition();
}

ScenePosition MapEntity::GetFinalDrawingPosition(const ScenePosition sp) const
{
    return (sp-GetDisplayOffset())*m_camera.GetZoom();
}

void MapEntity::DrawTexture() const
{
    const Vec2 sprite = m_animation.GetCurrentSprite(); 
    const SDL_Rect src{sprite.x, sprite.y, m_textureWidth, m_textureHeight};
    const ScenePosition cameraPosition = m_camera.GetPosition()-m_camera.GetScreenOffset();
    const float zoom = m_camera.GetZoom();
    const SDL_Rect dst{m_position.x-cameraPosition.x, m_position.y-cameraPosition.y,
                       static_cast<int>(m_textureWidth*zoom), static_cast<int>(m_textureHeight*zoom)};
    m_textureController.RenderTexture(m_textureKey, src, dst);
}

void MapEntity::OrderStartMovement(const Direction direction, const bool isFirstMovement, const bool canExitMap)
{
    // Should berify if direction != Direction::None ?
    MapMovement movement;
    movement.DefineMovement(direction);
    TryStartMovement(movement, isFirstMovement, canExitMap);
}

void MapEntity::OrderUpdateMovement(const float deltaTime)
{
    m_position = GetFinalDrawingPosition(ContinueMovement(deltaTime));
}

void MapEntity::OrderInteraction(const Direction direction)
{
    MapMovement movement; // To get targeted position, I need to initialize a MapMovement (to use GetMoveFromDirection)
    movement.DefineMovement(direction);
    const MapPosition targetPosition = GetMapPosition() + movement.GetMove();
    TryStartInteraction(targetPosition);
}

float MapEntity::GetWalkSpeed() const
{
    return m_walkSpeed;
}

float MapEntity::GetRunSpeed() const
{
    return m_runSpeed;
}

float MapEntity::GetCurrentSpeed() const
{
    return m_isRunning ? m_runSpeed : m_walkSpeed;
}

bool MapEntity::GetIsRunning() const
{
    return m_isRunning;
}

void MapEntity::SetState(const EntityState state)
{
    m_state = state;
}

void MapEntity::SetIsRunning(const bool isRunning)
{
    m_isRunning = isRunning;
}