#include "./SsgSwEnv11.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Processor/ProcessorValues.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

void GuiComponentSsgSwEnv11::applyLoopValues(bool enabled)
{
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

void GuiComponentSsgSwEnv11::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable)
{
    this->isEnable = isEnable;

    cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "[■]--- SSG SW ENV11 ---",
        .invisibleTitle = juce::String("") + "[□]--- SSG SW ENV11 ---",
        .enableChangeDetailVisible = true
        });

    flag.setup({ .parent = parent, .id = code + flagKey, .title = flagText, .isReset = true });
    flag.setWantsKeyboardFocus(true);
    flag.setExplicitFocusOrder(++tabOrder);

    flagSeparator.setupComponent(parent);

    steps.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::steps, .title = "STEP", .isReset = true, .labelFont = labelFont });
    steps.setWantsKeyboardFocus(true);
    steps.setExplicitFocusOrder(++tabOrder);
    steps.onValueChange = [this] {
        bool ssgEnvLoopEnable = loop.getToggleState();

        applyLoopValues(ssgEnvLoopEnable);
        };

    stepsSeparator.setupComponent(parent);

    loop.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::loop, .title = "LOOP", .isReset = true });
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

    loopTo.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::loopTo, .title = "L.TO", .isReset = true, .labelFont = labelFont });
    loopTo.setWantsKeyboardFocus(true);
    loopTo.setExplicitFocusOrder(++tabOrder);
    loopTo.onValueChange = [this] {
        bool ssgEnvLoopEnable = loop.getToggleState();

        applyLoopValues(ssgEnvLoopEnable);
        };

    loopCount.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::loopCount, .title = "L.CN", .isReset = true, .labelFont = labelFont });
    loopCount.setWantsKeyboardFocus(true);
    loopCount.setExplicitFocusOrder(++tabOrder);

    bool ssgEnvLoopEnable = loop.getToggleState();

    loopTo.setEnabled(ssgEnvLoopEnable);
    loopTo.label.setEnabled(ssgEnvLoopEnable);
    loopCount.setEnabled(ssgEnvLoopEnable);
    loopCount.label.setEnabled(ssgEnvLoopEnable);

    loopSeparator.setupComponent(parent);

    r1.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::r1, .title = "R1", .isReset = true, .labelFont = labelFont });
    r1.setWantsKeyboardFocus(true);
    r1.setExplicitFocusOrder(++tabOrder);

    r2.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::r2, .title = "R2", .isReset = true, .labelFont = labelFont });
    r2.setWantsKeyboardFocus(true);
    r2.setExplicitFocusOrder(++tabOrder);

    r3.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::r3, .title = "R3", .isReset = true, .labelFont = labelFont });
    r3.setWantsKeyboardFocus(true);
    r3.setExplicitFocusOrder(++tabOrder);

    r4.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::r4, .title = "R4", .isReset = true, .labelFont = labelFont });
    r4.setWantsKeyboardFocus(true);
    r4.setExplicitFocusOrder(++tabOrder);

    r5.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::r5, .title = "R5", .isReset = true, .labelFont = labelFont });
    r5.setWantsKeyboardFocus(true);
    r5.setExplicitFocusOrder(++tabOrder);

    r6.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::r6, .title = "R6", .isReset = true, .labelFont = labelFont });
    r6.setWantsKeyboardFocus(true);
    r6.setExplicitFocusOrder(++tabOrder);

    r7.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::r7, .title = "R7", .isReset = true, .labelFont = labelFont });
    r7.setWantsKeyboardFocus(true);
    r7.setExplicitFocusOrder(++tabOrder);

    r8.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::r8, .title = "R8", .isReset = true, .labelFont = labelFont });
    r8.setWantsKeyboardFocus(true);
    r8.setExplicitFocusOrder(++tabOrder);

    r9.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::r9, .title = "R9", .isReset = true, .labelFont = labelFont });
    r9.setWantsKeyboardFocus(true);
    r9.setExplicitFocusOrder(++tabOrder);

    r10.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::r10, .title = "R10", .isReset = true, .labelFont = labelFont });
    r10.setWantsKeyboardFocus(true);
    r10.setExplicitFocusOrder(++tabOrder);

    r11.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::r11, .title = "R11", .isReset = true, .labelFont = labelFont });
    r11.setWantsKeyboardFocus(true);
    r11.setExplicitFocusOrder(++tabOrder);

    rateSeparator.setupComponent(parent);

    startLevel.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::stl, .title = "STL", .isReset = true, .labelFont = labelFont });
    startLevel.setWantsKeyboardFocus(true);
    startLevel.setExplicitFocusOrder(++tabOrder);

    stlBtns.setupComponent(parent, startLevel, tabOrder, labelFont);

    l1.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::l1, .title = "L1", .isReset = true, .labelFont = labelFont });
    l1.setWantsKeyboardFocus(true);
    l1.setExplicitFocusOrder(++tabOrder);

    l1Btns.setupComponent(parent, l1, tabOrder, labelFont);

    l2.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::l2, .title = "L2", .isReset = true, .labelFont = labelFont });
    l2.setWantsKeyboardFocus(true);
    l2.setExplicitFocusOrder(++tabOrder);

    l2Btns.setupComponent(parent, l2, tabOrder, labelFont);

    l3.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::l3, .title = "L3", .isReset = true, .labelFont = labelFont });
    l3.setWantsKeyboardFocus(true);
    l3.setExplicitFocusOrder(++tabOrder);

    l3Btns.setupComponent(parent, l3, tabOrder, labelFont);

    l4.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::l4, .title = "L4", .isReset = true, .labelFont = labelFont });
    l4.setWantsKeyboardFocus(true);
    l4.setExplicitFocusOrder(++tabOrder);

    l4Btns.setupComponent(parent, l4, tabOrder, labelFont);

    l5.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::l5, .title = "L5", .isReset = true, .labelFont = labelFont });
    l5.setWantsKeyboardFocus(true);
    l5.setExplicitFocusOrder(++tabOrder);

    l5Btns.setupComponent(parent, l5, tabOrder, labelFont);

    l6.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::l6, .title = "L6", .isReset = true, .labelFont = labelFont });
    l6.setWantsKeyboardFocus(true);
    l6.setExplicitFocusOrder(++tabOrder);

    l6Btns.setupComponent(parent, l6, tabOrder, labelFont);

    l7.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::l7, .title = "L7", .isReset = true, .labelFont = labelFont });
    l7.setWantsKeyboardFocus(true);
    l7.setExplicitFocusOrder(++tabOrder);

    l7Btns.setupComponent(parent, l7, tabOrder, labelFont);

    l8.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::l8, .title = "L8", .isReset = true, .labelFont = labelFont });
    l8.setWantsKeyboardFocus(true);
    l8.setExplicitFocusOrder(++tabOrder);

    l8Btns.setupComponent(parent, l8, tabOrder, labelFont);

    l9.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::l9, .title = "L9", .isReset = true, .labelFont = labelFont });
    l9.setWantsKeyboardFocus(true);
    l9.setExplicitFocusOrder(++tabOrder);

    l9Btns.setupComponent(parent, l9, tabOrder, labelFont);

    l10.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::l10, .title = "L10", .isReset = true, .labelFont = labelFont });
    l10.setWantsKeyboardFocus(true);
    l10.setExplicitFocusOrder(++tabOrder);

    l10Btns.setupComponent(parent, l10, tabOrder, labelFont);

    l11.setup({ .parent = parent, .id = code + CPK::SsgSwEnv11::l11, .title = "L11", .isReset = true, .labelFont = labelFont });
    l11.setWantsKeyboardFocus(true);
    l11.setExplicitFocusOrder(++tabOrder);

    l11Btns.setupComponent(parent, l11, tabOrder, labelFont);
}

