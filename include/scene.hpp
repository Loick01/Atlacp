#pragma once 

#include <memory>
#include <stdexcept>
#include <vector>

#include "camera.hpp"
#include "entity_controller.hpp"
#include "event.hpp"
#include "file.hpp"
#include "sound.hpp"
#include "texture.hpp"
#include "tilemap.hpp"
#include "time.hpp"
#include "ui.hpp"
#include "window.hpp"

enum class SwitchEvent
{
    ToGameplay, ToEditor, ToBattle
};

struct GameContext
{
    Window& window;
    TextureController& textureController;
    SoundController& soundController;
    FileReader& fileReader; 

    std::unique_ptr<EventController> eventController;
    std::unique_ptr<UiController> uiController;
};

class Scene : public Notifier<SwitchEvent>
{
    protected:
        GameContext& m_context;
        Camera m_camera;
        bool m_gameloop;

        std::vector<const Drawable*> m_drawables; // Will be removed ?

    public:
        Scene(GameContext& context);
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
        TilemapScene(GameContext& context, const bool shouldCulling);
};

class GameplayTilemapScene : public TilemapScene
{
    private:
        Time m_time;
        EntityController m_entities;

        // In GameplayTilemapScene, TileLayer are rendered in two part : low_layer then high_layer. So I can draw entities between layers
        // It might be better to have 2 dinstinct vectors of TileLayer in TilemapScene ?
        const size_t m_layersSplitIndex; // Should not be const ?
        
    public:
        GameplayTilemapScene(GameContext& context);
        ~GameplayTilemapScene() = default;
        void Gameloop() override;
};

class EditorTilemapScene : public TilemapScene
{
    public:
        EditorTilemapScene(GameContext& context);
        void Gameloop() override;
};

class BattleScene : public Scene
{
    public:
        BattleScene(GameContext& context);
        void Gameloop() override;
};

class SceneController
{
    private:
        Window m_window;
        SoundController m_soundController;
        TextureController m_textureController;
        FileReader m_fileReader;

        GameContext m_context;
        std::unique_ptr<Scene> m_currentScene;

        void SetCurrentScene(const SwitchEvent e);
        SwitchEvent GetSwitchEventFromMode(const int mode) const;

    public:
        SceneController(const int mode);
        void StartGameloop(); 
};