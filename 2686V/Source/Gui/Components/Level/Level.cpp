#include "./Level.h"

#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Const/ConstGlobal.h"

void GuiComponentLevel::setupComponent(juce::Component& parent, int& tabOrder, const juce::String& prefix) {
    // 出力レベル
    levelSlider.setup({ .parent = parent, .id = prefix + CPK::level, .title = "LV", .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    levelPM1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-1.0", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false});
    levelPM1.setWantsKeyboardFocus(true);
    levelPM1.setExplicitFocusOrder(++tabOrder);
    levelPM1.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() - 1.0f, juce::sendNotification);
        };

    levelPM01.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-0.1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    levelPM01.setWantsKeyboardFocus(true);
    levelPM01.setExplicitFocusOrder(++tabOrder);
    levelPM01.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() - 0.1f, juce::sendNotification);
        };

    levelPM001.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-0.01", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    levelPM001.setWantsKeyboardFocus(true);
    levelPM001.setExplicitFocusOrder(++tabOrder);
    levelPM001.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() - 0.01f, juce::sendNotification);
        };

    levelTo1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "1.0", .bgColor = juce::Colours::yellow.brighter(0.5f), .isReset = false });
    levelTo1.setWantsKeyboardFocus(true);
    levelTo1.setExplicitFocusOrder(++tabOrder);
    levelTo1.onClick = [this]() {
        levelSlider.setValue(1.0f, juce::sendNotification);
        };

    levelP001.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+0.01", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    levelP001.setWantsKeyboardFocus(true);
    levelP001.setExplicitFocusOrder(++tabOrder);
    levelP001.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() + 0.01f, juce::sendNotification);
        };

    levelP01.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+0.1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    levelP01.setWantsKeyboardFocus(true);
    levelP01.setExplicitFocusOrder(++tabOrder);
    levelP01.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() + 0.1f, juce::sendNotification);
        };

    levelP1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+1.0", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    levelP1.setWantsKeyboardFocus(true);
    levelP1.setExplicitFocusOrder(++tabOrder);
    levelP1.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() + 1.0f, juce::sendNotification);
        };

    levelTo025.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.25", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    levelTo025.setWantsKeyboardFocus(true);
    levelTo025.setExplicitFocusOrder(++tabOrder);
    levelTo025.onClick = [this]() {
        levelSlider.setValue(0.25f, juce::sendNotification);
        };

    levelTo05.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.5", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    levelTo05.setWantsKeyboardFocus(true);
    levelTo05.setExplicitFocusOrder(++tabOrder);
    levelTo05.onClick = [this]() {
        levelSlider.setValue(0.5f, juce::sendNotification);
        };

    levelTo075.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.75", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    levelTo075.setWantsKeyboardFocus(true);
    levelTo075.setExplicitFocusOrder(++tabOrder);
    levelTo075.onClick = [this]() {
        levelSlider.setValue(0.75f, juce::sendNotification);
        };

    levelTo02.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.2", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    levelTo02.setWantsKeyboardFocus(true);
    levelTo02.setExplicitFocusOrder(++tabOrder);
    levelTo02.onClick = [this]() {
        levelSlider.setValue(0.2f, juce::sendNotification);
        };

    levelTo04.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.4", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    levelTo04.setWantsKeyboardFocus(true);
    levelTo04.setExplicitFocusOrder(++tabOrder);
    levelTo04.onClick = [this]() {
        levelSlider.setValue(0.4f, juce::sendNotification);
        };

    levelTo06.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.6", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    levelTo06.setWantsKeyboardFocus(true);
    levelTo06.setExplicitFocusOrder(++tabOrder);
    levelTo06.onClick = [this]() {
        levelSlider.setValue(0.6f, juce::sendNotification);
        };

    levelTo08.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.8", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    levelTo08.setWantsKeyboardFocus(true);
    levelTo08.setExplicitFocusOrder(++tabOrder);
    levelTo08.onClick = [this]() {
        levelSlider.setValue(0.8f, juce::sendNotification);
        };

    levelTo0125.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.125", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo0125.setWantsKeyboardFocus(true);
    levelTo0125.setExplicitFocusOrder(++tabOrder);
    levelTo0125.onClick = [this]() {
        levelSlider.setValue(0.125f, juce::sendNotification);
        };

    levelTo0142.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.142", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo0142.setWantsKeyboardFocus(true);
    levelTo0142.setExplicitFocusOrder(++tabOrder);
    levelTo0142.onClick = [this]() {
        levelSlider.setValue(0.142f, juce::sendNotification);
        };

    levelTo016.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.16", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo016.setWantsKeyboardFocus(true);
    levelTo016.setExplicitFocusOrder(++tabOrder);
    levelTo016.onClick = [this]() {
        levelSlider.setValue(0.16f, juce::sendNotification);
        };

    levelTo033.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.33", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo033.setWantsKeyboardFocus(true);
    levelTo033.setExplicitFocusOrder(++tabOrder);
    levelTo033.onClick = [this]() {
        levelSlider.setValue(0.33f, juce::sendNotification);
        };

    levelTo067.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.67", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo067.setWantsKeyboardFocus(true);
    levelTo067.setExplicitFocusOrder(++tabOrder);
    levelTo067.onClick = [this]() {
        levelSlider.setValue(0.67f, juce::sendNotification);
        };

    levelTo083.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.83", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo083.setWantsKeyboardFocus(true);
    levelTo083.setExplicitFocusOrder(++tabOrder);
    levelTo083.onClick = [this]() {
        levelSlider.setValue(0.83f, juce::sendNotification);
        };
}

