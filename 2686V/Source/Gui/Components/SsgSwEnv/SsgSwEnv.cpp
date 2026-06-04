#include "./SsgSwEnv.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

void GuiComponentSsgSwEnv::applyLoopValues(bool enabled)
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

void GuiComponentSsgSwEnv::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable)
{
    this->isEnable = isEnable;

    cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "[■]--- SSG SW ENV ---",
        .invisibleTitle = juce::String("") + "[□]--- SSG SW ENV ---",
        .enableChangeDetailVisible = true
        });

    flag.setup({ .parent = parent, .id = code + flagKey, .title = flagText, .isReset = true });
    flag.setWantsKeyboardFocus(true);
    flag.setExplicitFocusOrder(++tabOrder);

    steps.setup({ .parent = parent, .id = code + "_SSGSW_STEPS", .title = "Steps", .isReset = true, .labelFont = juce::Font(6.0f) });
    steps.setWantsKeyboardFocus(true);
    steps.setExplicitFocusOrder(++tabOrder);
    steps.onValueChange = [this] {
        bool ssgEnvLoopEnable = loop.getToggleState();

        applyLoopValues(ssgEnvLoopEnable);
        };

    loop.setup({ .parent = parent, .id = code + "_SSGSW_LOOP", .title = "Loop", .isReset = true });
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

    loopTo.setup({ .parent = parent, .id = code + "_SSGSW_LOOPTO", .title = "Loop To", .isReset = true, .labelFont = juce::Font(6.0f) });
    loopTo.setWantsKeyboardFocus(true);
    loopTo.setExplicitFocusOrder(++tabOrder);
    loopTo.onValueChange = [this] {
        bool ssgEnvLoopEnable = loop.getToggleState();

        applyLoopValues(ssgEnvLoopEnable);
        };

    loopCount.setup({ .parent = parent, .id = code + "_SSGSW_LOOPCNT", .title = "Loop Cnt", .isReset = true, .labelFont = juce::Font(6.0f) });
    loopCount.setWantsKeyboardFocus(true);
    loopCount.setExplicitFocusOrder(++tabOrder);

    bool ssgEnvLoopEnable = loop.getToggleState();

    loopTo.setEnabled(ssgEnvLoopEnable);
    loopTo.label.setEnabled(ssgEnvLoopEnable);
    loopCount.setEnabled(ssgEnvLoopEnable);
    loopCount.label.setEnabled(ssgEnvLoopEnable);

    startLevel.setup({ .parent = parent, .id = code + "_SSGSW_STL", .title = "Stl", .isReset = true, .labelFont = juce::Font(6.0f) });
    startLevel.setWantsKeyboardFocus(true);
    startLevel.setExplicitFocusOrder(++tabOrder);

    r1.setup({ .parent = parent, .id = code + "_SSGSW_R1", .title = "R1", .isReset = true, .labelFont = juce::Font(6.0f) });
    r1.setWantsKeyboardFocus(true);
    r1.setExplicitFocusOrder(++tabOrder);

    l1.setup({ .parent = parent, .id = code + "_SSGSW_L1", .title = "L1", .isReset = true, .labelFont = juce::Font(6.0f) });
    l1.setWantsKeyboardFocus(true);
    l1.setExplicitFocusOrder(++tabOrder);

    r2.setup({ .parent = parent, .id = code + "_SSGSW_R2", .title = "R2", .isReset = true, .labelFont = juce::Font(6.0f) });
    r2.setWantsKeyboardFocus(true);
    r2.setExplicitFocusOrder(++tabOrder);

    l2.setup({ .parent = parent, .id = code + "_SSGSW_L2", .title = "L2", .isReset = true, .labelFont = juce::Font(6.0f) });
    l2.setWantsKeyboardFocus(true);
    l2.setExplicitFocusOrder(++tabOrder);

    r3.setup({ .parent = parent, .id = code + "_SSGSW_R3", .title = "R3", .isReset = true, .labelFont = juce::Font(6.0f) });
    r3.setWantsKeyboardFocus(true);
    r3.setExplicitFocusOrder(++tabOrder);

    l3.setup({ .parent = parent, .id = code + "_SSGSW_L3", .title = "L3", .isReset = true, .labelFont = juce::Font(6.0f) });
    l3.setWantsKeyboardFocus(true);
    l3.setExplicitFocusOrder(++tabOrder);

    r4.setup({ .parent = parent, .id = code + "_SSGSW_R4", .title = "R4", .isReset = true, .labelFont = juce::Font(6.0f) });
    r4.setWantsKeyboardFocus(true);
    r4.setExplicitFocusOrder(++tabOrder);

    l4.setup({ .parent = parent, .id = code + "_SSGSW_L4", .title = "L4", .isReset = true, .labelFont = juce::Font(6.0f) });
    l4.setWantsKeyboardFocus(true);
    l4.setExplicitFocusOrder(++tabOrder);

    r5.setup({ .parent = parent, .id = code + "_SSGSW_R5", .title = "R5", .isReset = true, .labelFont = juce::Font(6.0f) });
    r5.setWantsKeyboardFocus(true);
    r5.setExplicitFocusOrder(++tabOrder);

    l5.setup({ .parent = parent, .id = code + "_SSGSW_L5", .title = "L5", .isReset = true, .labelFont = juce::Font(6.0f) });
    l5.setWantsKeyboardFocus(true);
    l5.setExplicitFocusOrder(++tabOrder);

    r6.setup({ .parent = parent, .id = code + "_SSGSW_R6", .title = "R6", .isReset = true, .labelFont = juce::Font(6.0f) });
    r6.setWantsKeyboardFocus(true);
    r6.setExplicitFocusOrder(++tabOrder);

    l6.setup({ .parent = parent, .id = code + "_SSGSW_L6", .title = "L6", .isReset = true, .labelFont = juce::Font(6.0f) });
    l6.setWantsKeyboardFocus(true);
    l6.setExplicitFocusOrder(++tabOrder);
}

