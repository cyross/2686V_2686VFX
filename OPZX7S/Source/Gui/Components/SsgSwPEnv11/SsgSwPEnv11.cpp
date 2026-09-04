#include "./SsgSwPEnv11.h"

#include "../../../Core/Gui/GuiRefresh.h"

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

namespace
{
    // 段ごとのパラメータ名。並びが番号と一致していることが前提。
    const juce::String rateKeys[] = { CPK::SsgSwPEnv11::r1, CPK::SsgSwPEnv11::r2, CPK::SsgSwPEnv11::r3, CPK::SsgSwPEnv11::r4, CPK::SsgSwPEnv11::r5, CPK::SsgSwPEnv11::r6, CPK::SsgSwPEnv11::r7, CPK::SsgSwPEnv11::r8, CPK::SsgSwPEnv11::r9, CPK::SsgSwPEnv11::r10, CPK::SsgSwPEnv11::r11 };

    // 先頭は STL。画面の対象つまみで 0 を選んだときがこれ。
    const juce::String levelKeys[] = { CPK::SsgSwPEnv11::stl, CPK::SsgSwPEnv11::l1, CPK::SsgSwPEnv11::l2, CPK::SsgSwPEnv11::l3, CPK::SsgSwPEnv11::l4, CPK::SsgSwPEnv11::l5, CPK::SsgSwPEnv11::l6, CPK::SsgSwPEnv11::l7, CPK::SsgSwPEnv11::l8, CPK::SsgSwPEnv11::l9, CPK::SsgSwPEnv11::l10, CPK::SsgSwPEnv11::l11 };

    constexpr int rateCount = 11;
    constexpr int levelCount = 11 + 1; // 先頭の STL のぶん

    // 値の帯を置く。1 行に 4 個までなので、段の数だけ折り返した行数ぶんを取る。
    void layoutStrip(juce::Rectangle<int>& rect, GuiStepValues& strip)
    {
        strip.setBounds(rect.removeFromTop(strip.getNaturalHeight()));
    }
}

float GuiComponentSsgSwPEnv11::getStepValue(const juce::String& key) const
{
    auto* v = ctx.apvts.getRawParameterValue(paramCode + key);

    return (v != nullptr) ? v->load() : 0.0f;
}

void GuiComponentSsgSwPEnv11::setStepValue(const juce::String& key, float value)
{
    if (auto* p = ctx.apvts.getParameter(paramCode + key)) {
        p->setValueNotifyingHost(p->convertTo0to1(value));
    }
}

void GuiComponentSsgSwPEnv11::rebindRate()
{
    const int idx = juce::jlimit(0, rateCount - 1, (int)rateTarget.getValue() - 1);

    rate.getSlider().rebind(paramCode + rateKeys[idx]);

    refreshStepValues();
}

void GuiComponentSsgSwPEnv11::rebindLevel()
{
    // 対象は 0 が STL、1 以降が L1 以降。表の並びと同じ。
    const int idx = juce::jlimit(0, levelCount - 1, (int)levelTarget.getValue());

    level.getSlider().rebind(paramCode + levelKeys[idx]);

    refreshStepValues();
}

