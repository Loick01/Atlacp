#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "ui/component/ui_component.hpp"

using ComponentKey = std::string;

class Time;
class UiController;

class UiComponentController // Rename
{
    private:
        std::unordered_map<ComponentKey, std::unique_ptr<UiComponent>> m_components;
        Time& m_time;
        UiController& m_uiController;
    
    public:
        UiComponentController(Time& time, UiController& uiController);

        // Each UiComponent must have its function here, add it in m_components
        void CreateDynamicList(const ComponentKey& componentKey, const std::string& templatePath);
        void CreateList(const ComponentKey& componentKey, const std::string& filePath);
        void CreateSelector(const ComponentKey& componentKey, const std::string& templatePath);
        void CreateSpriteAnimation(const ComponentKey& componentKey, const std::string& templatePath);
        void CreateTextSeries(const ComponentKey& componentKey, const std::string& filePath);
};