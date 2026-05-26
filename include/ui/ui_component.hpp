#pragma once

#include "ui/ui_controller.hpp"

class UiComponent
{
    protected:
        UiController& m_uiController;
        ElementKey m_elementKey;
        std::string m_uiFilepath;

    public:
        UiComponent(UiController& uiController, const std::string& uiFilepath);

        ElementKey GetKey() const; 
        void SetFilepath(const std::string& uiFilepath);
        virtual void Open();
        void Close();
};