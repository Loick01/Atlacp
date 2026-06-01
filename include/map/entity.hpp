#pragma once

#include "animation/animation.hpp"
#include "image/drawable.hpp"
#include "map/element.hpp"
#include "map/movement.hpp"
#include "system/notifier.hpp"

class Camera;
class TextureController;
class Tilemap;

enum class EntityEvent
{
    SortEntity, EnterInteraction, LeaveInteraction
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
        void TryStartInteraction(const MapPosition targetPosition);

    protected:
        Entity(TextureController& textureController, const std::string& spriteFilepath, Camera& camera, const FileReader& fileReader,
            Tilemap& tilemap, const Direction initialDirection, const float walkSpeed, const float runSpeed);
        
        ScenePosition GetFinalDrawingPosition(const ScenePosition sp) const;
    
    public:
        virtual void Update(const float deltaTime) = 0;
        void DrawTexture() const override;

        // Default parameters are used when this function is called by NPC behaviours
        void OrderStartMovement(const Direction direction, const bool isFirstMovement, const bool canExitMap=false); 
        void OrderUpdateMovement(const float deltaTime);

        void OrderInteraction(const Direction direction);
        void LeaveInteraction();

        EntityState GetState() const;
        EntityMovement GetCurrentMovement() const;
        float GetWalkSpeed() const;
        float GetRunSpeed() const;
        float GetCurrentSpeed() const;
        bool GetIsRunning() const;
        void SetState(const EntityState state);
        void SetIsRunning(const bool isRunning);
        void SetOrientation(const Direction direction);
        void Reset(const Direction direction);
};