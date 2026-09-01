#include "core/camera.hpp"

#include "core/window.hpp"
#include "map/map_entity.hpp"

Camera::Camera():
    m_position(ScenePosition{0,0}), m_speed(400.f), m_cameraState(CameraState::Anchored),
    m_anchoredEntity(nullptr)
{}

void Camera::ComputeViewport(Window& window, const GridSize rangeTile, const int tileSize)
{
    m_rangeTile = rangeTile;
    const AreaSize windowSize = window.GetSize();
    const Pair<float> bestPossibleZoom = {(windowSize.x/m_rangeTile.x)/static_cast<float>(tileSize),
                                            (windowSize.y/m_rangeTile.y)/static_cast<float>(tileSize)};
    m_zoom = std::min(bestPossibleZoom.x, bestPossibleZoom.y);
    m_viewport = m_rangeTile*tileSize*m_zoom;
    const ScenePosition outsideViewport = ScenePosition{windowSize.x, windowSize.y} - m_viewport; // Should rename ?
    if (bestPossibleZoom.x < bestPossibleZoom.y){ // Letterboxing
        window.SetBoxing(0, windowSize.y-outsideViewport.y/2, windowSize.x, outsideViewport.y/2);
        m_screenOffset = ScenePosition{0, outsideViewport.y/2};
    }else{ // Pillarboxing
        window.SetBoxing(windowSize.x-outsideViewport.x/2, 0, outsideViewport.x/2, windowSize.y);
        m_screenOffset = ScenePosition{outsideViewport.x/2, 0};
    }
}

ScreenPosition Camera::GetScreenOffset() const
{   
    return m_screenOffset;
}

ScenePosition Camera::GetPosition() const
{
    return m_position;
}

ScenePosition Camera::GetConstrainedCameraPosition(const ScenePosition sp) const
{
    ScenePosition pos = sp - m_viewport/2;
    if (m_isOffScreen.x) pos.x = std::clamp(pos.x, 0, m_tilemapSize.x - m_viewport.x); // Map must be render at ScenePosition{0,0}
    else pos.x = m_tilemapSize.x/2 - m_viewport.x/2;
    if (m_isOffScreen.y) pos.y = std::clamp(pos.y, 0, m_tilemapSize.y - m_viewport.y); // Map must be render at ScenePosition{0,0}
    else pos.y = m_tilemapSize.y/2 - m_viewport.y/2;
    return pos;
}

AreaSize Camera::GetViewport() const
{
    return m_viewport;
}

CameraState Camera::GetCameraState() const
{
    return m_cameraState;
}

Pair<int> Camera::GetStartIndex() const
{
    return m_startIndex;
}

Pair<int> Camera::GetEndIndex() const
{
    return m_endIndex;
}

Pair<bool> Camera::GetIsOffScreen() const
{
    return m_isOffScreen;
}

float Camera::GetZoom() const
{
    return m_zoom;
}

void Camera::AddZoom(const float z)
{
    m_zoom += z;
}   

void Camera::SetCameraState(const CameraState state)
{
    m_cameraState = state;
}

void Camera::SetTilemapInfo(const AreaSize tilemapSize)
{
    m_tilemapSize = tilemapSize*m_zoom;
    m_isOffScreen = m_tilemapSize > m_viewport;
}

void Camera::SetCameraPosition(const ScenePosition sp)
{
    m_position = sp;
} 

void Camera::MoveCameraPosition(const Vec2f v)
{
    m_position += v;
} 

void Camera::MoveCameraPosition(const ScenePosition sp)
{
    MoveCameraPosition(Vec2f(sp));
} 

void Camera::ResetPositionAndZoom() // Used in editor
{
    m_zoom = 1.0f;
    SetCameraPosition(ScenePosition{0,0});
}

// Should use 2 different version of this : x axis / y axis
void Camera::LookAt(const ScenePosition sp) // Center the camera on a scene position with checking map bounds
{
    ScenePosition constrainedPosition = GetConstrainedCameraPosition(sp);
    SetCameraPosition(constrainedPosition);
}

void Camera::SetAnchoredEntity(MapEntity* entity)
{
    m_anchoredEntity = entity;
}

void Camera::SetShouldCulling(const bool shouldCulling)
{
    m_shouldCulling = shouldCulling;
}

void Camera::ComputeMapCulling(const GridSize layerSize, const int tileSize)
{
    m_startIndex = GridSize{0, 0};
    m_endIndex = layerSize;
    if (m_shouldCulling){ // No map culling in editor (find better way than just use a bool ?)
        if (m_isOffScreen.x){
            m_startIndex.x = m_position.x/(tileSize*m_zoom);
            // While animating a movement, m_endIndex could not be enough to fill the window with the map
            // So I add 1 to m_endIndex, and check if it becomes greater than map size
            m_endIndex.x = std::min(m_endIndex.x, m_startIndex.x + m_rangeTile.x + 1);
        }
        if (m_isOffScreen.y){
            m_startIndex.y = m_position.y/(tileSize*m_zoom);
            m_endIndex.y = std::min(m_endIndex.y, m_startIndex.y + m_rangeTile.y + 1);
        }
    }
}

void Camera::Update(const float deltaTime)
{
    switch (m_cameraState) {
        case CameraState::Anchored : {
            LookAt(m_anchoredEntity->GetScenePosition() + m_anchoredEntity->GetDisplayOffset());
            break;
        }
        case CameraState::Sliding : {
            Vec2f toEndPosition = m_slidingInfo.endPosition - ScenePosition(m_position); 
            const float remainingDistance = toEndPosition.Norm();
            const float frameDistance = m_speed * deltaTime;
            if (frameDistance >= remainingDistance) {
                SetCameraPosition(m_slidingInfo.endPosition);
                m_cameraState = CameraState::DoneSliding;
                Notify(UselessEvent::None);
            } else {
                MoveCameraPosition(toEndPosition.Normalize() * frameDistance);
            }
            break;
        }
        default:
            break;
    }
}

void Camera::StartSlidingTo(const ScenePosition sp)
{
    m_slidingInfo.endPosition = GetConstrainedCameraPosition(sp);
    m_cameraState = CameraState::Sliding;
}