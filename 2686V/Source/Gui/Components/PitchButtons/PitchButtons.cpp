#include "./PitchButtons.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

void GuiComponentPitchButtons::setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, std::optional<juce::Font> font)
{
    minus1200.setup({ .parent = parent, .id = "", .title = "-1200", .font = font, .isReset = false });
    minus1200.setWantsKeyboardFocus(true);
    minus1200.setExplicitFocusOrder(++tabOrder);
    minus1200.onClick = [this, &slider] {
        slider.setValue(slider.getValue() - 1200);
        };

    minus100.setup({ .parent = parent, .id = "", .title = "-100", .font = font, .isReset = false });
    minus100.setWantsKeyboardFocus(true);
    minus100.setExplicitFocusOrder(++tabOrder);
    minus100.onClick = [this, &slider] {
        slider.setValue(slider.getValue() - 100);
        };

    pm0.setup({ .parent = parent, .id = "", .title = "0", .font = font, .isReset = false });
    pm0.setWantsKeyboardFocus(true);
    pm0.setExplicitFocusOrder(++tabOrder);
    pm0.onClick = [this, &slider] {
        slider.setValue(0);
        };

    plus100.setup({ .parent = parent, .id = "", .title = "+100", .font = font, .isReset = false });
    plus100.setWantsKeyboardFocus(true);
    plus100.setExplicitFocusOrder(++tabOrder);
    plus100.onClick = [this, &slider] {
        slider.setValue(slider.getValue() + 100);
        };

    plus1200.setup({ .parent = parent, .id = "", .title = "+120", .font = font, .isReset = false });
    plus1200.setWantsKeyboardFocus(true);
    plus1200.setExplicitFocusOrder(++tabOrder);
    plus1200.onClick = [this, &slider] {
        slider.setValue(slider.getValue() + 120);
        };
}

void GuiComponentPitchButtons::layoutComponent(juce::Rectangle<int>& rect, int height)
{
    layoutMainFiveComps({
        .rect = rect,
        .comp1 = &minus1200,
        .comp2 = &minus100,
        .comp3 = &pm0,
        .comp4 = &plus100,
        .comp5 = &plus1200,
        .rowHeight = height
        });
}

void GuiComponentPitchButtons::layoutComponentRow(juce::Rectangle<int>& rect, int height)
{
    layoutRowFiveComps({
        .rect = rect,
        .comp1 = &minus1200,
        .comp2 = &minus100,
        .comp3 = &pm0,
        .comp4 = &plus100,
        .comp5 = &plus1200,
        .rowHeight = height
        });
}

void GuiComponentPitchButtons::setVisibles(bool visible)
{
    minus1200.setVisible(visible);
    minus100.setVisible(visible);
    pm0.setVisible(visible);
    plus100.setVisible(visible);
    plus1200.setVisible(visible);
}

void GuiComponentPitchButtons::setEnables(bool enabled)
{
    minus1200.setEnabled(enabled);
    minus100.setEnabled(enabled);
    pm0.setEnabled(enabled);
    plus100.setEnabled(enabled);
    plus1200.setEnabled(enabled);
}
