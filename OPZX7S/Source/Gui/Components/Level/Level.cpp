#include "./Level.h"

#include "../../../Core/Io/ParamFile.h"

#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Const/ConstGlobal.h"

static const double roundingPrecisionBase = std::pow(10, Global::floatDecimalPlaces); // 小数点以下の丸め精度を決定するための基準値

void GuiComponentLevel::applyStepSnap()
{
    int index = stepSelector.getSelectedItemIndex();
    if (index <= 0) return; // 0 (Free) の場合は何もしない

    double numSteps = 1.0;
    switch (index) {
    case 1: numSteps = 1.0; break;
    case 2: numSteps = 2.0; break;
    case 3: numSteps = 4.0; break;
    case 4: numSteps = 8.0; break;
    case 5: numSteps = 16.0; break;
    case 6: numSteps = 32.0; break;
    case 7: numSteps = 64.0; break;
    case 8: numSteps = 128.0; break;
    case 9: numSteps = 256.0; break;
	case 10: numSteps = 3.0; break;
	case 11: numSteps = 6.0; break;
    case 12: numSteps = 7.0; break;
    case 13: numSteps = 10.0; break;
	case 14: numSteps = 100.0; break;
    default: return;
    }

    double currentVal = (double)levelSlider.getValue();

	// C++では小数点以下の丸め精度を決定するために、まず基準値を掛けて整数化し、丸めた後に元のスケールに戻す
    double baseVal = (double)(std::round(currentVal * numSteps)) * roundingPrecisionBase;
    float snappedVal = (float)(std::round(baseVal / numSteps) / roundingPrecisionBase);

    // スライダーの最小・最大値の範囲内にクランプする
    snappedVal = std::clamp(snappedVal, (float)levelSlider.getMinimum(), (float)levelSlider.getMaximum());

    // 再帰呼び出しを防ぐため、dontSendNotification で更新
    levelSlider.setValue(snappedVal);
}

