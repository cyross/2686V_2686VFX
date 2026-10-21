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

    // 3.6.3 までの 6 段は 5 段 + リリース。6 番目がリリース。
    constexpr int sixStepSteps = 5;
    constexpr int sixStepRelease = 6;

    // 3.0.0 より前の行の並びのファイルの行数。頭の 6 行 + R / L の組。
    // [11] は 6 + 22 = 28 行、6 段は 6 + 12 = 18 行。
    constexpr int legacyLines = 6 + legacyRelease * 2;

    // LOOP MODE の並び。並びの番号がパラメータの値 (StepLoop::Mode) になる
    const std::vector<SelectItem>& loopModeItems()
    {
        static const std::vector<SelectItem> items = {
            { .name = "Original Mode", .value = 1 },
            { .name = "Rich Mode", .value = 2 },
        };

        return items;
    }

    // ループの組をパラメータファイルへ書く形。START, END, COUNT を順に並べる
    std::vector<float> flattenLoops(const std::vector<StepLoop::Entry>& loops)
    {
        std::vector<float> values;

        for (const auto& e : loops) {
            values.push_back((float)e.start);
            values.push_back((float)e.end);
            values.push_back((float)e.count);
        }

        return values;
    }

    std::vector<StepLoop::Entry> unflattenLoops(const std::vector<float>& values)
    {
        std::vector<StepLoop::Entry> loops;

        for (size_t i = 0; i + 2 < values.size(); i += 3) {
            loops.push_back({ (int)values[i], (int)values[i + 1], (int)values[i + 2] });
        }

        return loops;
    }
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
    loopMode(context),
    loopTo(context),
    loopCount(context),
    loops(context),
    loopSeparator(context),
    rateTarget(context),
    rate(context),
    rateNudge(context),
    rateSeparator(context),
    levelTarget(context),
    level(context),
    ampBtns(context),
    pitchBtns(context),
    valueBtns(context),
    releaseSeparator(context),
    releaseRate(context),
    releaseLevel(context),
    releaseRateBtns(context),
    releaseLevelBtns(context),
    endLevelSeparator(context),
    endLevelEnable(context),
    endLevel(context),
    levelSteps(context)
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

    if (isAmp() || isTl()) cat.setupSwAmpCategory(catConfig);
    else if (isPitch()) cat.setupSwPitchCategory(catConfig);
    else cat.setupSwCategory(catConfig);

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

    if (hasKeep()) {
        keep.setup({ .parent = parent, .id = code + spec.keep, .title = "KEEP", .isReset = true });
        keep.setWantsKeyboardFocus(true);
        keep.setExplicitFocusOrder(++tabOrder);

        keepSeparator.setupComponent(parent);
    }

    loop.setup({ .parent = parent, .id = code + spec.loop, .title = "LOOP", .isReset = true });
    loop.setWantsKeyboardFocus(true);
    loop.setExplicitFocusOrder(++tabOrder);
    // 押したときだけ、ループできる形へ STEP と LOOP.TO を直す
    loop.onClick = [this] { applyLoopValues(loop.getToggleState()); };

    loopMode.setup({ .parent = parent, .id = code + spec.loopMode, .title = "LOOP MODE", .items = loopModeItems(), .isReset = true, .labelFont = labelFont });
    loopMode.setWantsKeyboardFocus(true);
    loopMode.setExplicitFocusOrder(++tabOrder);
    loopMode.onChange = [this] { applyLoopMode(); };

    loopTo.setup({ .parent = parent, .id = code + spec.loopTo, .title = "LOOP.TO", .isReset = true, .labelFont = labelFont });
    loopTo.setWantsKeyboardFocus(true);
    loopTo.setExplicitFocusOrder(++tabOrder);
    loopTo.onValueChange = [this] { applyLoopValues(loop.getToggleState()); changed(); };

    loopCount.setup({ .parent = parent, .id = code + spec.loopCount, .title = "LOOP.CNT", .isReset = true, .labelFont = labelFont });
    loopCount.setWantsKeyboardFocus(true);
    loopCount.setExplicitFocusOrder(++tabOrder);
    loopCount.onValueChange = [this] { changed(); };

    loops.setupComponent(parent, tabOrder, labelFont,
        [this] { return table(); },
        [this] { return stepCount(); },
        [this] {
            markSeen();
            relayoutIfNeeded();
            changed();
        });

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
    rate.getSlider().setNumDecimalPlacesToDisplay(3);
    rate.getSlider().onValueChange = [this] { onRateChanged(); };

    rateNudge.setupComponent(parent, rate.getSlider(), tabOrder, rateButtonEntries(), labelFont, 4);

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

    if (usesUnitButtons()) ampBtns.setupComponent(parent, level.getSlider(), tabOrder, labelFont);
    else if (usesValueButtons()) valueBtns.setupComponent(parent, level.getSlider(), tabOrder, valueButtonEntries(), labelFont);
    else if (isPitch()) pitchBtns.setupComponent(parent, level.getSlider(), tabOrder, labelFont);

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

    releaseRateBtns.setupComponent(parent, releaseRate, tabOrder, rateButtonEntries(), labelFont, 4);

    if (hasReleaseLevelButtons()) releaseLevelBtns.setupComponent(parent, releaseLevel, tabOrder, releaseLevelEntries(), labelFont);

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
    if (hasLevelSteps()) levelSteps.setupComponent(parent, { &level.getSlider(), &releaseLevel, &endLevel }, tabOrder, "STEPS", labelFont);

    flag.onStateChange = [this] { changed(); };
    if (hasKeep()) keep.onStateChange = [this] { changed(); };
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
    if (hasKeep()) keep.rebind(code + spec.keep);
    loop.rebind(code + spec.loop);
    loopMode.rebind(code + spec.loopMode);
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

