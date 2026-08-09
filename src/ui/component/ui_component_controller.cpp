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

UiComponent* UiComponentController::GetComponent(const ComponentKey& componentKey)
{
    if (m_components.find(componentKey) == m_components.end())
        throw std::runtime_error("This key is not used for a component in UiComponentController : " + componentKey);
    return (m_components[componentKey]).get();
}

void UiComponentController::CheckKeyAvailable(const ComponentKey& componentKey) const
{
    if (m_components.find(componentKey) != m_components.end())
        throw std::runtime_error("This key is already used for a component in UiComponentController : " + componentKey);
}

void UiComponentController::DeleteComponent(const ComponentKey& componentKey)
{
    std::unordered_map<ComponentKey, std::unique_ptr<UiComponent>>::iterator it = m_components.find(componentKey);
    if (it == m_components.end())
        throw std::runtime_error("This key is not used for a component in UiComponentController : " + componentKey); // ?
    m_components.erase(it);
}

void UiComponentController::OpenComponent(const ComponentKey& componentKey)
{
    if (m_components.find(componentKey) == m_components.end())
        throw std::runtime_error("This key is not used for a component in UiComponentController : " + componentKey);
    m_components[componentKey]->Open();
}

void UiComponentController::CloseComponent(const ComponentKey& componentKey)
{
    if (m_components.find(componentKey) == m_components.end())
        throw std::runtime_error("This key is not used for a component in UiComponentController : " + componentKey);
    m_components[componentKey]->Close();
}

void UiComponentController::CreateDynamicList(const ComponentKey& componentKey, const std::string& templatePath)
{
    std::unique_ptr<UiDynamicList> c = std::make_unique<UiDynamicList>(m_uiController, templatePath);
    CheckKeyAvailable(componentKey);
    m_components[componentKey] = std::move(c);
}

void UiComponentController::CreateList(const ComponentKey& componentKey, const std::string& filePath)
{
    std::unique_ptr<UiList> c = std::make_unique<UiList>(m_uiController, filePath);
    CheckKeyAvailable(componentKey);
    m_components[componentKey] = std::move(c);
}

void UiComponentController::CreateSelector(const ComponentKey& componentKey, const std::string& templatePath)
{
    std::unique_ptr<UiSelector> c = std::make_unique<UiSelector>(m_uiController, templatePath);
    CheckKeyAvailable(componentKey);
    m_components[componentKey] = std::move(c);
}

void UiComponentController::CreateSpriteAnimation(const ComponentKey& componentKey, const std::string& templatePath)
{
    std::unique_ptr<UiSpriteAnimation> c = std::make_unique<UiSpriteAnimation>(m_time, m_uiController, templatePath);
    CheckKeyAvailable(componentKey);
    m_components[componentKey] = std::move(c);
}

void UiComponentController::CreateTextSeries(const ComponentKey& componentKey, const std::string& filePath)
{
    std::unique_ptr<UiTextSeries> c = std::make_unique<UiTextSeries>(m_uiController, filePath);
    CheckKeyAvailable(componentKey);
    m_components[componentKey] = std::move(c);
}