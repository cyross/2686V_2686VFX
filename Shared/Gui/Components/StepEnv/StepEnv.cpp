#include "./StepEnv.h"

#include "Shared/Core/Gui/GuiRefresh.h"
#include "Shared/Core/Gui/GuiI18n.h"
#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiGraphValues.h"
#include "Shared/Core/Gui/GuiParamEdit.h"
#include "Shared/Core/Gui/GuiLiveGraph.h"
#include "Shared/Core/Processor/ProcessorParamMod.h"
#include "Shared/Core/Const/ConstGlobal.h"

namespace
{
    // 整数や選択肢のパラメータか。PARAM ENV はそういう指し先では常に段を保つ (KEEP)
    bool isSteppedParam(const juce::RangedAudioParameter* p)
    {
        return p != nullptr && (dynamic_cast<const juce::AudioParameterInt*>(p) != nullptr
            || dynamic_cast<const juce::AudioParameterChoice*>(p) != nullptr
            || dynamic_cast<const juce::AudioParameterBool*>(p) != nullptr);
    }

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

    // 段の進め方 (MODE / RR MODE) の並び。並びの番号が StepRate::Step::rate (0 が TIME)
    const std::vector<SelectItem>& rateModeItems()
    {
        static const std::vector<SelectItem> items = {
            { .name = "TIME", .value = 1 },
            { .name = "RATE", .value = 2 },
        };

        return items;
    }

    // T SYNC の 1 刻み (TICK NOTE / RR NOTE) の並び。並びの番号が StepRate::Step::tickNote
    const std::vector<SelectItem>& tickNoteItems()
    {
        static const std::vector<SelectItem> items = [] {
            std::vector<SelectItem> v;

            for (int i = 0; i < StepRate::noteCount; ++i) v.push_back({ .name = StepRate::notes[(size_t)i].label, .value = i + 1 });

            return v;
        }();

        return items;
    }

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
    targetBox(context),
    targetSeparator(context),
    steps(context),
    stepsSeparator(context),
    keep(context),
    strictRate(context),
    startNow(context),
    tickSync(context),
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
    rateMode(context),
    delta(context),
    deltaBtns(context),
    tick(context),
    tickBtns(context),
    tickNote(context),
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
    releaseMode(context),
    releaseDelta(context),
    releaseTick(context),
    releaseTickNote(context),
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

    if (isAmp()) cat.setupSwAmpCategory(catConfig);
    else if (isPitch()) cat.setupSwPitchCategory(catConfig);
    else if (isParam()) cat.setupParamCategory(catConfig);
    else cat.setupSwCategory(catConfig);

    m_flagKey = flagKey;

    flag.setup({ .parent = parent, .id = code + flagKey, .title = flagText, .isReset = true });
    flag.setWantsKeyboardFocus(true);
    flag.setExplicitFocusOrder(++tabOrder);

    flagSeparator.setupComponent(parent);

