#pragma once

#include <iostream>

#include "drawable.hpp"
#include "element.hpp"

enum class EntityState // Should be called MovementState, and I should add AnimationState
{
    Free,
    Moving,
    OnStop
};

class EntityMovement
{
    private:
        MapPosition m_move;
        MapDirection m_direction;

        MapPosition m_startMapPosition;
        ScenePosition m_startPosition;
        ScenePosition m_endPosition;
        float m_progress;

    public:
        EntityMovement();
        ~EntityMovement();

        MapPosition GetMove() const;
        MapDirection GetDirection() const;
        MapPosition GetStartPosition() const;
        ScenePosition GetScenePosition() const;
        EntityState UpdateProgress(const float speed, const float deltaTime); // Return the new state the entity should have
        
        // Not sure to keep these 2 functions
        MapPosition GetMoveFromDirection(const MapDirection direction) const;
        MapDirection GetDirectionFromMove(const MapPosition move) const;

        void DefineMovement(const MapDirection direction);
        void Initialize(const int tileSize, const MapPosition startPosition, const MapPosition endPosition);
};

enum class EntityEvent
{
    SortEntity
};

// Rename MapEntity ?
class Entity : public SceneDrawable, public MapElement, public Notifier<EntityEvent>
{
    private:
        EntityMovement m_currentMovement;
        Animation m_animation;
        EntityState m_state;
        float m_walkSpeed;
        float m_runSpeed;
        bool m_isRunning;

        ScenePosition ContinueMovement(const float deltaTime);
        void TryStartMovement(const EntityMovement movement, const bool isFirstMovement, const bool canExitMap);

    protected:
        Entity(TextureController& textureController, const std::string& spriteFilepath, Camera& camera, const FileReader& fileReader,
            Tilemap& tilemap, const float walkSpeed, const float runSpeed);
        
        ScenePosition GetFinalDrawingPosition(const ScenePosition sp) const;
    
    public:
        virtual void Update(const float deltaTime) = 0;
        void DrawTexture() const override;

        // Default parameters are used when this function is called by NPC behaviours
        void OrderStartMovement(const MapDirection direction, const bool isFirstMovement, const bool canExitMap=false); 
        void OrderUpdateMovement(const float deltaTime);

        EntityState GetState() const;
        EntityMovement GetCurrentMovement() const;
        float GetWalkSpeed() const;
        float GetRunSpeed() const;
        float GetCurrentSpeed() const;
        bool GetIsRunning() const;
        void SetIsRunning(const bool isRunning);
        void Reset(const MapDirection direction=MapDirection::None);
};