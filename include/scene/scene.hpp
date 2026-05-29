#pragma once 

#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

#include "battle/battle_controller.hpp"
#include "event/event.hpp"
#include "image/texture.hpp"
#include "map/entity_controller.hpp"
#include "sound/sound.hpp"
#include "system/camera.hpp"
#include "system/file.hpp"
#include "system/time.hpp"
#include "system/window.hpp"
#include "tile/tilemap.hpp"
#include "ui/ui_controller.hpp"

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

class GameplayTilemapScene : public TilemapScene
{
    private:
        Time m_time;
        EntityController m_entities; // Could be in TilemapScene with empty vectors ? (and remove HandleTilemapEvent override)

        // In GameplayTilemapScene, TileLayer are rendered in two part : low_layer then high_layer. So I can draw entities between layers
        // It might be better to have 2 dinstinct vectors of TileLayer in TilemapScene ?
        const size_t m_layersSplitIndex; // Should not be const ?
        
    public:
        GameplayTilemapScene(GameContext& context);
        ~GameplayTilemapScene() = default;
        void Gameloop() override;
        void HandleTilemapEvent(const TilemapEvent e) override;
};

class EditorTilemapScene : public TilemapScene
{
    private:
        int m_lastLayer; // UiValue ?
        
    public:
        EditorTilemapScene(GameContext& context);
        void Gameloop() override;
};

class BattleScene : public Scene
{
    private:
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
        SoundController m_soundController;
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