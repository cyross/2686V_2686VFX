#include "./PresetName.h"

#include "../../../Core/Gui/GuiHelpers.h"

void GuiComponentPresetName::setupComponent(juce::Component& parent, int& tabOrder, const juce::String& name) {
    presetNameLabel.setup({ .parent = parent, .title = "" });
    presetNameLabel.setText(name, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(
        juce::Font(juce::FontOptions(18.0f))
    );
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    parent.addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });
}

void GuiComponentPresetName::layoutComponent(juce::Rectangle<int>& rect) {

    layoutMainParamName({ .mainRect = rect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = rect.removeFromTop(20);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

}

void GuiComponentPresetName::updatePresetName(const juce::String& name) {
    presetNameLabel.setText(name, juce::NotificationType::dontSendNotification);
}
