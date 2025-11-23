#include "camera.hpp"

Camera::Camera(const ScenePosition window_size):
    m_window_size(window_size)
{
    m_position = ScenePosition{0,0};
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

ScenePosition Camera::GetRangeTile() const
{
    return m_range_tile;
}

Pair<bool> Camera::GetCulling() const
{
    return m_culling;
}

void Camera::SetTilemapInfo(const ScenePosition tilemap_size, const int tile_size)
{
    m_tilemap_size = tilemap_size;
    m_range_tile = m_window_size/tile_size+1;
    m_culling = m_tilemap_size > m_window_size;
}

void Camera::SetCameraPosition(const ScenePosition sp)
{
    m_position = sp;
} 

void Camera::ResetCameraPosition()
{
    SetCameraPosition(ScenePosition{0,0});
}

// Should use 2 different version of this : x axis / y axis
void Camera::LookAt(const ScenePosition sp) // Center the camera on a scene position with checking map bounds
{
    ScenePosition camera_position = sp - m_window_size/2;
    if (m_culling.x) camera_position.x = std::clamp(camera_position.x, 0, m_tilemap_size.x-m_window_size.x); // Map must be render at ScenePosition{0,0}
    else camera_position.x = m_tilemap_size.x/2 - m_window_size.x/2;
    if (m_culling.y) camera_position.y = std::clamp(camera_position.y, 0, m_tilemap_size.y-m_window_size.y); // Map must be render at ScenePosition{0,0}
    else camera_position.y = m_tilemap_size.y/2 - m_window_size.y/2;
    SetCameraPosition(camera_position);
}