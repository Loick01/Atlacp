#pragma once

#include <algorithm>
#include <iostream>

#include "type.hpp"

class Camera
{
    private:
        ScenePosition m_position;
        const ScenePosition m_window_size;

        // Rename m_range_tile
        ScenePosition m_range_tile; // How many tiles can fit in the window (width and height)
        // Currently this value is get from window size, but this value will be given to the initialization of the camera (and will never change)
        // Camera zoom will be compute from this value  
        
        ScenePosition m_tilemap_size; // Could change when loading a new map
        Pair<bool> m_is_off_screen;
        float m_zoom;

    public:
        Camera(const ScenePosition window_size);
        ~Camera();

        ScenePosition GetCameraPosition() const;
        ScenePosition GetWindowSize() const;
        ScenePosition GetRangeTile() const;
        Pair<bool> GetIsOffScreen() const;
        float GetZoom() const;
        void AddZoom(const float z);
        void SetTilemapInfo(const ScenePosition tilemap_size, const int tile_size);
        void SetCameraPosition(const ScenePosition sp); // Set m_position to sp
        void MoveCameraPosition(const ScenePosition sp); // Add sp to m_position
        void Reset();
        void LookAt(const ScenePosition sp);
};