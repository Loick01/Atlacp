#pragma once

#include "ui/ui_component.hpp"
#include "ui/ui_controller.hpp"
#include "ui/ui_element.hpp"

class UiList : public UiComponent
{
    private:
        static unsigned int m_instanceCount;
        std::vector<ElementKey> m_itemsKey;
    
    public:
        UiList(UiController& uiController, const std::string& uiFilepath = "undefined_ui_list_file");

        unsigned int GetInstanceCount() override;
        void Open() override;
        
        std::vector<ElementKey> GetItemsKey() const;
};