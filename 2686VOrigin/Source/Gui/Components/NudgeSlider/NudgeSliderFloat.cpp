#include "./NudgeSliderFloat.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Processor/ProcessorValues.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

void GuiComponentNudgeSliderFloat::setupComponent(
    juce::Component& parent,
    const juce::String& id,
    const juce::String& title,
    int& tabOrder,
    std::optional<std::function<void()>> onValueChange,
    std::optional<juce::Font> labelFont,
    bool isReset
)
{
    mainSlider.setup({ .parent = parent, .id = id, .title = title, .isReset = isReset, .labelFont = labelFont });
    mainSlider.setNumDecimalPlacesToDisplay(0);
    mainSlider.setWantsKeyboardFocus(true);
    mainSlider.setExplicitFocusOrder(++tabOrder);
    if (onValueChange.has_value()) {
        mainSlider.onValueChange = onValueChange.value();
    }

    nudgeVisibleButton.setup({ .parent = parent, .title = "N", .bgColor = juce::Colours::darkorange });
    nudgeVisibleButton.setWantsKeyboardFocus(true);
    nudgeVisibleButton.setExplicitFocusOrder(++tabOrder);
    nudgeVisibleButton.onClick = [this] {
        visibleNudgeButtons = !visibleNudgeButtons;

        ctx.editor.resized();
        };
}

void GuiComponentNudgeSliderFloat::layoutComponent(juce::Rectangle<int>& rect, int rowHeight)
{
    auto rowRect = rect.removeFromTop(rowHeight);

    // layoutMain と同じ行送りにする
    rect.removeFromTop(CoreGuiValue::MainGroup::Row::paddingTop);

    auto nbRect = rowRect.removeFromRight(nudgeButtonWidth);

    rowRect.removeFromRight(nudgeButtonGap);

    auto lRect = rowRect.removeFromLeft(CoreGuiValue::MainGroup::Label::width);

    mainSlider.label.setBounds(lRect);
    mainSlider.setBounds(rowRect);
    nudgeVisibleButton.setBounds(nbRect);

    rect.removeFromTop(CoreGuiValue::MainGroup::Row::paddingBottom);
}

void GuiComponentNudgeSliderFloat::layoutComponentRow(juce::Rectangle<int>& rect, int rowHeight)
{
    auto rowRect = rect.removeFromTop(rowHeight);

    // layoutRow と同じ行送りにする
    rect.removeFromTop(CoreGuiValue::ParamGroup::Row::paddingTop);

    auto nbRect = rowRect.removeFromRight(nudgeButtonWidth);

    rowRect.removeFromRight(nudgeButtonGap);

    auto lRect = rowRect.removeFromLeft(CoreGuiValue::ParamGroup::Label::width);

    mainSlider.label.setBounds(lRect);
    mainSlider.setBounds(rowRect);
    nudgeVisibleButton.setBounds(nbRect);

    rect.removeFromTop(CoreGuiValue::ParamGroup::Row::paddingBottom);
}

void GuiComponentNudgeSliderFloat::setVisibles(bool visible) {
    mainSlider.setVisibleWithLabel(visible);
    nudgeVisibleButton.setVisible(visible);
}

void GuiComponentNudgeSliderFloat::setEnabled(bool enabled) {
    mainSlider.setEnabledWithLabel(enabled);
    nudgeVisibleButton.setEnabled(enabled);
}

void GuiComponentNudgeSliderFloat::setImportingParams(juce::StringArray& lines, int& index) {
    mainSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
}

juce::String GuiComponentNudgeSliderFloat::getExportedParams() {
    return juce::String(mainSlider.getValue()) + "\n";
}
