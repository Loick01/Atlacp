#include "camera.hpp"

Camera::Camera(const ScenePosition window_size, const ScenePosition viewport, const int tile_size):
    m_window_size(window_size), m_viewport(viewport)
{
    m_position = ScenePosition{0,0};
    const Pair<float> best_possible_zoom = {(window_size.x/viewport.x)/static_cast<float>(tile_size),
                                            (window_size.y/viewport.y)/static_cast<float>(tile_size)};
    m_zoom = std::min(best_possible_zoom.x, best_possible_zoom.y);
}

Camera::~Camera()
{

}

ScenePosition Camera::GetCameraPosition() const
{
    return m_position;
}

ScenePosition Camera::GetWindowSize() const
{
    return m_window_size;
}

ScenePosition Camera::GetViewport() const
{
    return m_viewport;
}

Pair<bool> Camera::GetIsOffScreen() const
{
    return m_is_off_screen;
}

float Camera::GetZoom() const
{
    return m_zoom;
}

void Camera::AddZoom(const float z)
{
    m_zoom += z;
}   

void Camera::SetTilemapInfo(const ScenePosition tilemap_size)
{
    m_tilemap_size = tilemap_size;
    m_is_off_screen = m_tilemap_size > m_window_size;
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
    ScenePosition camera_position = sp - m_window_size/2;
    if (m_is_off_screen.x) camera_position.x = std::clamp(camera_position.x, 0, m_tilemap_size.x-m_window_size.x); // Map must be render at ScenePosition{0,0}
    else camera_position.x = m_tilemap_size.x/2 - m_window_size.x/2;
    if (m_is_off_screen.y) camera_position.y = std::clamp(camera_position.y, 0, m_tilemap_size.y-m_window_size.y); // Map must be render at ScenePosition{0,0}
    else camera_position.y = m_tilemap_size.y/2 - m_window_size.y/2;
    SetCameraPosition(camera_position);
}