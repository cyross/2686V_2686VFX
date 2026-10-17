#include "./StepEnv.h"

#include "Shared/Core/Gui/GuiRefresh.h"
#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiGraphValues.h"
#include "Shared/Core/Gui/GuiParamEdit.h"
#include "Shared/Core/Const/ConstGlobal.h"

namespace
{
    // 値の帯を置く。段の数だけ折り返した行数ぶんを取る。
    void layoutStrip(juce::Rectangle<int>& rect, GuiStepValues& strip)
    {
        strip.setBounds(rect.removeFromTop(strip.getNaturalHeight()));
    }

    // 3.6.3 までの [11] は 10 段 + リリース。11 番目がリリース。
    constexpr int legacySteps = 10;
    constexpr int legacyRelease = 11;
}

GuiComponentStepEnv::GuiComponentStepEnv(const GuiContext& context, const Spec& s) :
    GuiBase(context),
    spec(s),
    cat(context),
    flag(context),
    flagSeparator(context),
    steps(context),
    stepsSeparator(context),
    keep(context),
    keepSeparator(context),
    loop(context),
    loopTo(context),
    loopCount(context),
    loopSeparator(context),
    rateTarget(context),
    rate(context),
    rateNudge(context),
    rateSeparator(context),
    levelTarget(context),
    level(context),
    levelSteps(context),
    ampBtns(context),
    pitchBtns(context),
    releaseSeparator(context),
    releaseRate(context),
    releaseLevel(context),
    endLevelSeparator(context),
    endLevelEnable(context),
    endLevel(context)
{
}

StepTable* GuiComponentStepEnv::table() const
{
    if (paramCode.isEmpty()) return nullptr;

    return ctx.audioProcessor.stepTables.find(paramCode + spec.tableKey);
}

void GuiComponentStepEnv::changed()
{
    if (repaintGraph) repaintGraph();
}

void GuiComponentStepEnv::markSeen()
{
    if (auto* t = table()) seenVersion = t->version.load();
}

void GuiComponentStepEnv::timerCallback()
{
    auto* t = table();

    if (t == nullptr || t->version.load() == seenVersion) return;

    // 外から書き換わった。つまみと帯と絵を映し直す
    applyStepCount();
    changed();
}

