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

// ============================================================================
// 段の値を横向きの棒で並べる (TAP SEQUENCER)
// ============================================================================
// 段ごとに 1 本。上から 1 段目、2 段目 … と並べ、値の大きさを横の長さで描く。
// 次に押したときに使う段 (cursor) と、値のつまみが指している段を色で分ける。
class GuiTapBars : public juce::Component
{
public:
    void enablementChanged() override { setAlpha(isEnabled() ? 1.0f : 0.4f); }

    std::vector<float> values;
    float minValue = 0.0f;
    float maxValue = 1.0f;

    // 0 の線 (音程は 0 から左右へ伸ばす)。範囲の外なら左端
    float zero = 0.0f;

    int cursor = -1;    // 次に使う段 (0 から)
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
//   S.TARGET / RATIO (CENT) / 値のボタン
//   棒 (段ごとの値)
//   帯 (S1: 値 / S2: 値 / ...)
//   INCREMENT / DECREMENT / RESET  (次に使う段を動かす)
//
// 頭 (Enable・STEPS と、段を動かす札) はパラメータ、段の値は表 (StepTable) にある。
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

    GuiSlider target;
    GuiComponentNudgeSliderFloat value;
    GuiComponentSsgSwButtons ampBtns;
    GuiComponentPitchButtons pitchBtns;
    GuiTapBars bars;
    GuiStepValues list;
    NormalSeparator listSeparator;

    GuiTextButton increment;
    GuiTextButton decrement;
    GuiTextButton reset;

    juce::String paramCode;
    bool outerEnabled = true;
    bool isLoadingValue = false;

    // 組み立てを終えたか。組み立ての途中で並べ直しを頼むと、まだ作っていない部品の
    // 並びが崩れたまま残る (2686VFX のキーアサインの一覧が出たままになった)。
    bool isReady = false;

    std::uint32_t seenVersion = 0;
    int seenCursor = -1;

    bool isAmp() const { return kind == Kind::Amp; }

    StepTable* table() const;
    int stepCount() const { return juce::jlimit(1, StepTable::size, (int)steps.getValue()); }

    void applyStepCount();
    void rebindValue();
    void onValueChanged();
    void moveCursor(int delta, bool toStart);
    void refresh();
    void applyActive();

    void readValues(const Io::ParamReader& r);
    void writeValues(Io::ParamWriter& w);

    void timerCallback() override;
};