void GuiComponentSsgSwEnv11::layoutComponent(juce::Rectangle<int>& rect)
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
    r2.setVisibleWithLabel(visible);
    r3.setVisibleWithLabel(visible);
    r4.setVisibleWithLabel(visible);
    r5.setVisibleWithLabel(visible);
    r6.setVisibleWithLabel(visible);
	r7.setVisibleWithLabel(visible);
	r8.setVisibleWithLabel(visible);
	r9.setVisibleWithLabel(visible);
	r10.setVisibleWithLabel(visible);
	r11.setVisibleWithLabel(visible);
	rateSeparator.setVisible(visible);
    startLevel.setVisibleWithLabel(visible);
    stlBtns.setVisibles(visible);
    l1.setVisibleWithLabel(visible);
    l1Btns.setVisibles(visible);
    l2.setVisibleWithLabel(visible);
    l2Btns.setVisibles(visible);
    l3.setVisibleWithLabel(visible);
    l3Btns.setVisibles(visible);
    l4.setVisibleWithLabel(visible);
    l4Btns.setVisibles(visible);
    l5.setVisibleWithLabel(visible);
    l5Btns.setVisibles(visible);
    l6.setVisibleWithLabel(visible);
    l6Btns.setVisibles(visible);
    l7.setVisibleWithLabel(visible);
    l7Btns.setVisibles(visible);
    l8.setVisibleWithLabel(visible);
    l8Btns.setVisibles(visible);
    l9.setVisibleWithLabel(visible);
    l9Btns.setVisibles(visible);
    l10.setVisibleWithLabel(visible);
    l10Btns.setVisibles(visible);
    l11.setVisibleWithLabel(visible);
    l11Btns.setVisibles(visible);

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
        layoutMain({ .mainRect = rect, .label = &r1.label, .component = &r1, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &r2.label, .component = &r2, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &r3.label, .component = &r3, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &r4.label, .component = &r4, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &r5.label, .component = &r5, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &r6.label, .component = &r6, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &r7.label, .component = &r7, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &r8.label, .component = &r8, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &r9.label, .component = &r9, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &r10.label, .component = &r10, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &r11.label, .component = &r11, .rowHeight = 13 });
        rateSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &startLevel.label, .component = &startLevel, .rowHeight = 13 });
        stlBtns.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .label = &l1.label, .component = &l1, .rowHeight = 13 });
        l1Btns.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .label = &l2.label, .component = &l2, .rowHeight = 13 });
        l2Btns.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .label = &l3.label, .component = &l3, .rowHeight = 13 });
        l3Btns.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .label = &l4.label, .component = &l4, .rowHeight = 13 });
        l4Btns.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .label = &l5.label, .component = &l5, .rowHeight = 13 });
        l5Btns.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .label = &l6.label, .component = &l6, .rowHeight = 13 });
        l6Btns.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .label = &l7.label, .component = &l7, .rowHeight = 13 });
        l7Btns.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .label = &l8.label, .component = &l8, .rowHeight = 13 });
        l8Btns.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .label = &l9.label, .component = &l9, .rowHeight = 13 });
        l9Btns.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .label = &l10.label, .component = &l10, .rowHeight = 13 });
        l10Btns.layoutComponent(rect, 13);
        layoutMain({ .mainRect = rect, .label = &l11.label, .component = &l11, .rowHeight = 13 });
        l11Btns.layoutComponent(rect, 13);
    }
}

