#pragma once

#include "ui/ui_list.hpp"

class UiDynamicList : public UiList
{
    private:
        unsigned int m_nrItem;
    
    public:
        UiDynamicList(UiController& uiController, const std::string& uiFilepath = "undefined_ui_dynamic_list_file");

        void SetNrItem(const unsigned int nrItem);
        // void Open() override;
};