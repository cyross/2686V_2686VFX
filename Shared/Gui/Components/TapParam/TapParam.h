#pragma once

#include <JuceHeader.h>

#include <functional>

#include "../../../Core/Io/ParamFile.h"
#include "Shared/Core/Synth/StepTable.h"
#include "Shared/Core/Processor/ProcessorParamMod.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiValues.h"
#include "../Separator/NormalSeparator.h"
#include "../NudgeSlider/NudgeSliderFloat.h"
#include "../StepLoops/StepLoops.h"

// ============================================================================
// 段ごとの組を 1 行ずつ並べる (TAP PARAM SEQUENCER)
// ============================================================================
// 段ごとに 1 行。「S1: OP1 AR 31, OP2 TL 20」のように、その段の組を並べて書く。
// いまの段 (cursor) は行の地を変え、S.TARGET の段は文字の色を変える。
// SKIP の段は「SKIP」、組の無い段は「-」と書く。
class GuiTapPairList : public juce::Component
{
public:
    void enablementChanged() override { setAlpha(isEnabled() ? 1.0f : 0.4f); }

    static constexpr int rowHeight = 14;

    std::vector<juce::String> lines;
    std::vector<bool> skips;

    int cursor = -1;    // いまの段 (0 から)。負は開始前
    int selected = -1;  // S.TARGET の段 (0 から)

    int getNaturalHeight() const { return juce::jmax(1, (int)lines.size()) * rowHeight; }

    void paint(juce::Graphics& g) override;
};

// ============================================================================
// TAP PARAM SEQUENCER の区分 (4.3.0)
// ============================================================================
// 鍵を押すたびに段を 1 つ進め、その段に入れた (パラメータ, 値) の組で、
// パラメータをその音のあいだ置き換える。1 つの段に組をいくつでも
// (StepTable::maxPairs まで) 入れられる。段の進め方は TAP AMP / PITCH SEQUENCER と同じ。
//
//   Enable
//   STEPS
//   LOOP / LP.TARGET / START / END / COUNT / ADD・DELETE / 一覧
//   S.TARGET / SKIP
//   P.TARGET / OP / PARAM / VALUE / 値のボタン       (S.TARGET の段の組)
//   ADD PAIR / DELETE PAIR / CLEAR PAIRS
//   一覧 (段ごとの組)
//   INCREMENT / DECREMENT / CLEAR STEP
//
// 値は指し先のパラメータの 0〜1 で持ち、画面には指し先の書き方で出す。
// OP は FM のオペレーター (RHYTHM はパッド)。CH はチャンネルの指し先。
class GuiComponentTapParam : public GuiBase, private juce::Timer {
public:
    // ops はオペレーター (パッド) の数。0 ならチャンネルの指し先だけ
    GuiComponentTapParam(const GuiContext& context, const ParamModCatalog::Catalog& catalog, int ops);
    ~GuiComponentTapParam() override { stopTimer(); }

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
    const ParamModCatalog::Catalog& catalog;
    const int opCount;

    juce::Font labelFont = juce::Font(juce::FontOptions(12.0f));

    GuiCategoryLabel cat;
    GuiToggleButton enable;
    NormalSeparator enableSeparator;
    GuiSlider steps;
    NormalSeparator stepsSeparator;

    GuiToggleButton loop;
    GuiComponentStepLoops loops;
    NormalSeparator loopSeparator;

    GuiSlider target;

    // SKIP は TAP AMP / PITCH と同じ形の行 (左にラベル、右に OFF / ON のボタン)
    GuiLabel skipLabel;
    GuiTextButton skip;
    NormalSeparator skipSeparator;

    // S.TARGET の段の組を 1 つずつ触る。P.TARGET で組を選ぶ
    GuiSlider pairTarget;
    GuiSlider scope;
    GuiComboBox param;
    GuiComponentNudgeSliderFloat value;
    GuiTextButton valueMin;
    GuiTextButton valueDown;
    GuiTextButton valueUp;
    GuiTextButton valueMax;

    GuiTextButton addPair;
    GuiTextButton deletePair;
    GuiTextButton clearPairs;

    GuiTapPairList list;
    NormalSeparator listSeparator;

    GuiTextButton increment;
    GuiTextButton decrement;
    GuiTextButton clearStep;

    juce::String paramCode;
    bool outerEnabled = true;
    bool isLoadingValue = false;
    bool isReady = false;

    std::uint32_t seenVersion = 0;
    int seenCursor = -1;
    int laidOutLoopRows = 0;

    // いま PARAM に並べている scope (-1 はチャンネル)
    int listedScope = -2;

    StepTable* table() const;
    int stepCount() const { return juce::jlimit(1, StepTable::size, (int)steps.getValue()); }
    int currentStep() const { return juce::jlimit(1, stepCount(), (int)target.getValue()); }
    int currentScope() const { return (int)scope.getValue() - 1; }

    // 指し先のパラメータ (書き方と範囲を引く)
    juce::RangedAudioParameter* paramOf(int scopeIndex, int index) const;
    juce::String valueText(const StepPair& p) const;

    // 指し先の 1 刻み (0〜1 の値で)。実数の指し先は 0 (刻まない)
    double stepOf(int scopeIndex, int index) const;

    void listParams(int scopeIndex);
    void applyStepCount();
    void relayoutIfNeeded();
    void rebindValue();
    void rebindPair();
    void writePair();
    void addCurrentPair();
    void deleteCurrentPair();
    void clearCurrentPairs();
    void toggleSkip();
    void showSkip(bool on);
    void moveCursor(int delta, bool toStart);
    void nudgeValue(double delta, bool set);
    void refresh();
    void applyActive();

    void readValues(const Io::ParamReader& r);
    void writeValues(Io::ParamWriter& w);

    void timerCallback() override;
};