void GuiComponentSsgSwEnv11::layoutComponentRow(juce::Rectangle<int>& rect)
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
    r2.setVisibleWithLabel(visible);
    r3.setVisibleWithLabel(visible);
    r4.setVisibleWithLabel(visible);
    r5.setVisibleWithLabel(visible);
    r6.setVisibleWithLabel(visible);
    r7.setVisibleWithLabel(visible);
    r8.setVisibleWithLabel(visible);
    r9.setVisibleWithLabel(visible);
    r10.setVisibleWithLabel(visible);
    r11.setVisibleWithLabel(visible);
    rateSeparator.setVisible(visible);
    startLevel.setVisibleWithLabel(visible);
    stlBtns.setVisibles(visible);
    l1.setVisibleWithLabel(visible);
    l1Btns.setVisibles(visible);
    l2.setVisibleWithLabel(visible);
    l2Btns.setVisibles(visible);
    l3.setVisibleWithLabel(visible);
    l3Btns.setVisibles(visible);
    l4.setVisibleWithLabel(visible);
    l4Btns.setVisibles(visible);
    l5.setVisibleWithLabel(visible);
    l5Btns.setVisibles(visible);
    l6.setVisibleWithLabel(visible);
    l6Btns.setVisibles(visible);
    l7.setVisibleWithLabel(visible);
    l7Btns.setVisibles(visible);
    l8.setVisibleWithLabel(visible);
    l8Btns.setVisibles(visible);
    l9.setVisibleWithLabel(visible);
    l9Btns.setVisibles(visible);
    l10.setVisibleWithLabel(visible);
    l10Btns.setVisibles(visible);
    l11.setVisibleWithLabel(visible);
    l11Btns.setVisibles(visible);

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
        layoutRow({ .rowRect = rect, .label = &r1.label, .component = &r1, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &r2.label, .component = &r2, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &r3.label, .component = &r3, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &r4.label, .component = &r4, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &r5.label, .component = &r5, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &r6.label, .component = &r6, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &r7.label, .component = &r7, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &r8.label, .component = &r8, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &r9.label, .component = &r9, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &r10.label, .component = &r10, .rowHeight = 12 });
        layoutRow({ .rowRect = rect, .label = &r11.label, .component = &r11, .rowHeight = 12 });
        rateSeparator.layoutComponent(rect);
        layoutRow({ .rowRect = rect, .label = &startLevel.label, .component = &startLevel, .rowHeight = 12 });
        stlBtns.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .label = &l1.label, .component = &l1, .rowHeight = 12 });
        l1Btns.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .label = &l2.label, .component = &l2, .rowHeight = 12 });
        l2Btns.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .label = &l3.label, .component = &l3, .rowHeight = 12 });
        l3Btns.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .label = &l4.label, .component = &l4, .rowHeight = 12 });
        l4Btns.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .label = &l5.label, .component = &l5, .rowHeight = 12 });
        l5Btns.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .label = &l6.label, .component = &l6, .rowHeight = 12 });
        l6Btns.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .label = &l7.label, .component = &l7, .rowHeight = 12 });
        l7Btns.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .label = &l8.label, .component = &l8, .rowHeight = 12 });
        l8Btns.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .label = &l9.label, .component = &l9, .rowHeight = 12 });
        l9Btns.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .label = &l10.label, .component = &l10, .rowHeight = 12 });
        l10Btns.layoutComponentRow(rect, 12);
        layoutRow({ .rowRect = rect, .label = &l11.label, .component = &l11, .rowHeight = 12 });
        l11Btns.layoutComponentRow(rect, 12);
    }
}