void GuiComponentSsgSwPEnv11::refreshStepValues()
{
    const int usedSteps = (int)steps.getValue();

    rateValues.labels.clear();
    rateValues.values.clear();

    for (int i = 0; i < rateCount; ++i) {
        rateValues.labels.push_back("R" + juce::String(i + 1));
        rateValues.values.push_back(getStepValue(rateKeys[i]));
    }

    rateValues.selected = juce::jlimit(0, rateCount - 1, (int)rateTarget.getValue() - 1);
    rateValues.activeCount = usedSteps;
    rateValues.lastIsRelease = true; // 最後の段はリリース。STEP の外でも必ず使う。
    rateValues.decimals = Global::floatDecimalPlaces;
    rateValues.repaint();

    levelValues.labels.clear();
    levelValues.values.clear();

    for (int i = 0; i < levelCount; ++i) {
        levelValues.labels.push_back(i == 0 ? juce::String("STL") : ("L" + juce::String(i)));
        levelValues.values.push_back(getStepValue(levelKeys[i]));
    }

    // STL は段数に関わらず使うので、薄くする境目は 1 つ後ろ。
    levelValues.selected = juce::jlimit(0, levelCount - 1, (int)levelTarget.getValue());
    levelValues.activeCount = usedSteps + 1;
    levelValues.lastIsRelease = true;
    levelValues.decimals = Global::floatDecimalPlaces;
    levelValues.repaint();
}

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

    cat.setupSwPitchCategory({
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

    // 段ごとにつまみを並べる代わりに、対象を選ぶつまみと値のつまみを 1 組ずつ置く。
    // 並びは 対象 → 値 → 各段の値。
    paramCode = code;

    rateTarget.setup({ .parent = parent, .title = "R.TG", .isReset = false, .labelFont = labelFont });
    rateTarget.setRange(1.0, (double)rateCount, 1.0);
    rateTarget.setNumDecimalPlacesToDisplay(0);
    rateTarget.setWantsKeyboardFocus(true);
    rateTarget.setExplicitFocusOrder(++tabOrder);
    rateTarget.onValueChange = [this] { rebindRate(); };

    rate.setupComponent(parent, "", "RATE", tabOrder, std::nullopt, labelFont);
    rate.getSlider().onValueChange = [this] { refreshStepValues(); };

    rateNudge.setupComponent(parent, rate.getSlider(), tabOrder);

    parent.addAndMakeVisible(rateValues);

    rateSeparator.setupComponent(parent);

    // 対象の 0 が STL、1 以降が L1 以降。
    levelTarget.setup({ .parent = parent, .title = "L.TG", .isReset = false, .labelFont = labelFont });
    levelTarget.setRange(0.0, (double)(levelCount - 1), 1.0);
    levelTarget.setNumDecimalPlacesToDisplay(0);
    levelTarget.setWantsKeyboardFocus(true);
    levelTarget.setExplicitFocusOrder(++tabOrder);
    levelTarget.onValueChange = [this] { rebindLevel(); };

    level.setupComponent(parent, "", "LEVL", tabOrder, std::nullopt, labelFont);
    level.getSlider().onValueChange = [this] { refreshStepValues(); };

    levelBtns.setupComponent(parent, level.getSlider(), tabOrder, labelFont);

    parent.addAndMakeVisible(levelValues);

    // onValueChange は値が変わらないと呼ばれないので、最初の束縛はここで明示的に行う。
    rateTarget.setValue(1, juce::dontSendNotification);
    levelTarget.setValue(0, juce::dontSendNotification);

    rebindRate();
    rebindLevel();
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
    rateTarget.setVisibleWithLabel(visible);
    rate.setVisibleWithLabel(visible);
    rateNudge.setVisibles(visible && rate.isVisibleNudge());
    rateValues.setVisible(visible);
    rateSeparator.setVisible(visible);
    levelTarget.setVisibleWithLabel(visible);
    level.setVisibleWithLabel(visible);
    levelBtns.setVisibles(visible && level.isVisibleNudge());
    levelValues.setVisible(visible);

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
        // 対象 → 値 → 各段の値、の順。
        layoutMain({ .mainRect = rect, .label = &rateTarget.label, .component = &rateTarget, .rowHeight = 13 });
        rate.layoutComponent(rect, 13);
        if (rate.isVisibleNudge()) rateNudge.layoutComponent(rect, 13);
        layoutStrip(rect, rateValues);
        rateSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &levelTarget.label, .component = &levelTarget, .rowHeight = 13 });
        level.layoutComponent(rect, 13);
        if (level.isVisibleNudge()) levelBtns.layoutComponent(rect, 13);
        layoutStrip(rect, levelValues);

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
    rateTarget.setVisibleWithLabel(visible);
    rate.setVisibleWithLabel(visible);
    rateNudge.setVisibles(visible && rate.isVisibleNudge());
    rateValues.setVisible(visible);
    rateSeparator.setVisible(visible);
    levelTarget.setVisibleWithLabel(visible);
    level.setVisibleWithLabel(visible);
    levelBtns.setVisibles(visible && level.isVisibleNudge());
    levelValues.setVisible(visible);

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
        // 対象 → 値 → 各段の値、の順。
        layoutMain({ .mainRect = rect, .label = &rateTarget.label, .component = &rateTarget, .rowHeight = 12 });
        rate.layoutComponentRow(rect, 12);
        if (rate.isVisibleNudge()) rateNudge.layoutComponentRow(rect, 12);
        layoutStrip(rect, rateValues);
        rateSeparator.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .label = &levelTarget.label, .component = &levelTarget, .rowHeight = 12 });
        level.layoutComponentRow(rect, 12);
        if (level.isVisibleNudge()) levelBtns.layoutComponentRow(rect, 12);
        layoutStrip(rect, levelValues);

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

    // 値のつまみは 1 組しかないので、そこに繋ぐだけで全段ぶんを見たことになる。
    rate.getSlider().onValueChange = [this, repaintGraph] { refreshStepValues(); repaintGraph(); };
    level.getSlider().onValueChange = [this, repaintGraph] { refreshStepValues(); repaintGraph(); };
}

