#pragma once

#include <algorithm>
#include <iostream>

#include "type.hpp"

class Camera
{
    private:
        ScenePosition m_position;
        const ScenePosition m_window_size;
        ScenePosition m_range_tile; // How many tiles can fit in the window (width and height), used for map culling. This should not be a ScenePosition
        ScenePosition m_tilemap_size; // Could change when loading a new map

    public:
        Camera(const ScenePosition window_size);
        ~Camera();

        ScenePosition GetCameraPosition() const;
        ScenePosition GetWindowSize() const;
        ScenePosition GetRangeTile() const;
        void SetRangeTile(const int tile_size);
        void SetCameraPosition(const ScenePosition sp); // Set m_position to sp
        void MoveCameraPosition(const ScenePosition sp); // Add the argument sp to m_position (used when moving the player)
        void LookAt(const ScenePosition sp);
        void SetTilemapSize(const ScenePosition tilemap_size);
};