    // PARAM ENV。動かすパラメータを選ぶ。変えたら値の書き方も変わる
    if (isParam()) {
        std::vector<SelectItem> items;

        for (int i = 0; i < spec.targetLabels.size(); ++i) items.push_back({ spec.targetLabels[i], i + 1 });

        targetBox.setup({ .parent = parent, .id = code + spec.target, .title = "TARGET", .items = items, .isReset = true, .labelFont = labelFont });
        targetBox.setWantsKeyboardFocus(true);
        targetBox.setExplicitFocusOrder(++tabOrder);
        targetBox.onChange = [this] {
            applyTargetDisplay();
            rebindLevel();
            changed();
        };

        targetSeparator.setupComponent(parent);
    }

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
    }

    // 切っていると、行き先の値に着いたところで段を終える (4.2.x までと同じ)
    if (hasStrictRate()) {
        strictRate.setup({ .parent = parent, .id = code + spec.strictRate, .title = "STRICT R", .isReset = true });
        strictRate.setTooltip(I18n::pick(u8"段もリリースも R (RR) 秒を守る。切ると、前の段と同じ値の段はすぐ終わる",
            u8"Keep each step and the release for R (RR) seconds. When off, a step with the same value as the previous one ends at once"));
        strictRate.setWantsKeyboardFocus(true);
        strictRate.setExplicitFocusOrder(++tabOrder);
    }

    // 押したときにまだ走っていれば (離したあとの余韻など)、STL ではなくその時点の値から始める
    if (hasStartNow()) {
        startNow.setup({ .parent = parent, .id = code + spec.startNow, .title = "START NOW", .isReset = true });
        startNow.setTooltip(isParam()
            ? I18n::pick(u8"押したときにまだ走っていれば、その時点の値から始める。止まっていれば、指し先のつまみの今の値から",
                u8"If the envelope is still running when a key is pressed, start from its current value. Otherwise start from the target knob's current value")
            : I18n::pick(u8"押したときにまだ走っていれば (離したあとの余韻など)、STL ではなくその時点の値から始める",
                u8"If the envelope is still running when a key is pressed (such as during the release), start from its current value instead of STL"));
        startNow.setWantsKeyboardFocus(true);
        startNow.setExplicitFocusOrder(++tabOrder);
        startNow.onStateChange = [this] { changed(); };
    }

    // RATE の段の 1 刻みを、秒 (TICK) ではなくテンポに合わせた音符の長さ (TICK NOTE) で取る
    if (hasTickSync()) {
        tickSync.setup({ .parent = parent, .id = code + spec.tickSync, .title = "T SYNC", .isReset = true });
        tickSync.setTooltip(I18n::pick(u8"RATE の段の 1 刻みを、TICK (秒) ではなく TICK NOTE (テンポに合わせた音符の長さ) で取る",
            u8"Take each tick of a RATE step from TICK NOTE (a note length at the host tempo) instead of TICK (seconds)"));
        tickSync.setWantsKeyboardFocus(true);
        tickSync.setExplicitFocusOrder(++tabOrder);
        tickSync.onStateChange = [this] { changed(); };
    }

    if (hasHeadToggles()) keepSeparator.setupComponent(parent);

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

    // 段の進め方 (4.3.0)。TIME は R 秒で L へ、RATE は DELTA ずつ L へ (R は打ち切りの時間。最小なら打ち切らない)
    rateMode.setup({ .parent = parent, .title = "MODE", .items = rateModeItems(), .isReset = false, .labelFont = labelFont });
    rateMode.setTooltip(I18n::pick(u8"TIME: R 秒で L へ進む。RATE: DELTA ずつ L へ進み、L に届くか R 秒たったら終える (R が最小なら時間で打ち切らない)",
        u8"TIME: reach L in R seconds. RATE: move toward L by DELTA, and end on reaching L or after R seconds (R at its minimum never cuts it short)"));
    rateMode.setWantsKeyboardFocus(true);
    rateMode.setExplicitFocusOrder(++tabOrder);
    rateMode.onChange = [this] { onRateStepChanged(false); };

    delta.setup({ .parent = parent, .title = "DELTA", .isReset = false, .labelFont = labelFont });
    delta.setTooltip(I18n::pick(u8"RATE の段の変化量。TICK が 0 なら 1 秒あたり、0 より大きければ 1 刻みあたりに動く量",
        u8"How far a RATE step moves: per second when TICK is 0, otherwise per tick"));
    delta.setWantsKeyboardFocus(true);
    delta.setExplicitFocusOrder(++tabOrder);
    delta.onValueChange = [this] { onRateStepChanged(false); };

    deltaBtns.setupComponent(parent, delta, tabOrder, deltaButtonEntries(), labelFont, isParam() ? 4 : GuiComponentValueButtons::defaultPerRow);

    tick.setup({ .parent = parent, .title = "TICK", .isReset = false, .labelFont = labelFont });
    tick.setRange(0.0, spec.rateMax, spec.rateInterval);
    tick.setNumDecimalPlacesToDisplay(3);
    tick.setTooltip(I18n::pick(u8"RATE の段の 1 刻みの秒。段に入ってから TICK 秒ごとに DELTA ずつ動く。0 なら線形",
        u8"Seconds per tick of a RATE step. It moves by DELTA every TICK seconds after the step begins. 0 moves smoothly"));
    tick.setWantsKeyboardFocus(true);
    tick.setExplicitFocusOrder(++tabOrder);
    tick.onValueChange = [this] { onRateStepChanged(false); };

    tickBtns.setupComponent(parent, tick, tabOrder, tickButtonEntries(), labelFont, 4);

    tickNote.setup({ .parent = parent, .title = "TICK NOTE", .items = tickNoteItems(), .isReset = false, .labelFont = labelFont });
    tickNote.setTooltip(I18n::pick(u8"T SYNC のときの 1 刻みの長さ (ホストのテンポに合わせる。LINEAR は線形)",
        u8"Length of a tick when T SYNC is on (follows the host tempo. LINEAR moves smoothly)"));
    tickNote.setWantsKeyboardFocus(true);
    tickNote.setExplicitFocusOrder(++tabOrder);
    tickNote.onChange = [this] { onRateStepChanged(false); };

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
    else if (usesValueButtons()) valueBtns.setupComponent(parent, level.getSlider(), tabOrder, valueButtonEntries(), labelFont, isParam() ? 6 : GuiComponentValueButtons::defaultPerRow);
    else if (isPitch()) pitchBtns.setupComponent(parent, level.getSlider(), tabOrder, labelFont);

    parent.addAndMakeVisible(levelValues);

    // リリース
    releaseSeparator.setupComponent(parent);

    releaseRate.setup({ .parent = parent, .id = code + spec.rr, .title = "RR", .isReset = true, .labelFont = labelFont });
    releaseRate.setWantsKeyboardFocus(true);
    releaseRate.setExplicitFocusOrder(++tabOrder);
    releaseRate.onValueChange = [this] { changed(); };

    // PARAM ENV の LR / ENDL は TARGET ごとの値の組 (表) にあるので、パラメータへ繋がない
    releaseLevel.setup({ .parent = parent, .id = isParam() ? juce::String() : code + spec.lr, .title = "LR", .isReset = !isParam(), .labelFont = labelFont });
    releaseLevel.setWantsKeyboardFocus(true);
    releaseLevel.setExplicitFocusOrder(++tabOrder);
    releaseLevel.onValueChange = [this] { onSetLevelChanged(releaseLevel, StepTable::setLr); };

    releaseRateBtns.setupComponent(parent, releaseRate, tabOrder, rateButtonEntries(), labelFont, 4);

    if (hasReleaseLevelButtons()) releaseLevelBtns.setupComponent(parent, releaseLevel, tabOrder, releaseLevelEntries(), labelFont, isParam() ? 6 : GuiComponentValueButtons::defaultPerRow);

    // リリースの進め方 (4.3.0)。RATE なら RR は打ち切りの時間 (最小なら打ち切らない)。
    // RR DELTA が 0 のときは、いつまでも終わらないので TIME と同じに進める
    releaseMode.setup({ .parent = parent, .title = "RR MODE", .items = rateModeItems(), .isReset = false, .labelFont = labelFont });
    releaseMode.setTooltip(I18n::pick(u8"TIME: RR 秒で LR へ進む。RATE: RR DELTA ずつ LR へ進み、LR に届くか RR 秒たったら終える (RR DELTA が 0 なら TIME と同じ)",
        u8"TIME: reach LR in RR seconds. RATE: move toward LR by RR DELTA, and end on reaching LR or after RR seconds (with RR DELTA at 0 it works as TIME)"));
    releaseMode.setWantsKeyboardFocus(true);
    releaseMode.setExplicitFocusOrder(++tabOrder);
    releaseMode.onChange = [this] { onRateStepChanged(true); };

    releaseDelta.setup({ .parent = parent, .title = "RR DELTA", .isReset = false, .labelFont = labelFont });
    releaseDelta.setWantsKeyboardFocus(true);
    releaseDelta.setExplicitFocusOrder(++tabOrder);
    releaseDelta.onValueChange = [this] { onRateStepChanged(true); };

    releaseTick.setup({ .parent = parent, .title = "RR TICK", .isReset = false, .labelFont = labelFont });
    releaseTick.setRange(0.0, spec.rateMax, spec.rateInterval);
    releaseTick.setNumDecimalPlacesToDisplay(3);
    releaseTick.setWantsKeyboardFocus(true);
    releaseTick.setExplicitFocusOrder(++tabOrder);
    releaseTick.onValueChange = [this] { onRateStepChanged(true); };

    releaseTickNote.setup({ .parent = parent, .title = "RR NOTE", .items = tickNoteItems(), .isReset = false, .labelFont = labelFont });
    releaseTickNote.setWantsKeyboardFocus(true);
    releaseTickNote.setExplicitFocusOrder(++tabOrder);
    releaseTickNote.onChange = [this] { onRateStepChanged(true); };

    setDeltaRange();

    // PARAM ENV の DELTA の ±1 も指し先の 1 刻み
    if (isParam()) deltaBtns.setStepSize([this] { return paramInterval; });

    // PARAM ENV の ±1 は指し先の 1 刻み
    if (isParam()) {
        valueBtns.setStepSize([this] { return paramInterval; });
        releaseLevelBtns.setStepSize([this] { return paramInterval; });
    }

    endLevelSeparator.setupComponent(parent);

    endLevelEnable.setup({ .parent = parent, .id = code + spec.endlEnable, .title = "Use Endl", .isReset = true });
    endLevelEnable.setWantsKeyboardFocus(true);
    endLevelEnable.setExplicitFocusOrder(++tabOrder);
    endLevelEnable.onStateChange = [this] { changed(); };

    endLevel.setup({ .parent = parent, .id = isParam() ? juce::String() : code + spec.endl, .title = "ENDL", .isReset = !isParam(), .labelFont = labelFont });
    endLevel.setWantsKeyboardFocus(true);
    endLevel.setExplicitFocusOrder(++tabOrder);
    endLevel.onValueChange = [this] { onSetLevelChanged(endLevel, StepTable::setEndl); };

    // 音量と DUTY / PEAK RATIO ENVELOPE は、LEVEL・LR・ENDL を決まった刻みへ丸められる
    if (hasLevelSteps()) {
        levelSteps.setupComponent(parent, { &level.getSlider(), &releaseLevel, &endLevel }, tabOrder, "STEPS", labelFont,
            isAmp() ? GuiComponentStepSnap::Choices::Level : GuiComponentStepSnap::Choices::Ratio);
    }

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

    if (hasTickSync()) tickSync.watchToggle([this] { applyActive(); });

    applyActive();

    startTimerHz(10);
}

