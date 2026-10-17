#pragma once

#include <JuceHeader.h>

#include <functional>
#include <memory>

#include "../../../Core/Io/ParamFile.h"
#include "Shared/Core/Synth/StepTable.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiValues.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
#include "Shared/Advanced/Curve/AdvancedCurve.h"
#include "../Separator/NormalSeparator.h"
#include "../StepSnap/StepSnap.h"
#include "../SsgSwButtons/SsgSwButtons.h"
#include "../PitchButtons/PitchButtons.h"
#include "../NudgeButtons/NudgeButtons.h"
#include "../NudgeSlider/NudgeSliderFloat.h"
#include "../StepValues/StepValues.h"

// ============================================================================
// 256 段のエンベロープの区分 (SSG SW AMP / PITCH ENV[256] など)
// ============================================================================
// 段の R / L は 256 段ぶんあり、パラメータではなくプロセッサの表
// (StepTable) が持つ。つまみは「対象を選ぶもの + 値」の 1 組だけ置き、
// 選んでいない段の値は帯 (GuiStepValues) へまとめて描く。帯は STEPS の
// 段数ぶんだけ並べるので、段を増やすと区分が縦に伸びる。
//
// 頭 (BYPASS / STEPS / KEEP / LOOP / STL / RR / LR / ENDL) はパラメータ。
// 値の対象の 0 は STL (パラメータ)、1 以降が表の L1〜。
//
//   Bypass / Enable
//   STEPS
//   KEEP
//   LOOP / LOOP.TO / LOOP.CNT
//   R.TARGET / RATE / 帯 (R1〜)
//   L.TARGET / LEVEL / (STEPS) / 帯 (STL, L1〜)
//   RR / LR      リリース
//   Use Endl / ENDL
//
// 種類ごとに違うところ (見出し・鍵・値の範囲・補正ボタンなど) は Spec で渡す。
class GuiComponentStepEnv : public GuiBase, private juce::Timer {
public:
    struct Spec {
        enum class Kind { Amp, Pitch };

        Kind kind = Kind::Amp;
        juce::String title;

        // 表の鍵とパラメータの頭 (CPK::ssgSwEnv256 など)。表は code + tableKey。
        juce::String tableKey;

        // パラメータの鍵 (code の後ろ)
        juce::String steps, keep, loop, loopTo, loopCount, stl, lr, rr, endl, endlEnable;

        // 値の範囲。整数なら interval を 1 にする
        double rateMin = 0.0, rateMax = 1.0, rateInterval = 0.001;
        double levelMin = 0.0, levelMax = 1.0, levelInterval = 0.001;
        int levelDecimals = 3;

        // 表示の名前
        juce::String startLabel = "STL";
        juce::String levelPrefix = "L";
        juce::String levelLabel = "LEVEL";
        juce::String levelTargetLabel = "L.TARGET";

        // 区分のパラメータファイル
        juce::String fileFormat;            // 印 ("ssgSwEnv256")
        juce::String legacyFileFormat;      // 3.6.3 までの [11] の印 ("ssgSwEnv11")
        juce::String browserKind;
        juce::String extension;
    };

    GuiComponentStepEnv(const GuiContext& context, const Spec& spec);
    ~GuiComponentStepEnv() override { stopTimer(); }

    // 区分の見出し。右クリックの IMPORT / EXPORT をつなぐのに使う
    GuiCategoryLabel& category() { return cat; }

    // 簡易表示モードで丸ごと隠す。見出しごと消え、縦の場所も取らない。
    void setCategoryVisible(bool visible) {
        cat.setHidden(!visible);
        cat.setVisible(visible);
    }

    // 簡易表示モードの一括操作で使う口。「切ってあるか」で答える。
    bool hasBypassSwitch() const { return true; }
    bool isCategoryBypassed() const { return isEnable ? !flag.getToggleState() : flag.getToggleState(); }

    void setCategoryBypassed(bool bypassed) {
        flag.setToggleState(isEnable ? !bypassed : bypassed, juce::sendNotification);
    }