// ---------------------------------------------------------------- 組み立て
void GuiComponentStepEnv::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& flagKey, const juce::String& flagText, bool enable)
{
    this->isEnable = enable;

    const GuiCategoryLabel::Config catConfig{ .parent = parent, .title = spec.title, .enableChangeDetailVisible = true };

    if (isAmp()) cat.setupSwAmpCategory(catConfig);
    else cat.setupSwPitchCategory(catConfig);

    m_flagKey = flagKey;

    flag.setup({ .parent = parent, .id = code + flagKey, .title = flagText, .isReset = true });
    flag.setWantsKeyboardFocus(true);
    flag.setExplicitFocusOrder(++tabOrder);

    flagSeparator.setupComponent(parent);

    steps.setup({ .parent = parent, .id = code + spec.steps, .title = "STEP", .isReset = true, .labelFont = labelFont });
    steps.setWantsKeyboardFocus(true);
    steps.setExplicitFocusOrder(++tabOrder);
    steps.onValueChange = [this] {
        applyLoopValues(loop.getToggleState());
        applyStepCount();
        changed();
        };

    stepsSeparator.setupComponent(parent);

    keep.setup({ .parent = parent, .id = code + spec.keep, .title = "KEEP", .isReset = true });
    keep.setWantsKeyboardFocus(true);
    keep.setExplicitFocusOrder(++tabOrder);

    keepSeparator.setupComponent(parent);

    loop.setup({ .parent = parent, .id = code + spec.loop, .title = "LOOP", .isReset = true });
    loop.setWantsKeyboardFocus(true);
    loop.setExplicitFocusOrder(++tabOrder);
    // 押したときだけ、ループできる形へ STEP と LOOP.TO を直す
    loop.onClick = [this] { applyLoopValues(loop.getToggleState()); };

    loopTo.setup({ .parent = parent, .id = code + spec.loopTo, .title = "LOOP.TO", .isReset = true, .labelFont = labelFont });
    loopTo.setWantsKeyboardFocus(true);
    loopTo.setExplicitFocusOrder(++tabOrder);
    loopTo.onValueChange = [this] { applyLoopValues(loop.getToggleState()); changed(); };

    loopCount.setup({ .parent = parent, .id = code + spec.loopCount, .title = "LOOP.CNT", .isReset = true, .labelFont = labelFont });
    loopCount.setWantsKeyboardFocus(true);
    loopCount.setExplicitFocusOrder(++tabOrder);
    loopCount.onValueChange = [this] { changed(); };

    loopSeparator.setupComponent(parent);

    // 段の R。対象を選ぶつまみと値のつまみを 1 組だけ置く。並びは 対象 → 値 → 各段の値。
    paramCode = code;

    rateTarget.setup({ .parent = parent, .title = "R.TARGET", .isReset = false, .labelFont = labelFont });
    rateTarget.setRange(1.0, 2.0, 1.0);
    rateTarget.setNumDecimalPlacesToDisplay(0);
    rateTarget.setWantsKeyboardFocus(true);
    rateTarget.setExplicitFocusOrder(++tabOrder);
    rateTarget.onValueChange = [this] { rebindRate(); };

    // 値は表にあるので、つまみはパラメータへ繋がない
    rate.setupComponent(parent, "", "RATE", tabOrder, std::nullopt, labelFont, false);
    rate.getSlider().setRange(spec.rateMin, spec.rateMax, spec.rateInterval);
    rate.getSlider().onValueChange = [this] { onRateChanged(); };

    rateNudge.setupComponent(parent, rate.getSlider(), tabOrder);

    parent.addAndMakeVisible(rateValues);

    rateSeparator.setupComponent(parent);

    // 段の L。対象の 0 が開始の値 (パラメータ)、1 以降が表の段。
    levelTarget.setup({ .parent = parent, .title = spec.levelTargetLabel, .isReset = false, .labelFont = labelFont });
    levelTarget.setRange(0.0, 1.0, 1.0);
    levelTarget.setNumDecimalPlacesToDisplay(0);
    levelTarget.setWantsKeyboardFocus(true);
    levelTarget.setExplicitFocusOrder(++tabOrder);
    levelTarget.onValueChange = [this] { rebindLevel(); };

    level.setupComponent(parent, "", spec.levelLabel, tabOrder, std::nullopt, labelFont, false);
    setLevelRange();
    level.getSlider().onValueChange = [this] { onLevelChanged(); };

    if (isAmp()) ampBtns.setupComponent(parent, level.getSlider(), tabOrder, labelFont);
    else pitchBtns.setupComponent(parent, level.getSlider(), tabOrder, labelFont);

    parent.addAndMakeVisible(levelValues);

    // リリース
    releaseSeparator.setupComponent(parent);

    releaseRate.setup({ .parent = parent, .id = code + spec.rr, .title = "RR", .isReset = true, .labelFont = labelFont });
    releaseRate.setWantsKeyboardFocus(true);
    releaseRate.setExplicitFocusOrder(++tabOrder);
    releaseRate.onValueChange = [this] { changed(); };

    releaseLevel.setup({ .parent = parent, .id = code + spec.lr, .title = "LR", .isReset = true, .labelFont = labelFont });
    releaseLevel.setWantsKeyboardFocus(true);
    releaseLevel.setExplicitFocusOrder(++tabOrder);
    releaseLevel.onValueChange = [this] { changed(); };

    endLevelSeparator.setupComponent(parent);

    endLevelEnable.setup({ .parent = parent, .id = code + spec.endlEnable, .title = "Use Endl", .isReset = true });
    endLevelEnable.setWantsKeyboardFocus(true);
    endLevelEnable.setExplicitFocusOrder(++tabOrder);
    endLevelEnable.onStateChange = [this] { changed(); };

    endLevel.setup({ .parent = parent, .id = code + spec.endl, .title = "ENDL", .isReset = true, .labelFont = labelFont });
    endLevel.setWantsKeyboardFocus(true);
    endLevel.setExplicitFocusOrder(++tabOrder);
    endLevel.onValueChange = [this] { changed(); };

    // 音量だけ、LEVEL・LR・ENDL を決まった刻みへ丸められる
    if (isAmp()) levelSteps.setupComponent(parent, { &level.getSlider(), &releaseLevel, &endLevel }, tabOrder, "STEPS", labelFont);

    flag.onStateChange = [this] { changed(); };
    keep.onStateChange = [this] { changed(); };
    loop.onStateChange = [this] { changed(); };

    rateTarget.setValue(1, juce::dontSendNotification);
    levelTarget.setValue(0, juce::dontSendNotification);

    rebind(code);

    // 札の入り切りは、押したときだけでなく TARGET の切り替えや
    // プリセットの読み込みでも変わる。状態の変化を受ける。
    flag.watchToggle([this] { applyActive(); });
    loop.watchToggle([this] { applyActive(); });
    endLevelEnable.watchToggle([this] { applyActive(); });

    applyActive();

    startTimerHz(10);
}

