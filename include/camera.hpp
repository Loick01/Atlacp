#pragma once

#include <algorithm>
#include <iostream>

#include "type.hpp"

class Camera
{
    private:
        ScenePosition m_position;
        const ScenePosition m_window_size;
        ScenePosition m_viewport; // How many tiles (width and height) will be rendered in the window. Should not be a ScenePosition
        
        ScenePosition m_tilemap_size; // Could change when loading a new map
        Pair<bool> m_is_off_screen;
        float m_zoom;

    public:
        Camera(const ScenePosition window_size, const ScenePosition viewport, const int tile_size);
        ~Camera();

        ScenePosition GetCameraPosition() const;
        ScenePosition GetWindowSize() const;
        ScenePosition GetViewport() const;
        Pair<bool> GetIsOffScreen() const;
        float GetZoom() const;
        void AddZoom(const float z);
        void SetTilemapInfo(const ScenePosition tilemap_size);
        void SetCameraPosition(const ScenePosition sp); // Set m_position to sp
        void MoveCameraPosition(const ScenePosition sp); // Add sp to m_position
        void Reset();
        void LookAt(const ScenePosition sp);
};