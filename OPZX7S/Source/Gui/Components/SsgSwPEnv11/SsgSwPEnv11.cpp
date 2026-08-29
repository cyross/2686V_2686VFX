#include "./SsgSwPEnv11.h"

#include "../../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat ssgSwPEnv11Format{ "ssgSwPEnv11", 1 };
}

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

void GuiComponentSsgSwPEnv11::applyLoopValues(bool enabled)
{
    // setValue は同期で onValueChange を呼び返すので、ここから値を
    // 直すと自分自身が入れ子で呼ばれる。ドラッグ中はマウスと値の
    // 押し合いになり、操作を受け付けなくなったように見える。
    // 外側の 1 回で辻褄は合うので、入れ子ぶんは弾く。
    if (isApplyingLoopValues) return;

    const juce::ScopedValueSetter<bool> guard(isApplyingLoopValues, true);

    if (enabled)
    {
        int stepsValue = static_cast<int>(steps.getValue());

        // Steps が 1 のときはループできないため、Steps を 2 にする
        if (stepsValue < 2) {
            stepsValue = 2;
            steps.setValue(stepsValue);
        }

        int loopToValue = static_cast<int>(loopTo.getValue());

        // Steps - LoopTo が 2未満のときは、LoopTo を Steps - 2 にする
        if (stepsValue - loopToValue < 2) {
            loopTo.setValue(stepsValue - 2);
        }
    }
}

