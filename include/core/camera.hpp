#pragma once

#include <algorithm> // std::clamp

#include "core/core_types.hpp" // GridSize, AreaSize
#include "core/notifier.hpp"
#include "image/image_types.hpp" // ScenePosition

class MapEntity;
class Window;

enum class CameraState
{
    Free, Anchored, Sliding, DoneSliding // TODO : Rename DoneSliding (it will not be used only for CameraSlideToEntity/PositionOrder)
};
 
struct SlidingInfo // Could use Animation instead ?
{
    ScenePosition endPosition;
};

class Camera : public Notifier<UselessEvent> // Will Notify() for Order execution
{
    private:
        Vec2f m_position;
        GridSize m_rangeTile; // How many tiles (width and height) will be rendered in the window. Should not be a ScenePosition
        AreaSize m_viewport; // Size of rendering area
        
        ScreenPosition m_screenOffset;
        AreaSize m_tilemapSize; // Could change when loading a new map
        Pair<bool> m_isOffScreen;
        float m_zoom;
        float m_speed; // Will use this value when sliding to a ScenePosition (CameraSlideToPositionOrder) or a MapEntity (CameraSlideToEntityOrder)
        
        bool m_shouldCulling;
        Pair<int> m_startIndex;
        Pair<int> m_endIndex;
        CameraState m_cameraState;
        SlidingInfo m_slidingInfo;

        MapEntity* m_anchoredEntity; // Should be SceneDrawable m_anchoredDrawable ?

    public:
        Camera();

        void ComputeViewport(Window& window, const GridSize rangeTile, const int tileSize);
        ScreenPosition GetScreenOffset() const;
        ScenePosition GetPosition() const;
        ScenePosition GetConstrainedCameraPosition(const ScenePosition sp) const; // Return the closest ScenePosition to sp which respect the map culling
        AreaSize GetViewport() const;
        CameraState GetCameraState() const;
        Pair<bool> GetIsOffScreen() const;
        Pair<int> GetStartIndex() const;
        Pair<int> GetEndIndex() const;
        float GetZoom() const;
        void AddZoom(const float z);
        void SetCameraState(const CameraState state);
        void SetTilemapInfo(const AreaSize tilemapSize);
        void SetCameraPosition(const ScenePosition sp);
        void MoveCameraPosition(const Vec2f v);
        void MoveCameraPosition(const ScenePosition sp); 
        void ResetPositionAndZoom();
        void LookAt(const ScenePosition sp);
        void SetAnchoredEntity(MapEntity* entity);

        void SetShouldCulling(const bool shouldCulling);
        void ComputeMapCulling(const GridSize layerSize, const int tileSize);

        void Update(const float deltaTime);
        void StartSlidingTo(const ScenePosition sp);
};