#pragma once

#include "animation/map_entity_animation.hpp"
#include "core/notifier.hpp"
#include "image/drawable.hpp"
#include "map/map_element.hpp"
#include "map/map_movement.hpp"

class TextureController;

enum class EntityEvent
{
    HasMoved, 
    EnterInteraction, ContinueInteraction,
    ContinueTrigger,
    RequestLoadMap
};

class MapEntity : public SceneDrawable, public MapElement, public Notifier<EntityEvent>
{
    private:
        EntityMovementState m_movementState;
        EntityInteractionState m_interactionState;
        MapMovement m_currentMovement;
        MapEntityAnimation m_animation;
        
        ScenePosition m_displayOffset;
        ScenePosition m_cameraPosition;
        float m_cameraZoom;

        float m_walkSpeed;
        float m_runSpeed;
        unsigned int m_id;
        bool m_isRunning;

        ScenePosition ContinueMovement(const float deltaTime);
        ScenePosition GetFinalDrawingPosition(const ScenePosition sp) const;

        void TryStartMovement(const MapMovement movement, const bool isFirstMovement, const bool canExitMap);
        void TryStartInteraction(const MapPosition targetPosition);

    protected:
        MapEntity(TextureController& textureController, const std::string& spriteFilepath, const FileReader& fileReader,
            Tilemap& tilemap, const Direction initialDirection, const float walkSpeed, const float runSpeed, const float cameraZoom,
            const unsigned int id = 0); // For now, id = 0 is reserved for MapEntity::Player
    
    public:
        EntityMovementState GetMovementState() const;
        EntityInteractionState GetInteractionState() const;
        MapMovement GetCurrentMovement() const;

        ScenePosition GetDisplayOffset() const;

        float GetWalkSpeed() const;
        float GetRunSpeed() const;
        float GetCurrentSpeed() const;

        unsigned int GetId() const;
        bool GetIsRunning() const;

        void SetMovementState(const EntityMovementState state);
        void SetInteractionState(const EntityInteractionState state);

        void SetDisplayOffset(const ScenePosition offset);
        void SetCameraPosition(const ScenePosition sp);
        void SetCameraZoom(const float zoom);
        
        void SetIsRunning(const bool isRunning);
        
        void SetOrientation(const Direction direction);
        void Reset(const Direction direction);

        void OnInteracting(const Direction direction) override;
        void ReleaseInteracting() override;

        void FreePosition();
        void TakePosition();

        virtual void Update(const float deltaTime) = 0;
        void DrawTexture() const override;
        void UpdateDrawingPosition(const ScenePosition sp);

        // Default parameters are used when this function is called by NPC behaviours
        void OrderStartMovement(const Direction direction, const bool isFirstMovement, const bool canExitMap=false); 
        void OrderUpdateMovement(const float deltaTime);

        void OrderInteraction(const Direction direction);
        void ContinueInteraction();
        void ContinueTrigger();
        // TODO : void LeaveInteraction(); --> Notify(EntityEvent::LeaveInteraction); (???)
};