// 束縛先を丸ごと差し替える。setup で組んだ見た目はそのままに、繋ぎだけを張り替える。
void GuiComponentStepEnv::rebind(const juce::String& code)
{
    paramCode = code;

    flag.rebind(code + m_flagKey);
    steps.rebind(code + spec.steps);
    keep.rebind(code + spec.keep);
    loop.rebind(code + spec.loop);
    loopTo.rebind(code + spec.loopTo);
    loopCount.rebind(code + spec.loopCount);
    releaseRate.rebind(code + spec.rr);
    releaseLevel.rebind(code + spec.lr);
    endLevelEnable.rebind(code + spec.endlEnable);
    endLevel.rebind(code + spec.endl);

    // 区分の右クリックで表も写すための印
    const auto key = code + spec.tableKey;

    rate.getSlider().getProperties().set(ParamEdit::Property::stepTable, key);
    level.getSlider().getProperties().set(ParamEdit::Property::stepTable, key);

    applyStepCount();
}

void GuiComponentStepEnv::setLevelRange()
{
    level.getSlider().setRange(spec.levelMin, spec.levelMax, spec.levelInterval);
    level.getSlider().setNumDecimalPlacesToDisplay(spec.levelInterval >= 1.0 ? 0 : spec.levelDecimals);
}

// ---------------------------------------------------------------- 段の数
// 対象のつまみの範囲と帯を、段の数に合わせる。帯の行数が変わったときは
// 区分の高さが変わるので、並べ直してもらう。
void GuiComponentStepEnv::applyStepCount()
{
    const int n = stepCount();
    const int rowsBefore = rateValues.getRowCount() + levelValues.getRowCount();

    // 範囲は幅を持たせる必要があるので、1 段のときも 1〜2 にして先で丸める
    rateTarget.setRange(1.0, (double)juce::jmax(2, n), 1.0);
    levelTarget.setRange(0.0, (double)n, 1.0);

    if ((int)rateTarget.getValue() > n) rateTarget.setValue(n, juce::dontSendNotification);
    if ((int)levelTarget.getValue() > n) levelTarget.setValue(n, juce::dontSendNotification);

    rebindRate();
    rebindLevel();

    markSeen();

    const int rowsAfter = rateValues.getRowCount() + levelValues.getRowCount();

    if (rowsBefore != rowsAfter && rowsBefore > 0) ctx.editor.resized();
}

void GuiComponentStepEnv::applyLoopValues(bool enabled)
{
    // setValue は同期で onValueChange を呼び返すので、入れ子ぶんは弾く
    if (isApplyingLoopValues) return;

    const juce::ScopedValueSetter<bool> guard(isApplyingLoopValues, true);

    loopTo.setRange(0.0, (double)juce::jmax(1, stepCount() - 1), 1.0);

    if (enabled)
    {
        int stepsValue = stepCount();

        // Steps が 1 のときはループできないため、Steps を 2 にする
        if (stepsValue < 2) {
            stepsValue = 2;
            steps.setValue(stepsValue);
        }

        // Steps - LoopTo が 2 未満のときは、LoopTo を Steps - 2 にする
        if (stepsValue - (int)loopTo.getValue() < 2) loopTo.setValue(stepsValue - 2);
    }

    refreshStepValues();
}

// ---------------------------------------------------------------- 値のつまみ
void GuiComponentStepEnv::rebindRate()
{
    const int idx = juce::jlimit(1, stepCount(), (int)rateTarget.getValue());

    if (auto* t = table()) {
        const juce::ScopedValueSetter<bool> loading(isLoadingValue, true);

        rate.getSlider().setValue(t->rate(idx), juce::dontSendNotification);
    }

    refreshStepValues();
}

