#pragma once

#include "ui/ui_list.hpp"

class UiDynamicList : public UiList
{
    private:
        static unsigned int m_instanceCount;
        
        UiParams m_firstItemParams; // This UiParams will be used only for the first built item, following item will use values given in the template file
        unsigned int m_nrItem;
    
    public:
        UiDynamicList(UiController& uiController, const std::string& uiFilepath = "undefined_ui_dynamic_list_file.uit");
        
        unsigned int GetInstanceCount() override;
        void Open() override;
        void Close() override;
        
        void SetFirstItemParams(const UiParams& params);
        void SetNrItem(const unsigned int nrItem);
};