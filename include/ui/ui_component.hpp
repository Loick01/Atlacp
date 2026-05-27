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

        virtual unsigned int GetInstanceCount() = 0;
        virtual void Open();

        ElementKey GetKey() const; 
        void SetFilepath(const std::string& uiFilepath);
        void Close();
};