void GuiComponentStepEnv::rebindLevel()
{
    const int idx = juce::jlimit(0, stepCount(), (int)levelTarget.getValue());

    // 0 は開始の値。これだけはパラメータなので、つまみをそのまま繋ぐ
    if (idx == 0) {
        level.getSlider().rebind(paramCode + spec.stl);
    }
    else {
        level.getSlider().unbind();
        setLevelRange();

        if (auto* t = table()) {
            const juce::ScopedValueSetter<bool> loading(isLoadingValue, true);

            level.getSlider().setValue(t->level(idx), juce::dontSendNotification);
        }
    }

    refreshStepValues();
}

void GuiComponentStepEnv::onRateChanged()
{
    if (isLoadingValue) return;

    if (auto* t = table()) t->setRate(juce::jlimit(1, stepCount(), (int)rateTarget.getValue()), (float)rate.getSlider().getValue());

    markSeen();
    refreshStepValues();
    changed();
}

void GuiComponentStepEnv::onLevelChanged()
{
    if (isLoadingValue) return;

    const int idx = juce::jlimit(0, stepCount(), (int)levelTarget.getValue());

    // 0 (開始の値) はパラメータに繋いであるので、つまみの側で書き込まれる
    if (idx > 0) {
        if (auto* t = table()) t->setLevel(idx, (float)level.getSlider().getValue());
    }

    markSeen();
    refreshStepValues();
    changed();
}

// 帯に出す値を今の状態から作り直す。使っている段だけを並べる。
void GuiComponentStepEnv::refreshStepValues()
{
    if (paramCode.isEmpty()) return;

    const int n = stepCount();
    const auto* t = table();

    rateValues.labels.clear();
    rateValues.values.clear();

    for (int s = 1; s <= n; ++s) {
        rateValues.labels.push_back("R" + juce::String(s));
        rateValues.values.push_back(t != nullptr ? t->rate(s) : 0.0f);
    }

    rateValues.selected = juce::jlimit(1, n, (int)rateTarget.getValue()) - 1;
    rateValues.activeCount = -1;
    rateValues.decimals = Global::floatDecimalPlaces;
    rateValues.repaint();

    levelValues.labels.clear();
    levelValues.values.clear();

    auto* stl = ctx.apvts.getRawParameterValue(paramCode + spec.stl);

    levelValues.labels.push_back(spec.startLabel);
    levelValues.values.push_back(stl != nullptr ? stl->load() : 0.0f);

    for (int s = 1; s <= n; ++s) {
        levelValues.labels.push_back(spec.levelPrefix + juce::String(s));
        levelValues.values.push_back(t != nullptr ? t->level(s) : 0.0f);
    }

    levelValues.selected = juce::jlimit(0, n, (int)levelTarget.getValue());
    levelValues.activeCount = -1;
    levelValues.decimals = spec.levelInterval >= 1.0 ? 0 : spec.levelDecimals;
    levelValues.repaint();
}

