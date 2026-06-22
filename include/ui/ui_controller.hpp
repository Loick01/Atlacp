#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ui/ui_element.hpp"
#include "ui/ui_types.hpp"

class FileReader;
class FontController;
class TextureController;

class UiController
{       
    private:
        // UiElement are not marked as root, but I don't want it to be possible to delete the root
        // That's why UiController is considered as the root (with its own size and position) from which the branches start
        // UiElement in this vector are considered as the root of distinct branches, and they have global position = local position 
        std::unordered_map<UiKey, UiElement*> m_elements; // Instead of searching elements in the tree structure (from root)
        std::vector<std::unique_ptr<UiElement>> m_subRoots; 
        // WARNING : https://isocpp.org/wiki/faq/dtors#order-dtors-for-locals
        // Class members are deleted in reverse order of declaration
        // When UiElement in m_subRoots are deleted, the Notify(Delete) in ~UiElement calls Remove() which erase the key of the current UiElement from m_elements
        // If m_elements is deleted m_subRoots, the map is empty when deleting m_subRoots, and Remove() call will throw errors because the keys will not
        // be able to be found in m_elements

        const FileReader& m_fileReader;
        FontController& m_fontController;
        TextureController& m_textureController;
        
        const std::string m_fontFilepath; // Will be removed ? (should store a Font object instead of the path ?)
        ScreenPosition m_position; // Initialized with the viewport position
        AreaSize m_size; // Initialized with the viewport size

        std::string GetFileExtension(const std::string& filepath) const; // Will be a static function in FileReader

        float GetPartialElementSizeOnAxis(const UiKey& key, const Axis axis, const float amount) const; // Rename
        float GetPartialRootSizeOnAxis(const Axis axis, const float amount) const; // Rename
        
        // Add/Remove in UiController::m_elements
        void AddElement(const UiKey& key, UiElement* element);
        void RemoveElement(const UiKey& key);

        void HandleUiEvent(const UiElementEvent e, const UiKey& key);
        void UpdatePosition(); // Compute the rendering position for every UiElement in every branch
        void BuildSubRoot(std::unique_ptr<UiElement> subRoot);

    public:
        UiController(const FileReader& fileReader, FontController& fontController, TextureController& textureController, const std::string& fontFilepath); 

        std::unique_ptr<UiElement> CreateElement(const UiKey& key, const std::string& textureFilepath);
        std::unique_ptr<UiTextElement> CreateTextElement(const UiKey& key);
        std::unique_ptr<UiTextElement> CreateTextElement(const UiKey& key, const std::string& fontFilepath); // Remove ?
        std::unique_ptr<UiElement> RemoveSubRoots(const UiKey& key);

        std::unique_ptr<UiElement> GenerateElementFromData(const DataUi& data); // Rename
        std::vector<UiKey> BuildUiFile(const std::string& filepath);

        std::unordered_map<UiKey, UiElement*>::const_iterator GetIteratorOnElement(const UiKey& key) const;
        UiElement* GetElement(const UiKey& key) const;
        float GetResultFromPartialSize(const PartialSize& ps) const;
        
        bool IsBaseUiFile(const std::string& filepath) const;
        bool IsTemplateUiFile(const std::string& filepath) const;
        
        void ClearAll();
        void BuildElement(std::unique_ptr<UiElement>& element, const UiKey& parentKey); // Rename
        void DeleteElement(const UiKey& key);

        void SetSize(const AreaSize size);
        void SetPosition(const ScreenPosition position);

        // Rename these functions, I should not use Update in the name
        // void UpdatePath(const UiKey& key, const std::string& path);
        void UpdatePath(const UiValue<std::string>& path);
        void UpdateText(const UiKey& key, const std::string& text);
        void UpdateText(const UiValue<std::string>& uiv);
        void UpdateText(const UiValue<unsigned int>& uiv);
        void UpdateParent(const UiKey& key, const UiKey& parent);
        void UpdateScalingSize(const UiKey& key, const PartialSize ps); // Should use UiKey+Axis+float, instead of PartialSize (which is from file.hpp)
        // Should add UpdateScalingAxis ?
        void UpdatePadding(const UiKey& key, const PartialSize ps, const Axis axis);
        void UpdateKey(const UiKey& key, const UiKey& newKey); // Used for ui template instanciation (uit file)
        void UpdateParams(const UiKey& key, const UiParams& params);

        void Draw() const;
        void OpenDialogBox(const std::string& text); // Will be removed
};