#include "./DutyButtons.h"

#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiStructs.h"

GuiComponentDutyButtons::GuiComponentDutyButtons(const GuiContext& context) :
    GuiBase(context)
{
    for (auto& b : buttons) b = std::make_unique<GuiTextButton>(context);
}

void GuiComponentDutyButtons::setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, std::optional<juce::Font> font)
{
    if (font == std::nullopt) font = juce::Font(juce::FontOptions(12.0f));

    for (size_t i = 0; i < buttons.size(); ++i) {
        const float value = percents[i];
        auto& b = *buttons[i];

        b.setup({ .parent = parent, .id = "", .title = juce::String(value, value == std::floor(value) ? 0 : 2), .font = font, .isReset = false });
        b.setWantsKeyboardFocus(true);
        b.setExplicitFocusOrder(++tabOrder);
        b.onClick = [&slider, value] { slider.setValue(value); };
    }
}

// 5 個と 4 個の 2 行に並べる
void GuiComponentDutyButtons::layoutComponent(juce::Rectangle<int>& rect, int height)
{
    layoutMainFiveComps({
        .rect = rect,
        .comp1 = buttons[0].get(),
        .comp2 = buttons[1].get(),
        .comp3 = buttons[2].get(),
        .comp4 = buttons[3].get(),
        .comp5 = buttons[4].get(),
        .rowHeight = height
        });

    layoutMainFourComps({
        .rect = rect,
        .comp1 = buttons[5].get(),
        .comp2 = buttons[6].get(),
        .comp3 = buttons[7].get(),
        .comp4 = buttons[8].get(),
        .rowHeight = height
        });
}

void GuiComponentDutyButtons::setVisibles(bool visible)
{
    for (auto& b : buttons) b->setVisible(visible);
}

void GuiComponentDutyButtons::setEnables(bool enabled)
{
    for (auto& b : buttons) b->setEnabled(enabled);
}
