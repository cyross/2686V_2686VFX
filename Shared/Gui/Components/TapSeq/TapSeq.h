#pragma once

#include <JuceHeader.h>

#include <functional>

#include "../../../Core/Io/ParamFile.h"
#include "Shared/Core/Synth/StepTable.h"
#include "Shared/Core/Processor/ProcessorTapSeq.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiValues.h"
#include "../Separator/NormalSeparator.h"
#include "../SsgSwButtons/SsgSwButtons.h"
#include "../PitchButtons/PitchButtons.h"
#include "../NudgeSlider/NudgeSliderFloat.h"
#include "../StepValues/StepValues.h"
#include "../StepLoops/StepLoops.h"

// ============================================================================
// 段の値を横向きの棒で並べる (TAP SEQUENCER)
// ============================================================================
// 段ごとに 1 本。上から 1 段目、2 段目 … と並べ、値の大きさを横の長さで描く。
// いまの段 (cursor。最後に鍵で使った段) は行の地を変え、値のつまみが指している段は
// 棒の色を変える。開始前は、どの行の地も変えない。
// SKIP の段は、値によらず左から右までいっぱいの赤い棒 (指しているときは橙) で描く。
class GuiTapBars : public juce::Component
{
public:
    void enablementChanged() override { setAlpha(isEnabled() ? 1.0f : 0.4f); }

    std::vector<float> values;
    float minValue = 0.0f;
    float maxValue = 1.0f;

    // 0 の線 (音程は 0 から左右へ伸ばす)。範囲の外なら左端
    float zero = 0.0f;

    std::vector<bool> skips;

    int cursor = -1;    // いまの段 (0 から)。負は開始前
    int selected = -1;  // 値のつまみが指している段 (0 から)

    // 段の数で 1 本の高さを決める。多いときは細くして、全体を抑える
    int getNaturalHeight() const;

    void paint(juce::Graphics& g) override;
};

// ============================================================================
// TAP AMP SEQUENCER / TAP PITCH SEQUENCER の区分
// ============================================================================
// 鍵を押すたびに段を 1 つ進め、その段の値で音量 (割合) か音程 (セント) を補正する。
//
//   Enable
//   STEPS
//   LOOP / LP.TARGET / START / END / COUNT / ADD・DELETE / 一覧 (4.1.0)
//   S.TARGET / RATIO (CENT) / SKIP (4.1.0) / 値のボタン
//   棒 (段ごとの値)
//   帯 (S1: 値 / S2: 値 / ...)
//   INCREMENT / DECREMENT / CLEAR STEP  (いまの段を動かす。CLEAR STEP は開始前へ戻す)
//
// 頭 (Enable・STEPS・LOOP と、段を動かす札) はパラメータ、段の値・SKIP・ループの組は
// 表 (StepTable) にある。SKIP は S.TARGET の段を補正しない段にする。立てたまま
// RATIO (CENT) を動かすと外れる。
class GuiComponentTapSeq : public GuiBase, private juce::Timer {
public:
    enum class Kind { Amp, Pitch };

    GuiComponentTapSeq(const GuiContext& context, Kind kind);
    ~GuiComponentTapSeq() override { stopTimer(); }

    GuiCategoryLabel& category() { return cat; }

    void setCategoryVisible(bool visible) {
        cat.setHidden(!visible);
        cat.setVisible(visible);
    }

    bool hasBypassSwitch() const { return true; }
    bool isCategoryBypassed() const { return !enable.getToggleState(); }
    void setCategoryBypassed(bool bypassed) { enable.setToggleState(!bypassed, juce::sendNotification); }
    void setCategoryOpen(bool open) { cat.setDetailVisible(open); }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
    void setEnabled(bool enabled);

    void importParams();
    void applyParamsFile(const juce::File& file);
    void exportParams();
    void writeParamsFile(const juce::File& file);

    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);

private:
    const Kind kind;
    const TapSeq::Spec& spec;

    juce::Font labelFont = juce::Font(juce::FontOptions(12.0f));

    GuiCategoryLabel cat;
    GuiToggleButton enable;
    NormalSeparator enableSeparator;
    GuiSlider steps;
    NormalSeparator stepsSeparator;

    // ループの組に沿って段を進めるか。組そのものは表に持つ
    GuiToggleButton loop;
    GuiComponentStepLoops loops;
    NormalSeparator loopSeparator;

    GuiSlider target;
    GuiComponentNudgeSliderFloat value;
    GuiTextButton skip;
    GuiComponentSsgSwButtons ampBtns;
    GuiComponentPitchButtons pitchBtns;
    GuiTapBars bars;
    GuiStepValues list;
    NormalSeparator listSeparator;

    GuiTextButton increment;
    GuiTextButton decrement;
    GuiTextButton clearStep;

    juce::String paramCode;
    bool outerEnabled = true;
    bool isLoadingValue = false;

    // 組み立てを終えたか。組み立ての途中で並べ直しを頼むと、まだ作っていない部品の
    // 並びが崩れたまま残る (2686VFX のキーアサインの一覧が出たままになった)。
    bool isReady = false;

    std::uint32_t seenVersion = 0;
    int seenCursor = -1;

    // 最後に並べたときのループの一覧の行数。0 はまだ並べていない
    int laidOutLoopRows = 0;

    // ループの一覧の行数が、最後に並べたときと変わっていれば並べ直してもらう
    void relayoutIfNeeded();

    bool isAmp() const { return kind == Kind::Amp; }

    StepTable* table() const;
    int stepCount() const { return juce::jlimit(1, StepTable::size, (int)steps.getValue()); }

    void applyStepCount();
    void rebindValue();
    void onValueChanged();
    void toggleSkip();
    void moveCursor(int delta, bool toStart);
    void refresh();
    void applyActive();

    void readValues(const Io::ParamReader& r);
    void writeValues(Io::ParamWriter& w);

    void timerCallback() override;
};
