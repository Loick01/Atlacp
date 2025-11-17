#include "camera.hpp"

Camera::Camera()
{
    m_position = ScreenPosition{0,0};
}

Camera::~Camera()
{

}

ScreenPosition Camera::GetCameraPosition() const
{
    return m_position;
}

void Camera::SetCameraPosition(const ScreenPosition sp)
{
    m_position = sp;
} 

void Camera::MoveCameraPosition(const ScreenPosition sp)
{
    m_position = m_position + sp;
}