void GuiComponentSsgSwPEnv11::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable)
{
    this->isEnable = isEnable;

    cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "SSG SW PITCH ENV[11]",
        .enableChangeDetailVisible = true
        });

    flag.setup({ .parent = parent, .id = code + flagKey, .title = flagText, .isReset = true });
    flag.setWantsKeyboardFocus(true);
    flag.setExplicitFocusOrder(++tabOrder);

    flagSeparator.setupComponent(parent);

    steps.setup({ .parent = parent, .id = code + CPK::SsgSwPEnv11::steps, .title = "STEP", .isReset = true, .labelFont = labelFont });
    steps.setWantsKeyboardFocus(true);
    steps.setExplicitFocusOrder(++tabOrder);
    steps.onValueChange = [this] {
        bool ssgEnvLoopEnable = loop.getToggleState();

        applyLoopValues(ssgEnvLoopEnable);
        };

    stepsSeparator.setupComponent(parent);

    loop.setup({ .parent = parent, .id = code + CPK::SsgSwPEnv11::loop, .title = "LOOP", .isReset = true });
    loop.setWantsKeyboardFocus(true);
    loop.setExplicitFocusOrder(++tabOrder);
    loop.onClick = [this] {
        bool ssgEnvLoopEnable = loop.getToggleState();

        loopTo.setEnabled(ssgEnvLoopEnable);
        loopTo.label.setEnabled(ssgEnvLoopEnable);
        loopCount.setEnabled(ssgEnvLoopEnable);
        loopCount.label.setEnabled(ssgEnvLoopEnable);

        applyLoopValues(ssgEnvLoopEnable);
        };

    loopTo.setup({ .parent = parent, .id = code + CPK::SsgSwPEnv11::loopTo, .title = "L.TO", .isReset = true, .labelFont = labelFont });
    loopTo.setWantsKeyboardFocus(true);
    loopTo.setExplicitFocusOrder(++tabOrder);
    loopTo.onValueChange = [this] {
        bool ssgEnvLoopEnable = loop.getToggleState();

        applyLoopValues(ssgEnvLoopEnable);
        };

    loopCount.setup({ .parent = parent, .id = code + CPK::SsgSwPEnv11::loopCount, .title = "L.CN", .isReset = true, .labelFont = labelFont });
    loopCount.setWantsKeyboardFocus(true);
    loopCount.setExplicitFocusOrder(++tabOrder);

    bool ssgEnvLoopEnable = loop.getToggleState();

    loopTo.setEnabled(ssgEnvLoopEnable);
    loopTo.label.setEnabled(ssgEnvLoopEnable);
    loopCount.setEnabled(ssgEnvLoopEnable);
    loopCount.label.setEnabled(ssgEnvLoopEnable);

    loopSeparator.setupComponent(parent);

    r1.setupComponent(parent, code + CPK::SsgSwPEnv11::r1, "R1", tabOrder, std::nullopt, labelFont);

    r1Nudge.setupComponent(parent, r1.getSlider(), tabOrder);

    r2.setupComponent(parent, code + CPK::SsgSwPEnv11::r2, "R2", tabOrder, std::nullopt, labelFont);

    r2Nudge.setupComponent(parent, r2.getSlider(), tabOrder);

    r3.setupComponent(parent, code + CPK::SsgSwPEnv11::r3, "R3", tabOrder, std::nullopt, labelFont);

    r3Nudge.setupComponent(parent, r3.getSlider(), tabOrder);

    r4.setupComponent(parent, code + CPK::SsgSwPEnv11::r4, "R4", tabOrder, std::nullopt, labelFont);

    r4Nudge.setupComponent(parent, r4.getSlider(), tabOrder);

    r5.setupComponent(parent, code + CPK::SsgSwPEnv11::r5, "R5", tabOrder, std::nullopt, labelFont);

    r5Nudge.setupComponent(parent, r5.getSlider(), tabOrder);

    r6.setupComponent(parent, code + CPK::SsgSwPEnv11::r6, "R6", tabOrder, std::nullopt, labelFont);

    r6Nudge.setupComponent(parent, r6.getSlider(), tabOrder);

    r7.setupComponent(parent, code + CPK::SsgSwPEnv11::r7, "R7", tabOrder, std::nullopt, labelFont);

    r7Nudge.setupComponent(parent, r7.getSlider(), tabOrder);

    r8.setupComponent(parent, code + CPK::SsgSwPEnv11::r8, "R8", tabOrder, std::nullopt, labelFont);

    r8Nudge.setupComponent(parent, r8.getSlider(), tabOrder);

    r9.setupComponent(parent, code + CPK::SsgSwPEnv11::r9, "R9", tabOrder, std::nullopt, labelFont);

    r9Nudge.setupComponent(parent, r9.getSlider(), tabOrder);

    r10.setupComponent(parent, code + CPK::SsgSwPEnv11::r10, "R10", tabOrder, std::nullopt, labelFont);

    r10Nudge.setupComponent(parent, r10.getSlider(), tabOrder);

    r11.setupComponent(parent, code + CPK::SsgSwPEnv11::r11, "R11", tabOrder, std::nullopt, labelFont);

    r11Nudge.setupComponent(parent, r11.getSlider(), tabOrder);

    rateSeparator.setupComponent(parent);

    startLevel.setupComponent(parent, code + CPK::SsgSwPEnv11::stl, "STL", tabOrder, std::nullopt, labelFont);

    startLevelButtons.setupComponent(parent, startLevel.getSlider(), tabOrder, labelFont);

    l1.setupComponent(parent, code + CPK::SsgSwPEnv11::l1, "L1", tabOrder, std::nullopt, labelFont);

    l1Buttons.setupComponent(parent, l1.getSlider(), tabOrder, labelFont);

    l2.setupComponent(parent, code + CPK::SsgSwPEnv11::l2, "L2", tabOrder, std::nullopt, labelFont);

    l2Buttons.setupComponent(parent, l2.getSlider(), tabOrder, labelFont);

    l3.setupComponent(parent, code + CPK::SsgSwPEnv11::l3, "L3", tabOrder, std::nullopt, labelFont);

    l3Buttons.setupComponent(parent, l3.getSlider(), tabOrder, labelFont);

    l4.setupComponent(parent, code + CPK::SsgSwPEnv11::l4, "L4", tabOrder, std::nullopt, labelFont);

    l4Buttons.setupComponent(parent, l4.getSlider(), tabOrder, labelFont);

    l5.setupComponent(parent, code + CPK::SsgSwPEnv11::l5, "L5", tabOrder, std::nullopt, labelFont);

    l5Buttons.setupComponent(parent, l5.getSlider(), tabOrder, labelFont);

    l6.setupComponent(parent, code + CPK::SsgSwPEnv11::l6, "L6", tabOrder, std::nullopt, labelFont);

    l6Buttons.setupComponent(parent, l6.getSlider(), tabOrder, labelFont);

    l7.setupComponent(parent, code + CPK::SsgSwPEnv11::l7, "L7", tabOrder, std::nullopt, labelFont);

    l7Buttons.setupComponent(parent, l7.getSlider(), tabOrder, labelFont);

    l8.setupComponent(parent, code + CPK::SsgSwPEnv11::l8, "L8", tabOrder, std::nullopt, labelFont);

    l8Buttons.setupComponent(parent, l8.getSlider(), tabOrder, labelFont);

    l9.setupComponent(parent, code + CPK::SsgSwPEnv11::l9, "L9", tabOrder, std::nullopt, labelFont);

    l9Buttons.setupComponent(parent, l9.getSlider(), tabOrder, labelFont);

    l10.setupComponent(parent, code + CPK::SsgSwPEnv11::l10, "L10", tabOrder, std::nullopt, labelFont);

    l10Buttons.setupComponent(parent, l10.getSlider(), tabOrder, labelFont);

    l11.setupComponent(parent, code + CPK::SsgSwPEnv11::l11, "L11", tabOrder, std::nullopt, labelFont);

    l11Buttons.setupComponent(parent, l11.getSlider(), tabOrder, labelFont);
}