// ---------------------------------------------------------------- 並べる
void GuiComponentStepEnv::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    const bool visible = cat.isDetailVisible();

    flag.setVisible(visible);
    flagSeparator.setVisible(visible);
    steps.setVisibleWithLabel(visible);
    stepsSeparator.setVisible(visible);
    keep.setVisible(visible);
    keepSeparator.setVisible(visible);
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
    levelSteps.setVisibleWithLabel(visible && isAmp());
    ampBtns.setVisibles(visible && isAmp() && level.isVisibleNudge());
    pitchBtns.setVisibles(visible && !isAmp() && level.isVisibleNudge());
    levelValues.setVisible(visible);
    releaseSeparator.setVisible(visible);
    releaseRate.setVisibleWithLabel(visible);
    releaseLevel.setVisibleWithLabel(visible);
    endLevelSeparator.setVisible(visible);
    endLevelEnable.setVisible(visible);
    endLevel.setVisibleWithLabel(visible);

    if (!visible) return;

    constexpr int h = 13;

    layoutMain({ .mainRect = rect, .component = &flag });
    flagSeparator.layoutComponent(rect);
    layoutMain({ .mainRect = rect, .label = &steps.label, .component = &steps, .rowHeight = h });
    stepsSeparator.layoutComponent(rect);
    layoutMain({ .mainRect = rect, .component = &keep, .rowHeight = h });
    keepSeparator.layoutComponent(rect);
    layoutMain({ .mainRect = rect, .component = &loop, .rowHeight = h });
    layoutMain({ .mainRect = rect, .label = &loopTo.label, .component = &loopTo, .rowHeight = h });
    layoutMain({ .mainRect = rect, .label = &loopCount.label, .component = &loopCount, .rowHeight = h });
    loopSeparator.layoutComponent(rect);

    // 対象 → 値 → 各段の値、の順。
    layoutMain({ .mainRect = rect, .label = &rateTarget.label, .component = &rateTarget, .rowHeight = h });
    rate.layoutComponent(rect, h);
    if (rate.isVisibleNudge()) rateNudge.layoutComponent(rect, h);
    layoutStrip(rect, rateValues);
    rateSeparator.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .label = &levelTarget.label, .component = &levelTarget, .rowHeight = h });
    level.layoutComponent(rect, h);

    if (isAmp()) {
        levelSteps.layoutComponent(rect, h);
        if (level.isVisibleNudge()) ampBtns.layoutComponent(rect, h);
    }
    else if (level.isVisibleNudge()) {
        pitchBtns.layoutComponent(rect, h);
    }

    layoutStrip(rect, levelValues);

    releaseSeparator.layoutComponent(rect);
    layoutMain({ .mainRect = rect, .label = &releaseRate.label, .component = &releaseRate, .rowHeight = h });
    layoutMain({ .mainRect = rect, .label = &releaseLevel.label, .component = &releaseLevel, .rowHeight = h });

    endLevelSeparator.layoutComponent(rect);
    layoutMain({ .mainRect = rect, .component = &endLevelEnable, .rowHeight = h });
    layoutMain({ .mainRect = rect, .label = &endLevel.label, .component = &endLevel, .rowHeight = h });

    rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}

void GuiComponentStepEnv::layoutComponentRow(juce::Rectangle<int>& rect)
{
    // 行の詰め方が違うだけなので、並びは同じものを使う
    layoutComponent(rect);
}

// ---------------------------------------------------------------- グラフ
void GuiComponentStepEnv::setupGraph(std::function<void()> repaint)
{
    repaintGraph = std::move(repaint);
}

void GuiComponentStepEnv::updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx)
{
    const auto v = GuiGraphValues::stepEnv256(ctx.apvts, ctx.audioProcessor.stepTables, paramCode, spec.tableKey,
        spec.steps, spec.loop, spec.loopTo, spec.loopCount, spec.stl, spec.rr, spec.lr, (float)spec.levelMax);

    graph.updateBypass(isCategoryBypassed());

    // KEEP のときはカーブを効かせない。音の側も補間そのものを止めてある。
    const bool keepOn = keep.getToggleState();

    graph.setKeepLevels(keepOn);

    if (isAmp()) graph.updateSsgSwEnv256(v, keepOn ? nullptr : p_curveCore, keepOn ? false : isCurveMode, posIdx);
    else graph.updateSsgSwPEnv256(v, keepOn ? nullptr : p_curveCore, keepOn ? false : isCurveMode, posIdx);
}

// ---------------------------------------------------------------- 止める
void GuiComponentStepEnv::setEnabled(bool enabled)
{
    outerEnabled = enabled;

    cat.setEnabled(enabled);

    applyActive();
}

