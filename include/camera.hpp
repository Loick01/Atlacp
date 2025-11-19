#pragma once

#include <iostream>

#include "type.hpp"

class Camera
{
    private:
        ScenePosition m_position;
        const ScenePosition m_window_center; // Coordinates at the center of the window (used to look at a specific ScenePosition)

    public:
        Camera(const ScenePosition window_center);
        ~Camera();

        ScenePosition GetCameraPosition() const;
        void SetCameraPosition(const ScenePosition sp); // Set m_position to sp
        void MoveCameraPosition(const ScenePosition sp); // Add the argument sp to m_position (used when moving the player)
        void LookAt(const ScenePosition sp);
};