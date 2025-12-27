#include "ui.hpp"

UiElement::UiElement(TextureController& texture_controller, const std::string& texture_filepath):
    ScreenDrawable(texture_controller, texture_filepath)
{

}

UiElement::UiElement(TextureController& texture_controller):
    ScreenDrawable(texture_controller)
{

}

TextArea::TextArea(TextureController& texture_controller, const std::string& font_filepath, const SDL_Color color):
    UiElement(texture_controller), m_text_color(color)
{
    const unsigned int text_size = 48;
    m_texture_key = font_filepath; // Use hash function to get a key from the filepath (unless TextureKey is already std::string)
    texture_controller.LoadFontFromFile(font_filepath, m_texture_key, text_size);
}

TextArea::~TextArea()
{
    m_texture_controller.DeleteFont(m_texture_key);
}

void TextArea::SetText(const std::string& text)
{
    m_text_texture = m_texture_controller.BuildTextureFromText(m_texture_key, text, m_area_size, m_text_color);
}

void TextArea::DrawTexture() const
{
    if (m_should_draw){
        const SDL_Rect dst{m_position.x, m_position.y, static_cast<int>(m_area_size.x*m_zoom), static_cast<int>(m_area_size.y*m_zoom)};
        m_texture_controller.RenderFont(m_text_texture, dst);
    }
}

void UiController::Draw() const
{
    for (UiElement* e : m_ui_elements)
        e->DrawTexture();
}

GameplayUiController::GameplayUiController(TextureController& texture_controller, const Camera& camera):
    m_dialog_box(texture_controller, "../assets/ui/box.png"), m_text_area(texture_controller, "../assets/ui/fonts/NormalFont.ttf")
{
    // Will be done to find zoom and position for each UiElement
    const ScenePosition viewport_size = camera.GetViewport();
    const float scale = 0.5; // Wanted scale value in [0, 1]
    m_dialog_box.SetZoom((viewport_size.x*scale)/m_dialog_box.GetTextureWidth());
    // Could be improve : GetTextureWidth/Height could use camera zoom for SceneDrawable and zoom for ScreenDrawable (few updates will be necessary in EventController)
    const ScreenPosition padding = {50, 50};
    const ScreenPosition new_size = m_dialog_box.GetSize()*m_dialog_box.GetZoom(); 
    const ScreenPosition delta_size = viewport_size - new_size;
    m_dialog_box.SetScreenPosition(camera.GetScreenOffset() + ScreenPosition{delta_size.x/2, delta_size.y - padding.y});
    m_ui_elements.push_back(&m_dialog_box);

    // Will be done for any TextArea
    m_text_area.SetText("Hello world !");
    m_text_area.SetScreenPosition({50, 50});
    m_ui_elements.push_back(&m_text_area);
}