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

    r1.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::r1, .title = "R1", .isReset = true, .labelFont = labelFont });
    r1.setWantsKeyboardFocus(true);
    r1.setExplicitFocusOrder(++tabOrder);

    r2.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::r2, .title = "R2", .isReset = true, .labelFont = labelFont });
    r2.setWantsKeyboardFocus(true);
    r2.setExplicitFocusOrder(++tabOrder);

    r3.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::r3, .title = "R3", .isReset = true, .labelFont = labelFont });
    r3.setWantsKeyboardFocus(true);
    r3.setExplicitFocusOrder(++tabOrder);

    r4.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::r4, .title = "R4", .isReset = true, .labelFont = labelFont });
    r4.setWantsKeyboardFocus(true);
    r4.setExplicitFocusOrder(++tabOrder);

    r5.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::r5, .title = "R5", .isReset = true, .labelFont = labelFont });
    r5.setWantsKeyboardFocus(true);
    r5.setExplicitFocusOrder(++tabOrder);

    r6.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::r6, .title = "R6", .isReset = true, .labelFont = labelFont });
    r6.setWantsKeyboardFocus(true);
    r6.setExplicitFocusOrder(++tabOrder);

    rateSeparator.setupComponent(parent);

    startLevel.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::stl, .title = "STL", .isReset = true, .labelFont = labelFont });
    startLevel.setWantsKeyboardFocus(true);
    startLevel.setExplicitFocusOrder(++tabOrder);

    stlBtns.setupComponent(parent, startLevel, tabOrder, labelFont);

    l1.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::l1, .title = "L1", .isReset = true, .labelFont = labelFont });
    l1.setWantsKeyboardFocus(true);
    l1.setExplicitFocusOrder(++tabOrder);

    l1Btns.setupComponent(parent, l1, tabOrder, labelFont);

    l2.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::l2, .title = "L2", .isReset = true, .labelFont = labelFont });
    l2.setWantsKeyboardFocus(true);
    l2.setExplicitFocusOrder(++tabOrder);

    l2Btns.setupComponent(parent, l2, tabOrder, labelFont);

    l3.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::l3, .title = "L3", .isReset = true, .labelFont = labelFont });
    l3.setWantsKeyboardFocus(true);
    l3.setExplicitFocusOrder(++tabOrder);

    l3Btns.setupComponent(parent, l3, tabOrder, labelFont);

    l4.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::l4, .title = "L4", .isReset = true, .labelFont = labelFont });
    l4.setWantsKeyboardFocus(true);
    l4.setExplicitFocusOrder(++tabOrder);

    l4Btns.setupComponent(parent, l4, tabOrder, labelFont);

    l5.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::l5, .title = "L5", .isReset = true, .labelFont = labelFont });
    l5.setWantsKeyboardFocus(true);
    l5.setExplicitFocusOrder(++tabOrder);

    l5Btns.setupComponent(parent, l5, tabOrder, labelFont);

    l6.setup({ .parent = parent, .id = code + CPK::SsgSwEnv::l6, .title = "L6", .isReset = true, .labelFont = labelFont });
    l6.setWantsKeyboardFocus(true);
    l6.setExplicitFocusOrder(++tabOrder);

    l6Btns.setupComponent(parent, l6, tabOrder, labelFont);
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
    r2.setVisibleWithLabel(visible);
    r3.setVisibleWithLabel(visible);
    r4.setVisibleWithLabel(visible);
    r5.setVisibleWithLabel(visible);
    r6.setVisibleWithLabel(visible);
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
    r2.setVisibleWithLabel(visible);
    r3.setVisibleWithLabel(visible);
    r4.setVisibleWithLabel(visible);
    r5.setVisibleWithLabel(visible);
    r6.setVisibleWithLabel(visible);
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

void GuiComponentSsgSwEnv::updateGraph(GuiEnvelopeGraph& graph) {
    graph.updateBypass(this->isEnable ? !flag.getToggleState() : flag.getToggleState());

    graph.updateSsgSwEnv(
        steps,
        loop,
        loopTo,
        loopCount,
        { nullptr, &r1, &r2, &r3, &r4, &r5, &r6 },
        { &startLevel, &l1, &l2, &l3, &l4, &l5, &l6 }
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
    l1.setEnabled(enabled);
    l1Btns.setEnables(enabled);
    r2.setEnabled(enabled);
    l2.setEnabled(enabled);
    l2Btns.setEnables(enabled);
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