// 束縛先を丸ごと差し替える。setup で組んだ見た目はそのままに、繋ぎだけを張り替える。
void GuiComponentStepEnv::rebind(const juce::String& code)
{
    paramCode = code;

    flag.rebind(code + m_flagKey);
    if (isParam()) targetBox.rebind(code + spec.target);
    steps.rebind(code + spec.steps);
    if (hasKeep()) keep.rebind(code + spec.keep);
    if (hasStrictRate()) strictRate.rebind(code + spec.strictRate);
    if (hasStartNow()) startNow.rebind(code + spec.startNow);
    if (hasTickSync()) tickSync.rebind(code + spec.tickSync);
    loop.rebind(code + spec.loop);
    loopMode.rebind(code + spec.loopMode);
    loopTo.rebind(code + spec.loopTo);
    loopCount.rebind(code + spec.loopCount);
    releaseRate.rebind(code + spec.rr);
    endLevelEnable.rebind(code + spec.endlEnable);

    if (!isParam()) {
        releaseLevel.rebind(code + spec.lr);
        endLevel.rebind(code + spec.endl);
    }

    // 区分の右クリックで表も写すための印
    const auto key = code + spec.tableKey;

    rate.getSlider().getProperties().set(ParamEdit::Property::stepTable, key);
    level.getSlider().getProperties().set(ParamEdit::Property::stepTable, key);

    applyStepCount();
    applyTargetDisplay();
}

// ---------------------------------------------------------------- PARAM ENV
juce::RangedAudioParameter* GuiComponentStepEnv::targetParam() const
{
    if (!isParam() || !spec.targetParamId || paramCode.isEmpty()) return nullptr;

    auto* raw = ctx.apvts.getRawParameterValue(paramCode + spec.target);
    const int t = raw != nullptr ? juce::roundToInt(raw->load()) : 0;

    return ctx.apvts.getParameter(spec.targetParamId(paramCode, t));
}

void GuiComponentStepEnv::applyTargetText(GuiSlider& slider)
{
    auto* p = targetParam();

    slider.textFromValueFunction = [p](double v) { return p != nullptr ? p->getText((float)juce::jlimit(0.0, 1.0, v), 0) : juce::String(v, 3); };
    slider.valueFromTextFunction = [p](const juce::String& t) { return p != nullptr ? (double)p->getValueForText(t) : t.getDoubleValue(); };
    slider.updateText();
}

// 値は指し先のパラメータの 0〜1。画面には指し先の書き方で出し、打ち込みも指し先の値で受ける。
// 値 (STL・段の L・LR・ENDL) は TARGET ごとに別に持つので、TARGET を変えたら映し直す。
void GuiComponentStepEnv::applyTargetDisplay()
{
    if (!isParam()) return;

    auto* p = targetParam();

    const bool stepped = isSteppedParam(p);

    const auto range = p != nullptr ? p->getNormalisableRange() : juce::NormalisableRange<float>(0.0f, 1.0f);
    const int span = stepped ? juce::roundToInt(range.end - range.start) : 0;

    paramInterval = span > 0 ? 1.0 / (double)span : 0.001;

    applyTargetText(level.getSlider());
    applyTargetText(releaseLevel);
    applyTargetText(endLevel);

    levelValues.format = [p](float v) { return p != nullptr ? p->getText(juce::jlimit(0.0f, 1.0f, v), 0) : juce::String(v, 3); };

    // 値のつまみは刻みも指し先に合わせる。
    // 丸められた値を表へ書き戻さない (表は元の値のまま、音の側で丸める)
    {
        const juce::ScopedValueSetter<bool> loading(isLoadingValue, true);

        setLevelRange();
        level.getSlider().setValue(levelAt(juce::jlimit(0, stepCount(), (int)levelTarget.getValue())), juce::dontSendNotification);
    }

    loadSetLevels();
    setDeltaRange();
    refreshStepValues();
}

// ---------------------------------------------------------------- 値の組 (PARAM ENV)
int GuiComponentStepEnv::levelSet() const
{
    if (!isParam() || paramCode.isEmpty()) return -1;

    const auto* t = table();

    if (t == nullptr || t->setCount() == 0) return -1;

    auto* raw = ctx.apvts.getRawParameterValue(paramCode + spec.target);

    return juce::jlimit(0, t->setCount() - 1, raw != nullptr ? juce::roundToInt(raw->load()) : 0);
}

float GuiComponentStepEnv::levelAt(int slot) const
{
    const auto* t = table();

    if (const int set = levelSet(); set >= 0) return t->setValue(set, slot);

    if (slot == 0) {
        auto* stl = ctx.apvts.getRawParameterValue(paramCode + spec.stl);

        return stl != nullptr ? stl->load() : 0.0f;
    }

    return t != nullptr ? t->level(slot) : 0.0f;
}

