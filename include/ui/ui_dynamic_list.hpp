#pragma once

#include "ui/ui_list.hpp"

class UiDynamicList : public UiList
{
    private:
        static unsigned int m_instanceCount;
        unsigned int m_nrItem;
    
    public:
        UiDynamicList(UiController& uiController, const std::string& uiFilepath = "undefined_ui_dynamic_list_file");
        
        unsigned int GetInstanceCount() override;
        // void Open() override;
        
        void SetNrItem(const unsigned int nrItem);
};