// 値のつまみの下のボタン。DUTY は PRESET のデューティ比、TL は刻みと両端、
// ALG は ±1 と、アルゴリズムの番号。
std::vector<GuiComponentValueButtons::Entry> GuiComponentStepEnv::valueButtonEntries() const
{
    if (isDuty()) {
        // 並びは SSG の PRESET と同じ (SsgCore::dutyPresets)
        return { { "50", false, 50.0 }, { "43.75", false, 43.75 }, { "37.5", false, 37.5 }, { "31.25", false, 31.25 }, { "25", false, 25.0 },
                 { "20", false, 20.0 }, { "18.75", false, 18.75 }, { "12.5", false, 12.5 }, { "6.25", false, 6.25 } };
    }

    const juce::String maxLabel((int)spec.levelMax);

    if (isAlg()) {
        const int lo = (int)spec.levelMin;
        const int hi = (int)spec.levelMax;

        // 数が少なければ (OPN などは 8 個)、番号を直に置くボタンを並べる
        if (hi - lo < 8) {
            std::vector<GuiComponentValueButtons::Entry> entries = { { "-1", true, -1.0 }, { "+1", true, 1.0 } };

            for (int a = lo; a <= hi; ++a) entries.push_back({ juce::String(a), false, (double)a });

            return entries;
        }

        // OPZX7 は 130 個あるので、TL と同じく刻みと両端
        return { { "-10", true, -10.0 }, { "-1", true, -1.0 }, { juce::String(lo), false, (double)lo }, { "+1", true, 1.0 }, { "+10", true, 10.0 },
                 { maxLabel, false, spec.levelMax } };
    }

    return { { "-8", true, -8.0 }, { "-1", true, -1.0 }, { "0", false, 0.0 }, { "+1", true, 1.0 }, { "+8", true, 8.0 },
             { maxLabel, false, spec.levelMax } };
}

// RATE / RR のボタン。上の 2 行は足し引き、下の行は直に置く値
std::vector<GuiComponentValueButtons::Entry> GuiComponentStepEnv::rateButtonEntries()
{
    return { { "-1.0", true, -1.0 }, { "-0.1", true, -0.1 }, { "-0.01", true, -0.01 }, { "-0.001", true, -0.001 },
             { "+0.001", true, 0.001 }, { "+0.01", true, 0.01 }, { "+0.1", true, 0.1 }, { "+1.0", true, 1.0 },
             { "0.001", false, 0.001 }, { "0.01", false, 0.01 }, { "0.1", false, 0.1 }, { "1.0", false, 1.0 } };
}

