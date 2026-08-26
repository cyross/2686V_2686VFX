#include "./SsgSwEnv.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Processor/ProcessorValues.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

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

    flagSeparator.setupComponent(parent);

    steps.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::steps, .title = "STEP", .isReset = true, .labelFont = labelFont });
    steps.setWantsKeyboardFocus(true);
    steps.setExplicitFocusOrder(++tabOrder);
    steps.onValueChange = [this] {
        bool ssgEnvLoopEnable = loop.getToggleState();

        applyLoopValues(ssgEnvLoopEnable);
        };

    stepsSeparator.setupComponent(parent);

    loop.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::loop, .title = "LOOP", .isReset = true });
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

    loopTo.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::loopTo, .title = "L.TO", .isReset = true, .labelFont = labelFont });
    loopTo.setWantsKeyboardFocus(true);
    loopTo.setExplicitFocusOrder(++tabOrder);
    loopTo.onValueChange = [this] {
        bool ssgEnvLoopEnable = loop.getToggleState();

        applyLoopValues(ssgEnvLoopEnable);
        };

    loopCount.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::loopCount, .title = "L.CN", .isReset = true, .labelFont = labelFont });
    loopCount.setWantsKeyboardFocus(true);
    loopCount.setExplicitFocusOrder(++tabOrder);

    bool ssgEnvLoopEnable = loop.getToggleState();

    loopTo.setEnabled(ssgEnvLoopEnable);
    loopTo.label.setEnabled(ssgEnvLoopEnable);
    loopCount.setEnabled(ssgEnvLoopEnable);
    loopCount.label.setEnabled(ssgEnvLoopEnable);

    loopSeparator.setupComponent(parent);

    r1.setupComponent(parent, code + CPK::SsgSwEnv::r1, "R1", tabOrder, std::nullopt, labelFont);

    r1Nudge.setupComponent(parent, r1.getSlider(), tabOrder);

    r2.setupComponent(parent, code + CPK::SsgSwEnv::r2, "R2", tabOrder, std::nullopt, labelFont);

    r2Nudge.setupComponent(parent, r2.getSlider(), tabOrder);

    r3.setupComponent(parent, code + CPK::SsgSwEnv::r3, "R3", tabOrder, std::nullopt, labelFont);

    r3Nudge.setupComponent(parent, r3.getSlider(), tabOrder);

    r4.setupComponent(parent, code + CPK::SsgSwEnv::r4, "R4", tabOrder, std::nullopt, labelFont);

    r4Nudge.setupComponent(parent, r4.getSlider(), tabOrder);

    r5.setupComponent(parent, code + CPK::SsgSwEnv::r5, "R5", tabOrder, std::nullopt, labelFont);

    r5Nudge.setupComponent(parent, r5.getSlider(), tabOrder);

    r6.setupComponent(parent, code + CPK::SsgSwEnv::r6, "R6", tabOrder, std::nullopt, labelFont);

    r6Nudge.setupComponent(parent, r6.getSlider(), tabOrder);

    rateSeparator.setupComponent(parent);

    startLevel.setupComponent(parent, code + CPK::SsgSwEnv::stl, "STL", tabOrder, std::nullopt, labelFont);

    stlBtns.setupComponent(parent, startLevel.getSlider(), tabOrder, labelFont);

    l1.setupComponent(parent, code + CPK::SsgSwEnv::l1, "L1", tabOrder, std::nullopt, labelFont);

    l1Btns.setupComponent(parent, l1.getSlider(), tabOrder, labelFont);

    l2.setupComponent(parent, code + CPK::SsgSwEnv::l2, "L2", tabOrder, std::nullopt, labelFont);

    l2Btns.setupComponent(parent, l2.getSlider(), tabOrder, labelFont);

    l3.setupComponent(parent, code + CPK::SsgSwEnv::l3, "L3", tabOrder, std::nullopt, labelFont);

    l3Btns.setupComponent(parent, l3.getSlider(), tabOrder, labelFont);

    l4.setupComponent(parent, code + CPK::SsgSwEnv::l4, "L4", tabOrder, std::nullopt, labelFont);

    l4Btns.setupComponent(parent, l4.getSlider(), tabOrder, labelFont);

    l5.setupComponent(parent, code + CPK::SsgSwEnv::l5, "L5", tabOrder, std::nullopt, labelFont);

    l5Btns.setupComponent(parent, l5.getSlider(), tabOrder, labelFont);

    l6.setupComponent(parent, code + CPK::SsgSwEnv::l6, "L6", tabOrder, std::nullopt, labelFont);

    l6Btns.setupComponent(parent, l6.getSlider(), tabOrder, labelFont);
}

