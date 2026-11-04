#include "ui/component/ui_component_controller.hpp"

#include "core/time.hpp"
#include "ui/component/ui_dialog_box.hpp"
#include "ui/component/ui_dynamic_list.hpp"
#include "ui/component/ui_list.hpp"
#include "ui/component/ui_selector.hpp"
#include "ui/component/ui_sprite_animation.hpp"
#include "ui/component/ui_frame_text.hpp"
#include "ui/element/ui_controller.hpp"

UiComponentController::UiComponentController(Time& time, UiController& uiController) :
    m_time(time), m_uiController(uiController)
{}

bool UiComponentController::IsKeyAvailable(const ComponentKey& key) const
{
    if (m_components.find(key) != m_components.end()) {
        Logger::GetInstance().AddLog(Level::L4, "This key is already used for a component in UiComponentController: " + key);
        return false;
    }
    return true;
}

bool UiComponentController::IsKeyUsed(const ComponentKey& key) const
{
    if (m_components.find(key) == m_components.end()) {
        Logger::GetInstance().AddLog(Level::L4, "This key is not used for a component in UiComponentController: " + key);
        return false;
    }
    return true;
}

UiComponent* UiComponentController::GetComponent(const ComponentKey& key)
{
    if (IsKeyUsed(key))
        return (m_components[key]).get();
    return nullptr; // TODO : ?
}

void UiComponentController::DeleteComponent(const ComponentKey& key)
{
    if (IsKeyUsed(key))
        m_components.erase(m_components.find(key));
}

void UiComponentController::DeleteAll()
{
    m_components.clear();
}

void UiComponentController::OpenComponent(const ComponentKey& key)
{
    if (IsKeyUsed(key))
        m_components[key]->Open();
}

void UiComponentController::CloseComponent(const ComponentKey& key)
{
    if (IsKeyUsed(key))
        m_components[key]->Close();
}

UiDialogBox* UiComponentController::CreateDialogBox(const ComponentKey& key, const std::string& filePath)
{
    std::unique_ptr<UiDialogBox> c = std::make_unique<UiDialogBox>(m_uiController, filePath);
    if (IsKeyAvailable(key)) {
        UiDialogBox* r = c.get();
        m_components[key] = std::move(c);
        return r;
    }
    return nullptr;
}

UiDynamicList* UiComponentController::CreateDynamicList(const ComponentKey& key, const std::string& templatePath)
{
    std::unique_ptr<UiDynamicList> c = std::make_unique<UiDynamicList>(m_uiController, templatePath);
    if (IsKeyAvailable(key)) {
        UiDynamicList* r = c.get();
        m_components[key] = std::move(c);
        return r;
    }
    return nullptr;
}

UiList* UiComponentController::CreateList(const ComponentKey& key, const std::string& filePath)
{
    std::unique_ptr<UiList> c = std::make_unique<UiList>(m_uiController, filePath);
    if (IsKeyAvailable(key)) {
        UiList* r = c.get();
        m_components[key] = std::move(c);
        return r;
    }
    return nullptr;
}

UiSelector* UiComponentController::CreateSelector(const ComponentKey& key, const std::string& templatePath)
{
    std::unique_ptr<UiSelector> c = std::make_unique<UiSelector>(m_uiController, templatePath);
    if (IsKeyAvailable(key)) {
        UiSelector* r = c.get();
        m_components[key] = std::move(c);
        return r;
    }
    return nullptr;
}

UiSpriteAnimation* UiComponentController::CreateSpriteAnimation(const ComponentKey& key, const std::string& templatePath)
{
    std::unique_ptr<UiSpriteAnimation> c = std::make_unique<UiSpriteAnimation>(m_time, m_uiController, templatePath);
    if (IsKeyAvailable(key)) {
        UiSpriteAnimation* r = c.get();
        m_components[key] = std::move(c);
        return r;
    }
    return nullptr;
}

UiFrameText* UiComponentController::CreateFrameText(const ComponentKey& key, const std::string& filePath)
{
    std::unique_ptr<UiFrameText> c = std::make_unique<UiFrameText>(m_uiController, filePath);
    if (IsKeyAvailable(key)) {
        UiFrameText* r = c.get();
        m_components[key] = std::move(c);
        return r;
    }
    return nullptr;
}