#include "./CountButtons.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

void GuiComponentCountButtons::setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder, std::optional<juce::Font> font)
{
    if (font == nullopt) {
        // 下の段は "-1000" の 5 文字が入る必要がある。ピッチのボタンより
        // 1 段ぶん多く縦に積むので、そのぶん小さめにしてある。
        font = juce::Font(juce::FontOptions(12.0f));
    }

    // 刻む量と札を組にして並べる。押したときの中身はどれも同じなので、
    // 1 か所にまとめて配る。
    auto bind = [&](GuiTextButton& button, const juce::String& title, int delta) {
        button.setup({ .parent = parent, .id = "", .title = title, .font = font, .isReset = false });
        button.setWantsKeyboardFocus(true);
        button.setExplicitFocusOrder(++tabOrder);
        button.onClick = [&slider, delta] { slider.setValue(slider.getValue() + delta); };
        };

    bind(minus10, "-10", -10);
    bind(minus1, "-1", -1);

    // 0 は足すのではなく 0 そのものへ戻す。無限に回す指定がこれ。
    pm0.setup({ .parent = parent, .id = "", .title = "0", .font = font, .bgColor = juce::Colours::aquamarine, .isReset = false });
    pm0.setWantsKeyboardFocus(true);
    pm0.setExplicitFocusOrder(++tabOrder);
    pm0.onClick = [&slider] { slider.setValue(0); };

    bind(plus1, "+1", 1);
    bind(plus10, "+10", 10);

    bind(minus1000, "-1000", -1000);
    bind(minus100, "-100", -100);
    bind(plus100, "+100", 100);
    bind(plus1000, "+1000", 1000);
}

void GuiComponentCountButtons::layoutComponent(juce::Rectangle<int>& rect, int height)
{
    layoutMainFiveComps({
        .rect = rect,
        .comp1 = &minus10,
        .comp2 = &minus1,
        .comp3 = &pm0,
        .comp4 = &plus1,
        .comp5 = &plus10,
        .rowHeight = height
        });

    layoutMainFourComps({
        .rect = rect,
        .comp1 = &minus1000,
        .comp2 = &minus100,
        .comp3 = &plus100,
        .comp4 = &plus1000,
        .rowHeight = height
        });
}

void GuiComponentCountButtons::layoutComponentRow(juce::Rectangle<int>& rect, int height)
{
    layoutRowFiveComps({
        .rect = rect,
        .comp1 = &minus10,
        .comp2 = &minus1,
        .comp3 = &pm0,
        .comp4 = &plus1,
        .comp5 = &plus10,
        .rowHeight = height
        });

    layoutRowFourComps({
        .rect = rect,
        .comp1 = &minus1000,
        .comp2 = &minus100,
        .comp3 = &plus100,
        .comp4 = &plus1000,
        .rowHeight = height
        });
}

void GuiComponentCountButtons::setVisibles(bool visible)
{
    minus10.setVisible(visible);
    minus1.setVisible(visible);
    pm0.setVisible(visible);
    plus1.setVisible(visible);
    plus10.setVisible(visible);

    minus1000.setVisible(visible);
    minus100.setVisible(visible);
    plus100.setVisible(visible);
    plus1000.setVisible(visible);
}

void GuiComponentCountButtons::setEnables(bool enabled)
{
    minus10.setEnabled(enabled);
    minus1.setEnabled(enabled);
    pm0.setEnabled(enabled);
    plus1.setEnabled(enabled);
    plus10.setEnabled(enabled);

    minus1000.setEnabled(enabled);
    minus100.setEnabled(enabled);
    plus100.setEnabled(enabled);
    plus1000.setEnabled(enabled);
}
