#include "ui/component/ui_component_controller.hpp"

#include "ui/component/ui_dialog_box.hpp"
#include "ui/component/ui_dynamic_list.hpp"
#include "ui/component/ui_list.hpp"
#include "ui/component/ui_selector.hpp"
#include "ui/component/ui_sprite_animation.hpp"
#include "ui/component/ui_frame_text.hpp"
#include "ui/element/ui_controller.hpp"

UiComponentController::UiComponentController(UiController& uiController) :
    m_uiController(uiController)
{}

bool UiComponentController::IsKeyAvailable(const ComponentKey& key) const
{
    if (m_components.find(key) != m_components.end()) {
        Logger::GetInstance().AddLog(Level::L4, "This key is already used for a component in UiComponentController: " + key); // TODO : Level
        return false;
    }
    return true;
}

bool UiComponentController::IsKeyUsed(const ComponentKey& key) const
{
    if (m_components.find(key) == m_components.end()) {
        Logger::GetInstance().AddLog(Level::L4, "This key is not used for a component in UiComponentController: " + key); // TODO : Level
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
    if (!IsKeyAvailable(key))
        return nullptr;

    std::unique_ptr<UiDialogBox> c = std::make_unique<UiDialogBox>(m_uiController, filePath);
    UiDialogBox* r = c.get();
    m_components[key] = std::move(c);
    return r;
}

UiDynamicList* UiComponentController::CreateDynamicList(const ComponentKey& key, const std::string& templatePath)
{
    if (!IsKeyAvailable(key))
        return nullptr;

    std::unique_ptr<UiDynamicList> c = std::make_unique<UiDynamicList>(m_uiController, templatePath);
    UiDynamicList* r = c.get();
    m_components[key] = std::move(c);
    return r;
}

UiList* UiComponentController::CreateList(const ComponentKey& key, const std::string& filePath)
{
    if (!IsKeyAvailable(key))
        return nullptr;

    std::unique_ptr<UiList> c = std::make_unique<UiList>(m_uiController, filePath);
    UiList* r = c.get();
    m_components[key] = std::move(c);
    return r;
}

UiSelector* UiComponentController::CreateSelector(const ComponentKey& key, const std::string& templatePath)
{
    if (!IsKeyAvailable(key))
        return nullptr;

    std::unique_ptr<UiSelector> c = std::make_unique<UiSelector>(m_uiController, templatePath);
    UiSelector* r = c.get();
    m_components[key] = std::move(c);
    return r;
}

UiSpriteAnimation* UiComponentController::CreateSpriteAnimation(const ComponentKey& key, const std::string& templatePath)
{
    if (!IsKeyAvailable(key))
        return nullptr;

    std::unique_ptr<UiSpriteAnimation> c = std::make_unique<UiSpriteAnimation>(m_uiController, templatePath);
    UiSpriteAnimation* r = c.get();
    m_components[key] = std::move(c);
    return r;
}

UiFrameText* UiComponentController::CreateFrameText(const ComponentKey& key, const std::string& filePath)
{
    if (!IsKeyAvailable(key))
        return nullptr;
    
    std::unique_ptr<UiFrameText> c = std::make_unique<UiFrameText>(m_uiController, filePath);
    UiFrameText* r = c.get();
    m_components[key] = std::move(c);
    return r;
}