void GuiComponentSsgSwPEnv11::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    flag.setVisible(visible);
	flagSeparator.setVisible(visible);
    steps.setVisibleWithLabel(visible);
	stepsSeparator.setVisible(visible);
    loop.setVisible(visible);
    loopTo.setVisibleWithLabel(visible);
    loopCount.setVisibleWithLabel(visible);
	loopSeparator.setVisible(visible);
    r1.setVisibleWithLabel(visible);
    r1Nudge.setVisibles(visible && r1.isVisibleNudge());
    r2.setVisibleWithLabel(visible);
    r2Nudge.setVisibles(visible && r2.isVisibleNudge());
    r3.setVisibleWithLabel(visible);
    r3Nudge.setVisibles(visible && r3.isVisibleNudge());
    r4.setVisibleWithLabel(visible);
    r4Nudge.setVisibles(visible && r4.isVisibleNudge());
    r5.setVisibleWithLabel(visible);
    r5Nudge.setVisibles(visible && r5.isVisibleNudge());
    r6.setVisibleWithLabel(visible);
    r6Nudge.setVisibles(visible && r6.isVisibleNudge());
    r7.setVisibleWithLabel(visible);
    r7Nudge.setVisibles(visible && r7.isVisibleNudge());
    r8.setVisibleWithLabel(visible);
    r8Nudge.setVisibles(visible && r8.isVisibleNudge());
    r9.setVisibleWithLabel(visible);
    r9Nudge.setVisibles(visible && r9.isVisibleNudge());
    r10.setVisibleWithLabel(visible);
    r10Nudge.setVisibles(visible && r10.isVisibleNudge());
    r11.setVisibleWithLabel(visible);
    r11Nudge.setVisibles(visible && r11.isVisibleNudge());
    rateSeparator.setVisible(visible);
    startLevel.setVisibleWithLabel(visible);
	startLevelButtons.setVisibles(visible && startLevel.isVisibleNudge());
    l1.setVisibleWithLabel(visible);
	l1Buttons.setVisibles(visible && l1.isVisibleNudge());
    l2.setVisibleWithLabel(visible);
    l2Buttons.setVisibles(visible && l2.isVisibleNudge());
    l3.setVisibleWithLabel(visible);
    l3Buttons.setVisibles(visible && l3.isVisibleNudge());
    l4.setVisibleWithLabel(visible);
    l4Buttons.setVisibles(visible && l4.isVisibleNudge());
    l5.setVisibleWithLabel(visible);
    l5Buttons.setVisibles(visible && l5.isVisibleNudge());
    l6.setVisibleWithLabel(visible);
    l6Buttons.setVisibles(visible && l6.isVisibleNudge());
    l7.setVisibleWithLabel(visible);
    l7Buttons.setVisibles(visible && l7.isVisibleNudge());
    l8.setVisibleWithLabel(visible);
    l8Buttons.setVisibles(visible && l8.isVisibleNudge());
    l9.setVisibleWithLabel(visible);
    l9Buttons.setVisibles(visible && l9.isVisibleNudge());
    l10.setVisibleWithLabel(visible);
    l10Buttons.setVisibles(visible && l10.isVisibleNudge());
    l11.setVisibleWithLabel(visible);
    l11Buttons.setVisibles(visible && l11.isVisibleNudge());

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &flag });
        flagSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &steps.label, .component = &steps, .rowHeight = 13 });
        stepsSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &loop, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &loopTo.label, .component = &loopTo, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &loopCount.label, .component = &loopCount, .rowHeight = 13 });
        loopSeparator.layoutComponent(rect);
        r1.layoutComponent(rect, 13);
        if (r1.isVisibleNudge()) r1Nudge.layoutComponent(rect, 13);
        r2.layoutComponent(rect, 13);
        if (r2.isVisibleNudge()) r2Nudge.layoutComponent(rect, 13);
        r3.layoutComponent(rect, 13);
        if (r3.isVisibleNudge()) r3Nudge.layoutComponent(rect, 13);
        r4.layoutComponent(rect, 13);
        if (r4.isVisibleNudge()) r4Nudge.layoutComponent(rect, 13);
        r5.layoutComponent(rect, 13);
        if (r5.isVisibleNudge()) r5Nudge.layoutComponent(rect, 13);
        r6.layoutComponent(rect, 13);
        if (r6.isVisibleNudge()) r6Nudge.layoutComponent(rect, 13);
        r7.layoutComponent(rect, 13);
        if (r7.isVisibleNudge()) r7Nudge.layoutComponent(rect, 13);
        r8.layoutComponent(rect, 13);
        if (r8.isVisibleNudge()) r8Nudge.layoutComponent(rect, 13);
        r9.layoutComponent(rect, 13);
        if (r9.isVisibleNudge()) r9Nudge.layoutComponent(rect, 13);
        r10.layoutComponent(rect, 13);
        if (r10.isVisibleNudge()) r10Nudge.layoutComponent(rect, 13);
        r11.layoutComponent(rect, 13);
        if (r11.isVisibleNudge()) r11Nudge.layoutComponent(rect, 13);
        rateSeparator.layoutComponent(rect);
        startLevel.layoutComponent(rect, 13);
        if (startLevel.isVisibleNudge()) startLevelButtons.layoutComponent(rect, 13);
        l1.layoutComponent(rect, 13);
        if (l1.isVisibleNudge()) l1Buttons.layoutComponent(rect, 13);
        l2.layoutComponent(rect, 13);
        if (l2.isVisibleNudge()) l2Buttons.layoutComponent(rect, 13);
        l3.layoutComponent(rect, 13);
        if (l3.isVisibleNudge()) l3Buttons.layoutComponent(rect, 13);
        l4.layoutComponent(rect, 13);
        if (l4.isVisibleNudge()) l4Buttons.layoutComponent(rect, 13);
        l5.layoutComponent(rect, 13);
        if (l5.isVisibleNudge()) l5Buttons.layoutComponent(rect, 13);
        l6.layoutComponent(rect, 13);
        if (l6.isVisibleNudge()) l6Buttons.layoutComponent(rect, 13);
        l7.layoutComponent(rect, 13);
        if (l7.isVisibleNudge()) l7Buttons.layoutComponent(rect, 13);
        l8.layoutComponent(rect, 13);
        if (l8.isVisibleNudge()) l8Buttons.layoutComponent(rect, 13);
        l9.layoutComponent(rect, 13);
        if (l9.isVisibleNudge()) l9Buttons.layoutComponent(rect, 13);
        l10.layoutComponent(rect, 13);
        if (l10.isVisibleNudge()) l10Buttons.layoutComponent(rect, 13);
        l11.layoutComponent(rect, 13);
        if (l11.isVisibleNudge()) l11Buttons.layoutComponent(rect, 13);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiComponentSsgSwPEnv11::layoutComponentRow(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    flag.setVisible(visible);
    flagSeparator.setVisible(visible);
    steps.setVisibleWithLabel(visible);
    stepsSeparator.setVisible(visible);
    loop.setVisible(visible);
    loopTo.setVisibleWithLabel(visible);
    loopCount.setVisibleWithLabel(visible);
    loopSeparator.setVisible(visible);
    r1.setVisibleWithLabel(visible);
    r1Nudge.setVisibles(visible && r1.isVisibleNudge());
    r2.setVisibleWithLabel(visible);
    r2Nudge.setVisibles(visible && r2.isVisibleNudge());
    r3.setVisibleWithLabel(visible);
    r3Nudge.setVisibles(visible && r3.isVisibleNudge());
    r4.setVisibleWithLabel(visible);
    r4Nudge.setVisibles(visible && r4.isVisibleNudge());
    r5.setVisibleWithLabel(visible);
    r5Nudge.setVisibles(visible && r5.isVisibleNudge());
    r6.setVisibleWithLabel(visible);
    r6Nudge.setVisibles(visible && r6.isVisibleNudge());
    r7.setVisibleWithLabel(visible);
    r7Nudge.setVisibles(visible && r7.isVisibleNudge());
    r8.setVisibleWithLabel(visible);
    r8Nudge.setVisibles(visible && r8.isVisibleNudge());
    r9.setVisibleWithLabel(visible);
    r9Nudge.setVisibles(visible && r9.isVisibleNudge());
    r10.setVisibleWithLabel(visible);
    r10Nudge.setVisibles(visible && r10.isVisibleNudge());
    r11.setVisibleWithLabel(visible);
    r11Nudge.setVisibles(visible && r11.isVisibleNudge());
    rateSeparator.setVisible(visible);
    startLevel.setVisibleWithLabel(visible);
    startLevelButtons.setVisibles(visible && startLevel.isVisibleNudge());
    l1.setVisibleWithLabel(visible);
    l1Buttons.setVisibles(visible && l1.isVisibleNudge());
    l2.setVisibleWithLabel(visible);
    l2Buttons.setVisibles(visible && l2.isVisibleNudge());
    l3.setVisibleWithLabel(visible);
    l3Buttons.setVisibles(visible && l3.isVisibleNudge());
    l4.setVisibleWithLabel(visible);
    l4Buttons.setVisibles(visible && l4.isVisibleNudge());
    l5.setVisibleWithLabel(visible);
    l5Buttons.setVisibles(visible && l5.isVisibleNudge());
    l6.setVisibleWithLabel(visible);
    l6Buttons.setVisibles(visible && l6.isVisibleNudge());
    l7.setVisibleWithLabel(visible);
    l7Buttons.setVisibles(visible && l7.isVisibleNudge());
    l8.setVisibleWithLabel(visible);
    l8Buttons.setVisibles(visible && l8.isVisibleNudge());
    l9.setVisibleWithLabel(visible);
    l9Buttons.setVisibles(visible && l9.isVisibleNudge());
    l10.setVisibleWithLabel(visible);
    l10Buttons.setVisibles(visible && l10.isVisibleNudge());
    l11.setVisibleWithLabel(visible);
    l11Buttons.setVisibles(visible && l11.isVisibleNudge());

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &flag });
        flagSeparator.layoutComponent(rect);
        layoutRow({ .rowRect = rect, .label = &steps.label, .component = &steps, .rowHeight = 12 });
        stepsSeparator.layoutComponent(rect);
        layoutRow({ .rowRect = rect, .component = &loop, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &loopTo.label, .component = &loopTo, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &loopCount.label, .component = &loopCount, .rowHeight = 12 });
        loopSeparator.layoutComponent(rect);
        r1.layoutComponentRow(rect, 12);
        if (r1.isVisibleNudge()) r1Nudge.layoutComponentRow(rect, 12);
        r2.layoutComponentRow(rect, 12);
        if (r2.isVisibleNudge()) r2Nudge.layoutComponentRow(rect, 12);
        r3.layoutComponentRow(rect, 12);
        if (r3.isVisibleNudge()) r3Nudge.layoutComponentRow(rect, 12);
        r4.layoutComponentRow(rect, 12);
        if (r4.isVisibleNudge()) r4Nudge.layoutComponentRow(rect, 12);
        r5.layoutComponentRow(rect, 12);
        if (r5.isVisibleNudge()) r5Nudge.layoutComponentRow(rect, 12);
        r6.layoutComponentRow(rect, 12);
        if (r6.isVisibleNudge()) r6Nudge.layoutComponentRow(rect, 12);
        r7.layoutComponentRow(rect, 12);
        if (r7.isVisibleNudge()) r7Nudge.layoutComponentRow(rect, 12);
        r8.layoutComponentRow(rect, 12);
        if (r8.isVisibleNudge()) r8Nudge.layoutComponentRow(rect, 12);
        r9.layoutComponentRow(rect, 12);
        if (r9.isVisibleNudge()) r9Nudge.layoutComponentRow(rect, 12);
        r10.layoutComponentRow(rect, 12);
        if (r10.isVisibleNudge()) r10Nudge.layoutComponentRow(rect, 12);
        r11.layoutComponentRow(rect, 12);
        if (r11.isVisibleNudge()) r11Nudge.layoutComponentRow(rect, 12);
        rateSeparator.layoutComponent(rect);
        startLevel.layoutComponentRow(rect, 12);
        if (startLevel.isVisibleNudge()) startLevelButtons.layoutComponentRow(rect, 12);
        l1.layoutComponentRow(rect, 12);
        if (l1.isVisibleNudge()) l1Buttons.layoutComponentRow(rect, 12);
        l2.layoutComponentRow(rect, 12);
        if (l2.isVisibleNudge()) l2Buttons.layoutComponentRow(rect, 12);
        l3.layoutComponentRow(rect, 12);
        if (l3.isVisibleNudge()) l3Buttons.layoutComponentRow(rect, 12);
        l4.layoutComponentRow(rect, 12);
        if (l4.isVisibleNudge()) l4Buttons.layoutComponentRow(rect, 12);
        l5.layoutComponentRow(rect, 12);
        if (l5.isVisibleNudge()) l5Buttons.layoutComponentRow(rect, 12);
        l6.layoutComponentRow(rect, 12);
        if (l6.isVisibleNudge()) l6Buttons.layoutComponentRow(rect, 12);
        l7.layoutComponentRow(rect, 12);
        if (l7.isVisibleNudge()) l7Buttons.layoutComponentRow(rect, 12);
        l8.layoutComponentRow(rect, 12);
        if (l8.isVisibleNudge()) l8Buttons.layoutComponentRow(rect, 12);
        l9.layoutComponentRow(rect, 12);
        if (l9.isVisibleNudge()) l9Buttons.layoutComponentRow(rect, 12);
        l10.layoutComponentRow(rect, 12);
        if (l10.isVisibleNudge()) l10Buttons.layoutComponentRow(rect, 12);
        l11.layoutComponentRow(rect, 12);
        if (l11.isVisibleNudge()) l11Buttons.layoutComponentRow(rect, 12);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiComponentSsgSwPEnv11::setupGraph(std::function<void()> repaintGraph) {

    flag.onStateChange = repaintGraph;
    loop.onStateChange = repaintGraph;

    steps.onValueChange = [this, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = loop.getToggleState();
        applyLoopValues(ssgEnvLoopEnable);
        repaintGraph();
        };
    loopTo.onValueChange = [this, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = loop.getToggleState();
        applyLoopValues(ssgEnvLoopEnable);
        repaintGraph();
        };
    loopCount.onValueChange = [this, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = loop.getToggleState();
        applyLoopValues(ssgEnvLoopEnable);
        repaintGraph();
        };

    r1.getSlider().onValueChange = repaintGraph;
    r2.getSlider().onValueChange = repaintGraph;
    r3.getSlider().onValueChange = repaintGraph;
    r4.getSlider().onValueChange = repaintGraph;
    r5.getSlider().onValueChange = repaintGraph;
    r6.getSlider().onValueChange = repaintGraph;
    r7.getSlider().onValueChange = repaintGraph;
    r8.getSlider().onValueChange = repaintGraph;
    r9.getSlider().onValueChange = repaintGraph;
    r10.getSlider().onValueChange = repaintGraph;
    r11.getSlider().onValueChange = repaintGraph;

    startLevel.getSlider().onValueChange = repaintGraph;
    l1.getSlider().onValueChange = repaintGraph;
    l2.getSlider().onValueChange = repaintGraph;
    l3.getSlider().onValueChange = repaintGraph;
    l4.getSlider().onValueChange = repaintGraph;
    l5.getSlider().onValueChange = repaintGraph;
    l6.getSlider().onValueChange = repaintGraph;
    l7.getSlider().onValueChange = repaintGraph;
    l8.getSlider().onValueChange = repaintGraph;
    l9.getSlider().onValueChange = repaintGraph;
    l10.getSlider().onValueChange = repaintGraph;
    l11.getSlider().onValueChange = repaintGraph;
}

