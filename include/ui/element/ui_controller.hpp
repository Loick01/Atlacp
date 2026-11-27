#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ui/element/ui_animated_element.hpp"
#include "ui/element/ui_element.hpp"
#include "ui/element/ui_text_element.hpp"
#include "ui/ui_types.hpp"

struct Time;
class FileReader;
class TextureController;

class UiController // TODO : Should be renamed UiElementController ?
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
        // If m_elements is deleted m_subRoots, the map is empty when deleting m_subRoots, and Remove() call will generate errors because the keys will not
        // be able to be found in m_elements

        const Time& m_time;
        const FileReader& m_fileReader;
        TextureController& m_textureController;
        
        const std::string m_fontFilepath; // TODO : Will be removed ? (should store a Font object instead of the path ?)
        ScreenPosition m_position; // Initialized with the viewport position
        AreaSize m_size; // Initialized with the viewport size

        float GetPartialElementSizeOnAxis(const UiKey& key, const Axis axis, const float amount) const;
        float GetPartialViewportSizeOnAxis(const Axis axis, const float amount) const;
        
        // Add/Remove in UiController::m_elements
        void AddElement(const UiKey& key, UiElement* element);
        void RemoveElement(const UiKey& key);

        void HandleUiEvent(const UiElementEvent e, const UiKey& key);
        void UpdatePosition(); // Compute the rendering position for every UiElement in every branch
        void BuildSubRoot(std::unique_ptr<UiElement> subRoot);

    public:
        UiController(const Time& time, const FileReader& fileReader, TextureController& textureController, const std::string& fontFilepath); 

        std::unique_ptr<UiElement> CreateElement(const UiKey& key, const std::string& textureFilepath);
        std::unique_ptr<UiTextElement> CreateTextElement(const UiKey& key);
        std::unique_ptr<UiAnimatedElement> CreateAnimatedElement(const UiKey& key, const std::string& animationPath);
        std::unique_ptr<UiElement> RemoveSubRoots(const UiKey& key);

        std::unique_ptr<UiElement> GenerateElementFromData(const DataUiElement& data); // TODO : Rename
        std::vector<UiKey> BuildUiFile(const std::string& filepath);

        std::unordered_map<UiKey, UiElement*>::const_iterator GetIteratorOnElement(const UiKey& key) const;
        UiElement* GetElement(const UiKey& key) const;
        float GetResultFromPartialSize(const PartialSize& ps) const;
        
        bool IsBaseUiFile(const std::string& filepath) const;
        bool IsTemplateUiFile(const std::string& filepath) const;
        
        void ClearAll();
        void BuildElement(std::unique_ptr<UiElement>& element, const UiKey& parentKey); // TODO : Rename
        void HideElement(const UiKey& key);
        void DeleteElement(const UiKey& key);

        void SetSize(const AreaSize size);
        void SetPosition(const ScreenPosition position);

        void EditPath(const UiKey& key, const std::string& path);
        void EditPath(const UiValue<std::string>& path);
        void EditText(const UiKey& key, const std::string& text);
        void EditText(const UiValue<std::string>& uiv);
        void EditText(const UiValue<unsigned int>& uiv);
        void EditParent(const UiKey& key, const UiKey& parent);
        void EditScalingSize(const UiKey& key, const PartialSize ps);
        // TODO : Should add UpdateScalingAxis ? (I don't need it for now)
        void EditPadding(const UiKey& key, const PartialSize ps, const Axis axis);
        void EditKey(const UiKey& key, const UiKey& newKey); // Used for ui template instanciation (uit file)
        void EditParams(const UiKey& key, const UiParams& params);

        void Draw() const;
};