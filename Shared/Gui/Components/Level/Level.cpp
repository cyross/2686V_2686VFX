#include "./Level.h"

#include <cmath>

#include "../../../Core/Io/ParamFile.h"

#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Const/ConstGlobal.h"

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

    // 既に丸まっているなら書き戻さない。書き戻すと知らせがもう一巡して、
    // 1 回の変更で画面の作り直しが二重に走る。
    if (std::abs((float)levelSlider.getValue() - snappedVal) < 1.0e-6f) return;

    // ここは知らせを止められない。止めると、丸めた値がパラメータへ
    // 伝わらず、画面の見た目と音がずれる。
    levelSlider.setValue(snappedVal, juce::sendNotification);
}

void GuiComponentLevel::setupComponent(juce::Component& parent, int& tabOrder, const juce::String& prefix, bool withDelay) {
    m_withDelay = withDelay;

    // 出力レベル
    levelSlider.setupComponent(
        parent,
        prefix + CPK::level,
        "LEVEL",
        tabOrder,
        [this]() {
            applyStepSnap();
        }
        );

    if (m_withDelay) {
        delaySeparator.setupComponent(parent);

        delaySlider.setupComponent(parent, prefix + CPK::delay, "DELAY", tabOrder, std::nullopt);

        delayNudge.setupComponent(parent, delaySlider.getSlider(), tabOrder);
    }

    stepSelector.setup({ .parent = parent, .id = "", .title = "STEPS", .items = stepItems, .isReset = false });
    stepSelector.setSelectedItemIndex(0, juce::dontSendNotification); // デフォルトはFree
    stepSelector.setWantsKeyboardFocus(true);
    stepSelector.setExplicitFocusOrder(++tabOrder);
    stepSelector.onChange = [this]() {
        applyStepSnap(); // 切り替えた瞬間に現在の値を丸める
        };

    // N ボタンで出す、値を動かすボタンの並び
    levelNudge.setupComponent(parent, levelSlider.getSlider(), tabOrder);
}

void GuiComponentLevel::layoutComponent(juce::Rectangle<int>& rect) {
    setVisible(true);

    levelSlider.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .label = &stepSelector.label, .component = &stepSelector });

    bool isVisibleNudge = levelSlider.isVisibleNudge();

    if (isVisibleNudge) {
        levelNudge.layoutComponent(rect);
    }

    if (m_withDelay) {
        delaySeparator.layoutComponent(rect);

        delaySlider.layoutComponent(rect);

        if (delaySlider.isVisibleNudge()) delayNudge.layoutComponent(rect);
    }

    rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}

void GuiComponentLevel::layoutComponentRow(juce::Rectangle<int>& rect) {
    setVisible(true);

    levelSlider.layoutComponentRow(rect);

    layoutRow({ .rowRect = rect, .label = &stepSelector.label, .component = &stepSelector });

    bool isVisibleNudge = levelSlider.isVisibleNudge();

    if (isVisibleNudge) {
        levelNudge.layoutComponentRow(rect);
    }

    if (m_withDelay) {
        delaySeparator.layoutComponent(rect);

        delaySlider.layoutComponentRow(rect);

        if (delaySlider.isVisibleNudge()) delayNudge.layoutComponentRow(rect);
    }

    rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}

void GuiComponentLevel::setVisible(bool visible) {
    levelSlider.setVisibles(visible);
    stepSelector.setVisibleWithLabel(visible);

    if (m_withDelay) {
        delaySeparator.setVisible(visible);
        delaySlider.setVisibles(visible);
        delayNudge.setVisibles(visible && delaySlider.isVisibleNudge());
    }

    bool isVisibleNudge = levelSlider.isVisibleNudge();

    levelNudge.setVisibles(visible && isVisibleNudge);
}

void GuiComponentLevel::setEnable(bool enabled) {
    levelSlider.setEnabled(enabled);
    stepSelector.setEnabledWithLabel(enabled);

    if (m_withDelay) {
        delaySeparator.setEnabled(enabled);
        delaySlider.setEnabled(enabled);
        delayNudge.setEnables(enabled);
    }
    levelNudge.setEnables(enabled);
}

float GuiComponentLevel::getLevel() {
	return levelSlider.getValue();
}

void GuiComponentLevel::setLevel(float level) {
    // 知らせ方を決めて渡す。既定は後回しにする形なので、まとめの外で
    // 描き直しが走ってしまい、まとめた意味が無くなる。
    levelSlider.setValue(level, juce::sendNotification);

    applyStepSnap();
}

void GuiComponentLevel::setImportingParams(juce::StringArray& lines, int& index) {
    levelSlider.setValue(lines[index++].getFloatValue());
}

void GuiComponentLevel::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    levelSlider.setValue(r.getFloat("level", (float)levelSlider.getValue()));
    if (m_withDelay) delaySlider.setValue(r.getFloat("delay", (float)delaySlider.getValue()));
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
    if (m_withDelay) w.set("delay", (float)delaySlider.getValue());
}
