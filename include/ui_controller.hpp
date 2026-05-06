#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "ui.hpp"

using ElementKey = std::string; // Will be int ?

class UiController
{       
    private:
        // UiElement are not marked as root, but I don't want it to be possible to delete the root
        // That's why UiController is considered as the root (with its own size and position) from which the branches start
        // UiElement in this vector are considered as the root of distinct branches, and they have global position = local position 
        std::vector<std::unique_ptr<UiElement>> m_branches; 
        std::unordered_map<ElementKey, UiElement*> m_elements; // Instead of searching elements in the tree structure (from root)

        const ScreenPosition m_position; // Will be initialized with the viewport position
        const AreaSize m_size; // Will be initialized with the viewport size

    protected:
        UiElement* GetElement(const ElementKey& key) const;
        float GetPartialElementSizeOnAxis(const ElementKey& key, const Axis axis, const float amount) const; // Rename
        float GetPartialRootSizeOnAxis(const Axis axis, const float amount) const; // Rename
        void AddElement(const ElementKey& key, UiElement* element);
        void UpdatePosition(); // Compute the rendering position for every UiElement in every branch --> This function must be called in every UiController constructors 
        void BuildSubRoot(std::unique_ptr<UiElement> subRoot);

    public:
        UiController(const AreaSize size, const ScreenPosition position);

        virtual void Update() = 0;
        void Draw() const;
        void UpdateText(const ElementKey& key, const std::string& newText); // Should be in TextArea ?
};

// UI configuration will not stay in constructor
class GameplayUiController : public UiController // Not a EventStateHolder<GameplayEventHolder> ? (Don't need it for now)
{
    public:
        GameplayUiController(TextureController& textureController, const std::string& fontFilepath,
            const AreaSize viewportSize, const ScreenPosition viewportPosition);
        void Update() override;
};

class EditorUiController : public UiController, public EventStateHolder<EditorEventState>
{
    private:
        int m_lastLayer; // Should create EditorEventState struct, and have a parameter in UiController::Draw or EditorUiController::UpdateState ?

    public:
        EditorUiController(TextureController& textureController, const std::string& fontFilepath, 
            const AreaSize viewportSize, const ScreenPosition viewportPosition);
        void Update() override;
};

class BattleUiController : public UiController, public EventStateHolder<BattleEventState>
{
    public:
        BattleUiController(TextureController& textureController, const std::string& fontFilepath, 
            const AreaSize viewportSize, const ScreenPosition viewportPosition);
        void Update() override;
};