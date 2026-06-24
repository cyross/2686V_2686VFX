#include "./ViewMode.h"

#include "../../../Core/Gui/GuiHelpers.h"

void GuiComponentViewMode::setupComponent(juce::Component& parent, int& tabOrder) {
    parent.addAndMakeVisible(viewModeSeparator);
    viewModeSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    viewModeLabel.setup({ .parent = parent, .title = "", .color = juce::Colours::gold.brighter(0.5f) });
    viewModeLabel.setText("VIEW MODE: TWIN", juce::sendNotification);
    viewModeLabel.setWantsKeyboardFocus(false);

    viewModeToTopButton.setup({ .parent = parent, .title = juce::String("") + "▲", .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    viewModeToTopButton.setWantsKeyboardFocus(true);
    viewModeToTopButton.setExplicitFocusOrder(++tabOrder);
    viewModeToTopButton.onClick = [this] {
        viewMode = GuiComponentViewModes::Top;
        viewModeLabel.setText("VIEW MODE: TOP", juce::sendNotification);

        if (onChangeViewMode != nullptr) {
            onChangeViewMode(viewMode);
        }
        };

    viewModeToTwinButton.setup({ .parent = parent, .title = juce::String("") + "■", .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    viewModeToTwinButton.setWantsKeyboardFocus(true);
    viewModeToTwinButton.setExplicitFocusOrder(++tabOrder);
    viewModeToTwinButton.onClick = [this] {
        viewMode = GuiComponentViewModes::Twin;
        viewModeLabel.setText("VIEW MODE: TWIN", juce::sendNotification);

        if (onChangeViewMode != nullptr) {
            onChangeViewMode(viewMode);
        }
        };

    viewModeToBottomButton.setup({ .parent = parent, .title = juce::String("") + "▼", .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    viewModeToBottomButton.setWantsKeyboardFocus(true);
    viewModeToBottomButton.setExplicitFocusOrder(++tabOrder);
    viewModeToBottomButton.onClick = [this] {
        viewMode = GuiComponentViewModes::Bottom;
        viewModeLabel.setText("VIEW MODE: BOTTOM", juce::sendNotification);

        if (onChangeViewMode != nullptr) {
            onChangeViewMode(viewMode);
        }
        };
}

void GuiComponentViewMode::layoutComponent(juce::Rectangle<int>& rect) {
    viewModeLabel.setVisible(true);
    viewModeToTopButton.setVisible(true);
    viewModeToTwinButton.setVisible(true);
    viewModeToBottomButton.setVisible(true);
    viewModeSeparator.setVisible(true);

    layoutMainViewMode({ .rect = rect, .label = viewModeLabel, .comp1 = &viewModeToTopButton, .comp2 = &viewModeToTwinButton, .comp3 = &viewModeToBottomButton });

    auto viewModeSeparatorArea = rect.removeFromTop(20);
    viewModeSeparator.setBounds(viewModeSeparatorArea);
}
