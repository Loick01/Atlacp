#include "camera.hpp"

Camera::Camera(Window& window, const GridSize rangeTile, const int tileSize):
    m_rangeTile(rangeTile)
{
    m_position = ScenePosition{0,0};
    const int windowWidth = window.GetWidth();
    const int windowHeight = window.GetHeight();
    const Pair<float> bestPossibleZoom = {(windowWidth/rangeTile.x)/static_cast<float>(tileSize),
                                            (windowHeight/rangeTile.y)/static_cast<float>(tileSize)};
    m_zoom = std::min(bestPossibleZoom.x, bestPossibleZoom.y);
    m_viewport = m_rangeTile*tileSize*m_zoom;

    const ScenePosition outsideViewport = ScenePosition{windowWidth, windowHeight} - m_viewport; // Should rename ?
    if (bestPossibleZoom.x < bestPossibleZoom.y){ // Letterboxing
        window.SetBoxing(0, windowHeight-outsideViewport.y/2, windowWidth, outsideViewport.y/2);
        m_screenOffset = ScenePosition{0, outsideViewport.y/2};
    }else{ // Pillarboxing
        window.SetBoxing(windowWidth-outsideViewport.x/2, 0, outsideViewport.x/2, windowHeight);
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

AreaSize Camera::GetViewport() const
{
    return m_viewport;
}

GridSize Camera::GetRangeTile() const
{
    return m_rangeTile;
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

void Camera::SetTilemapInfo(const AreaSize tilemapSize)
{
    m_tilemapSize = tilemapSize*m_zoom;
    m_isOffScreen = m_tilemapSize > m_viewport;
}

void Camera::SetCameraPosition(const ScenePosition sp)
{
    m_position = sp;
} 

void Camera::MoveCameraPosition(const ScenePosition sp)
{
    m_position += sp;
} 

void Camera::Reset() // Used in editor
{
    m_zoom = 1.0f;
    SetCameraPosition(ScenePosition{0,0});
}

// Should use 2 different version of this : x axis / y axis
void Camera::LookAt(const ScenePosition sp) // Center the camera on a scene position with checking map bounds
{
    ScenePosition cameraPosition = sp - m_viewport/2;
    if (m_isOffScreen.x) cameraPosition.x = std::clamp(cameraPosition.x, 0, m_tilemapSize.x - m_viewport.x); // Map must be render at ScenePosition{0,0}
    else cameraPosition.x = m_tilemapSize.x/2 - m_viewport.x/2;
    if (m_isOffScreen.y) cameraPosition.y = std::clamp(cameraPosition.y, 0, m_tilemapSize.y - m_viewport.y); // Map must be render at ScenePosition{0,0}
    else cameraPosition.y = m_tilemapSize.y/2 - m_viewport.y/2;
    SetCameraPosition(cameraPosition);
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