// LR のボタン。値のつまみ (LEVEL / CENT / DUTY / PEAK / TL) と同じ刻み
std::vector<GuiComponentValueButtons::Entry> GuiComponentStepEnv::releaseLevelEntries() const
{
    if (isPitch()) {
        return { { "-1200", true, -1200.0 }, { "-100", true, -100.0 }, { "0", false, 0.0 }, { "+100", true, 100.0 }, { "+1200", true, 1200.0 } };
    }

    if (usesValueButtons()) return valueButtonEntries();

    // 0〜1 の値 (AMP / PEAK)
    return { { "-0.01", true, -0.01 }, { "-0.1", true, -0.1 }, { "0.0", false, 0.0 }, { "1.0", false, 1.0 }, { "+0.1", true, 0.1 }, { "+0.01", true, 0.01 } };
}

// ---------------------------------------------------------------- 段の数
// 対象のつまみの範囲と帯を、段の数に合わせる。帯の行数が変わったときは
// 区分の高さが変わるので、並べ直してもらう。
// 行数は、最後に並べたときのもの (laidOutRows) と比べる。STEPS を動かすと
// ここより先に applyLoopValues が帯を作り直すので、ここで測った行数は
// もう新しい段数のものになっていて、比べても変わったと分からなかった。
void GuiComponentStepEnv::applyStepCount()
{
    const int n = stepCount();

    // 範囲は幅を持たせる必要があるので、1 段のときも 1〜2 にして先で丸める
    rateTarget.setRange(1.0, (double)juce::jmax(2, n), 1.0);
    levelTarget.setRange(0.0, (double)n, 1.0);

    if ((int)rateTarget.getValue() > n) rateTarget.setValue(n, juce::dontSendNotification);
    if ((int)levelTarget.getValue() > n) levelTarget.setValue(n, juce::dontSendNotification);

    rebindRate();
    rebindLevel();
    loops.refresh();

    markSeen();

    relayoutIfNeeded();
}

void GuiComponentStepEnv::relayoutIfNeeded()
{
    // まだ一度も並べていない (組み立ての途中) ときは、後で並べられるので呼ばない
    if (laidOutRows > 0 && (laidOutRows != stripRowCount() || laidOutRich != isRich())) ctx.editor.resized();
}

// LOOP MODE を切り替えた。Original は LOOP.TO / LOOP.CNT、Rich はループの組を並べる。
// 並ぶつまみが変わるので、並べ直してもらう。
void GuiComponentStepEnv::applyLoopMode()
{
    applyLoopValues(loop.getToggleState());
    loops.refresh();
    applyActive();
    relayoutIfNeeded();
    changed();
}

