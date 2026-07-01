#include "./Level.h"

#include "../../../Core/Gui/GuiHelpers.h"

void GuiComponentLevel::setupComponent(juce::Component& parent, int& tabOrder, const juce::String& prefix) {
    // 出力レベル
    levelSlider.setup({ .parent = parent, .id = prefix + "_LEVEL", .title = "LV", .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    levelPM1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-1.0", .isReset = false });
    levelPM1.setWantsKeyboardFocus(true);
    levelPM1.setExplicitFocusOrder(++tabOrder);
    levelPM1.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() - 1.0f, juce::sendNotification);
        };

    levelPM01.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-0.1", .isReset = false });
    levelPM01.setWantsKeyboardFocus(true);
    levelPM01.setExplicitFocusOrder(++tabOrder);
    levelPM01.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() - 0.1f, juce::sendNotification);
        };

    levelPM001.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-0.01", .isReset = false });
    levelPM001.setWantsKeyboardFocus(true);
    levelPM001.setExplicitFocusOrder(++tabOrder);
    levelPM001.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() - 0.01f, juce::sendNotification);
        };

    levelTo1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-> 1.0", .isReset = false });
    levelTo1.setWantsKeyboardFocus(true);
    levelTo1.setExplicitFocusOrder(++tabOrder);
    levelTo1.onClick = [this]() {
        levelSlider.setValue(1.0f, juce::sendNotification);
        };

    levelP001.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+0.01", .isReset = false });
    levelP001.setWantsKeyboardFocus(true);
    levelP001.setExplicitFocusOrder(++tabOrder);
    levelP001.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() + 0.01f, juce::sendNotification);
        };

    levelP01.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+0.1", .isReset = false });
    levelP01.setWantsKeyboardFocus(true);
    levelP01.setExplicitFocusOrder(++tabOrder);
    levelP01.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() + 0.1f, juce::sendNotification);
        };

    levelP1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+1.0", .isReset = false });
    levelP1.setWantsKeyboardFocus(true);
    levelP1.setExplicitFocusOrder(++tabOrder);
    levelP1.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() + 1.0f, juce::sendNotification);
        };
}

void GuiComponentLevel::layoutComponent(juce::Rectangle<int>& rect) {
    layoutMain({ .mainRect = rect, .label = &levelSlider.label, .component = &levelSlider });
    layoutMainThreeComps({ .rect = rect, .comp1 = &levelPM1, .comp2 = &levelTo1, .comp3 = &levelP1 });
    layoutMainFourComps({ .rect = rect, .comp1 = &levelPM001, .comp2 = &levelPM01, .comp3 = &levelP01, .comp4 = &levelP001 });
}

void GuiComponentLevel::layoutComponentRow(juce::Rectangle<int>& rect) {
    layoutRow({ .rowRect = rect, .label = &levelSlider.label, .component = &levelSlider });
    layoutRowThreeComps({ .rect = rect, .comp1 = &levelPM1, .comp2 = &levelTo1, .comp3 = &levelP1 });
    layoutRowFourComps({ .rect = rect, .comp1 = &levelPM001, .comp2 = &levelPM01, .comp3 = &levelP01, .comp4 = &levelP001 });
}

void GuiComponentLevel::setVisible(bool visible) {
    levelSlider.setVisible(visible);
    levelPM1.setVisible(visible);
    levelPM01.setVisible(visible);
    levelPM001.setVisible(visible);
    levelTo1.setVisible(visible);
    levelP001.setVisible(visible);
    levelP01.setVisible(visible);
    levelP1.setVisible(visible);
}

void GuiComponentLevel::setEnable(bool enabled) {
    levelSlider.setEnabled(enabled);
    levelPM1.setEnabled(enabled);
    levelPM01.setEnabled(enabled);
    levelPM001.setEnabled(enabled);
    levelTo1.setEnabled(enabled);
    levelP001.setEnabled(enabled);
    levelP01.setEnabled(enabled);
    levelP1.setEnabled(enabled);
}

float GuiComponentLevel::getLevel() {
	return levelSlider.getValue();
}

void GuiComponentLevel::setLevel(float level) {
	levelSlider.setValue(level, juce::dontSendNotification);
}
