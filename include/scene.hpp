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
        // Will use a drawable/layer controller 
        std::vector<Drawable*> m_drawables;


    public:
        Scene();
        virtual void Gameloop() = 0;
        bool GetGameloop() const;
};

class TilemapScene : public Scene
{
    protected:
        Tileset m_tileset;
        Tilemap m_tilemap;

    public:
        TilemapScene();
};

class GameplayTilemapScene : public TilemapScene
{
    private:
        Time m_time;
        GameplayEventController m_game_event_controller; // Will be in Scene as a EventController 
        GameplayUiController m_game_ui_controller; // Will be in Scene as a UiController
        Player m_player;
        std::vector<Entity*> m_rendered_entities; // Need dynamic dispatch
        // For now, m_rendered_entities is sorted by y position. Because a specific order could be necessary for Entity updating 
        // (for example with FollowEntityBehaviour), I use a second vector of Entity*
        std::vector<Entity*> m_updated_entities;
        
    public:
        GameplayTilemapScene();
        ~GameplayTilemapScene();
        void Gameloop() override;
};

class EditorTilemapScene : public TilemapScene
{
    private:
        EditorEventController m_editor_event_controller; // Should be in Scene as a EventController ?

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