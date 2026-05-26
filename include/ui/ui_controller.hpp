#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "image/texture.hpp"
#include "system/file.hpp"
#include "ui/ui_element.hpp"

class UiController
{       
    private:
        // UiElement are not marked as root, but I don't want it to be possible to delete the root
        // That's why UiController is considered as the root (with its own size and position) from which the branches start
        // UiElement in this vector are considered as the root of distinct branches, and they have global position = local position 
        std::unordered_map<ElementKey, UiElement*> m_elements; // Instead of searching elements in the tree structure (from root)
        std::vector<std::unique_ptr<UiElement>> m_subRoots; 
        // WARNING : https://isocpp.org/wiki/faq/dtors#order-dtors-for-locals
        // Class members are deleted in reverse order of declaration
        // When UiElement in m_subRoots are deleted, the Notify(Delete) in ~UiElement calls Remove() which erase the key of the current UiElement from m_elements
        // If m_elements is deleted m_subRoots, the map is empty when deleting m_subRoots, and Remove() call will throw errors because the keys will not
        // be able to be found in m_elements

        TextureController& m_textureController;

        ScreenPosition m_position; // Initialized with the viewport position
        AreaSize m_size; // Initialized with the viewport size

        void AddElement(const ElementKey& key, UiElement* element); // Add in UiController::m_elements
        void RemoveElement(const ElementKey& key); // Remove from UiController::m_elements

    protected:
        // Should be private ?
        const std::string m_fontFilepath; // Will be removed ? (should store a Font object instead of the path ?) 

        const FileReader& m_fileReader; // Should be private ?

        void HandleUiEvent(const UiElementEvent e, const ElementKey& key);
        void UpdatePosition(); // Compute the rendering position for every UiElement in every branch
        void BuildSubRoot(std::unique_ptr<UiElement> subRoot);

    public:
        UiController(const FileReader& fileReader, TextureController& textureController, const std::string& fontFilepath); 

        void Draw() const;

        std::unordered_map<ElementKey, UiElement*>::const_iterator GetIteratorOnElement(const ElementKey& key) const;
        UiElement* GetElement(const ElementKey& key) const;
        
        float GetPartialElementSizeOnAxis(const ElementKey& key, const Axis axis, const float amount) const; // Rename
        float GetPartialRootSizeOnAxis(const Axis axis, const float amount) const; // Rename
        float GetResultFromPartialSize(const PartialSize& ps) const;
        
        std::unique_ptr<UiElement> CreateElement(const ElementKey& key, const std::string& textureFilepath);
        std::unique_ptr<TextArea> CreateTextElement(const ElementKey& key);
        std::unique_ptr<TextArea> CreateTextElement(const ElementKey& key, const std::string& fontFilepath); // Remove ?
        std::unique_ptr<UiElement> RemoveSubRoots(const ElementKey& key);
        void Clear();
        void DeleteElement(const ElementKey& key);

        void SetSize(const AreaSize size);
        void SetPosition(const ScreenPosition position);

        // Rename these functions, I should not use Update in the name
        void UpdateText(const ElementKey& key, const std::string& text);
        void UpdateText(const UiValue<std::string>& uiv);
        void UpdateText(const UiValue<unsigned int>& uiv);
        void UpdateParent(const ElementKey& key, const ElementKey& parent);
        void UpdateScalingSize(const ElementKey& key, const PartialSize ps); // Should use ElementKey+Axis+float, instead of PartialSize (which is from file.hpp)
        // Should add UpdateScalingAxis ?
        
        void BuildElement(std::unique_ptr<UiElement>& element, const ElementKey& parentKey); // Rename
        std::unique_ptr<UiElement> GenerateElementFromData(const DataUi& data); // Rename
        ElementKey BuildUiFile(const std::string& filepath);
        void OpenDialogBox(const std::string& text);
};