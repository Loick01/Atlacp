#include "ui/component/ui_component_controller.hpp"

#include "core/time.hpp"
#include "ui/component/ui_dynamic_list.hpp"
#include "ui/component/ui_list.hpp"
#include "ui/component/ui_selector.hpp"
#include "ui/component/ui_sprite_animation.hpp"
#include "ui/component/ui_text_series.hpp"
#include "ui/element/ui_controller.hpp"

UiComponentController::UiComponentController(Time& time, UiController& uiController) :
    m_time(time), m_uiController(uiController)
{}

void UiComponentController::CheckKeyAvailable(const ComponentKey& key) const
{
    if (m_components.find(key) != m_components.end())
        throw std::runtime_error("This key is already used for a component in UiComponentController : " + key);
}

void UiComponentController::CheckKeyUsed(const ComponentKey& key) const
{
    if (m_components.find(key) == m_components.end())
        throw std::runtime_error("This key is not used for a component in UiComponentController : " + key);
}

UiComponent* UiComponentController::GetComponent(const ComponentKey& key)
{
    CheckKeyUsed(key);
    return (m_components[key]).get();
}

void UiComponentController::DeleteComponent(const ComponentKey& key)
{
    CheckKeyUsed(key); // ?
    m_components.erase(m_components.find(key));
}

void UiComponentController::OpenComponent(const ComponentKey& key)
{
    CheckKeyUsed(key);
    m_components[key]->Open();
}

void UiComponentController::CloseComponent(const ComponentKey& key)
{
    CheckKeyUsed(key);
    m_components[key]->Close();
}

void UiComponentController::CreateDynamicList(const ComponentKey& key, const std::string& templatePath)
{
    std::unique_ptr<UiDynamicList> c = std::make_unique<UiDynamicList>(m_uiController, templatePath);
    CheckKeyAvailable(key);
    m_components[key] = std::move(c);
}

void UiComponentController::CreateList(const ComponentKey& key, const std::string& filePath)
{
    std::unique_ptr<UiList> c = std::make_unique<UiList>(m_uiController, filePath);
    CheckKeyAvailable(key);
    m_components[key] = std::move(c);
}

void UiComponentController::CreateSelector(const ComponentKey& key, const std::string& templatePath)
{
    std::unique_ptr<UiSelector> c = std::make_unique<UiSelector>(m_uiController, templatePath);
    CheckKeyAvailable(key);
    m_components[key] = std::move(c);
}

void UiComponentController::CreateSpriteAnimation(const ComponentKey& key, const std::string& templatePath)
{
    std::unique_ptr<UiSpriteAnimation> c = std::make_unique<UiSpriteAnimation>(m_time, m_uiController, templatePath);
    CheckKeyAvailable(key);
    m_components[key] = std::move(c);
}

void UiComponentController::CreateTextSeries(const ComponentKey& key, const std::string& filePath)
{
    std::unique_ptr<UiTextSeries> c = std::make_unique<UiTextSeries>(m_uiController, filePath);
    CheckKeyAvailable(key);
    m_components[key] = std::move(c);
}