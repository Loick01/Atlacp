#pragma once

#include <string>

using UiKey = std::string; // string ? Rename UiElementKey ?

enum class UiElementEvent
{
    Delete // Create ?
};

enum class Axis
{
    Width, Height, None
};

enum class Anchor
{
    LeftIn, LeftOut, 
    RightIn, RightOut,
    Center,
    TopIn, TopOut,
    BottomIn, BottomOut
};

template<typename T>
struct UiValue
{
    UiKey id; // const ?
    T value;

    UiValue(UiKey key, T v):
        id(key), value(v) 
    {}
};

struct UiParams
{
    float scale;
    Axis scaleAxis; // UiTextElement don't use this value (text size is handled by the font, the scale value is used for SetMaxWidth())
    Anchor xAnchor;
    Anchor yAnchor;
    float xPadding;
    float yPadding;

    UiParams() {
        scale = 1.f; scaleAxis = Axis::None;
        xAnchor = Anchor::Center; yAnchor = Anchor::Center;
        xPadding = 0.f; yPadding = 0.f;
    }

    UiParams(const float s, const Axis sA, const Anchor xA, const Anchor yA, const float xP, const float yP) {
        scale = s; scaleAxis = sA;
        xAnchor = xA; yAnchor= yA;
        xPadding = xP; yPadding = yP;
    }
};

struct PartialSize // Should be in ui_element.hpp ?
{
    UiKey srcElement;
    Axis axis;
    float amount;

    PartialSize() {
        srcElement = "undefined_element"; // In UiController, I use srcElement to know if there is a padding
    }

    PartialSize(const UiKey& key, const Axis axs, const float amnt) { srcElement = key; axis = axs; amount = amnt; }
};

struct DataUi // Rename + Should not be here ?
{
    UiKey parentKey; // If "root", the current UiElement is a subroot, thus it will be created with BuildSubRoot instead of BuildChild
    UiKey key;
    std::string path; // image path if UiElement, font filepath if UiTextElement
    std::string type; // "uielement" or "textelement"
    std::string text; // Only used for UiTextElement (when type = "textelement"), should not be here ?
    // Can't use UiParams
    PartialSize scale;
    Axis dstScaleAxis; // Only use for UiElement (when type = "uielement"), should not be here ?
    Anchor xAnchor;
    Anchor yAnchor;
    PartialSize xPadding;
    PartialSize yPadding;

    DataUi() {
        text = "invalid_text"; // Should not happen
    }
};