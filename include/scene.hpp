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
#include "window.hpp"

class Scene
{
    protected:
        Window m_window;
        FileReader m_file_reader;
        TextureController m_texture_controller;
        Camera m_camera;
        bool m_gameloop;
        std::vector<Drawable*> m_drawables; // Must be a vector of Drawable* because we need dynamic dispatch

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
        GameplayEventController m_game_event_controller; // Should be in Scene as a EventController ?
        Player m_player;
        std::vector<Entity*> m_rendered_entities; // Must be a vector of Entity* because we need dynamic dispatch
        // For now, m_rendered_entities is sorted by y position. Because a specific order could be necessary for Entity updating 
        // (for example with FollowEntityBehaviour), I use a second vector of Entity*
        std::vector<Entity*> m_updated_entities;
        
    public:
        GameplayTilemapScene();
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