void GuiComponentSsgSwEnv::layoutComponent(juce::Rectangle<int>& rect)
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
	rateSeparator.setVisible(visible);
    startLevel.setVisibleWithLabel(visible);
    stlBtns.setVisibles(visible && startLevel.isVisibleNudge());
    l1.setVisibleWithLabel(visible);
    l1Btns.setVisibles(visible && l1.isVisibleNudge());
    l2.setVisibleWithLabel(visible);
    l2Btns.setVisibles(visible && l2.isVisibleNudge());
    l3.setVisibleWithLabel(visible);
    l3Btns.setVisibles(visible && l3.isVisibleNudge());
    l4.setVisibleWithLabel(visible);
    l4Btns.setVisibles(visible && l4.isVisibleNudge());
    l5.setVisibleWithLabel(visible);
    l5Btns.setVisibles(visible && l5.isVisibleNudge());
    l6.setVisibleWithLabel(visible);
    l6Btns.setVisibles(visible && l6.isVisibleNudge());

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
        rateSeparator.layoutComponent(rect);
        startLevel.layoutComponent(rect, 13);
        if (startLevel.isVisibleNudge()) stlBtns.layoutComponent(rect, 13);
        l1.layoutComponent(rect, 13);
        if (l1.isVisibleNudge()) l1Btns.layoutComponent(rect, 13);
        l2.layoutComponent(rect, 13);
        if (l2.isVisibleNudge()) l2Btns.layoutComponent(rect, 13);
        l3.layoutComponent(rect, 13);
        if (l3.isVisibleNudge()) l3Btns.layoutComponent(rect, 13);
        l4.layoutComponent(rect, 13);
        if (l4.isVisibleNudge()) l4Btns.layoutComponent(rect, 13);
        l5.layoutComponent(rect, 13);
        if (l5.isVisibleNudge()) l5Btns.layoutComponent(rect, 13);
        l6.layoutComponent(rect, 13);
        if (l6.isVisibleNudge()) l6Btns.layoutComponent(rect, 13);
    }
}