void GuiComponentStepEnv::applyLoopValues(bool enabled)
{
    // setValue は同期で onValueChange を呼び返すので、入れ子ぶんは弾く
    if (isApplyingLoopValues) return;

    const juce::ScopedValueSetter<bool> guard(isApplyingLoopValues, true);

    loopTo.setRange(0.0, (double)juce::jmax(1, stepCount() - 1), 1.0);

    // Rich Mode は 1 段でもループできるので、STEP と LOOP.TO を直さない
    if (enabled && !isRich())
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

    laidOutRows = stripRowCount();
    laidOutRich = isRich();

    const bool visible = cat.isDetailVisible();

    flag.setVisible(visible);
    flagSeparator.setVisible(visible);
    steps.setVisibleWithLabel(visible);
    stepsSeparator.setVisible(visible);
    keep.setVisible(visible && hasKeep());
    keepSeparator.setVisible(visible && hasKeep());
    loop.setVisible(visible);
    loopMode.setVisibleWithLabel(visible);
    loopTo.setVisibleWithLabel(visible && !isRich());
    loopCount.setVisibleWithLabel(visible && !isRich());
    loops.setVisibles(visible && isRich());
    loopSeparator.setVisible(visible);
    rateTarget.setVisibleWithLabel(visible);
    rate.setVisibleWithLabel(visible);
    rateNudge.setVisibles(visible && rate.isVisibleNudge());
    rateValues.setVisible(visible);
    rateSeparator.setVisible(visible);
    levelTarget.setVisibleWithLabel(visible);
    level.setVisibleWithLabel(visible);
    levelSteps.setVisibleWithLabel(visible && hasLevelSteps());
    ampBtns.setVisibles(visible && usesUnitButtons() && level.isVisibleNudge());
    pitchBtns.setVisibles(visible && isPitch() && level.isVisibleNudge());
    valueBtns.setVisibles(visible && usesValueButtons() && level.isVisibleNudge());
    levelValues.setVisible(visible);
    releaseSeparator.setVisible(visible);
    releaseRate.setVisibleWithLabel(visible);
    releaseLevel.setVisibleWithLabel(visible);
    releaseRateBtns.setVisibles(visible && rate.isVisibleNudge());
    releaseLevelBtns.setVisibles(visible && hasReleaseLevelButtons() && level.isVisibleNudge());
    endLevelSeparator.setVisible(visible);
    endLevelEnable.setVisible(visible);
    endLevel.setVisibleWithLabel(visible);

    if (!visible) return;

    constexpr int h = 13;

    layoutMain({ .mainRect = rect, .component = &flag });
    flagSeparator.layoutComponent(rect);
    layoutMain({ .mainRect = rect, .label = &steps.label, .component = &steps, .rowHeight = h });
    stepsSeparator.layoutComponent(rect);
    if (hasKeep()) {
        layoutMain({ .mainRect = rect, .component = &keep, .rowHeight = h });
        keepSeparator.layoutComponent(rect);
    }
    layoutMain({ .mainRect = rect, .component = &loop, .rowHeight = h });
    layoutMain({ .mainRect = rect, .label = &loopMode.label, .component = &loopMode, .rowHeight = h });

    if (isRich()) {
        loops.layoutComponent(rect, h);
    }
    else {
        layoutMain({ .mainRect = rect, .label = &loopTo.label, .component = &loopTo, .rowHeight = h });
        layoutMain({ .mainRect = rect, .label = &loopCount.label, .component = &loopCount, .rowHeight = h });
    }

    loopSeparator.layoutComponent(rect);

    // 対象 → 値 → 各段の値、の順。
    layoutMain({ .mainRect = rect, .label = &rateTarget.label, .component = &rateTarget, .rowHeight = h });
    rate.layoutComponent(rect, h);
    if (rate.isVisibleNudge()) rateNudge.layoutComponent(rect, h);
    layoutStrip(rect, rateValues);
    rateSeparator.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .label = &levelTarget.label, .component = &levelTarget, .rowHeight = h });
    level.layoutComponent(rect, h);

    if (hasLevelSteps()) levelSteps.layoutComponent(rect, h);

    if (level.isVisibleNudge()) {
        if (usesUnitButtons()) ampBtns.layoutComponent(rect, h);
        else if (usesValueButtons()) valueBtns.layoutComponent(rect, h);
        else if (isPitch()) pitchBtns.layoutComponent(rect, h);
    }

    layoutStrip(rect, levelValues);

    releaseSeparator.layoutComponent(rect);
    layoutMain({ .mainRect = rect, .label = &releaseRate.label, .component = &releaseRate, .rowHeight = h });
    if (rate.isVisibleNudge()) releaseRateBtns.layoutComponent(rect, h);
    layoutMain({ .mainRect = rect, .label = &releaseLevel.label, .component = &releaseLevel, .rowHeight = h });
    if (hasReleaseLevelButtons() && level.isVisibleNudge()) releaseLevelBtns.layoutComponent(rect, h);

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
        spec.steps, spec.loop, spec.loopTo, spec.loopCount, spec.loopMode, spec.stl, spec.rr, spec.lr, (float)spec.levelMax);

    graph.updateBypass(isCategoryBypassed());

    // KEEP のときはカーブを効かせない。音の側も補間そのものを止めてある。
    // TL ENV は常に KEEP。
    const bool keepOn = !hasKeep() || keep.getToggleState();

    graph.setKeepLevels(keepOn);

    if (isPitch()) graph.updateSsgSwPEnv256(v, keepOn ? nullptr : p_curveCore, keepOn ? false : isCurveMode, posIdx);
    else graph.updateSsgSwEnv256(v, keepOn ? nullptr : p_curveCore, keepOn ? false : isCurveMode, posIdx);
}

// ---------------------------------------------------------------- 止める
void GuiComponentStepEnv::setEnabled(bool enabled)
{
    outerEnabled = enabled;

    cat.setEnabled(enabled);

    applyActive();
}

