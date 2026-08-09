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

void UiComponentController::CreateDynamicList(const ComponentKey& componentKey, const std::string& templatePath)
{
    std::unique_ptr<UiDynamicList> c = std::make_unique<UiDynamicList>(m_uiController, templatePath);
    m_components[componentKey] = std::move(c);
}

void UiComponentController::CreateList(const ComponentKey& componentKey, const std::string& filePath)
{
    std::unique_ptr<UiList> c = std::make_unique<UiList>(m_uiController, filePath);
    m_components[componentKey] = std::move(c);
}

void UiComponentController::CreateSelector(const ComponentKey& componentKey, const std::string& templatePath)
{
    std::unique_ptr<UiSelector> c = std::make_unique<UiSelector>(m_uiController, templatePath);
    m_components[componentKey] = std::move(c);
}

void UiComponentController::CreateSpriteAnimation(const ComponentKey& componentKey, const std::string& templatePath)
{
    std::unique_ptr<UiSpriteAnimation> c = std::make_unique<UiSpriteAnimation>(m_time, m_uiController, templatePath);
    m_components[componentKey] = std::move(c);
}

void UiComponentController::CreateTextSeries(const ComponentKey& componentKey, const std::string& filePath)
{
    std::unique_ptr<UiTextSeries> c = std::make_unique<UiTextSeries>(m_uiController, filePath);
    m_components[componentKey] = std::move(c);
}