void GuiComponentLevel::layoutComponent(juce::Rectangle<int>& rect) {
    layoutMain({ .mainRect = rect, .label = &levelSlider.label, .component = &levelSlider });
    layoutMainThreeComps({ .rect = rect, .comp1 = &levelPM1, .comp2 = &levelTo1, .comp3 = &levelP1 });
    layoutMainThreeComps({ .rect = rect, .comp1 = &levelTo025, .comp2 = &levelTo05, .comp3 = &levelTo075 });
    layoutMainFourComps({ .rect = rect, .comp1 = &levelTo02, .comp2 = &levelTo04, .comp3 = &levelTo06, .comp4 = &levelTo08 });
    layoutMainSixComps({ .rect = rect, .comp1 = &levelTo0125, .comp2 = &levelTo0142, .comp3 = &levelTo016, .comp4 = &levelTo033, .comp5 = &levelTo067, .comp6 = &levelTo083 });
    layoutMainFourComps({ .rect = rect, .comp1 = &levelPM001, .comp2 = &levelPM01, .comp3 = &levelP01, .comp4 = &levelP001 });
}

void GuiComponentLevel::layoutComponentRow(juce::Rectangle<int>& rect) {
    layoutRow({ .rowRect = rect, .label = &levelSlider.label, .component = &levelSlider });
    layoutRowThreeComps({ .rect = rect, .comp1 = &levelPM1, .comp2 = &levelTo1, .comp3 = &levelP1 });
    layoutRowThreeComps({ .rect = rect, .comp1 = &levelTo025, .comp2 = &levelTo05, .comp3 = &levelTo075 });
    layoutRowFourComps({ .rect = rect, .comp1 = &levelTo02, .comp2 = &levelTo04, .comp3 = &levelTo06, .comp4 = &levelTo08 });
    layoutRowSixComps({ .rect = rect, .comp1 = &levelTo0125, .comp2 = &levelTo0142, .comp3 = &levelTo016, .comp4 = &levelTo033, .comp5 = &levelTo067, .comp6 = &levelTo083 });
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
    levelTo025.setVisible(visible);
    levelTo05.setVisible(visible);
    levelTo075.setVisible(visible);
    levelTo02.setVisible(visible);
    levelTo04.setVisible(visible);
    levelTo06.setVisible(visible);
    levelTo08.setVisible(visible);
	levelTo0125.setVisible(visible);
	levelTo0142.setVisible(visible);
    levelTo016.setVisible(visible);
    levelTo033.setVisible(visible);
	levelTo067.setVisible(visible);
    levelTo083.setVisible(visible);
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
    levelTo025.setEnabled(enabled);
    levelTo05.setEnabled(enabled);
    levelTo075.setEnabled(enabled);
    levelTo02.setEnabled(enabled);
    levelTo04.setEnabled(enabled);
    levelTo06.setEnabled(enabled);
    levelTo08.setEnabled(enabled);
    levelTo0125.setEnabled(enabled);
    levelTo0142.setEnabled(enabled);
    levelTo016.setEnabled(enabled);
    levelTo033.setEnabled(enabled);
    levelTo067.setEnabled(enabled);
    levelTo083.setEnabled(enabled);
}

float GuiComponentLevel::getLevel() {
	return levelSlider.getValue();
}

void GuiComponentLevel::setLevel(float level) {
	levelSlider.setValue(level, juce::dontSendNotification);
}

void GuiComponentLevel::setImportingParams(juce::StringArray& lines, int& index) {
    levelSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
}

juce::String GuiComponentLevel::getExportedParams() {
    juce::String content = "";

    content += juce::String(levelSlider.getValue(), Global::floatDecimalPlaces) + "\n";

    return content;
}
