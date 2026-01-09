#pragma once 

#include <iostream>
#include <vector>

#include "camera.hpp"
#include "event.hpp"
#include "file.hpp"
#include "npc.hpp"
#include "player.hpp"
#include "texture.hpp"
#include "tilemap.hpp"
#include "time.hpp"
#include "ui.hpp"
#include "window.hpp"

class Scene
{
    protected:
        Window m_window;
        FileReader m_file_reader;
        TextureController m_texture_controller;
        Camera m_camera;
        bool m_gameloop;

        std::vector<const Drawable*> m_drawables; // Will be removed ?

    public:
        Scene();
        virtual void Gameloop() = 0;
        bool GetGameloop() const;
};

class TilemapScene : public Scene
{
    protected:
        std::vector<const TileLayer*> m_layers;
        Tileset m_tileset;
        Tilemap m_tilemap;

        void UpdateTilemapLayer(); // Use in constructor + when a new map is loading --> m_tilemap.AddListener(...);

    public:
        TilemapScene();
};

class GameplayTilemapScene : public TilemapScene
{
    private:
        Time m_time;
        GameplayEventController m_event_controller; // Will be in Scene as a EventController 
        GameplayUiController m_ui_controller; // Will be in Scene as a UiController
        Player m_player;
        std::vector<Entity*> m_rendered_entities; // Sorted by y position
        // A specific order could be necessary for Entity updating (for example with FollowEntityBehaviour), I use a second vector of Entity*
        std::vector<Entity*> m_updated_entities;

        // In GameplayTilemapScene, TileLayer are rendered in two part : low_layer then high_layer. So I can draw entities between layers
        // It might be better to have 2 dinstinct vectors of TileLayer in TilemapScene ?
        const size_t m_layers_split_index; // Should not be const ?
        
    public:
        GameplayTilemapScene();
        ~GameplayTilemapScene();
        void Gameloop() override;
};

class EditorTilemapScene : public TilemapScene
{
    private:
        EditorEventController m_event_controller; // Should be in Scene as a EventController ?
        EditorUiController m_ui_controller; // Will be in Scene as a UiController

    public:
        EditorTilemapScene();
        void Gameloop() override;
};

class FightScene : public Scene
{
    private:
        EventController m_event_controller;

    public:
        FightScene();
        void Gameloop() override;
};