void GuiComponentStepEnv::setLevelAt(int slot, float value)
{
    auto* t = table();

    if (const int set = levelSet(); set >= 0) {
        t->setSetValue(set, slot, juce::jlimit(0.0f, 1.0f, value));
        markSeen();
        return;
    }

    if (slot == 0) {
        if (auto* p = ctx.apvts.getParameter(paramCode + spec.stl)) p->setValueNotifyingHost(p->convertTo0to1(value));
        return;
    }

    if (t != nullptr) t->setLevel(slot, value);
}

float GuiComponentStepEnv::levelDefault() const
{
    const auto* t = table();

    if (const int set = levelSet(); set >= 0) return t->setDefault(set);

    return t != nullptr ? t->defaultL : 0.0f;
}

void GuiComponentStepEnv::loadSetLevels()
{
    if (!isParam()) return;

    {
        const juce::ScopedValueSetter<bool> loading(isLoadingValue, true);

        releaseLevel.setRange(0.0, 1.0, paramInterval);
        endLevel.setRange(0.0, 1.0, paramInterval);

        releaseLevel.setValue(levelAt(StepTable::setLr), juce::dontSendNotification);
        endLevel.setValue(levelAt(StepTable::setEndl), juce::dontSendNotification);
    }

    // 範囲を変えると書き方が戻ることがあるので、掛け直す
    applyTargetText(releaseLevel);
    applyTargetText(endLevel);
}

void GuiComponentStepEnv::onSetLevelChanged(GuiSlider& slider, int slot)
{
    if (isParam()) {
        if (isLoadingValue) return;

        setLevelAt(slot, (float)slider.getValue());
    }

    changed();
}

void GuiComponentStepEnv::putSetLevel(GuiSlider& slider, int slot, float value)
{
    if (isParam()) {
        setLevelAt(slot, value);
        loadSetLevels();
        changed();
        return;
    }

    slider.setValue(value, juce::sendNotification);
}

void GuiComponentStepEnv::setLevelRange()
{
    if (isParam()) {
        level.getSlider().setRange(0.0, 1.0, paramInterval);
        return;
    }

    level.getSlider().setRange(spec.levelMin, spec.levelMax, spec.levelInterval);
    level.getSlider().setNumDecimalPlacesToDisplay(spec.levelInterval >= 1.0 ? 0 : spec.levelDecimals);
}

