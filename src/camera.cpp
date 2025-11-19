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

void Camera::SetCameraPosition(const ScenePosition sp)
{
    m_position = sp;
} 

void Camera::MoveCameraPosition(const ScenePosition sp)
{
    m_position += sp;
}

// Should use 2 different version of this : x axis / y axis
void Camera::LookAt(const ScenePosition sp) // Center the camera on a scene position with checking map bounds
{
    ScenePosition camera_position = sp - m_window_size/2;
    camera_position.x = std::max(camera_position.x, 0); 
    camera_position.x = std::min(camera_position.x, m_tilemap_size.x-m_window_size.x); // Map must be render at ScenePosition{0,0}
    camera_position.y = std::max(camera_position.y, 0);
    camera_position.y = std::min(camera_position.y, m_tilemap_size.y-m_window_size.y);
    SetCameraPosition(camera_position);
}

void Camera::SetTilemapSize(const ScenePosition tilemap_size)
{
    m_tilemap_size = tilemap_size;
}