void GuiComponentSsgSwEnv11::setupGraph(std::function<void()> repaintGraph) {

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

    r1.onValueChange = repaintGraph;
    r2.onValueChange = repaintGraph;
    r3.onValueChange = repaintGraph;
    r4.onValueChange = repaintGraph;
    r5.onValueChange = repaintGraph;
    r6.onValueChange = repaintGraph;
    r7.onValueChange = repaintGraph;
    r8.onValueChange = repaintGraph;
    r9.onValueChange = repaintGraph;
    r10.onValueChange = repaintGraph;
    r11.onValueChange = repaintGraph;

    startLevel.onValueChange = repaintGraph;
    l1.onValueChange = repaintGraph;
    l2.onValueChange = repaintGraph;
    l3.onValueChange = repaintGraph;
    l4.onValueChange = repaintGraph;
    l5.onValueChange = repaintGraph;
    l6.onValueChange = repaintGraph;
    l7.onValueChange = repaintGraph;
    l8.onValueChange = repaintGraph;
    l9.onValueChange = repaintGraph;
    l10.onValueChange = repaintGraph;
    l11.onValueChange = repaintGraph;
}

void GuiComponentSsgSwEnv11::updateGraph(GuiEnvelopeGraph& graph) {
    graph.updateBypass(this->isEnable ? !flag.getToggleState() : flag.getToggleState());

    graph.updateSsgSwEnv11(
        steps,
        loop,
        loopTo,
        loopCount,
        { nullptr, &r1, &r2, &r3, &r4, &r5, &r6, &r7, &r8, &r9, &r10, &r11 },
        { &startLevel, &l1, &l2, &l3, &l4, &l5, &l6, &l7, &l8, &l9, &l10, &l11 }
    );
}