void GuiComponentSsgSwEnv::layoutComponentRow(juce::Rectangle<int>& rect)
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
    rateSeparator.setVisible(visible);
    startLevel.setVisibleWithLabel(visible);
    stlBtns.setVisibles(visible && startLevel.isVisibleNudge());
    l1.setVisibleWithLabel(visible);
    l1Btns.setVisibles(visible && l1.isVisibleNudge());
    l2.setVisibleWithLabel(visible);
    l2Btns.setVisibles(visible && l2.isVisibleNudge());
    l3.setVisibleWithLabel(visible);
    l3Btns.setVisibles(visible && l3.isVisibleNudge());
    l4.setVisibleWithLabel(visible);
    l4Btns.setVisibles(visible && l4.isVisibleNudge());
    l5.setVisibleWithLabel(visible);
    l5Btns.setVisibles(visible && l5.isVisibleNudge());
    l6.setVisibleWithLabel(visible);
    l6Btns.setVisibles(visible && l6.isVisibleNudge());

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
        rateSeparator.layoutComponent(rect);
        startLevel.layoutComponentRow(rect, 12);
        if (startLevel.isVisibleNudge()) stlBtns.layoutComponentRow(rect, 12);
        l1.layoutComponentRow(rect, 12);
        if (l1.isVisibleNudge()) l1Btns.layoutComponentRow(rect, 12);
        l2.layoutComponentRow(rect, 12);
        if (l2.isVisibleNudge()) l2Btns.layoutComponentRow(rect, 12);
        l3.layoutComponentRow(rect, 12);
        if (l3.isVisibleNudge()) l3Btns.layoutComponentRow(rect, 12);
        l4.layoutComponentRow(rect, 12);
        if (l4.isVisibleNudge()) l4Btns.layoutComponentRow(rect, 12);
        l5.layoutComponentRow(rect, 12);
        if (l5.isVisibleNudge()) l5Btns.layoutComponentRow(rect, 12);
        l6.layoutComponentRow(rect, 12);
        if (l6.isVisibleNudge()) l6Btns.layoutComponentRow(rect, 12);
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

    r1.getSlider().onValueChange = repaintGraph;
    r2.getSlider().onValueChange = repaintGraph;
    r3.getSlider().onValueChange = repaintGraph;
    r4.getSlider().onValueChange = repaintGraph;
    r5.getSlider().onValueChange = repaintGraph;
    r6.getSlider().onValueChange = repaintGraph;

    startLevel.getSlider().onValueChange = repaintGraph;
    l1.getSlider().onValueChange = repaintGraph;
    l2.getSlider().onValueChange = repaintGraph;
    l3.getSlider().onValueChange = repaintGraph;
    l4.getSlider().onValueChange = repaintGraph;
    l5.getSlider().onValueChange = repaintGraph;
    l6.getSlider().onValueChange = repaintGraph;
}

void GuiComponentSsgSwEnv::updateGraph(GuiEnvelopeGraph& graph) {
    graph.updateBypass(this->isEnable ? !flag.getToggleState() : flag.getToggleState());

    graph.updateSsgSwEnv(
        steps,
        loop,
        loopTo,
        loopCount,
        { nullptr, &r1.getSlider(), &r2.getSlider(), &r3.getSlider(), &r4.getSlider(), &r5.getSlider(), &r6.getSlider() },
        { &startLevel.getSlider(), &l1.getSlider(), &l2.getSlider(), &l3.getSlider(), &l4.getSlider(), &l5.getSlider(), &l6.getSlider() }
    );
}

void GuiComponentSsgSwEnv::setEnabled(bool enabled) {
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
    r1Nudge.setEnables(enabled);
    l1.setEnabled(enabled);
    l1Btns.setEnables(enabled);
    r2.setEnabled(enabled);
    r2Nudge.setEnables(enabled);
    l2.setEnabled(enabled);
    l2Btns.setEnables(enabled);
    r3.setEnabled(enabled);
    r3Nudge.setEnables(enabled);
    l3.setEnabled(enabled);
    l3Btns.setEnables(enabled);
    r4.setEnabled(enabled);
    r4Nudge.setEnables(enabled);
    l4.setEnabled(enabled);
    l4Btns.setEnables(enabled);
    r5.setEnabled(enabled);
    r5Nudge.setEnables(enabled);
    l5.setEnabled(enabled);
    l5Btns.setEnables(enabled);
    r6.setEnabled(enabled);
    r6Nudge.setEnables(enabled);
    l6.setEnabled(enabled);
    l6Btns.setEnables(enabled);
}

void GuiComponentSsgSwEnv::copyParams(CopyEnvSsgSw& copyObj) {
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
}

void GuiComponentSsgSwEnv::pasteParams(CopyEnvSsgSw& copyObj) {
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
}

void GuiComponentSsgSwEnv::importParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgSwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importSsgSwEnvParamFile, defaultDir, Io::ExtensionGlob::SsgSwEnvParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 18) return;

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
            }
        });
}

void GuiComponentSsgSwEnv::exportParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgSwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportSsgSwEnvParamFile, defaultDir.getChildFile("default.ssgSwEnv"), Io::ExtensionGlob::SsgSwEnvParam);
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

                file.replaceWithText(content);
            }
        });
}

void GuiComponentSsgSwEnv::setImportingParams(juce::StringArray& lines, int& index) {
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
}

juce::String GuiComponentSsgSwEnv::getExportedParams() {
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

    return content;
}
