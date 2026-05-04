#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "ui.hpp"

using ElementKey = std::string; // Will be int ?

class UiController
{
    protected:
        std::unique_ptr<UiElement> m_root; 

        std::unordered_map<ElementKey, UiElement*> m_elements; // Instead of searching elements in the tree structure (from root)
        
        UiElement* GetElement(ElementKey key);

        void BuildRoot(std::unique_ptr<UiElement> ui_root, const AreaSize parentSize, // Because the root has no parent, we must specified its initial size
            const float zoomScale, const Axis zoomAxis, // ComputeZoom
            const Anchor xAnchor, const Anchor yAnchor, // ComputePosition
            const float paddingScale, const Axis sourceAxis, const Axis paddingAxis); // AddPadding

    public:
        virtual void Update() = 0;
        void Draw() const;
        // void OpenDialogBox(); // Will be removed
        void UpdateText(const ElementKey& key, const std::string& newText);
};

// UI configuration will not stay in constructor
class GameplayUiController : public UiController // Not a EventStateHolder<GameplayEventHolder> ? (Don't need it for now)
{
    public:
        GameplayUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath);
        void Update() override;
};

class EditorUiController : public UiController, public EventStateHolder<EditorEventState>
{
    private:
        int m_lastLayer; // Should create EditorEventState struct, and have a parameter in UiController::Draw or EditorUiController::UpdateState ?

    public:
        EditorUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath);
        void Update() override;
};

class BattleUiController : public UiController, public EventStateHolder<BattleEventState>
{
    public:
        BattleUiController(TextureController& textureController, const Camera& camera, const std::string& fontFilepath);
        void Update() override;
};