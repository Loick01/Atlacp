#pragma once

#include <iostream>

#include "type.hpp"

class Camera
{
    private:
        ScreenPosition m_position;

    public:
        Camera();
        ~Camera();

        ScreenPosition GetCameraPosition() const;
        void SetCameraPosition(const ScreenPosition sp); // Set m_position to sp
        void MoveCameraPosition(const ScreenPosition sp); // Add the argument sp to m_position (used when moving the player)
};