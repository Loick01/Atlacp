#pragma once

#include "ui/ui_component.hpp"
#include "ui/ui_controller.hpp"
#include "ui/ui_element.hpp"

class UiList : public UiComponent
{
    private:
        std::vector<ElementKey> m_elements; // Rename
    
    public:
        UiList(UiController& uiController, const std::string& uiFilepath = "undefined_ui_list_file");

        std::vector<ElementKey> GetElementsKey() const; // Rename
};