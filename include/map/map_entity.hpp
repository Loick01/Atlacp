#pragma once

#include "animation/map_entity_animation.hpp"
#include "core/notifier.hpp"
#include "image/drawable.hpp"
#include "map/map_element.hpp"
#include "map/map_movement.hpp"

class Camera;
class TextureController;
class Tilemap;

enum class EntityEvent
{
    SortEntity, EnterInteraction, ContinueInteraction, LeaveInteraction
};

class MapEntity : public SceneDrawable, public MapElement, public Notifier<EntityEvent>
{
    private:
        MapMovement m_currentMovement;
        MapEntityAnimation m_animation;
        EntityState m_state;
        float m_walkSpeed;
        float m_runSpeed;
        bool m_isRunning;

        ScenePosition ContinueMovement(const float deltaTime);
        void TryStartMovement(const MapMovement movement, const bool isFirstMovement, const bool canExitMap);
        void TryStartInteraction(const MapPosition targetPosition);

    protected:
        MapEntity(TextureController& textureController, const std::string& spriteFilepath, Camera& camera, const FileReader& fileReader,
            Tilemap& tilemap, const Direction initialDirection, const float walkSpeed, const float runSpeed);
        
        ScenePosition GetFinalDrawingPosition(const ScenePosition sp) const;
    
    public:
        virtual void Update(const float deltaTime) = 0;
        void DrawTexture() const override;

        // Default parameters are used when this function is called by NPC behaviours
        void OrderStartMovement(const Direction direction, const bool isFirstMovement, const bool canExitMap=false); 
        void OrderUpdateMovement(const float deltaTime);

        void OrderInteraction(const Direction direction);
        void ContinueInteraction();
        void LeaveInteraction();

        EntityState GetState() const;
        MapMovement GetCurrentMovement() const;
        float GetWalkSpeed() const;
        float GetRunSpeed() const;
        float GetCurrentSpeed() const;
        bool GetIsRunning() const;
        void SetState(const EntityState state);
        void SetIsRunning(const bool isRunning);
        void SetOrientation(const Direction direction);
        void Reset(const Direction direction);

        void OnInteracting(const Direction direction) override;
        void ReleaseInteracting() override;
};