void GuiComponentLevel::setupComponent(juce::Component& parent, int& tabOrder, const juce::String& prefix) {
    // 出力レベル
    levelSlider.setupComponent(
        parent,
        prefix + CPK::level,
        "LV",
        tabOrder,
        [this]() {
            applyStepSnap();
        }
        );

    stepSelector.setup({ .parent = parent, .id = "", .title = "Steps", .items = stepItems, .isReset = false });
    stepSelector.setSelectedItemIndex(0, juce::dontSendNotification); // デフォルトはFree
    stepSelector.setWantsKeyboardFocus(true);
    stepSelector.setExplicitFocusOrder(++tabOrder);
    stepSelector.onChange = [this]() {
        applyStepSnap(); // 切り替えた瞬間に現在の値を丸める
        };

    levelPM1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-1.0", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false});
    levelPM1.setWantsKeyboardFocus(true);
    levelPM1.setExplicitFocusOrder(++tabOrder);
    levelPM1.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() - 1.0f);
        };

    levelPM01.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-0.1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    levelPM01.setWantsKeyboardFocus(true);
    levelPM01.setExplicitFocusOrder(++tabOrder);
    levelPM01.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() - 0.1f);
        };

    levelPM001.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "-0.01", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    levelPM001.setWantsKeyboardFocus(true);
    levelPM001.setExplicitFocusOrder(++tabOrder);
    levelPM001.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() - 0.01f);
        };

    levelTo1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "1.0", .bgColor = juce::Colours::yellow.brighter(0.5f), .isReset = false });
    levelTo1.setWantsKeyboardFocus(true);
    levelTo1.setExplicitFocusOrder(++tabOrder);
    levelTo1.onClick = [this]() {
        levelSlider.setValue(1.0f);
        };

    levelP001.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+0.01", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    levelP001.setWantsKeyboardFocus(true);
    levelP001.setExplicitFocusOrder(++tabOrder);
    levelP001.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() + 0.01f);
        };

    levelP01.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+0.1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    levelP01.setWantsKeyboardFocus(true);
    levelP01.setExplicitFocusOrder(++tabOrder);
    levelP01.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() + 0.1f);
        };

    levelP1.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "+1.0", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    levelP1.setWantsKeyboardFocus(true);
    levelP1.setExplicitFocusOrder(++tabOrder);
    levelP1.onClick = [this]() {
        levelSlider.setValue(levelSlider.getValue() + 1.0f);
        };

    levelTo025.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.25", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    levelTo025.setWantsKeyboardFocus(true);
    levelTo025.setExplicitFocusOrder(++tabOrder);
    levelTo025.onClick = [this]() {
        levelSlider.setValue(0.25f);
        };

    levelTo05.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.5", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    levelTo05.setWantsKeyboardFocus(true);
    levelTo05.setExplicitFocusOrder(++tabOrder);
    levelTo05.onClick = [this]() {
        levelSlider.setValue(0.5f);
        };

    levelTo075.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.75", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    levelTo075.setWantsKeyboardFocus(true);
    levelTo075.setExplicitFocusOrder(++tabOrder);
    levelTo075.onClick = [this]() {
        levelSlider.setValue(0.75f);
        };

    levelTo02.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.2", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    levelTo02.setWantsKeyboardFocus(true);
    levelTo02.setExplicitFocusOrder(++tabOrder);
    levelTo02.onClick = [this]() {
        levelSlider.setValue(0.2f);
        };

    levelTo04.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.4", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    levelTo04.setWantsKeyboardFocus(true);
    levelTo04.setExplicitFocusOrder(++tabOrder);
    levelTo04.onClick = [this]() {
        levelSlider.setValue(0.4f);
        };

    levelTo06.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.6", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    levelTo06.setWantsKeyboardFocus(true);
    levelTo06.setExplicitFocusOrder(++tabOrder);
    levelTo06.onClick = [this]() {
        levelSlider.setValue(0.6f);
        };

    levelTo08.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.8", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    levelTo08.setWantsKeyboardFocus(true);
    levelTo08.setExplicitFocusOrder(++tabOrder);
    levelTo08.onClick = [this]() {
        levelSlider.setValue(0.8f);
        };

    levelTo0125.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.125", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo0125.setWantsKeyboardFocus(true);
    levelTo0125.setExplicitFocusOrder(++tabOrder);
    levelTo0125.onClick = [this]() {
        levelSlider.setValue(0.125f);
        };

    levelTo0142.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.142", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo0142.setWantsKeyboardFocus(true);
    levelTo0142.setExplicitFocusOrder(++tabOrder);
    levelTo0142.onClick = [this]() {
        levelSlider.setValue(0.142f);
        };

    levelTo016.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.16", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo016.setWantsKeyboardFocus(true);
    levelTo016.setExplicitFocusOrder(++tabOrder);
    levelTo016.onClick = [this]() {
        levelSlider.setValue(0.16f);
        };

    levelTo033.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.33", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo033.setWantsKeyboardFocus(true);
    levelTo033.setExplicitFocusOrder(++tabOrder);
    levelTo033.onClick = [this]() {
        levelSlider.setValue(0.33f);
        };

    levelTo067.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.67", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo067.setWantsKeyboardFocus(true);
    levelTo067.setExplicitFocusOrder(++tabOrder);
    levelTo067.onClick = [this]() {
        levelSlider.setValue(0.67f);
        };

    levelTo083.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = "0.83", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    levelTo083.setWantsKeyboardFocus(true);
    levelTo083.setExplicitFocusOrder(++tabOrder);
    levelTo083.onClick = [this]() {
        levelSlider.setValue(0.83f);
        };
}

void GuiComponentLevel::layoutComponent(juce::Rectangle<int>& rect) {
    setVisible(true);

    levelSlider.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .label = &stepSelector.label, .component = &stepSelector });

    bool isVisibleNudge = levelSlider.isVisibleNudge();

    if (isVisibleNudge) {
        layoutMainThreeComps({ .rect = rect, .comp1 = &levelPM1, .comp2 = &levelTo1, .comp3 = &levelP1 });
        layoutMainThreeComps({ .rect = rect, .comp1 = &levelTo025, .comp2 = &levelTo05, .comp3 = &levelTo075 });
        layoutMainFourComps({ .rect = rect, .comp1 = &levelTo02, .comp2 = &levelTo04, .comp3 = &levelTo06, .comp4 = &levelTo08 });
        layoutMainSixComps({ .rect = rect, .comp1 = &levelTo0125, .comp2 = &levelTo0142, .comp3 = &levelTo016, .comp4 = &levelTo033, .comp5 = &levelTo067, .comp6 = &levelTo083 });
        layoutMainFourComps({ .rect = rect, .comp1 = &levelPM001, .comp2 = &levelPM01, .comp3 = &levelP01, .comp4 = &levelP001 });
    }

    rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}