void GuiComponentSsgSwPEnv11::updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx) {
    // つまみは 1 組しかないので、値を並べ直して渡す。
    // R 側の [0] は使わない (元のつまみ配列と同じ並び)。
    std::array<float, (size_t)levelCount> rArr{};
    std::array<float, (size_t)levelCount> lArr{};

    for (int i = 0; i < rateCount; ++i) rArr[(size_t)i + 1] = getStepValue(rateKeys[i]);
    for (int i = 0; i < levelCount; ++i) lArr[(size_t)i] = getStepValue(levelKeys[i]);

    graph.updateBypass(this->isEnable ? !flag.getToggleState() : flag.getToggleState());

    graph.updateSsgSwPEnv11(
        steps,
        loop,
        loopTo,
        loopCount,
        rArr, (float)rate.getSlider().getMaximum(),
        lArr, (float)level.getSlider().getMaximum(),
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
    rateTarget.setEnabled(enabled);
    rate.setEnabled(enabled);
    rateNudge.setEnables(enabled);
    rateSeparator.setEnabled(enabled);
    levelTarget.setEnabled(enabled);
    level.setEnabled(enabled);
    levelBtns.setEnables(enabled);
}

void GuiComponentSsgSwPEnv11::copyParams(CopyPEnvSsgSw11& copyObj) {
    copyObj.flag = flag.getToggleState();
    copyObj.steps = steps.getValue();
    copyObj.loop = loop.getToggleState();
    copyObj.loopTo = loopTo.getValue();
    copyObj.loopCount = loopCount.getValue();
    copyObj.stl = getStepValue(levelKeys[0]);

    for (int i = 0; i < rateCount; ++i) {
        copyObj.r[i] = getStepValue(rateKeys[i]);
        copyObj.l[i] = getStepValue(levelKeys[i + 1]);
    }
}

void GuiComponentSsgSwPEnv11::pasteParams(CopyPEnvSsgSw11& copyObj) {
    flag.setToggleState(copyObj.flag, juce::sendNotification);
    steps.setValue(copyObj.steps, juce::sendNotification);
    loop.setToggleState(copyObj.loop, juce::sendNotification);
    loopTo.setValue(copyObj.loopTo, juce::sendNotification);
    loopCount.setValue(copyObj.loopCount, juce::sendNotification);
    setStepValue(levelKeys[0], copyObj.stl);

    for (int i = 0; i < rateCount; ++i) {
        setStepValue(rateKeys[i], copyObj.r[i]);
        setStepValue(levelKeys[i + 1], copyObj.l[i]);
    }

    refreshStepValues();
}

void GuiComponentSsgSwPEnv11::importParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgSwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importSsgSwEnvParamFile, defaultDir, Io::ExtensionGlob::SsgSwPEnvParam11);
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
                    Io::ParamWriter writer(ssgSwPEnv11Format);

                    writeParams(writer, Io::ParamKey::values);
                    writer.hoist(Io::ParamKey::values);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, ssgSwPEnv11Format);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
                // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
                GuiRefresh::Batch batch;

                flag.setToggleState(reader->getBool("flag", flag.getToggleState()), juce::sendNotification);
                steps.setValue(reader->getInt("steps", (int)steps.getValue()), juce::sendNotification);
                loop.setToggleState(reader->getBool("loop", loop.getToggleState()), juce::sendNotification);
                loopTo.setValue(reader->getInt("loopTo", (int)loopTo.getValue()), juce::sendNotification);
                loopCount.setValue(reader->getInt("loopCount", (int)loopCount.getValue()), juce::sendNotification);
                setStepValue(levelKeys[0], reader->getFloat("startLevel", getStepValue(levelKeys[0])));

                for (int i = 0; i < rateCount; ++i) {
                    const juce::String no(i + 1);

                    setStepValue(rateKeys[i], reader->getFloat("r" + no, getStepValue(rateKeys[i])));
                    setStepValue(levelKeys[i + 1], reader->getFloat("l" + no, getStepValue(levelKeys[i + 1])));
                }

                refreshStepValues();
            }
        });
}

