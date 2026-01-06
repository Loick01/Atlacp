#pragma once

#include <algorithm>
#include <iostream>

#include "type.hpp"
#include "window.hpp"

class Camera
{
    private:
        ScenePosition m_position;
        GridSize m_range_tile; // How many tiles (width and height) will be rendered in the window. Should not be a ScenePosition
        AreaSize m_viewport; // Size of rendering area
        
        ScreenPosition m_screen_offset;
        AreaSize m_tilemap_size; // Could change when loading a new map
        Pair<bool> m_is_off_screen;
        float m_zoom;

    public:
        Camera(Window& window, const GridSize range_tile, const int tile_size);

        ScreenPosition GetScreenOffset() const;
        ScenePosition GetPosition() const;
        GridSize GetRangeTile() const;
        AreaSize GetViewport() const;
        Pair<bool> GetIsOffScreen() const;
        float GetZoom() const;
        void AddZoom(const float z);
        void SetTilemapInfo(const AreaSize tilemap_size);
        void SetCameraPosition(const ScenePosition sp); // Set m_position to sp
        void MoveCameraPosition(const ScenePosition sp); // Add sp to m_position
        void Reset();
        void LookAt(const ScenePosition sp);
};