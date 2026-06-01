#pragma once

#include <string>

#include "ui/ui_types.hpp"

class UiController;

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
        virtual void Close();

        ElementKey GetKey() const; 
};