void GuiComponentSsgSwPEnv11::exportParams() {
    juce::File defaultDir(ctx.audioProcessor.defaultSsgSwEnvParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportSsgSwEnvParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::SsgSwPEnvParam11)), Io::saveGlob(Io::Extension::SsgSwPEnvParam11));
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
                writer.set("startLevel", getStepValue(levelKeys[0]));

                for (int i = 0; i < rateCount; ++i) {
                    const juce::String no(i + 1);

                    writer.set("r" + no, getStepValue(rateKeys[i]));
                    writer.set("l" + no, getStepValue(levelKeys[i + 1]));
                }

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
    setStepValue(levelKeys[0], lines[index++].getFloatValue());

    for (int i = 0; i < rateCount; ++i) {
        setStepValue(rateKeys[i], lines[index++].getFloatValue());
        setStepValue(levelKeys[i + 1], lines[index++].getFloatValue());
    }

    refreshStepValues();
}

void GuiComponentSsgSwPEnv11::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    flag.setToggleState(r.getBool("flag", flag.getToggleState()), juce::sendNotification);
    steps.setValue(r.getInt("steps", (int)steps.getValue()), juce::sendNotification);
    loop.setToggleState(r.getBool("loop", loop.getToggleState()), juce::sendNotification);
    loopTo.setValue(r.getInt("loopTo", (int)loopTo.getValue()), juce::sendNotification);
    loopCount.setValue(r.getInt("loopCount", (int)loopCount.getValue()), juce::sendNotification);
    setStepValue(levelKeys[0], r.getFloat("startLevel", getStepValue(levelKeys[0])));

    for (int i = 0; i < rateCount; ++i) {
        const juce::String no(i + 1);

        setStepValue(rateKeys[i], r.getFloat("r" + no, getStepValue(rateKeys[i])));
        setStepValue(levelKeys[i + 1], r.getFloat("l" + no, getStepValue(levelKeys[i + 1])));
    }

    refreshStepValues();
}

juce::String GuiComponentSsgSwPEnv11::getExportedParams() {
    juce::String content = "";

    content += juce::String(flag.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(steps.getValue()) + "\n";
    content += juce::String(loop.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(loopTo.getValue()) + "\n";
    content += juce::String(loopCount.getValue()) + "\n";
    content += juce::String(getStepValue(levelKeys[0]), Global::floatDecimalPlaces) + "\n";

    for (int i = 0; i < rateCount; ++i) {
        content += juce::String(getStepValue(rateKeys[i]), Global::floatDecimalPlaces) + "\n";
        content += juce::String(getStepValue(levelKeys[i + 1]), Global::floatDecimalPlaces) + "\n";
    }

    return content;
}

void GuiComponentSsgSwPEnv11::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    w.set("flag", flag.getToggleState());
    w.set("steps", (float)steps.getValue());
    w.set("loop", loop.getToggleState());
    w.set("loopTo", (float)loopTo.getValue());
    w.set("loopCount", (float)loopCount.getValue());
    w.set("startLevel", getStepValue(levelKeys[0]));

    for (int i = 0; i < rateCount; ++i) {
        const juce::String no(i + 1);

        w.set("r" + no, getStepValue(rateKeys[i]));
        w.set("l" + no, getStepValue(levelKeys[i + 1]));
    }
}
