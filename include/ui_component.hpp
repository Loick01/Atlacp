#pragma once

#include "ui_controller.hpp"

class UiComponent
{
    private:
        const std::string m_uiFilepath; // const ?

    public:
        UiController& m_uiController;
        ElementKey m_elementKey;
        
        UiComponent(UiController& uiController, const std::string& uiFilepath);

        void Open();
        void Close();
};