// 効いていないつまみは押せなくする。LOOP.TO / LOOP.CNT は LOOP が切れていても、
// ENDL は Use Endl が切れていても止める。
void GuiComponentStepEnv::applyActive()
{
    const bool active = outerEnabled && !isCategoryBypassed();
    const bool looping = active && loop.getToggleState();

    flag.setEnabled(outerEnabled);

    flagSeparator.setEnabled(active);
    steps.setEnabledWithLabel(active);
    stepsSeparator.setEnabled(active);

    // トグルは止めずに薄くする。止めると、そのあいだに来た値を捨ててしまう。
    keep.setEnabled(outerEnabled);
    keep.setDimmed(!active);
    keepSeparator.setEnabled(active);

    loop.setEnabled(outerEnabled);
    loop.setDimmed(!active);
    loopTo.setEnabledWithLabel(looping);
    loopCount.setEnabledWithLabel(looping);
    loopSeparator.setEnabled(active);

    rateTarget.setEnabledWithLabel(active);
    rate.setEnabled(active);
    rateNudge.setEnables(active);
    rateValues.setEnabled(active);
    rateSeparator.setEnabled(active);

    levelTarget.setEnabledWithLabel(active);
    level.setEnabled(active);
    levelSteps.setEnabledWithLabel(active);
    ampBtns.setEnables(active);
    pitchBtns.setEnables(active);
    levelValues.setEnabled(active);

    releaseSeparator.setEnabled(active);
    releaseRate.setEnabledWithLabel(active);
    releaseLevel.setEnabledWithLabel(active);

    endLevelSeparator.setEnabled(active);
    endLevelEnable.setEnabled(outerEnabled);
    endLevelEnable.setDimmed(!active);
    endLevel.setEnabledWithLabel(active && endLevelEnable.getToggleState());
}

// ---------------------------------------------------------------- ファイル
void GuiComponentStepEnv::importParams()
{
    // 一覧から選ぶ画面を出す。3.6.3 までの [11] のファイルも同じ種類として出る。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultSsgSwEnvParamDir,
        { spec.browserKind },
        [this](const juce::File& file) { applyParamsFile(file); });
}

void GuiComponentStepEnv::applyParamsFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;

    ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイル ([11] の行の並び)。読んでから新しい形式で書き出す。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            GuiRefresh::Batch batch;

            setImportingParams(lines, index);
        }

        Io::ParamWriter writer(Io::ParamFormat{ spec.fileFormat, 1 });

        writeParams(writer, Io::ParamKey::values);
        writer.hoist(Io::ParamKey::values);

        Io::writeConverted(file, writer);

        return;
    }

    // 3.6.3 までの [11] のファイルは、[256] へ写して読む
    if (auto legacy = Io::ParamReader::open(file, Io::ParamFormat{ spec.legacyFileFormat, 1 }, false)) {
        GuiRefresh::Batch batch;

        readValues(*legacy);
        return;
    }

    auto reader = Io::ParamReader::open(file, Io::ParamFormat{ spec.fileFormat, 1 });

    if (!reader.has_value()) return;

    GuiRefresh::Batch batch;

    readValues(*reader);
}

void GuiComponentStepEnv::exportParams()
{
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultSsgSwEnvParamDir,
        { spec.browserKind }, spec.extension,
        [this](const juce::File& file) { writeParamsFile(file); });
}

void GuiComponentStepEnv::writeParamsFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultSsgSwEnvParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(Io::ParamFormat{ spec.fileFormat, 1 });

    writeValues(writer);

    writer.writeTo(file);
}

// 段の値は r / l の並びに入れる。[11] のファイル (r1〜r11 / l1〜l11) も読める。
void GuiComponentStepEnv::readValues(const Io::ParamReader& r)
{
    flag.setToggleState(r.getBool("flag", flag.getToggleState()), juce::sendNotification);
    steps.setValue(r.getInt("steps", (int)steps.getValue()), juce::sendNotification);
    loop.setToggleState(r.getBool("loop", loop.getToggleState()), juce::sendNotification);
    loopTo.setValue(r.getInt("loopTo", (int)loopTo.getValue()), juce::sendNotification);
    loopCount.setValue(r.getInt("loopCount", (int)loopCount.getValue()), juce::sendNotification);
    keep.setToggleState(r.getBool("keep", keep.getToggleState()), juce::sendNotification);
    endLevel.setValue(r.getFloat("endLevel", (float)endLevel.getValue()), juce::sendNotification);
    endLevelEnable.setToggleState(r.getBool("endLevelEnable", endLevelEnable.getToggleState()), juce::sendNotification);

    if (auto* p = ctx.apvts.getParameter(paramCode + spec.stl)) {
        auto* raw = ctx.apvts.getRawParameterValue(paramCode + spec.stl);

        p->setValueNotifyingHost(p->convertTo0to1(r.getFloat("startLevel", raw != nullptr ? raw->load() : 0.0f)));
    }

    auto* t = table();
    const auto names = r.keys();

    if (names.contains("rates") || names.contains("levels")) {
        // [256] の形
        releaseRate.setValue(r.getFloat("releaseRate", (float)releaseRate.getValue()), juce::sendNotification);
        releaseLevel.setValue(r.getFloat("releaseLevel", (float)releaseLevel.getValue()), juce::sendNotification);

        if (t != nullptr) {
            const auto rates = r.getFloatArray("rates");
            const auto levels = r.getFloatArray("levels");

            for (int s = 1; s <= StepTable::size; ++s) {
                t->setRate(s, s <= (int)rates.size() ? rates[(size_t)s - 1] : t->defaultR);
                t->setLevel(s, s <= (int)levels.size() ? levels[(size_t)s - 1] : t->defaultL);
            }
        }
    }
    else {
        // 3.6.3 までの [11] の形。1〜10 段が表、11 番目がリリース。
        for (int i = 1; i <= legacyRelease; ++i) {
            const juce::String no(i);

            if (i == legacyRelease) {
                if (names.contains("r" + no)) releaseRate.setValue(r.getFloat("r" + no), juce::sendNotification);
                if (names.contains("l" + no)) releaseLevel.setValue(r.getFloat("l" + no), juce::sendNotification);
                continue;
            }

            if (t == nullptr) continue;

            if (names.contains("r" + no)) t->setRate(i, r.getFloat("r" + no));
            if (names.contains("l" + no)) t->setLevel(i, r.getFloat("l" + no));
        }
    }

    applyStepCount();
    changed();
}