void GuiComponentSsgSwPEnv11::updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx) {
    graph.updateBypass(this->isEnable ? !flag.getToggleState() : flag.getToggleState());

    graph.updateSsgSwPEnv11(
        steps,
        loop,
        loopTo,
        loopCount,
        { nullptr, &r1.getSlider(), &r2.getSlider(), &r3.getSlider(), &r4.getSlider(), &r5.getSlider(), &r6.getSlider(), &r7.getSlider(), &r8.getSlider(), &r9.getSlider(), &r10.getSlider(), &r11.getSlider() },
        { &startLevel.getSlider(), &l1.getSlider(), &l2.getSlider(), &l3.getSlider(), &l4.getSlider(), &l5.getSlider(), &l6.getSlider(), &l7.getSlider(), &l8.getSlider(), &l9.getSlider(), &l10.getSlider(), &l11.getSlider() },
        p_curveCore,
        isCurveMode,
        posIdx
    );
}

void GuiComponentSsgSwPEnv11::setEnabled(bool enabled) {
    bool ssgEnvLoopEnable = loop.getToggleState();

    cat.setEnabled(enabled);
    flag.setEnabled(enabled);
	flagSeparator.setEnabled(enabled);
    steps.setEnabled(enabled);
	stepsSeparator.setEnabled(enabled);
    loop.setEnabled(enabled);
    loopTo.setEnabled(enabled && ssgEnvLoopEnable);
    loopCount.setEnabled(enabled && ssgEnvLoopEnable);
	loopSeparator.setEnabled(enabled);
	rateSeparator.setEnabled(enabled);
    startLevel.setEnabled(enabled);
    startLevelButtons.setEnables(enabled);
    r1.setEnabledWithLabel(enabled);
    r1Nudge.setEnables(enabled);
    l1.setEnabledWithLabel(enabled);
    l1Buttons.setEnables(enabled);
    r2.setEnabledWithLabel(enabled);
    r2Nudge.setEnables(enabled);
    l2.setEnabledWithLabel(enabled);
    l2Buttons.setEnables(enabled);
    r3.setEnabledWithLabel(enabled);
    r3Nudge.setEnables(enabled);
    l3.setEnabledWithLabel(enabled);
    l3Buttons.setEnables(enabled);
    r4.setEnabledWithLabel(enabled);
    r4Nudge.setEnables(enabled);
    l4.setEnabledWithLabel(enabled);
    l4Buttons.setEnables(enabled);
    r5.setEnabledWithLabel(enabled);
    r5Nudge.setEnables(enabled);
    l5.setEnabledWithLabel(enabled);
    l5Buttons.setEnables(enabled);
    r6.setEnabledWithLabel(enabled);
    r6Nudge.setEnables(enabled);
    l6.setEnabledWithLabel(enabled);
    l6Buttons.setEnables(enabled);
    r7.setEnabledWithLabel(enabled);
    r7Nudge.setEnables(enabled);
    l7.setEnabledWithLabel(enabled);
    l7Buttons.setEnables(enabled);
    r8.setEnabledWithLabel(enabled);
    r8Nudge.setEnables(enabled);
    l8.setEnabledWithLabel(enabled);
    l8Buttons.setEnables(enabled);
    r9.setEnabledWithLabel(enabled);
    r9Nudge.setEnables(enabled);
    l9.setEnabledWithLabel(enabled);
    l9Buttons.setEnables(enabled);
    r10.setEnabledWithLabel(enabled);
    r10Nudge.setEnables(enabled);
    l10.setEnabledWithLabel(enabled);
    l10Buttons.setEnables(enabled);
    r11.setEnabledWithLabel(enabled);
    r11Nudge.setEnables(enabled);
    l11.setEnabledWithLabel(enabled);
    l11Buttons.setEnables(enabled);
}

