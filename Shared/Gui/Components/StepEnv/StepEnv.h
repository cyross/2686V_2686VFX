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
#include "../ValueButtons/ValueButtons.h"
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
        // Amp   : 音量 (SSG SW AMP ENV[256])
        // Pitch : 音程 (SSG SW PITCH ENV[256])
        // Duty  : 矩形波のデューティ比 % (DUTY RATIO ENV)
        // Peak  : 三角波の頂点 0〜1 (PEAK RATIO ENV)
        // Tl    : オペレーターの TL (TL ENV)。常に KEEP で、KEEP の札を持たない
        // Alg   : チャンネルの ALG (ALG ENV)。常に KEEP。値を動かすボタンも STEPS も持たない
        enum class Kind { Amp, Pitch, Duty, Peak, Tl, Alg };

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
        juce::String sixStepFileFormat;     // 3.6.3 までの 6 段の印 ("ssgSwEnv")。無ければ空
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

    // 3.0.0 より前のチャンネルのファイルは、6 段の行のすぐ後に [11] の行が続く。
    // 両方を読み進め、6 段だけが使われていたら 6 段を、そうでなければ [11] を写す。
    void setImportingParamsWithSixStep(juce::StringArray& lines, int& index);

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
    // RATE を動かすボタン。±0.001〜±1.0 と、0.001 / 0.01 / 0.1 / 1.0 を直に置くもの。
    // RATE の下限は 0.001 なので、「0」ではなく「0.001」を置く。
    GuiComponentValueButtons rateNudge;
    GuiStepValues rateValues;
    NormalSeparator rateSeparator;

    GuiSlider levelTarget;
    GuiComponentNudgeSliderFloat level;

    // 値を少しずつ動かすボタン。Amp は音量の、Pitch は音程の刻み
    GuiComponentSsgSwButtons ampBtns;
    GuiComponentPitchButtons pitchBtns;

    // DUTY RATIO ENV は MODE で選べるデューティ比を直に置き、TL ENV は TL を少しずつ動かす
    GuiComponentValueButtons valueBtns;
    GuiStepValues levelValues;

    NormalSeparator releaseSeparator;
    GuiSlider releaseRate;
    GuiSlider releaseLevel;

    // RR / LR を動かすボタン。RR は RATE と、LR は値 (LEVEL など) と同じ並び
    GuiComponentValueButtons releaseRateBtns;
    GuiComponentValueButtons releaseLevelBtns;

    NormalSeparator endLevelSeparator;
    GuiToggleButton endLevelEnable;
    GuiSlider endLevel;

    // LEVEL / LR / ENDL を決まった刻みへ丸める (Amp と TL)。
    // 3 つのつまみへ聞き手を付けるので、つまみより後に置く (先に壊れて外す側になる)。
    // 前に置いていたら、壊すときに先に消えたつまみから聞き手を外しに行って落ちていた。
    GuiComponentStepSnap levelSteps;

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
    bool isPitch() const { return spec.kind == Spec::Kind::Pitch; }
    bool isDuty() const { return spec.kind == Spec::Kind::Duty; }
    bool isTl() const { return spec.kind == Spec::Kind::Tl; }

    // 値を置く・足すボタン (valueBtns) を使うもの
    bool usesValueButtons() const { return isDuty() || isTl(); }

    // LEVEL・LR・ENDL を決まった刻みへ丸める STEPS を持つもの
    bool hasLevelSteps() const { return isAmp() || isTl(); }

    // KEEP の札を持つもの (TL ENV は常に KEEP)
    bool hasKeep() const { return spec.keep.isNotEmpty(); }

    // 0〜1 の値を少しずつ動かすボタン (ampBtns) を使うもの
    bool usesUnitButtons() const { return spec.kind == Spec::Kind::Amp || spec.kind == Spec::Kind::Peak; }

    StepTable* table() const;
    int stepCount() const { return juce::jlimit(1, StepTable::size, (int)steps.getValue()); }

    // 帯 (R と L) の行数の合計。区分の高さはこれで決まる
    int stripRowCount() const { return rateValues.getRowCount() + levelValues.getRowCount(); }

    // 最後に並べたときの帯の行数。0 はまだ並べていない
    int laidOutRows = 0;

    void applyLoopValues(bool enabled);
    void applyStepCount();
    void rebindRate();
    void rebindLevel();
    void refreshStepValues();
    void onRateChanged();
    void onLevelChanged();
    void changed();

    void setLevelRange();
    std::vector<GuiComponentValueButtons::Entry> valueButtonEntries() const;
    static std::vector<GuiComponentValueButtons::Entry> rateButtonEntries();
    std::vector<GuiComponentValueButtons::Entry> releaseLevelEntries() const;

    // LR にボタンを置くか (ALG ENV は置かない)
    bool hasReleaseLevelButtons() const { return spec.kind != Spec::Kind::Alg; }
    void readValues(const Io::ParamReader& r);
    void readSixStepValues(const Io::ParamReader& r);
    void setImportingSixStepParams(juce::StringArray& lines, int& index);
    bool isActiveIn(const Io::ParamReader& r) const;
    void writeValues(Io::ParamWriter& w);
};
