#include "./SsgSwEnv.h"

#include "../../../Core/Gui/GuiRefresh.h"

#include "../../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat ssgSwEnvFormat{ "ssgSwEnv", 1 };
}

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Processor/ProcessorValues.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

void GuiComponentSsgSwEnv::applyLoopValues(bool enabled)
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

void GuiComponentSsgSwEnv::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable)
{
    this->isEnable = isEnable;

    cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "SSG SW AMP ENV",
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

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
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

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
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
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importSsgSwEnvParamFile, defaultDir, Io::ExtensionGlob::SsgSwEnvParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

                // 3.0.0 より前のファイルは、当時の処理で読み込んでから
                // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
                // 読み込みは当時のものをそのまま使う。
                if (Io::isLegacyFile(file)) {
                	juce::StringArray lines;

                	file.readLines(lines);

                	int index = 0;

                	{
                		// 読み終えてからまとめて描き直す
                		GuiRefresh::Batch batch;

                		setImportingParams(lines, index);
                	}

                	// 単体のファイルは入れ子にせず、そのまま中身として書く
                	Io::ParamWriter writer(ssgSwEnvFormat);

                	writeParams(writer, Io::ParamKey::values);
                	writer.hoist(Io::ParamKey::values);

                	Io::writeConverted(file, writer);

                	return;
                }

                auto reader = Io::ParamReader::open(file, ssgSwEnvFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
                // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
                GuiRefresh::Batch batch;

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
            }
        });
}

void GuiComponentSsgSwEnv::exportParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgSwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportSsgSwEnvParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::SsgSwEnvParam)), Io::saveGlob(Io::Extension::SsgSwEnvParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(ssgSwEnvFormat);

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

                writer.writeTo(file);
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

void GuiComponentSsgSwEnv::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

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

void GuiComponentSsgSwEnv::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

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
}
