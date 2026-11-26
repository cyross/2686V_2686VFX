#include "./ShowHide.h"

#include "Shared/Core/Gui/GuiColor.h"
#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiValues.h"

void GuiShowHideButton::setupComponent(juce::Component& parent, const juce::String& name, int& tabOrder)
{
    partsName = name;

    setup({
        .parent = parent,
        .title = {},
        .font = juce::Font(juce::FontOptions(CoreGuiValue::ShowHide::fontSize, juce::Font::bold)),
        });

    setWantsKeyboardFocus(true);
    setExplicitFocusOrder(++tabOrder);

    onClick = [this] { setShown(!shown); };

    applyLook();
}

void GuiShowHideButton::setShown(bool shouldShow)
{
    if (shown == shouldShow) return;

    shown = shouldShow;

    applyLook();

    ctx.editor.resized();
}

void GuiShowHideButton::applyLook()
{
    setButtonText((shown ? "HIDE " : "SHOW ") + partsName);

    const juce::Colour text = shown ? GuiColor::ShowHide::HideText : GuiColor::ShowHide::ShowText;
    const juce::Colour bg = shown ? GuiColor::ShowHide::HideBg : GuiColor::ShowHide::ShowBg;

    setColour(juce::TextButton::textColourOffId, text);
    setColour(juce::TextButton::textColourOnId, text);
    setColour(juce::TextButton::buttonColourId, bg);

    repaint();
}

void GuiShowHideButton::layoutComponent(juce::Rectangle<int>& rect)
{
    rect.removeFromTop(CoreGuiValue::ShowHide::gapAbove);

    layoutMain({ .mainRect = rect, .component = this, .rowHeight = CoreGuiValue::ShowHide::height });
}

void GuiShowHideButton::layoutComponentRow(juce::Rectangle<int>& rect)
{
    rect.removeFromTop(CoreGuiValue::ShowHide::gapAbove);

    layoutRow({ .rowRect = rect, .component = this, .rowHeight = CoreGuiValue::ShowHide::height });
}