void GuiComponentSsgSwPEnv11::copyParams(CopyPEnvSsgSw11& copyObj) {
    copyObj.flag = flag.getToggleState();
    copyObj.steps = steps.getValue();
    copyObj.loop = loop.getToggleState();
    copyObj.loopTo = loopTo.getValue();
    copyObj.loopCount = loopCount.getValue();
    copyObj.stl = startLevel.getValue();
    copyObj.r[0] = r1.getValue();
    copyObj.l[0] = l1.getValue();
    copyObj.r[1] = r2.getValue();
    copyObj.l[1] = l2.getValue();
    copyObj.r[2] = r3.getValue();
    copyObj.l[2] = l3.getValue();
    copyObj.r[3] = r4.getValue();
    copyObj.l[3] = l4.getValue();
    copyObj.r[4] = r5.getValue();
    copyObj.l[4] = l5.getValue();
    copyObj.r[5] = r6.getValue();
    copyObj.l[5] = l6.getValue();
    copyObj.r[6] = r7.getValue();
    copyObj.l[6] = l7.getValue();
    copyObj.r[7] = r8.getValue();
    copyObj.l[7] = l8.getValue();
    copyObj.r[8] = r9.getValue();
    copyObj.l[8] = l9.getValue();
    copyObj.r[9] = r10.getValue();
    copyObj.l[9] = l10.getValue();
    copyObj.r[10] = r11.getValue();
    copyObj.l[10] = l11.getValue();
}

