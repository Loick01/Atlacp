#include "camera.hpp"

Camera::Camera(const ScenePosition window_center):
    m_window_center(window_center)
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

void Camera::LookAt(const ScenePosition sp) // Center the camera on a scene position without checking any map bounds
{
    SetCameraPosition(sp-m_window_center);
}