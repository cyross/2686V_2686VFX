#include "./PresetName.h"

#include "../../../Core/Gui/GuiHelpers.h"

void GuiComponentPresetName::setupComponent(juce::Component& parent, int& tabOrder, const juce::String& name) {
    presetNameLabel.setup({ .parent = parent, .title = "" });
    presetNameLabel.setText(name, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(
        juce::Font(juce::FontOptions(18.0f))
    );
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

	presetNameSeparator.setupComponent(parent);
}

void GuiComponentPresetName::layoutComponent(juce::Rectangle<int>& rect) {

    layoutMainParamName({ .mainRect = rect, .label = &presetNameLabel });

    presetNameSeparator.layoutComponent(rect);
}

void GuiComponentPresetName::updatePresetName(const juce::String& name) {
    presetNameLabel.setText(name, juce::NotificationType::dontSendNotification);
}