void GuiComponentSsgSwPEnv11::pasteParams(CopyPEnvSsgSw11& copyObj) {
    flag.setToggleState(copyObj.flag, juce::sendNotification);
    steps.setValue(copyObj.steps, juce::sendNotification);
    loop.setToggleState(copyObj.loop, juce::sendNotification);
    loopTo.setValue(copyObj.loopTo, juce::sendNotification);
    loopCount.setValue(copyObj.loopCount, juce::sendNotification);
    startLevel.setValue(copyObj.stl, juce::sendNotification);
    r1.setValue(copyObj.r[0], juce::sendNotification);
    l1.setValue(copyObj.l[0], juce::sendNotification);
    r2.setValue(copyObj.r[1], juce::sendNotification);
    l2.setValue(copyObj.l[1], juce::sendNotification);
    r3.setValue(copyObj.r[2], juce::sendNotification);
    l3.setValue(copyObj.l[2], juce::sendNotification);
    r4.setValue(copyObj.r[3], juce::sendNotification);
    l4.setValue(copyObj.l[3], juce::sendNotification);
    r5.setValue(copyObj.r[4], juce::sendNotification);
    l5.setValue(copyObj.l[4], juce::sendNotification);
    r6.setValue(copyObj.r[5], juce::sendNotification);
    l6.setValue(copyObj.l[5], juce::sendNotification);
    r7.setValue(copyObj.r[6], juce::sendNotification);
    l7.setValue(copyObj.l[6], juce::sendNotification);
    r8.setValue(copyObj.r[7], juce::sendNotification);
    l8.setValue(copyObj.l[7], juce::sendNotification);
    r9.setValue(copyObj.r[8], juce::sendNotification);
    l9.setValue(copyObj.l[8], juce::sendNotification);
    r10.setValue(copyObj.r[9], juce::sendNotification);
    l10.setValue(copyObj.l[9], juce::sendNotification);
    r11.setValue(copyObj.r[10], juce::sendNotification);
    l11.setValue(copyObj.l[10], juce::sendNotification);
}

