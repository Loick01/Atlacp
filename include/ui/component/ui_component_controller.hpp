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

        void CheckKeyAvailable(const ComponentKey& key) const;
        void CheckKeyUsed(const ComponentKey& key) const;
    
    public:
        UiComponentController(Time& time, UiController& uiController);

        UiComponent* GetComponent(const ComponentKey& key);
        void DeleteComponent(const ComponentKey& key);

        // Two way to Open/Close a UiComponent :
        //     Use UiComponentController::OpenComponent(key)/UiComponentController::CloseComponent(key)
        //     Get the UiComponent (with GetComponent(key) and then use UiComponent::Open()/UiComponent::Close())
        void OpenComponent(const ComponentKey& key);
        void CloseComponent(const ComponentKey& key);
        
        // Each UiComponent must have its function here, add it in m_components
        void CreateDynamicList(const ComponentKey& key, const std::string& templatePath);
        void CreateList(const ComponentKey& key, const std::string& filePath);
        void CreateSelector(const ComponentKey& key, const std::string& templatePath);
        void CreateSpriteAnimation(const ComponentKey& key, const std::string& templatePath);
        void CreateTextSeries(const ComponentKey& key, const std::string& filePath);
};