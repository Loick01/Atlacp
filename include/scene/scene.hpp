#pragma once 

#include <memory>
#include <optional>
#include <vector>

#include "battle/battle_controller.hpp"
#include "event/event.hpp"
#include "image/font.hpp"
#include "image/texture.hpp"
#include "map/interaction/interaction.hpp"
#include "map/interaction/trigger.hpp"
#include "map/map_element_controller.hpp"
#include "core/camera.hpp"
#include "core/file/file.hpp"
#include "core/time.hpp"
#include "core/window.hpp"
#include "tile/tilemap.hpp"
#include "tile/tileset.hpp"
#include "ui/component/ui_component_controller.hpp"
#include "ui/element/ui_controller.hpp"

enum class SwitchEvent
{
    ToMainMenu, ToGameMap, ToEditorMap, ToBattle
};

struct GameContext
{
    Window& window;
    Time& time;
    FontController& fontController;
    TextureController& textureController;
    FileReader& fileReader; 
    UiController& uiController;
    UiComponentController& uiComponentController;

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

        virtual void UpdateTilemapLayer(); // Use in constructor + when a new map is loading --> m_tilemap.AddListener(...);
        virtual void HandleTilemapEvent(const TilemapEvent e);

    public:
        TilemapScene(GameContext& context, const bool shouldCulling);
};

class GameMapScene : public TilemapScene
{
    private:
        MapElementController m_elementsController; // Could be in TilemapScene with empty vectors ? (and remove HandleTilemapEvent override)
        OrderController m_orderController;
        InteractionController m_interactionController; // Was in MapElementController, but I moved it here because I need to remove #include "interaction.hpp" from map_element_controller.hpp
        TriggerController m_triggerController;
        
        void HandleEntityEvent(const EntityEvent e);
        
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

        void UpdateTilemapLayer() override; // Add an extra layer for tile borders
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
        Time m_time;
        FontController m_fontController;
        TextureController m_textureController;
        FileReader m_fileReader;
        UiController m_uiController;
        UiComponentController m_uiComponentController;

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