#include "./SsgSwButtons.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

void GuiComponentSsgSwButtons::setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, std::optional<juce::Font> font)
{
	if (font == nullopt) {
		font = juce::Font(juce::FontOptions(16.0f));
	}

    minus001.setup({ .parent = parent, .id = "", .title = "-0.01", .font = font, .isReset = false });
    minus001.setWantsKeyboardFocus(true);
    minus001.setExplicitFocusOrder(++tabOrder);
    minus001.onClick = [this, &slider] {
        slider.setValue(slider.getValue() - 0.01);
        };

    minus01.setup({ .parent = parent, .id = "", .title = "-0.1", .font = font, .isReset = false });
    minus01.setWantsKeyboardFocus(true);
    minus01.setExplicitFocusOrder(++tabOrder);
    minus01.onClick = [this, &slider] {
        slider.setValue(slider.getValue() - 0.1);
        };

    pm0.setup({ .parent = parent, .id = "", .title = "0.0", .font = font, .bgColor = juce::Colours::aquamarine, .isReset = false });
    pm0.setWantsKeyboardFocus(true);
    pm0.setExplicitFocusOrder(++tabOrder);
    pm0.onClick = [this, &slider] {
        slider.setValue(0.0f);
        };

    pm1.setup({ .parent = parent, .id = "", .title = "1.0", .font = font, .bgColor = juce::Colours::yellow.brighter(0.5f), .isReset = false });
    pm1.setWantsKeyboardFocus(true);
    pm1.setExplicitFocusOrder(++tabOrder);
    pm1.onClick = [this, &slider] {
        slider.setValue(1.0f);
        };

    plus01.setup({ .parent = parent, .id = "", .title = "+0.1", .font = font, .isReset = false });
    plus01.setWantsKeyboardFocus(true);
    plus01.setExplicitFocusOrder(++tabOrder);
    plus01.onClick = [this, &slider] {
        slider.setValue(slider.getValue() + 0.1);
        };

    plus001.setup({ .parent = parent, .id = "", .title = "+0.01", .font = font, .isReset = false });
    plus001.setWantsKeyboardFocus(true);
    plus001.setExplicitFocusOrder(++tabOrder);
    plus001.onClick = [this, &slider] {
        slider.setValue(slider.getValue() + 0.01);
        };
}

void GuiComponentSsgSwButtons::layoutComponent(juce::Rectangle<int>& rect, int height)
{
    layoutMainSixComps({
        .rect = rect,
        .comp1 = &minus001,
        .comp2 = &minus01,
        .comp3 = &pm0,
        .comp4 = &pm1,
        .comp5 = &plus01,
        .comp6 = &plus001,
        .rowHeight = height
        });
}

void GuiComponentSsgSwButtons::layoutComponentRow(juce::Rectangle<int>& rect, int height)
{
    layoutRowSixComps({
        .rect = rect,
        .comp1 = &minus001,
        .comp2 = &minus01,
        .comp3 = &pm0,
        .comp4 = &pm1,
        .comp5 = &plus01,
        .comp6 = &plus001,
        .rowHeight = height
        });
}

void GuiComponentSsgSwButtons::setVisibles(bool visible)
{
    minus001.setVisible(visible);
    minus01.setVisible(visible);
    pm0.setVisible(visible);
    pm1.setVisible(visible);
    plus01.setVisible(visible);
    plus001.setVisible(visible);
}

void GuiComponentSsgSwButtons::setEnables(bool enabled)
{
    minus001.setEnabled(enabled);
    minus01.setEnabled(enabled);
    pm0.setEnabled(enabled);
    pm1.setEnabled(enabled);
    plus01.setEnabled(enabled);
    plus001.setEnabled(enabled);
}