    // 見出しの開閉
    void setCategoryOpen(bool open) { cat.setDetailVisible(open); }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable = false);

    // 束縛先を丸ごと差し替える。TARGET で指し先を切り替えるときに使う。
    void rebind(const juce::String& code);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    void setupGraph(std::function<void()> repaintGraph);

    // Curve を持たないプラグインは graph だけを渡す。
    void updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore = nullptr, bool isCurveMode = false, int posIdx = 0);
    void setEnabled(bool enabled);

    // 外から止められているか (鳴っていないオペレーターなど)。setEnabled で入る。
    bool outerEnabled = true;

    // 札 (Bypass / Enable) が効いていないあいだは、札のほかをすべて止める。
    void applyActive();

    void importParams();
    void applyParamsFile(const juce::File& file);
    void exportParams();
    void writeParamsFile(const juce::File& file);

    // 3.0.0 より前の、行の並びで書いたファイル ([11] の形)
    void setImportingParams(juce::StringArray& lines, int& index);

    // 名前で受け渡す。key の中身が無ければ、3.6.3 までの [11] の鍵
    // (key の "256" を "11" にしたもの) を読み、[256] へ写す。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();

private:
    const Spec spec;

    bool isEnable = false;
    juce::Font labelFont = juce::Font(juce::FontOptions(12.0f));

    GuiCategoryLabel cat;
    GuiToggleButton flag; // Bypass or Enable
    NormalSeparator flagSeparator;
    GuiSlider steps;
    NormalSeparator stepsSeparator;

    // 段ごとのレベルを斜めに繋がず、その段のあいだ保ち続ける。
    GuiToggleButton keep;
    NormalSeparator keepSeparator;
    GuiToggleButton loop;
    GuiSlider loopTo;
    GuiSlider loopCount;
    NormalSeparator loopSeparator;

    GuiSlider rateTarget;
    GuiComponentNudgeSliderFloat rate;
    GuiComponentNudgeButtons rateNudge;
    GuiStepValues rateValues;
    NormalSeparator rateSeparator;

    GuiSlider levelTarget;
    GuiComponentNudgeSliderFloat level;

    // 音量の LEVEL / LR / ENDL を決まった刻みへ丸める (Amp だけ)
    GuiComponentStepSnap levelSteps;

    // 値を少しずつ動かすボタン。Amp は音量の、Pitch は音程の刻み
    GuiComponentSsgSwButtons ampBtns;
    GuiComponentPitchButtons pitchBtns;
    GuiStepValues levelValues;

    NormalSeparator releaseSeparator;
    GuiSlider releaseRate;
    GuiSlider releaseLevel;

    NormalSeparator endLevelSeparator;
    GuiToggleButton endLevelEnable;
    GuiSlider endLevel;

    juce::String paramCode;
    juce::String m_flagKey;

    std::function<void()> repaintGraph;

    // applyLoopValues の入れ子呼び出しを弾くための印
    bool isApplyingLoopValues = false;

    // 表を書き換えているところ。値のつまみの知らせを表へ返さないための印
    bool isLoadingValue = false;

    // 表はパラメータでないので、プリセットの読み込み・初期化・貼り付けで
    // 変わっても知らせが来ない。版を見張って、変わったら映し直す。
    std::uint32_t seenVersion = 0;
    void timerCallback() override;
    void markSeen();

    bool isAmp() const { return spec.kind == Spec::Kind::Amp; }

    StepTable* table() const;
    int stepCount() const { return juce::jlimit(1, StepTable::size, (int)steps.getValue()); }

    void applyLoopValues(bool enabled);
    void applyStepCount();
    void rebindRate();
    void rebindLevel();
    void refreshStepValues();
    void onRateChanged();
    void onLevelChanged();
    void changed();

    void setLevelRange();
    void readValues(const Io::ParamReader& r);
    void writeValues(Io::ParamWriter& w);
};
