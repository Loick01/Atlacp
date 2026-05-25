#pragma once

#include "ui/ui_controller.hpp"

class UiComponent
{
    private:
        std::string m_uiFilepath;

    public:
        UiController& m_uiController;
        ElementKey m_elementKey;
        
        UiComponent(UiController& uiController, const std::string& uiFilepath);

        ElementKey GetKey() const; 
        void SetFilepath(const std::string& uiFilepath);
        void Open();
        void Close();
};