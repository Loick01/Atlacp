#pragma once

#include <algorithm>
#include <iostream>

#include "type.hpp"
#include "window.hpp"

class Camera
{
    private:
        ScenePosition m_position;
        ScenePosition m_range_tile; // How many tiles (width and height) will be rendered in the window. Should not be a ScenePosition
        ScenePosition m_viewport; // Size of rendering area
        
        ScenePosition m_tilemap_size; // Could change when loading a new map
        Pair<bool> m_is_off_screen;
        float m_zoom;

        ScenePosition m_offset; // Remove

    public:
        Camera(Window* window, const ScenePosition range_tile, const int tile_size);
        ~Camera();

        ScenePosition GetOffset() const; // Remove
        ScenePosition GetPosition() const;
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