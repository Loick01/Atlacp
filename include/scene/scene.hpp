#pragma once 

#include <memory>
#include <optional>
#include <vector>

#include "battle/battle_controller.hpp"
#include "event/event.hpp"
#include "image/font.hpp"
#include "image/texture.hpp"
#include "map/entity_controller.hpp"
#include "core/camera.hpp"
#include "core/file.hpp"
#include "core/time.hpp"
#include "core/window.hpp"
#include "tile/tilemap.hpp"
#include "tile/tileset.hpp"
#include "ui/ui_controller.hpp"

enum class SwitchEvent
{
    ToMainMenu, ToGameMap, ToEditorMap, ToBattle
};

struct GameContext
{
    Window& window;
    FontController& fontController;
    TextureController& textureController;
    FileReader& fileReader; 
    UiController& uiController;

    std::unique_ptr<EventController> eventController;
};

class Scene : public Notifier<SwitchEvent>
{
    protected:
        GameContext& m_context;
        Camera m_camera; // Could be in GameContext ?
        bool m_gameloop;

        std::vector<const Drawable*> m_drawables; // Will be removed ?

    public:
        Scene(GameContext& context);
        virtual ~Scene() = default;
        
        virtual void Gameloop() = 0;
        bool GetGameloop() const;
};

class MainMenuScene : public Scene
{
    private:
        UiList m_staticList;
        UiSelector m_selector;

    public:
        MainMenuScene(GameContext& context);
        void Gameloop() override;
};

class TilemapScene : public Scene
{
    protected:
        std::vector<const TileLayer*> m_layers;
        Tileset m_tileset;
        Tilemap m_tilemap;

        void UpdateTilemapLayer(); // Use in constructor + when a new map is loading --> m_tilemap.AddListener(...);
        virtual void HandleTilemapEvent(const TilemapEvent e);

    public:
        TilemapScene(GameContext& context, const bool shouldCulling);
};

class GameMapScene : public TilemapScene
{
    private:
        Time m_time;
        EntityController m_entities; // Could be in TilemapScene with empty vectors ? (and remove HandleTilemapEvent override)

        // In GameMapScene, TileLayer are rendered in two part : low_layer then high_layer. So I can draw entities between layers
        // It might be better to have 2 dinstinct vectors of TileLayer in TilemapScene ?
        const size_t m_layersSplitIndex; // Should not be const ?
        
    public:
        GameMapScene(GameContext& context);
        ~GameMapScene() = default;
        void Gameloop() override;
        void HandleTilemapEvent(const TilemapEvent e) override;
};

class EditorMapScene : public TilemapScene
{
    private:
        int m_lastLayer; // UiValue ?
        
    public:
        EditorMapScene(GameContext& context);
        void Gameloop() override;
};

class BattleScene : public Scene
{
    private:
        Time m_time;
        BattleController m_battleController;

    public:
        BattleScene(GameContext& context);
        void Gameloop() override;
        void Exit(const ExitEvent e);
};

class SceneController
{
    private:
        Window m_window;
        FontController m_fontController;
        TextureController m_textureController;
        FileReader m_fileReader;
        UiController m_uiController;

        GameContext m_context;
        std::unique_ptr<Scene> m_currentScene;
        std::optional<SwitchEvent> m_pendingSwitch; // std::optional allows to avoid having a default value in SwitchEvent enum

        SwitchEvent GetSwitchEventFromMode(const int mode) const;
        void SetCurrentScene(const SwitchEvent e);
        void RequestSwitchScene(const SwitchEvent e);
        void ApplySwitchScene();

    public:
        SceneController(const int mode);
        void StartGameloop(); 
};