#pragma once 

#include <iostream>
#include <memory>
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

class Scene : public Notifier
{
    protected:
        Window& m_window; // The Window instance comes from SceneController
        FileReader m_file_reader; // Should be in SceneController ?
        TextureController m_texture_controller; // Should be in SceneController ?
        Camera m_camera;
        bool m_gameloop;

        std::vector<const Drawable*> m_drawables; // Will be removed ?

    public:
        Scene(Window& window);
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
        TilemapScene(Window& window, const bool should_culling);
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
        GameplayTilemapScene(Window& window);
        ~GameplayTilemapScene();
        void Gameloop() override;
};

class EditorTilemapScene : public TilemapScene
{
    private:
        EditorEventController m_event_controller; // Should be in Scene as a EventController ?
        EditorUiController m_ui_controller; // Will be in Scene as a UiController

    public:
        EditorTilemapScene(Window& window);
        void Gameloop() override;
};

class BattleScene : public Scene
{
    private:
        EventController m_event_controller;
        BattleUiController m_ui_controller; // Will be in Scene as a UiController

    public:
        BattleScene(Window& window);
        void Gameloop() override;
};

class SceneController
{
    private:
        Window m_window;
        // TextureController and FileReader could be here ?
        std::unique_ptr<Scene> m_current_scene;

    public:
        SceneController(const int mode);
        void SwitchToScene(); 
        void StartGameloop(); 
};