void GuiComponentSsgSwPEnv11::importParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgSwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importSsgSwEnvParamFile, defaultDir, Io::ExtensionGlob::SsgSwPEnvParam11);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

                auto reader = Io::ParamReader::open(file, ssgSwPEnv11Format);

                if (!reader.has_value()) return;

                flag.setToggleState(reader->getBool("flag", flag.getToggleState()), juce::sendNotification);
                steps.setValue(reader->getInt("steps", (int)steps.getValue()), juce::sendNotification);
                loop.setToggleState(reader->getBool("loop", loop.getToggleState()), juce::sendNotification);
                loopTo.setValue(reader->getInt("loopTo", (int)loopTo.getValue()), juce::sendNotification);
                loopCount.setValue(reader->getInt("loopCount", (int)loopCount.getValue()), juce::sendNotification);
                startLevel.setValue(reader->getFloat("startLevel", (float)startLevel.getValue()), juce::sendNotification);
                r1.setValue(reader->getFloat("r1", (float)r1.getValue()), juce::sendNotification);
                l1.setValue(reader->getFloat("l1", (float)l1.getValue()), juce::sendNotification);
                r2.setValue(reader->getFloat("r2", (float)r2.getValue()), juce::sendNotification);
                l2.setValue(reader->getFloat("l2", (float)l2.getValue()), juce::sendNotification);
                r3.setValue(reader->getFloat("r3", (float)r3.getValue()), juce::sendNotification);
                l3.setValue(reader->getFloat("l3", (float)l3.getValue()), juce::sendNotification);
                r4.setValue(reader->getFloat("r4", (float)r4.getValue()), juce::sendNotification);
                l4.setValue(reader->getFloat("l4", (float)l4.getValue()), juce::sendNotification);
                r5.setValue(reader->getFloat("r5", (float)r5.getValue()), juce::sendNotification);
                l5.setValue(reader->getFloat("l5", (float)l5.getValue()), juce::sendNotification);
                r6.setValue(reader->getFloat("r6", (float)r6.getValue()), juce::sendNotification);
                l6.setValue(reader->getFloat("l6", (float)l6.getValue()), juce::sendNotification);
                r7.setValue(reader->getFloat("r7", (float)r7.getValue()), juce::sendNotification);
                l7.setValue(reader->getFloat("l7", (float)l7.getValue()), juce::sendNotification);
                r8.setValue(reader->getFloat("r8", (float)r8.getValue()), juce::sendNotification);
                l8.setValue(reader->getFloat("l8", (float)l8.getValue()), juce::sendNotification);
                r9.setValue(reader->getFloat("r9", (float)r9.getValue()), juce::sendNotification);
                l9.setValue(reader->getFloat("l9", (float)l9.getValue()), juce::sendNotification);
                r10.setValue(reader->getFloat("r10", (float)r10.getValue()), juce::sendNotification);
                l10.setValue(reader->getFloat("l10", (float)l10.getValue()), juce::sendNotification);
                r11.setValue(reader->getFloat("r11", (float)r11.getValue()), juce::sendNotification);
                l11.setValue(reader->getFloat("l11", (float)l11.getValue()), juce::sendNotification);
            }
        });
}

void GuiComponentSsgSwPEnv11::exportParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgSwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportSsgSwEnvParamFile, defaultDir.getChildFile("default.ssgSwPEnv11.json"), Io::ExtensionGlob::SsgSwPEnvParam11);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(ssgSwPEnv11Format);

                writer.set("flag", flag.getToggleState());
                writer.set("steps", (float)steps.getValue());
                writer.set("loop", loop.getToggleState());
                writer.set("loopTo", (float)loopTo.getValue());
                writer.set("loopCount", (float)loopCount.getValue());
                writer.set("startLevel", (float)startLevel.getValue());
                writer.set("r1", (float)r1.getValue());
                writer.set("l1", (float)l1.getValue());
                writer.set("r2", (float)r2.getValue());
                writer.set("l2", (float)l2.getValue());
                writer.set("r3", (float)r3.getValue());
                writer.set("l3", (float)l3.getValue());
                writer.set("r4", (float)r4.getValue());
                writer.set("l4", (float)l4.getValue());
                writer.set("r5", (float)r5.getValue());
                writer.set("l5", (float)l5.getValue());
                writer.set("r6", (float)r6.getValue());
                writer.set("l6", (float)l6.getValue());
                writer.set("r7", (float)r7.getValue());
                writer.set("l7", (float)l7.getValue());
                writer.set("r8", (float)r8.getValue());
                writer.set("l8", (float)l8.getValue());
                writer.set("r9", (float)r9.getValue());
                writer.set("l9", (float)l9.getValue());
                writer.set("r10", (float)r10.getValue());
                writer.set("l10", (float)l10.getValue());
                writer.set("r11", (float)r11.getValue());
                writer.set("l11", (float)l11.getValue());

                writer.writeTo(file);
            }
        });
}

void GuiComponentSsgSwPEnv11::setImportingParams(juce::StringArray& lines, int& index) {
    flag.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    steps.setValue(lines[index++].getIntValue(), juce::sendNotification);
    loop.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    loopTo.setValue(lines[index++].getIntValue(), juce::sendNotification);
    loopCount.setValue(lines[index++].getIntValue(), juce::sendNotification);
    startLevel.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    r1.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    l1.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    r2.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    l2.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    r3.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    l3.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    r4.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    l4.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    r5.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    l5.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    r6.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    l6.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    r7.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    l7.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    r8.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    l8.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    r9.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    l9.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    r10.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    l10.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    r11.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    l11.setValue(lines[index++].getFloatValue(), juce::sendNotification);
}