// 値のつまみの下のボタン。DUTY は PRESET のデューティ比、ALG は ±1 と、アルゴリズムの番号。
// PARAM ENV は両端と、指し先の 1 刻み、範囲の 1 割。
std::vector<GuiComponentValueButtons::Entry> GuiComponentStepEnv::valueButtonEntries() const
{
    // PARAM ENV は 0〜1。両端と、指し先の 1 刻み、範囲の 1 割
    if (isParam()) {
        return { { "MIN", false, 0.0 }, { "-10%", true, -0.1 }, { "-1", true, -1.0, true },
                 { "+1", true, 1.0, true }, { "+10%", true, 0.1 }, { "MAX", false, 1.0 } };
    }

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

        // OPZX7 は 130 個あるので、刻みと両端
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

// DELTA のボタン。値の刻みに合わせて、足し引きと、よく使う量を直に置くもの
std::vector<GuiComponentValueButtons::Entry> GuiComponentStepEnv::deltaButtonEntries() const
{
    // PARAM ENV は 0〜1。指し先の 1 刻みと、範囲の 1 割
    if (isParam()) return { { "-10%", true, -0.1 }, { "-1", true, -1.0, true }, { "+1", true, 1.0, true }, { "+10%", true, 0.1 } };

    if (isPitch()) {
        return { { "-1200", true, -1200.0 }, { "-100", true, -100.0 }, { "100", false, 100.0 }, { "+100", true, 100.0 }, { "+1200", true, 1200.0 } };
    }

    if (isDuty()) return { { "-1", true, -1.0 }, { "6.25", false, 6.25 }, { "12.5", false, 12.5 }, { "+1", true, 1.0 } };

    if (isAlg()) return { { "-1", true, -1.0 }, { "1", false, 1.0 }, { "+1", true, 1.0 } };

    // 0〜1 の値 (AMP / PEAK)。1/16 は SSG の音量の 1 段ぶん
    return { { "-0.1", true, -0.1 }, { "-0.01", true, -0.01 }, { "1/16", false, 1.0 / 16.0 }, { "+0.01", true, 0.01 }, { "+0.1", true, 0.1 } };
}

// TICK のボタン。足し引きと、よく使う長さ (1/60 は 60Hz の 1 コマ)
std::vector<GuiComponentValueButtons::Entry> GuiComponentStepEnv::tickButtonEntries()
{
    return { { "-0.1", true, -0.1 }, { "-0.01", true, -0.01 }, { "+0.01", true, 0.01 }, { "+0.1", true, 0.1 },
             { "0", false, 0.0 }, { "1/60", false, 1.0 / 60.0 }, { "0.05", false, 0.05 }, { "0.1", false, 0.1 } };
}

// LR のボタン。値のつまみ (LEVEL / CENT / DUTY / PEAK / ALG / PARAM) と同じ刻み
std::vector<GuiComponentValueButtons::Entry> GuiComponentStepEnv::releaseLevelEntries() const
{
    if (isPitch()) {
        return { { "-1200", true, -1200.0 }, { "-100", true, -100.0 }, { "0", false, 0.0 }, { "+100", true, 100.0 }, { "+1200", true, 1200.0 } };
    }

    if (usesValueButtons()) return valueButtonEntries();

    // 0〜1 の値 (AMP / PEAK)
    return { { "-0.01", true, -0.01 }, { "-0.1", true, -0.1 }, { "0.0", false, 0.0 }, { "1.0", false, 1.0 }, { "+0.1", true, 0.1 }, { "+0.01", true, 0.01 } };
}

// ---------------------------------------------------------------- 段の進め方 (4.3.0)
StepRate::Step GuiComponentStepEnv::rateStepAt(int step) const
{
    const auto* t = table();

    return t != nullptr ? t->rateStep(step) : StepRate::Step{};
}

void GuiComponentStepEnv::setRateStepAt(int step, const StepRate::Step& value)
{
    if (auto* t = table()) t->setRateStep(step, value);
}

// DELTA は値の幅まで。PARAM ENV は 0〜1 を指し先の 1 刻みで
void GuiComponentStepEnv::setDeltaRange()
{
    const juce::ScopedValueSetter<bool> loading(isLoadingValue, true);

    for (auto* slider : { &delta, &releaseDelta }) {
        if (isParam()) {
            slider->setRange(0.0, 1.0, paramInterval);
            slider->setNumDecimalPlacesToDisplay(3);
        }
        else {
            slider->setRange(0.0, juce::jmax(spec.levelInterval, spec.levelMax - spec.levelMin), spec.levelInterval);
            slider->setNumDecimalPlacesToDisplay(spec.levelInterval >= 1.0 ? 0 : spec.levelDecimals);
        }
    }

    loadRateStep();
}

void GuiComponentStepEnv::loadRateStep()
{
    if (paramCode.isEmpty()) return;

    const juce::ScopedValueSetter<bool> loading(isLoadingValue, true);

    const auto st = rateStepAt(juce::jlimit(1, stepCount(), (int)rateTarget.getValue()));

    rateMode.setSelectedItemIndex(st.rate ? 1 : 0, juce::dontSendNotification);
    delta.setValue(st.delta, juce::dontSendNotification);
    tick.setValue(st.tickSec, juce::dontSendNotification);
    tickNote.setSelectedItemIndex(juce::jlimit(0, StepRate::noteCount - 1, st.tickNote), juce::dontSendNotification);

    const auto rel = rateStepAt(StepTable::releaseStep);

    releaseMode.setSelectedItemIndex(rel.rate ? 1 : 0, juce::dontSendNotification);
    releaseDelta.setValue(rel.delta, juce::dontSendNotification);
    releaseTick.setValue(rel.tickSec, juce::dontSendNotification);
    releaseTickNote.setSelectedItemIndex(juce::jlimit(0, StepRate::noteCount - 1, rel.tickNote), juce::dontSendNotification);

    applyActive();
}

void GuiComponentStepEnv::onRateStepChanged(bool release)
{
    if (isLoadingValue) return;

    StepRate::Step st;

    if (release) {
        st.rate = releaseMode.getSelectedItemIndex() == 1;
        st.delta = (float)releaseDelta.getValue();
        st.tickSec = (float)releaseTick.getValue();
        st.tickNote = juce::jmax(0, releaseTickNote.getSelectedItemIndex());

        setRateStepAt(StepTable::releaseStep, st);
    }
    else {
        st.rate = rateMode.getSelectedItemIndex() == 1;
        st.delta = (float)delta.getValue();
        st.tickSec = (float)tick.getValue();
        st.tickNote = juce::jmax(0, tickNote.getSelectedItemIndex());

        setRateStepAt(juce::jlimit(1, stepCount(), (int)rateTarget.getValue()), st);
    }

    markSeen();
    applyActive();
    refreshStepValues();
    changed();
}

// 段の進め方は 256 段ぶんとリリース。後ろの TIME のままの段は書かない
void GuiComponentStepEnv::writeRateSteps(Io::ParamWriter& w) const
{
    std::vector<float> modes, deltas, ticks, notes;

    int last = StepTable::size;

    while (last > 0 && rateStepAt(last) == StepRate::Step{}) --last;

    for (int s = 1; s <= last; ++s) {
        const auto st = rateStepAt(s);

        modes.push_back(st.rate ? 1.0f : 0.0f);
        deltas.push_back(st.delta);
        ticks.push_back(st.tickSec);
        notes.push_back((float)st.tickNote);
    }

    w.setArray("rateModes", modes);
    w.setArray("deltas", deltas);
    w.setArray("tickSecs", ticks);
    w.setArray("tickNotes", notes);

    const auto rel = rateStepAt(StepTable::releaseStep);

    w.set("releaseMode", rel.rate);
    w.set("releaseDelta", rel.delta);
    w.set("releaseTick", rel.tickSec);
    w.set("releaseTickNote", (float)rel.tickNote);
}

// 4.3.0 より前のファイルには無い。無ければすべて TIME (これまでの鳴り方) にする
void GuiComponentStepEnv::readRateSteps(const Io::ParamReader& r)
{
    const auto modes = r.getFloatArray("rateModes");
    const auto deltas = r.getFloatArray("deltas");
    const auto ticks = r.getFloatArray("tickSecs");
    const auto notes = r.getFloatArray("tickNotes");

    auto at = [](const std::vector<float>& v, int s) { return s <= (int)v.size() ? v[(size_t)s - 1] : 0.0f; };

    for (int s = 1; s <= StepTable::size; ++s) {
        setRateStepAt(s, { at(modes, s) != 0.0f, at(deltas, s), at(ticks, s), (int)at(notes, s) });
    }

    setRateStepAt(StepTable::releaseStep, { r.getBool("releaseMode", false), r.getFloat("releaseDelta", 0.0f),
        r.getFloat("releaseTick", 0.0f), r.getInt("releaseTickNote", 0) });
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

    // 範囲は幅を持たせる必要があるので、1 段のときも 1〜2 にして先で丸める。
    // 1 段のときは選ぶものが無いので、つまみを止める (applyActive)
    rateTarget.setRange(1.0, (double)juce::jmax(2, n), 1.0);
    levelTarget.setRange(0.0, (double)n, 1.0);

    if ((int)rateTarget.getValue() > n) rateTarget.setValue(n, juce::dontSendNotification);
    if ((int)levelTarget.getValue() > n) levelTarget.setValue(n, juce::dontSendNotification);

    rebindRate();
    rebindLevel();
    loadSetLevels();
    loops.refresh();

    markSeen();

    applyActive();
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

    loadRateStep();
    refreshStepValues();
}

void GuiComponentStepEnv::rebindLevel()
{
    const int idx = juce::jlimit(0, stepCount(), (int)levelTarget.getValue());

    // 0 は開始の値。これだけはパラメータなので、つまみをそのまま繋ぐ。
    // PARAM ENV は開始の値も TARGET の組にあるので、段と同じに扱う
    if (idx == 0 && !isParam()) {
        level.getSlider().rebind(paramCode + spec.stl);
    }
    else {
        // 範囲を変えると値が丸められて知らせが来る。表へ書き戻さないよう、読み込み中にしておく
        const juce::ScopedValueSetter<bool> loading(isLoadingValue, true);

        level.getSlider().unbind();
        setLevelRange();
        level.getSlider().setValue(levelAt(idx), juce::dontSendNotification);

        if (isParam()) applyTargetText(level.getSlider());
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

    // 0 (開始の値) はパラメータに繋いであるので、つまみの側で書き込まれる (PARAM ENV は組へ書く)
    if (idx > 0 || isParam()) setLevelAt(idx, (float)level.getSlider().getValue());

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
    rateValues.texts.clear();

    // RATE の段は、見出しを D にして変化量を出す (R は打ち切りの時間なので、つまみで見る)
    const int deltaDecimals = isParam() ? 3 : (spec.levelInterval >= 1.0 ? 0 : spec.levelDecimals);

    for (int s = 1; s <= n; ++s) {
        const auto st = rateStepAt(s);

        rateValues.labels.push_back((st.rate ? "D" : "R") + juce::String(s));
        rateValues.values.push_back(t != nullptr ? t->rate(s) : 0.0f);
        rateValues.texts.push_back(st.rate ? juce::String(st.delta, deltaDecimals) : juce::String());
    }

    rateValues.selected = juce::jlimit(1, n, (int)rateTarget.getValue()) - 1;
    rateValues.activeCount = -1;
    rateValues.decimals = Global::floatDecimalPlaces;
    rateValues.repaint();

    levelValues.labels.clear();
    levelValues.values.clear();

    levelValues.labels.push_back(spec.startLabel);
    levelValues.values.push_back(levelAt(0));

    for (int s = 1; s <= n; ++s) {
        levelValues.labels.push_back(spec.levelPrefix + juce::String(s));
        levelValues.values.push_back(levelAt(s));
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
    targetBox.setVisibleWithLabel(visible && isParam());
    targetSeparator.setVisible(visible && isParam());
    steps.setVisibleWithLabel(visible);
    stepsSeparator.setVisible(visible);
    keep.setVisible(visible && hasKeep());
    strictRate.setVisible(visible && hasStrictRate());
    startNow.setVisible(visible && hasStartNow());
    tickSync.setVisible(visible && hasTickSync());
    keepSeparator.setVisible(visible && hasHeadToggles());
    loop.setVisible(visible);
    loopMode.setVisibleWithLabel(visible);
    loopTo.setVisibleWithLabel(visible && !isRich());
    loopCount.setVisibleWithLabel(visible && !isRich());
    loops.setVisibles(visible && isRich());
    loopSeparator.setVisible(visible);
    rateTarget.setVisibleWithLabel(visible);
    rate.setVisibleWithLabel(visible);
    rateNudge.setVisibles(visible && rate.isVisibleNudge());
    rateMode.setVisibleWithLabel(visible);
    delta.setVisibleWithLabel(visible);
    deltaBtns.setVisibles(visible && rate.isVisibleNudge());
    tick.setVisibleWithLabel(visible);
    tickBtns.setVisibles(visible && rate.isVisibleNudge());
    tickNote.setVisibleWithLabel(visible);
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
    releaseMode.setVisibleWithLabel(visible);
    releaseDelta.setVisibleWithLabel(visible);
    releaseTick.setVisibleWithLabel(visible);
    releaseTickNote.setVisibleWithLabel(visible);
    endLevelSeparator.setVisible(visible);
    endLevelEnable.setVisible(visible);
    endLevel.setVisibleWithLabel(visible);

    if (!visible) return;

    constexpr int h = 13;

    layoutMain({ .mainRect = rect, .component = &flag });
    flagSeparator.layoutComponent(rect);

    if (isParam()) {
        layoutMain({ .mainRect = rect, .label = &targetBox.label, .component = &targetBox, .rowHeight = h });
        targetSeparator.layoutComponent(rect);
    }

    layoutMain({ .mainRect = rect, .label = &steps.label, .component = &steps, .rowHeight = h });
    stepsSeparator.layoutComponent(rect);
    if (hasKeep()) layoutMain({ .mainRect = rect, .component = &keep, .rowHeight = h });
    if (hasStrictRate()) layoutMain({ .mainRect = rect, .component = &strictRate, .rowHeight = h });
    if (hasStartNow()) layoutMain({ .mainRect = rect, .component = &startNow, .rowHeight = h });
    if (hasTickSync()) layoutMain({ .mainRect = rect, .component = &tickSync, .rowHeight = h });
    if (hasHeadToggles()) keepSeparator.layoutComponent(rect);
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
    layoutMain({ .mainRect = rect, .label = &rateMode.label, .component = &rateMode, .rowHeight = h });
    layoutMain({ .mainRect = rect, .label = &delta.label, .component = &delta, .rowHeight = h });
    if (rate.isVisibleNudge()) deltaBtns.layoutComponent(rect, h);
    layoutMain({ .mainRect = rect, .label = &tick.label, .component = &tick, .rowHeight = h });
    if (rate.isVisibleNudge()) tickBtns.layoutComponent(rect, h);
    layoutMain({ .mainRect = rect, .label = &tickNote.label, .component = &tickNote, .rowHeight = h });
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
    layoutMain({ .mainRect = rect, .label = &releaseMode.label, .component = &releaseMode, .rowHeight = h });
    layoutMain({ .mainRect = rect, .label = &releaseDelta.label, .component = &releaseDelta, .rowHeight = h });
    layoutMain({ .mainRect = rect, .label = &releaseTick.label, .component = &releaseTick, .rowHeight = h });
    layoutMain({ .mainRect = rect, .label = &releaseTickNote.label, .component = &releaseTickNote, .rowHeight = h });

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

void GuiComponentStepEnv::drawParamGraph(GuiEnvelopeGraph& graph, const GuiContext& ctx, const Spec& spec, const juce::String& code)
{
    auto& apvts = ctx.apvts;
    const auto& tables = ctx.audioProcessor.stepTables;

    auto v = GuiGraphValues::stepEnv256(apvts, tables, code, spec.tableKey,
        spec.steps, spec.loop, spec.loopTo, spec.loopCount, spec.loopMode, spec.stl, spec.rr, spec.lr, (float)spec.levelMax);

    const auto* table = tables.find(code + spec.tableKey);
    const int target = juce::roundToInt(GuiGraphValues::value(apvts, code + spec.target));

    // 値 (STL・段の L・LR・ENDL) は TARGET の組から
    if (table != nullptr && table->setCount() > 0) {
        const int set = juce::jlimit(0, table->setCount() - 1, target);

        for (size_t s = 0; s < v.l.size(); ++s) v.l[s] = table->setValue(set, (int)s);

        v.lr = table->setValue(set, StepTable::setLr);
        v.endl = table->setValue(set, StepTable::setEndl);
    }

    v.endlEnable = GuiGraphValues::flag(apvts, code + spec.endlEnable);

    const auto* targetParam = spec.targetParamId ? apvts.getParameter(spec.targetParamId(code, target)) : nullptr;

    // START NOW (4.3.0)。止まっていた声は、指し先のつまみの今の値から始める。絵もそこから描く
    v.tickSync = spec.tickSync.isNotEmpty() && GuiGraphValues::flag(apvts, code + spec.tickSync);
    v.startNow = false;

    if (spec.startNow.isNotEmpty() && GuiGraphValues::flag(apvts, code + spec.startNow) && targetParam != nullptr) v.l[0] = targetParam->getValue();
    const auto label = target >= 0 && target < spec.targetLabels.size() ? spec.targetLabels[target] : juce::String();

    graph.updateBypass(!GuiGraphValues::flag(apvts, code + ParamEnv::keys().enable));
    graph.setKeepLevels(isSteppedParam(targetParam) || GuiGraphValues::flag(apvts, code + spec.keep));
    graph.updateParamEnv256(v, label.isEmpty() ? spec.title : spec.title + " : " + label);

    graph.setLiveSource(table != nullptr ? &table->live : nullptr, ctx.audioProcessor.paramEnvLiveRate());
}

void GuiComponentStepEnv::updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx)
{
    if (isParam()) {
        drawParamGraph(graph, ctx, spec, paramCode);
        return;
    }

    auto v = GuiGraphValues::stepEnv256(ctx.apvts, ctx.audioProcessor.stepTables, paramCode, spec.tableKey,
        spec.steps, spec.loop, spec.loopTo, spec.loopCount, spec.loopMode, spec.stl, spec.rr, spec.lr, (float)spec.levelMax);

    v.startNow = hasStartNow() && startNow.getToggleState();
    v.tickSync = isTickSync();

    graph.updateBypass(isCategoryBypassed());

    // KEEP のときはカーブを効かせない。音の側も補間そのものを止めてある。
    // ALG ENV は常に KEEP。
    const bool keepOn = !hasKeep() || keep.getToggleState();

    graph.setKeepLevels(keepOn);

    if (isPitch()) graph.updateSsgSwPEnv256(v, keepOn ? nullptr : p_curveCore, keepOn ? false : isCurveMode, posIdx);
    else graph.updateSsgSwEnv256(v, keepOn ? nullptr : p_curveCore, keepOn ? false : isCurveMode, posIdx);

    // 鳴っている位置 (グラフの縦線)
    graph.setLiveSource(GuiGraphValues::liveOfTable(ctx.audioProcessor.stepTables, paramCode + spec.tableKey), ctx.audioProcessor.envLiveRate());
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
    targetBox.setEnabledWithLabel(active);
    targetSeparator.setEnabled(active);
    steps.setEnabledWithLabel(active);
    stepsSeparator.setEnabled(active);

    // トグルは止めずに薄くする。止めると、そのあいだに来た値を捨ててしまう。
    keep.setEnabled(outerEnabled);
    keep.setDimmed(!active);
    strictRate.setEnabled(outerEnabled);
    strictRate.setDimmed(!active);
    startNow.setEnabled(outerEnabled);
    startNow.setDimmed(!active);
    tickSync.setEnabled(outerEnabled);
    tickSync.setDimmed(!active);
    keepSeparator.setEnabled(active);

    loop.setEnabled(outerEnabled);
    loop.setDimmed(!active);
    loopMode.setEnabledWithLabel(active);
    loopTo.setEnabledWithLabel(looping);
    loopCount.setEnabledWithLabel(looping);
    loops.setEnables(looping);
    loopSeparator.setEnabled(active);

    // 1 段のときは選ぶ段が無い
    rateTarget.setEnabledWithLabel(active && stepCount() > 1);
    rate.setEnabled(active);
    rateNudge.setEnables(active);
    rateValues.setEnabled(active);
    rateSeparator.setEnabled(active);

    // DELTA / TICK は RATE の段だけ。TICK (秒) と TICK NOTE は T SYNC で入れ替わる
    const bool sync = isTickSync();
    const bool stepRate = active && rateMode.getSelectedItemIndex() == 1;
    const bool releaseIsRate = active && releaseMode.getSelectedItemIndex() == 1;

    rateMode.setEnabledWithLabel(active);
    delta.setEnabledWithLabel(stepRate);
    deltaBtns.setEnables(stepRate);
    tick.setEnabledWithLabel(stepRate && !sync);
    tickBtns.setEnables(stepRate && !sync);
    tickNote.setEnabledWithLabel(stepRate && sync);

    releaseMode.setEnabledWithLabel(active);
    releaseDelta.setEnabledWithLabel(releaseIsRate);
    releaseTick.setEnabledWithLabel(releaseIsRate && !sync);
    releaseTickNote.setEnabledWithLabel(releaseIsRate && sync);

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

    // 4.2.x までの TL ENV のファイルは、TARGET を TL にして読む
    if (spec.legacyTlFormat.isNotEmpty()) {
        if (auto legacy = Io::ParamReader::open(file, Io::ParamFormat{ spec.legacyTlFormat, 1 }, false)) {
            GuiRefresh::Batch batch;

            readLegacyTl(*legacy);
            return;
        }
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
void GuiComponentStepEnv::readValues(const Io::ParamReader& r, float levelScale)
{
    flag.setToggleState(r.getBool("flag", flag.getToggleState()), juce::sendNotification);

    // PARAM ENV の指し先。番号は音源で違うので、名前が合えば名前で選ぶ
    if (isParam()) {
        int t = spec.targetLabels.indexOf(r.getString("targetName", {}));

        if (t < 0) t = r.getInt("target", -1);

        if (t >= 0 && t < spec.targetLabels.size()) targetBox.setSelectedItemIndex(t, juce::sendNotificationSync);
    }

    steps.setValue(r.getInt("steps", (int)steps.getValue()), juce::sendNotification);
    loop.setToggleState(r.getBool("loop", loop.getToggleState()), juce::sendNotification);
    loopTo.setValue(r.getInt("loopTo", (int)loopTo.getValue()), juce::sendNotification);
    loopCount.setValue(r.getInt("loopCount", (int)loopCount.getValue()), juce::sendNotification);

    // 4.1.0 より前のファイルには無い。無ければ Original Mode にする
    loopMode.setSelectedItemIndex(juce::jlimit((int)StepLoop::modeMin, (int)StepLoop::modeMax, r.getInt("loopMode", StepLoop::Original)), juce::sendNotification);

    if (hasKeep()) keep.setToggleState(r.getBool("keep", keep.getToggleState()), juce::sendNotification);

    // 4.3.0 で足した。古いファイルには無いので、無ければ切 (4.2.x までの鳴り方) にする
    if (hasStrictRate()) strictRate.setToggleState(r.getBool("strictRate", false), juce::sendNotification);
    if (hasStartNow()) startNow.setToggleState(r.getBool("startNow", false), juce::sendNotification);
    if (hasTickSync()) tickSync.setToggleState(r.getBool("tickSync", false), juce::sendNotification);
    putSetLevel(endLevel, StepTable::setEndl, r.getFloat("endLevel", (float)endLevel.getValue() / levelScale) * levelScale);
    endLevelEnable.setToggleState(r.getBool("endLevelEnable", endLevelEnable.getToggleState()), juce::sendNotification);

    // PARAM ENV の値 (STL・段の L・LR・ENDL) は、いま選んでいる TARGET の組へ入る
    setLevelAt(0, r.getFloat("startLevel", levelAt(0) / levelScale) * levelScale);

    auto* t = table();
    const auto names = r.keys();

    if (names.contains("rates") || names.contains("levels")) {
        // [256] の形
        releaseRate.setValue(r.getFloat("releaseRate", (float)releaseRate.getValue()), juce::sendNotification);
        putSetLevel(releaseLevel, StepTable::setLr, r.getFloat("releaseLevel", (float)releaseLevel.getValue() / levelScale) * levelScale);

        if (t != nullptr) {
            const auto rates = r.getFloatArray("rates");
            const auto levels = r.getFloatArray("levels");
            const float defaultLevel = levelDefault();

            for (int s = 1; s <= StepTable::size; ++s) {
                t->setRate(s, s <= (int)rates.size() ? rates[(size_t)s - 1] : t->defaultR);
                setLevelAt(s, s <= (int)levels.size() ? levels[(size_t)s - 1] * levelScale : defaultLevel);
            }

            // ループの組 (4.1.0)。無ければ空にする
            t->setLoops(unflattenLoops(r.getFloatArray("loops")));
        }

        // 段の進め方 (4.3.0)。無ければすべて TIME
        readRateSteps(r);
    }
    else {
        // 3.6.3 までの [11] の形。1〜10 段が表、11 番目がリリース。ループの組と段の進め方は無い
        if (t != nullptr) {
            t->setLoops({});
            t->resetRateSteps();
        }

        for (int i = 1; i <= legacyRelease; ++i) {
            const juce::String no(i);

            if (i == legacyRelease) {
                if (names.contains("r" + no)) releaseRate.setValue(r.getFloat("r" + no), juce::sendNotification);
                if (names.contains("l" + no)) putSetLevel(releaseLevel, StepTable::setLr, r.getFloat("l" + no));
                continue;
            }

            if (t == nullptr) continue;

            if (names.contains("r" + no)) t->setRate(i, r.getFloat("r" + no));
            if (names.contains("l" + no)) setLevelAt(i, r.getFloat("l" + no));
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
    strictRate.setToggleState(false, juce::sendNotification);
    startNow.setToggleState(false, juce::sendNotification);
    tickSync.setToggleState(false, juce::sendNotification);
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
    strictRate.setToggleState(false, juce::sendNotification);
    startNow.setToggleState(false, juce::sendNotification);
    tickSync.setToggleState(false, juce::sendNotification);
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

    // PARAM ENV の指し先。番号は音源で違うので名前も書く
    if (isParam()) {
        const int t = juce::jmax(0, targetBox.getSelectedItemIndex());

        w.set("target", (float)t);
        w.set("targetName", spec.targetLabels[t]);
    }

    w.set("steps", (float)steps.getValue());
    w.set("loop", loop.getToggleState());
    w.set("loopTo", (float)loopTo.getValue());
    w.set("loopCount", (float)loopCount.getValue());
    w.set("loopMode", (float)juce::jmax(0, loopMode.getSelectedItemIndex()));
    if (hasKeep()) w.set("keep", keep.getToggleState());
    if (hasStrictRate()) w.set("strictRate", strictRate.getToggleState());
    if (hasStartNow()) w.set("startNow", startNow.getToggleState());
    if (hasTickSync()) w.set("tickSync", tickSync.getToggleState());
    w.set("endLevel", (float)endLevel.getValue());
    w.set("endLevelEnable", endLevelEnable.getToggleState());

    // PARAM ENV の値 (STL・段の L・LR・ENDL) は、いま選んでいる TARGET の組のもの
    w.set("startLevel", levelAt(0));
    w.set("releaseRate", (float)releaseRate.getValue());
    w.set("releaseLevel", (float)releaseLevel.getValue());

    // 表は 256 段ぶん。後ろの初期値のままの段は書かない。
    std::vector<float> rates, levels;

    if (auto* t = table()) {
        const float defaultLevel = levelDefault();

        int last = StepTable::size;

        while (last > 0 && t->rate(last) == t->defaultR && levelAt(last) == defaultLevel) --last;

        for (int s = 1; s <= last; ++s) {
            rates.push_back(t->rate(s));
            levels.push_back(levelAt(s));
        }
    }

    w.setArray("rates", rates);
    w.setArray("levels", levels);

    if (auto* t = table()) w.setArray("loops", flattenLoops(t->loopList()));

    writeRateSteps(w);
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

    if (t != nullptr) {
        t->setLoops({});
        t->resetRateSteps();
    }

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

// 4.2.x までの TL ENV。値は TL のレジスタ値なので、TL の最大値で割って 0〜1 にする。
// TL ENV は常に段を保っていたので KEEP は切ったまま (TL は整数なので段を保つ)。
void GuiComponentStepEnv::readLegacyTl(const Io::ParamReader& r)
{
    if (!isParam()) return;

    targetBox.setSelectedItemIndex(spec.tlTarget, juce::sendNotificationSync);
    keep.setToggleState(false, juce::sendNotification);

    readValues(r, 1.0f / juce::jmax(1.0f, spec.tlMax));
}

void GuiComponentStepEnv::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    // 4.2.x までのチャンネルのファイルの TL ENV (tlEnv)
    if (r.keys().isEmpty() && isParam() && spec.legacyTlKey.isNotEmpty()) {
        const auto tl = reader.child(spec.legacyTlKey);

        if (!tl.keys().isEmpty()) readLegacyTl(tl);

        return;
    }

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
