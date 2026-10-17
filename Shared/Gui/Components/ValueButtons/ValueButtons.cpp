#include "./ValueButtons.h"

#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiStructs.h"

void GuiComponentValueButtons::setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, const std::vector<Entry>& entries, std::optional<juce::Font> font, int row)
{
    perRow = juce::jmax(1, row);

    if (font == std::nullopt) font = juce::Font(juce::FontOptions(12.0f));

    buttons.clear();

    for (const auto& e : entries) {
        auto b = std::make_unique<GuiTextButton>(ctx);

        b->setup({ .parent = parent, .id = "", .title = e.label, .font = font, .isReset = false });
        b->setWantsKeyboardFocus(true);
        b->setExplicitFocusOrder(++tabOrder);
        b->onClick = [&slider, e] { slider.setValue(e.add ? slider.getValue() + e.value : e.value); };

        buttons.push_back(std::move(b));
    }
}

// 1 行に perRow 個ずつ、等しい幅で並べる (余った行も同じ幅のまま左から)
void GuiComponentValueButtons::layoutComponent(juce::Rectangle<int>& rect, int height)
{
    const int gap = CoreGuiValue::MainGroup::Comps::Five::paddingInner;

    for (size_t first = 0; first < buttons.size(); first += (size_t)perRow) {
        auto row = rect.removeFromTop(height);

        rect.removeFromTop(CoreGuiValue::MainGroup::Row::paddingTop);

        const int w = (row.getWidth() - gap * (perRow - 1)) / perRow;

        for (size_t i = first; i < std::min(buttons.size(), first + (size_t)perRow); ++i) {
            buttons[i]->setBounds(row.removeFromLeft(w));
            row.removeFromLeft(gap);
        }
    }
}

void GuiComponentValueButtons::setVisibles(bool visible)
{
    for (auto& b : buttons) b->setVisible(visible);
}

void GuiComponentValueButtons::setEnables(bool enabled)
{
    for (auto& b : buttons) b->setEnabled(enabled);
}