void GuiComponentSsgSwEnv11::setEnabled(bool enabled) {
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
    stlBtns.setEnables(enabled);
    r1.setEnabled(enabled);
    l1.setEnabled(enabled);
    l1Btns.setEnables(enabled);
    r2.setEnabled(enabled);
    l2.setEnabled(enabled);
    l3Btns.setEnables(enabled);
    r3.setEnabled(enabled);
    l3.setEnabled(enabled);
    l3Btns.setEnables(enabled);
    r4.setEnabled(enabled);
    l4.setEnabled(enabled);
    l4Btns.setEnables(enabled);
    r5.setEnabled(enabled);
    l5.setEnabled(enabled);
    l5Btns.setEnables(enabled);
    r6.setEnabled(enabled);
    l6.setEnabled(enabled);
    l6Btns.setEnables(enabled);
    r7.setEnabled(enabled);
    l7.setEnabled(enabled);
    l7Btns.setEnables(enabled);
    r8.setEnabled(enabled);
    l8.setEnabled(enabled);
    l8Btns.setEnables(enabled);
    r9.setEnabled(enabled);
    l9.setEnabled(enabled);
    l9Btns.setEnables(enabled);
    r10.setEnabled(enabled);
    l10.setEnabled(enabled);
    l10Btns.setEnables(enabled);
    r11.setEnabled(enabled);
    l11.setEnabled(enabled);
    l11Btns.setEnables(enabled);
}

void GuiComponentSsgSwEnv11::copyParams(CopyEnvSsgSw11& copyObj) {
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

void GuiComponentSsgSwEnv11::pasteParams(CopyEnvSsgSw11& copyObj) {
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

void GuiComponentSsgSwEnv11::importParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgSwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importSsgSwEnvParamFile, defaultDir, Io::ExtensionGlob::SsgSwEnvParam11);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 28) return;

                flag.setToggleState(lines[0].getIntValue() == 1, juce::sendNotification);
                steps.setValue(lines[1].getIntValue(), juce::sendNotification);
                loop.setToggleState(lines[2].getIntValue() == 1, juce::sendNotification);
                loopTo.setValue(lines[3].getIntValue(), juce::sendNotification);
                loopCount.setValue(lines[4].getIntValue(), juce::sendNotification);
                startLevel.setValue(lines[5].getFloatValue(), juce::sendNotification);
                r1.setValue(lines[6].getFloatValue(), juce::sendNotification);
                l1.setValue(lines[7].getFloatValue(), juce::sendNotification);
                r2.setValue(lines[8].getFloatValue(), juce::sendNotification);
                l2.setValue(lines[9].getFloatValue(), juce::sendNotification);
                r3.setValue(lines[10].getFloatValue(), juce::sendNotification);
                l3.setValue(lines[11].getFloatValue(), juce::sendNotification);
                r4.setValue(lines[12].getFloatValue(), juce::sendNotification);
                l4.setValue(lines[13].getFloatValue(), juce::sendNotification);
                r5.setValue(lines[14].getFloatValue(), juce::sendNotification);
                l5.setValue(lines[15].getFloatValue(), juce::sendNotification);
                r6.setValue(lines[16].getFloatValue(), juce::sendNotification);
                l6.setValue(lines[17].getFloatValue(), juce::sendNotification);
                r7.setValue(lines[18].getFloatValue(), juce::sendNotification);
                l7.setValue(lines[19].getFloatValue(), juce::sendNotification);
                r8.setValue(lines[20].getFloatValue(), juce::sendNotification);
                l8.setValue(lines[21].getFloatValue(), juce::sendNotification);
                r9.setValue(lines[22].getFloatValue(), juce::sendNotification);
                l9.setValue(lines[23].getFloatValue(), juce::sendNotification);
                r10.setValue(lines[24].getFloatValue(), juce::sendNotification);
                l10.setValue(lines[25].getFloatValue(), juce::sendNotification);
                r11.setValue(lines[26].getFloatValue(), juce::sendNotification);
                l11.setValue(lines[27].getFloatValue(), juce::sendNotification);
            }
        });
}

void GuiComponentSsgSwEnv11::exportParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgSwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportSsgSwEnvParamFile, defaultDir.getChildFile("default.ssgSwEnv11"), Io::ExtensionGlob::SsgSwEnvParam11);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

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

                file.replaceWithText(content);
            }
        });
}