void GuiComponentLevel::layoutComponentRow(juce::Rectangle<int>& rect) {
    setVisible(true);

    levelSlider.layoutComponentRow(rect);

    layoutRow({ .rowRect = rect, .label = &stepSelector.label, .component = &stepSelector });

    bool isVisibleNudge = levelSlider.isVisibleNudge();

    if (isVisibleNudge) {
        layoutRowThreeComps({ .rect = rect, .comp1 = &levelPM1, .comp2 = &levelTo1, .comp3 = &levelP1 });
        layoutRowThreeComps({ .rect = rect, .comp1 = &levelTo025, .comp2 = &levelTo05, .comp3 = &levelTo075 });
        layoutRowFourComps({ .rect = rect, .comp1 = &levelTo02, .comp2 = &levelTo04, .comp3 = &levelTo06, .comp4 = &levelTo08 });
        layoutRowSixComps({ .rect = rect, .comp1 = &levelTo0125, .comp2 = &levelTo0142, .comp3 = &levelTo016, .comp4 = &levelTo033, .comp5 = &levelTo067, .comp6 = &levelTo083 });
        layoutRowFourComps({ .rect = rect, .comp1 = &levelPM001, .comp2 = &levelPM01, .comp3 = &levelP01, .comp4 = &levelP001 });
    }

    rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}

void GuiComponentLevel::setVisible(bool visible) {
    levelSlider.setVisibles(visible);
    stepSelector.setVisibleWithLabel(visible);

    bool isVisibleNudge = levelSlider.isVisibleNudge();

    levelPM1.setVisible(visible && isVisibleNudge);
    levelPM01.setVisible(visible && isVisibleNudge);
    levelPM001.setVisible(visible && isVisibleNudge);
    levelTo1.setVisible(visible && isVisibleNudge);
    levelP001.setVisible(visible && isVisibleNudge);
    levelP01.setVisible(visible && isVisibleNudge);
    levelP1.setVisible(visible && isVisibleNudge);
    levelTo025.setVisible(visible && isVisibleNudge);
    levelTo05.setVisible(visible && isVisibleNudge);
    levelTo075.setVisible(visible && isVisibleNudge);
    levelTo02.setVisible(visible && isVisibleNudge);
    levelTo04.setVisible(visible && isVisibleNudge);
    levelTo06.setVisible(visible && isVisibleNudge);
    levelTo08.setVisible(visible && isVisibleNudge);
	levelTo0125.setVisible(visible && isVisibleNudge);
	levelTo0142.setVisible(visible && isVisibleNudge);
    levelTo016.setVisible(visible && isVisibleNudge);
    levelTo033.setVisible(visible && isVisibleNudge);
	levelTo067.setVisible(visible && isVisibleNudge);
    levelTo083.setVisible(visible && isVisibleNudge);
}

void GuiComponentLevel::setEnable(bool enabled) {
    levelSlider.setEnabled(enabled);
    stepSelector.setEnabledWithLabel(enabled);
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
	levelSlider.setValue(level);
    applyStepSnap();
}

void GuiComponentLevel::setImportingParams(juce::StringArray& lines, int& index) {
    levelSlider.setValue(lines[index++].getFloatValue());
}

void GuiComponentLevel::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    levelSlider.setValue(r.getFloat("level", (float)levelSlider.getValue()));
}

juce::String GuiComponentLevel::getExportedParams() {
    juce::String content = "";

    content += juce::String(levelSlider.getValue(), Global::floatDecimalPlaces) + "\n";

    return content;
}

void GuiComponentLevel::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    w.set("level", (float)levelSlider.getValue());
}
