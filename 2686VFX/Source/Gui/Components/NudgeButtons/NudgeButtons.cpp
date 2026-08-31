#include "./NudgeButtons.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

void GuiComponentNudgeButtons::setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, std::optional<juce::Font> font)
{
    if (font == nullopt) {
        font = juce::Font(juce::FontOptions(defaultFontHeight));
    }

    minus1.setup({ .parent = parent, .id = "", .title = "-1.0", .font = font, .isReset = false });
    minus1.setWantsKeyboardFocus(true);
    minus1.setExplicitFocusOrder(++tabOrder);
    minus1.onClick = [this, &slider] {
        slider.setValue(slider.getValue() - 1.0);
        };

    minus01.setup({ .parent = parent, .id = "", .title = "-0.1", .font = font, .isReset = false });
    minus01.setWantsKeyboardFocus(true);
    minus01.setExplicitFocusOrder(++tabOrder);
    minus01.onClick = [this, &slider] {
        slider.setValue(slider.getValue() - 0.1);
        };

    minus001.setup({ .parent = parent, .id = "", .title = "-0.01", .font = font, .isReset = false });
    minus001.setWantsKeyboardFocus(true);
    minus001.setExplicitFocusOrder(++tabOrder);
    minus001.onClick = [this, &slider] {
        slider.setValue(slider.getValue() - 0.01);
        };

    pm0.setup({ .parent = parent, .id = "", .title = "0.0", .font = font, .bgColor = juce::Colours::aquamarine, .isReset = false });
    pm0.setWantsKeyboardFocus(true);
    pm0.setExplicitFocusOrder(++tabOrder);
    pm0.onClick = [this, &slider] {
        slider.setValue(0.0f);
        };

    plus001.setup({ .parent = parent, .id = "", .title = "+0.01", .font = font, .isReset = false });
    plus001.setWantsKeyboardFocus(true);
    plus001.setExplicitFocusOrder(++tabOrder);
    plus001.onClick = [this, &slider] {
        slider.setValue(slider.getValue() + 0.01);
        };

    plus01.setup({ .parent = parent, .id = "", .title = "+0.1", .font = font, .isReset = false });
    plus01.setWantsKeyboardFocus(true);
    plus01.setExplicitFocusOrder(++tabOrder);
    plus01.onClick = [this, &slider] {
        slider.setValue(slider.getValue() + 0.1);
        };

    plus1.setup({ .parent = parent, .id = "", .title = "+1.0", .font = font, .isReset = false });
    plus1.setWantsKeyboardFocus(true);
    plus1.setExplicitFocusOrder(++tabOrder);
    plus1.onClick = [this, &slider] {
        slider.setValue(slider.getValue() + 1.0);
        };
}

void GuiComponentNudgeButtons::layoutComponent(juce::Rectangle<int>& rect, int height)
{
    layoutMainSevenComps({
        .rect = rect,
        .comp1 = &minus1,
        .comp2 = &minus01,
        .comp3 = &minus001,
        .comp4 = &pm0,
        .comp5 = &plus001,
        .comp6 = &plus01,
        .comp7 = &plus1,
        .rowHeight = height
        });
}

void GuiComponentNudgeButtons::layoutComponentRow(juce::Rectangle<int>& rect, int height)
{
    layoutRowSevenComps({
        .rect = rect,
        .comp1 = &minus1,
        .comp2 = &minus01,
        .comp3 = &minus001,
        .comp4 = &pm0,
        .comp5 = &plus001,
        .comp6 = &plus01,
        .comp7 = &plus1,
        .rowHeight = height
        });
}

void GuiComponentNudgeButtons::setVisibles(bool visible)
{
    minus1.setVisible(visible);
    minus01.setVisible(visible);
    minus001.setVisible(visible);
    pm0.setVisible(visible);
    plus001.setVisible(visible);
    plus01.setVisible(visible);
    plus1.setVisible(visible);
}

void GuiComponentNudgeButtons::setEnables(bool enabled)
{
    minus1.setEnabled(enabled);
    minus01.setEnabled(enabled);
    minus001.setEnabled(enabled);
    pm0.setEnabled(enabled);
    plus001.setEnabled(enabled);
    plus01.setEnabled(enabled);
    plus1.setEnabled(enabled);
}