void GuiComponentSsgSwEnv::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    flag.setVisible(visible);
    steps.setVisibleWithLabel(visible);
    loop.setVisible(visible);
    loopTo.setVisibleWithLabel(visible);
    loopCount.setVisibleWithLabel(visible);
    startLevel.setVisibleWithLabel(visible);
    r1.setVisibleWithLabel(visible);
    l1.setVisibleWithLabel(visible);
    r2.setVisibleWithLabel(visible);
    l2.setVisibleWithLabel(visible);
    r3.setVisibleWithLabel(visible);
    l3.setVisibleWithLabel(visible);
    r4.setVisibleWithLabel(visible);
    l4.setVisibleWithLabel(visible);
    r5.setVisibleWithLabel(visible);
    l5.setVisibleWithLabel(visible);
    r6.setVisibleWithLabel(visible);
    l6.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &flag });
        layoutMain({ .mainRect = rect, .label = &steps.label, .component = &steps, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .component = &loop, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &loopTo.label, .component = &loopTo, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &loopCount.label, .component = &loopCount, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &startLevel.label, .component = &startLevel, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &r1.label, .component = &r1, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &l1.label, .component = &l1, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &r2.label, .component = &r2, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &l2.label, .component = &l2, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &r3.label, .component = &r3, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &l3.label, .component = &l3, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &r4.label, .component = &r4, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &l4.label, .component = &l4, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &r5.label, .component = &r5, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &l5.label, .component = &l5, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &r6.label, .component = &r6, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &l6.label, .component = &l6, .rowHeight = 10 });
    }
}

void GuiComponentSsgSwEnv::layoutComponentRow(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    flag.setVisible(visible);
    steps.setVisibleWithLabel(visible);
    loop.setVisible(visible);
    loopTo.setVisibleWithLabel(visible);
    loopCount.setVisibleWithLabel(visible);
    startLevel.setVisibleWithLabel(visible);
    r1.setVisibleWithLabel(visible);
    l1.setVisibleWithLabel(visible);
    r2.setVisibleWithLabel(visible);
    l2.setVisibleWithLabel(visible);
    r3.setVisibleWithLabel(visible);
    l3.setVisibleWithLabel(visible);
    r4.setVisibleWithLabel(visible);
    l4.setVisibleWithLabel(visible);
    r5.setVisibleWithLabel(visible);
    l5.setVisibleWithLabel(visible);
    r6.setVisibleWithLabel(visible);
    l6.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &flag });
        layoutRow({ .rowRect = rect, .label = &steps.label, .component = &steps, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .component = &loop, .rowHeight = 10 });
        layoutRow({ .rowRect = rect, .label = &loopTo.label, .component = &loopTo, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &loopCount.label, .component = &loopCount, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &startLevel.label, .component = &startLevel, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &r1.label, .component = &r1, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &l1.label, .component = &l1, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &r2.label, .component = &r2, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &l2.label, .component = &l2, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &r3.label, .component = &r3, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &l3.label, .component = &l3, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &r4.label, .component = &r4, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &l4.label, .component = &l4, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &r5.label, .component = &r5, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &l5.label, .component = &l5, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &r6.label, .component = &r6, .rowHeight = 8 });
        layoutRow({ .rowRect = rect, .label = &l6.label, .component = &l6, .rowHeight = 8 });
    }
}

void GuiComponentSsgSwEnv::setupGraph(std::function<void()> repaintGraph) {

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

    startLevel.onValueChange = repaintGraph;
    l1.onValueChange = repaintGraph;
    l2.onValueChange = repaintGraph;
    l3.onValueChange = repaintGraph;
    l4.onValueChange = repaintGraph;
    l5.onValueChange = repaintGraph;
    l6.onValueChange = repaintGraph;
}

void GuiComponentSsgSwEnv::updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx) {
    graph.updateBypass(this->isEnable ? !flag.getToggleState() : flag.getToggleState());

    graph.updateSsgSwEnv(
        steps,
        loop,
        loopTo,
        loopCount,
        { nullptr, &r1, &r2, &r3, &r4, &r5, &r6 },
        { &startLevel, &l1, &l2, &l3, &l4, &l5, &l6 },
        p_curveCore,
        isCurveMode,
        posIdx
    );
}

void GuiComponentSsgSwEnv::setEnabled(bool enabled) {
    bool ssgEnvLoopEnable = loop.getToggleState();

    flag.setEnabled(enabled);
    steps.setEnabled(enabled);
    loop.setEnabled(enabled);
    loopTo.setEnabled(enabled && ssgEnvLoopEnable);
    loopCount.setEnabled(enabled && ssgEnvLoopEnable);
    startLevel.setEnabled(enabled);
    r1.setEnabled(enabled);
    l1.setEnabled(enabled);
    r2.setEnabled(enabled);
    l2.setEnabled(enabled);
    r3.setEnabled(enabled);
    l3.setEnabled(enabled);
    r4.setEnabled(enabled);
    l4.setEnabled(enabled);
    r5.setEnabled(enabled);
    l5.setEnabled(enabled);
    r6.setEnabled(enabled);
    l6.setEnabled(enabled);
}