void GuiComponentStepEnv::writeValues(Io::ParamWriter& w)
{
    w.set("flag", flag.getToggleState());
    w.set("steps", (float)steps.getValue());
    w.set("loop", loop.getToggleState());
    w.set("loopTo", (float)loopTo.getValue());
    w.set("loopCount", (float)loopCount.getValue());
    w.set("keep", keep.getToggleState());
    w.set("endLevel", (float)endLevel.getValue());
    w.set("endLevelEnable", endLevelEnable.getToggleState());

    auto* stl = ctx.apvts.getRawParameterValue(paramCode + spec.stl);

    w.set("startLevel", stl != nullptr ? stl->load() : 0.0f);
    w.set("releaseRate", (float)releaseRate.getValue());
    w.set("releaseLevel", (float)releaseLevel.getValue());

    // 表は 256 段ぶん。後ろの初期値のままの段は書かない。
    std::vector<float> rates, levels;

    if (auto* t = table()) {
        int last = StepTable::size;

        while (last > 0 && t->rate(last) == t->defaultR && t->level(last) == t->defaultL) --last;

        for (int s = 1; s <= last; ++s) {
            rates.push_back(t->rate(s));
            levels.push_back(t->level(s));
        }
    }

    w.setArray("rates", rates);
    w.setArray("levels", levels);
}

// 3.0.0 より前の、行の並びで書いたファイル。並びは [11] のもの。
void GuiComponentStepEnv::setImportingParams(juce::StringArray& lines, int& index)
{
    flag.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    steps.setValue(lines[index++].getIntValue(), juce::sendNotification);
    loop.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    loopTo.setValue(lines[index++].getIntValue(), juce::sendNotification);
    loopCount.setValue(lines[index++].getIntValue(), juce::sendNotification);

    if (auto* p = ctx.apvts.getParameter(paramCode + spec.stl)) p->setValueNotifyingHost(p->convertTo0to1(lines[index].getFloatValue()));

    ++index;

    auto* t = table();

    for (int i = 1; i <= legacyRelease; ++i) {
        const float r = lines[index++].getFloatValue();
        const float l = lines[index++].getFloatValue();

        if (i == legacyRelease) {
            releaseRate.setValue(r, juce::sendNotification);
            releaseLevel.setValue(l, juce::sendNotification);
        }
        else if (t != nullptr && i <= legacySteps) {
            t->setRate(i, r);
            t->setLevel(i, l);
        }
    }

    applyStepCount();
    changed();
}

void GuiComponentStepEnv::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    // 3.6.3 までの [11] の鍵 (ssgSwEnv11 など) で書いたファイル
    if (r.keys().isEmpty() && key.contains("256")) r = reader.child(key.replace("256", "11"));

    readValues(r);
}

void GuiComponentStepEnv::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    writeValues(w);
}
