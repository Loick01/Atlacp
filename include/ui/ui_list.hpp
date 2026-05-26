#pragma once

#include "ui/ui_component.hpp"
#include "ui/ui_controller.hpp"
#include "ui/ui_element.hpp"

class UiList : public UiComponent
{
    private:
        std::vector<ElementKey> m_itemsKey;
    
    public:
        UiList(UiController& uiController, const std::string& uiFilepath = "undefined_ui_list_file");

        std::vector<ElementKey> GetItemsKey() const;
        void Open() override;
};