// 効いていないつまみは押せなくする。LOOP.TO / LOOP.CNT とループの組は LOOP が
// 切れていても、ENDL は Use Endl が切れていても止める。
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
    loopMode.setEnabledWithLabel(active);
    loopTo.setEnabledWithLabel(looping);
    loopCount.setEnabledWithLabel(looping);
    loops.setEnables(looping);
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
    valueBtns.setEnables(active);
    levelValues.setEnabled(active);

    releaseSeparator.setEnabled(active);
    releaseRate.setEnabledWithLabel(active);
    releaseLevel.setEnabledWithLabel(active);
    releaseRateBtns.setEnables(active);
    releaseLevelBtns.setEnables(active);

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

            lines.removeEmptyStrings();

            // 6 段のファイルは [11] より行が少ない
            if (spec.sixStepFileFormat.isNotEmpty() && lines.size() < legacyLines) setImportingSixStepParams(lines, index);
            else setImportingParams(lines, index);
        }

        Io::ParamWriter writer(Io::ParamFormat{ spec.fileFormat, 1 });

        writeParams(writer, Io::ParamKey::values);
        writer.hoist(Io::ParamKey::values);

        Io::writeConverted(file, writer);

        return;
    }

    // 3.6.3 までの [11] のファイルは、[256] へ写して読む
    if (spec.legacyFileFormat.isNotEmpty()) {
        if (auto legacy = Io::ParamReader::open(file, Io::ParamFormat{ spec.legacyFileFormat, 1 }, false)) {
            GuiRefresh::Batch batch;

            readValues(*legacy);
            return;
        }
    }

    // 3.6.3 までの 6 段のファイルも、[256] へ写して読む
    if (spec.sixStepFileFormat.isNotEmpty()) {
        if (auto six = Io::ParamReader::open(file, Io::ParamFormat{ spec.sixStepFileFormat, 1 }, false)) {
            GuiRefresh::Batch batch;

            readSixStepValues(*six);
            return;
        }
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

    // 4.1.0 より前のファイルには無い。無ければ Original Mode にする
    loopMode.setSelectedItemIndex(juce::jlimit((int)StepLoop::modeMin, (int)StepLoop::modeMax, r.getInt("loopMode", StepLoop::Original)), juce::sendNotification);

    if (hasKeep()) keep.setToggleState(r.getBool("keep", keep.getToggleState()), juce::sendNotification);
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

            // ループの組 (4.1.0)。無ければ空にする
            t->setLoops(unflattenLoops(r.getFloatArray("loops")));
        }
    }
    else {
        // 3.6.3 までの [11] の形。1〜10 段が表、11 番目がリリース。ループの組は無い
        if (t != nullptr) t->setLoops({});

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

// 3.6.3 までの 6 段の形 (r1〜r6 / l1〜l6)。1〜5 段が表、6 番目がリリース。
// 6 段に無かった KEEP と ENDL は切る。
void GuiComponentStepEnv::readSixStepValues(const Io::ParamReader& r)
{
    flag.setToggleState(r.getBool("flag", flag.getToggleState()), juce::sendNotification);
    steps.setValue(r.getInt("steps", (int)steps.getValue()), juce::sendNotification);
    loop.setToggleState(r.getBool("loop", loop.getToggleState()), juce::sendNotification);
    loopTo.setValue(r.getInt("loopTo", (int)loopTo.getValue()), juce::sendNotification);
    loopCount.setValue(r.getInt("loopCount", (int)loopCount.getValue()), juce::sendNotification);
    loopMode.setSelectedItemIndex(StepLoop::Original, juce::sendNotification);
    keep.setToggleState(false, juce::sendNotification);
    endLevelEnable.setToggleState(false, juce::sendNotification);

    if (auto* p = ctx.apvts.getParameter(paramCode + spec.stl)) {
        auto* raw = ctx.apvts.getRawParameterValue(paramCode + spec.stl);

        p->setValueNotifyingHost(p->convertTo0to1(r.getFloat("startLevel", raw != nullptr ? raw->load() : 0.0f)));
    }

    const auto names = r.keys();
    const juce::String release(sixStepRelease);

    if (names.contains("r" + release)) releaseRate.setValue(r.getFloat("r" + release), juce::sendNotification);
    if (names.contains("l" + release)) releaseLevel.setValue(r.getFloat("l" + release), juce::sendNotification);

    if (auto* t = table()) {
        t->reset();

        for (int i = 1; i <= sixStepSteps; ++i) {
            const juce::String no(i);

            if (names.contains("r" + no)) t->setRate(i, r.getFloat("r" + no));
            if (names.contains("l" + no)) t->setLevel(i, r.getFloat("l" + no));
        }
    }

    applyStepCount();
    changed();
}

// 3.0.0 より前の、行の並びで書いた 6 段のファイル
void GuiComponentStepEnv::setImportingSixStepParams(juce::StringArray& lines, int& index)
{
    flag.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    steps.setValue(lines[index++].getIntValue(), juce::sendNotification);
    loop.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    loopTo.setValue(lines[index++].getIntValue(), juce::sendNotification);
    loopCount.setValue(lines[index++].getIntValue(), juce::sendNotification);
    loopMode.setSelectedItemIndex(StepLoop::Original, juce::sendNotification);
    keep.setToggleState(false, juce::sendNotification);
    endLevelEnable.setToggleState(false, juce::sendNotification);

    if (auto* p = ctx.apvts.getParameter(paramCode + spec.stl)) p->setValueNotifyingHost(p->convertTo0to1(lines[index].getFloatValue()));

    ++index;

    auto* t = table();

    if (t != nullptr) t->reset();

    for (int i = 1; i <= sixStepRelease; ++i) {
        const float r = lines[index++].getFloatValue();
        const float l = lines[index++].getFloatValue();

        if (i == sixStepRelease) {
            releaseRate.setValue(r, juce::sendNotification);
            releaseLevel.setValue(l, juce::sendNotification);
        }
        else if (t != nullptr) {
            t->setRate(i, r);
            t->setLevel(i, l);
        }
    }

    applyStepCount();
    changed();
}

void GuiComponentStepEnv::setImportingParamsWithSixStep(juce::StringArray& lines, int& index)
{
    constexpr int sixStepLines = 6 + sixStepRelease * 2;

    const int sixAt = index;
    const int elevenAt = index + sixStepLines;

    auto activeAt = [&](int at) {
        const bool on = lines[at].getIntValue() == 1;

        return m_flagKey.endsWith(CPK::bypass) ? !on : on;
    };

    if (activeAt(sixAt) && !activeAt(elevenAt)) {
        int at = sixAt;

        setImportingSixStepParams(lines, at);
        index = elevenAt + legacyLines;
        return;
    }

    index = elevenAt;
    setImportingParams(lines, index);
}

// ファイルに書いた入切が「使う」になっているか。入切の鍵が BYPASS なら切が「使う」。
bool GuiComponentStepEnv::isActiveIn(const Io::ParamReader& r) const
{
    if (r.keys().isEmpty()) return false;

    const bool on = r.getBool("flag", false);

    return m_flagKey.endsWith(CPK::bypass) ? !on : on;
}

void GuiComponentStepEnv::writeValues(Io::ParamWriter& w)
{
    w.set("flag", flag.getToggleState());
    w.set("steps", (float)steps.getValue());
    w.set("loop", loop.getToggleState());
    w.set("loopTo", (float)loopTo.getValue());
    w.set("loopCount", (float)loopCount.getValue());
    w.set("loopMode", (float)juce::jmax(0, loopMode.getSelectedItemIndex()));
    if (hasKeep()) w.set("keep", keep.getToggleState());
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

    if (auto* t = table()) w.setArray("loops", flattenLoops(t->loopList()));
}

// 3.0.0 より前の、行の並びで書いたファイル。並びは [11] のもの。
void GuiComponentStepEnv::setImportingParams(juce::StringArray& lines, int& index)
{
    flag.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    steps.setValue(lines[index++].getIntValue(), juce::sendNotification);
    loop.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    loopTo.setValue(lines[index++].getIntValue(), juce::sendNotification);
    loopCount.setValue(lines[index++].getIntValue(), juce::sendNotification);
    loopMode.setSelectedItemIndex(StepLoop::Original, juce::sendNotification);

    if (auto* p = ctx.apvts.getParameter(paramCode + spec.stl)) p->setValueNotifyingHost(p->convertTo0to1(lines[index].getFloatValue()));

    ++index;

    auto* t = table();

    if (t != nullptr) t->setLoops({});

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

    // 3.6.3 までの 6 段 (ssgSwEnv)。6 段だけが使われていたときは 6 段を写す。
    // 両方が使われていたときは [256] を残す (掛け合わせは 1 つでは表せない)。
    if (spec.sixStepFileFormat.isNotEmpty() && key.contains("256")) {
        const auto six = reader.child(key.replace("256", ""));

        if (isActiveIn(six) && !isActiveIn(r)) {
            readSixStepValues(six);
            return;
        }
    }

    readValues(r);
}

void GuiComponentStepEnv::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    writeValues(w);
}