void GuiComponentSsgSwPEnv11::readParams(const Io::ParamReader& reader, const juce::String& prefix)
{
    Io::ParamReader r(reader, prefix);

    flag.setToggleState(r.getBool("flag", flag.getToggleState()), juce::sendNotification);
    steps.setValue(r.getInt("steps", (int)steps.getValue()), juce::sendNotification);
    loop.setToggleState(r.getBool("loop", loop.getToggleState()), juce::sendNotification);
    loopTo.setValue(r.getInt("loopTo", (int)loopTo.getValue()), juce::sendNotification);
    loopCount.setValue(r.getInt("loopCount", (int)loopCount.getValue()), juce::sendNotification);
    startLevel.setValue(r.getFloat("startLevel", (float)startLevel.getValue()), juce::sendNotification);
    r1.setValue(r.getFloat("r1", (float)r1.getValue()), juce::sendNotification);
    l1.setValue(r.getFloat("l1", (float)l1.getValue()), juce::sendNotification);
    r2.setValue(r.getFloat("r2", (float)r2.getValue()), juce::sendNotification);
    l2.setValue(r.getFloat("l2", (float)l2.getValue()), juce::sendNotification);
    r3.setValue(r.getFloat("r3", (float)r3.getValue()), juce::sendNotification);
    l3.setValue(r.getFloat("l3", (float)l3.getValue()), juce::sendNotification);
    r4.setValue(r.getFloat("r4", (float)r4.getValue()), juce::sendNotification);
    l4.setValue(r.getFloat("l4", (float)l4.getValue()), juce::sendNotification);
    r5.setValue(r.getFloat("r5", (float)r5.getValue()), juce::sendNotification);
    l5.setValue(r.getFloat("l5", (float)l5.getValue()), juce::sendNotification);
    r6.setValue(r.getFloat("r6", (float)r6.getValue()), juce::sendNotification);
    l6.setValue(r.getFloat("l6", (float)l6.getValue()), juce::sendNotification);
    r7.setValue(r.getFloat("r7", (float)r7.getValue()), juce::sendNotification);
    l7.setValue(r.getFloat("l7", (float)l7.getValue()), juce::sendNotification);
    r8.setValue(r.getFloat("r8", (float)r8.getValue()), juce::sendNotification);
    l8.setValue(r.getFloat("l8", (float)l8.getValue()), juce::sendNotification);
    r9.setValue(r.getFloat("r9", (float)r9.getValue()), juce::sendNotification);
    l9.setValue(r.getFloat("l9", (float)l9.getValue()), juce::sendNotification);
    r10.setValue(r.getFloat("r10", (float)r10.getValue()), juce::sendNotification);
    l10.setValue(r.getFloat("l10", (float)l10.getValue()), juce::sendNotification);
    r11.setValue(r.getFloat("r11", (float)r11.getValue()), juce::sendNotification);
    l11.setValue(r.getFloat("l11", (float)l11.getValue()), juce::sendNotification);
}

juce::String GuiComponentSsgSwPEnv11::getExportedParams() {
    juce::String content = "";

    content += juce::String(flag.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(steps.getValue()) + "\n";
    content += juce::String(loop.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(loopTo.getValue()) + "\n";
    content += juce::String(loopCount.getValue()) + "\n";
    content += juce::String(startLevel.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(r1.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(l1.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(r2.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(l2.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(r3.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(l3.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(r4.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(l4.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(r5.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(l5.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(r6.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(l6.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(r7.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(l7.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(r8.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(l8.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(r9.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(l9.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(r10.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(l10.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(r11.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(l11.getValue(), Global::floatDecimalPlaces) + "\n";

    return content;
}

void GuiComponentSsgSwPEnv11::writeParams(Io::ParamWriter& writer, const juce::String& prefix)
{
    Io::ParamWriter w(writer, prefix);

    w.set("flag", flag.getToggleState());
    w.set("steps", (float)steps.getValue());
    w.set("loop", loop.getToggleState());
    w.set("loopTo", (float)loopTo.getValue());
    w.set("loopCount", (float)loopCount.getValue());
    w.set("startLevel", (float)startLevel.getValue());
    w.set("r1", (float)r1.getValue());
    w.set("l1", (float)l1.getValue());
    w.set("r2", (float)r2.getValue());
    w.set("l2", (float)l2.getValue());
    w.set("r3", (float)r3.getValue());
    w.set("l3", (float)l3.getValue());
    w.set("r4", (float)r4.getValue());
    w.set("l4", (float)l4.getValue());
    w.set("r5", (float)r5.getValue());
    w.set("l5", (float)l5.getValue());
    w.set("r6", (float)r6.getValue());
    w.set("l6", (float)l6.getValue());
    w.set("r7", (float)r7.getValue());
    w.set("l7", (float)l7.getValue());
    w.set("r8", (float)r8.getValue());
    w.set("l8", (float)l8.getValue());
    w.set("r9", (float)r9.getValue());
    w.set("l9", (float)l9.getValue());
    w.set("r10", (float)r10.getValue());
    w.set("l10", (float)l10.getValue());
    w.set("r11", (float)r11.getValue());
    w.set("l11", (float)l11.getValue());
}
