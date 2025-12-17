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
    private:
        //Window m_window; 
        //Camera m_camera; 
        //TextureController m_texture_controller;
        //FileReader m_file_reader;
        //std::vector<Drawable*> m_drawables; // Must be a vector of Drawable* because we need dynamic dispatch
        //bool m_gameloop

    protected:
        Window m_window; // Should be private
        FileReader m_file_reader; // Should be private
        TextureController m_texture_controller; // Should be private
        Camera m_camera; // Should be private
        bool m_gameloop; // Should be private
        std::vector<Drawable*> m_drawables; // Should be private

    public:
        Scene();
        virtual void Gameloop() = 0;
        bool GetGameloop() const;
};

class TilemapScene : public Scene
{
    private:
        //Tileset m_tileset;
        //Tilemap m_tilemap;
    
    protected:
        Tileset m_tileset; // Should be private
        Tilemap m_tilemap; // Should be private

    public:
        TilemapScene();
};

class GameplayTilemapScene : public TilemapScene
{
    private:
        Time m_time;
        GameplayEventController m_game_event_controller; // Should be in Scene as a EventController ?
        Player m_player;
        std::vector<Entity*> m_entities; // Must be a vector of Drawable* because we need dynamic dispatch
        
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