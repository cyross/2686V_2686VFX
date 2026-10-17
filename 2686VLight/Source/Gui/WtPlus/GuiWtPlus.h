#pragma once

#include <JuceHeader.h>
#include <array>

#include "Shared/Gui/Components/PathRoot/PathRoot.h"
#include "Shared/Core/Gui/GuiColor.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
#include "Shared/Core/Const/ConstGlobal.h"
#include "Shared/Gui/Components/Fix/Fix.h"
#include "Shared/Gui/Components/Unison/Unison.h"
#include "Shared/Gui/Components/MulDetune/MulDetune.h"
#include "Shared/Gui/Components/AmpEnv/AmpEnv.h"
#include "Shared/Gui/Components/PitchEnv/PitchEnv.h"
#include "Shared/Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "Shared/Gui/Components/Midi/Midi.h"
#include "Shared/Gui/Components/Edit/Edit.h"
#include "Shared/Gui/Components/Edit/TargetsDialog.h"
#include "Shared/Gui/Components/LfoOpzx7/LfoOpzx7.h"
#include "Shared/Gui/Components/PresetName/PresetName.h"
#include "Shared/Gui/Components/ImportExport/ImportExport.h"
#include "Shared/Gui/Components/Level/Level.h"
#include "Shared/Gui/Components/Separator/NormalSeparator.h"
#include "Shared/Gui/Components/WaveHold/WaveHold.h"
#include "Shared/Gui/Components/Quality/Quality.h"
#include "Shared/Gui/Components/WtMod/WtMod.h"
#include "Shared/Gui/Components/WtAmpMod/WtAmpMod.h"
#include "Shared/Gui/Components/WavePreview/WavePreview.h"
#include "Shared/Gui/Components/WavePreview/WavePreviewGrid.h"
#include "Shared/Gui/Components/SsgSwEnv256/SsgSwEnv256.h"
#include "Shared/Gui/Components/SsgSwPEnv256/SsgSwPEnv256.h"
#include "Shared/Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "Shared/Gui/Components/SsgHwPEnv/SsgHwPEnv.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

// ==========================================================
// WT+ : 波形メモリを 32 スロット持つ拡張ウェーブテーブル
// ==========================================================
// 波形データはプロセッサが所有し、ここではロード操作とファイル名表示だけを扱う。
class GuiWtPlus : public GuiBase {
    GuiScrollGroup mainGroup;

    // WAVE と WAVE MEMORY をまとめる専用グループ
    GuiScrollGroup waveGroup;

    GuiComponentPresetName presetName;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

    GuiComponentFix fixComponent;

    GuiComponentUnison unisonComponent;

    GuiComponentAmpEnv ampEnvComponent;
    GuiComponentPitchEnv pitchEnvComponent;
    GuiComponentSsgSwEnv ssgSwEnvComponent;
    GuiComponentSsgSwEnv256 ssgSwEnv256Component;
    GuiComponentSsgSwPEnv256 ssgSwPEnv256Component;

    GuiComponentMulDetune mulDetuneComponent;

    GuiComponentLfoOpzx7 lfoComponent;

    GuiComponentSsgHwEnv ssgHwEnv;
    GuiComponentSsgHwPEnv ssgHwPEnv;

    GuiComponentMidi midiComponent;

    // EDIT 区分 (切り取り・コピー・貼り付け・初期化)
    GuiComponentEdit editComponent;

    // TARGET の複写・消去のダイアログ。WAVE MEMORY / FORM の枠を隠さない
    // ところ (枠の右の空き) へ出す。
    GuiTargetsDialog targetsDialog;
    juce::Rectangle<int> targetsDialogArea;
    void openTargetsDialog(GuiTargetsDialog::Mode mode);

    // ---------------- WAVE ----------------
    GuiCategoryLabel waveCat;
    GuiSlider slotSlider;
    GuiToggleButton interpolateButton;
    GuiComboBox stepsSelector;

    // ---------------- WAVE MEMORY (32 スロット) ----------------
    // OPTIONAL。いまは再生速度だけだが、この先もここへ足していく。
    GuiCategoryLabel optionalCat;
    GuiSlider speedSlider;
    NormalSeparator optSpeedSeparator;

    // ホールドと部分再生。SPEED の下へ置く。
    GuiComponentWaveHold waveHold;

    GuiCategoryLabel slotsCat;
    // スロットごとに読み込みボタンを並べる代わりに、対象を選ぶつまみと
    // 1 組のボタンを置く。波形はまとめて 1 つの区画へ描く。
    GuiSlider slotTarget;
    GuiTextButton slotWtBtn;
    GuiTextButton slotWt2Btn;
    GuiTextButton slotClearBtn;
    GuiLabel slotFileNameLabel;
    GuiWavePreviewGrid slotPreviews;

    // ---------------- MODULATION ----------------
    GuiComponentWtMod modComponent;
    GuiComponentWtAmpMod ampModComponent;


    // ---------------- UTILITY ----------------
    GuiCategoryLabel utilityCat;
    GuiComponentPathRoot pathRoot;

    // 大区分。[[AMP ENV]] と [[PITCH ENV]]
    GuiCategoryLabel ampMajorCat;
    GuiCategoryLabel pitchMajorCat;
    GuiTextButton broadcastLevelButton;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieAmpEnv;
    GuiComponentImportExport iePitchEnv;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgHwPEnv;
    GuiComponentImportExport ieWtMod;
    GuiComponentImportExport ieWtAmpMod;
    GuiComponentImportExport ieSsgSwEnv;
    GuiComponentImportExport ieSsgSwEnv256;
    GuiComponentImportExport ieSsgSwPEnv256;
    GuiComponentImportExport ieDetune;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    GuiEnvelopeGraph graph;
    GuiToggleButton graphBtnAmp;
    GuiToggleButton graphBtnPitch;
    GuiToggleButton graphBtnSsg;
    GuiToggleButton graphBtnSsg256;
    GuiToggleButton graphBtnSsgP256;
    NormalSeparator graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw256, SsgSwP256 };
    GraphMode currentGraphMode;

    bool isUpdatingGraph = false;

    void updateGraph();
    void setGraphMode(GraphMode mode);
public:
    // 区分の外に持っている素材 (ParamEdit の切り取り・コピー・貼り付け)
    void collectClipAssets(const juce::String& prefix, bool wholeChannel,
        std::map<juce::String, juce::String>& out) override;
    void applyClipAssets(const juce::String& prefix, bool wholeChannel,
        const std::map<juce::String, juce::String>& assets, bool clearOthers) override;

private:
    std::vector<ParamEdit::AssetSlot> clipAssetSlots(const juce::String& prefix, bool wholeChannel);
public:
    GuiWtPlus(const GuiContext& context) : GuiBase(context),
        mainGroup(context),
        waveGroup(context),
        presetName(context),
        levelComponent(context),
        qualityComponent(context),
        fixComponent(context),
        unisonComponent(context),
        ampEnvComponent(context),
        pitchEnvComponent(context),
        ssgSwEnvComponent(context),
        ssgSwEnv256Component(context),
        ssgSwPEnv256Component(context),
        mulDetuneComponent(context),
        lfoComponent(context),
        ssgHwEnv(context),
        ssgHwPEnv(context),
        midiComponent(context),
        editComponent(context),
        targetsDialog(context),
        waveCat(context),
        slotSlider(context),
        interpolateButton(context),
        stepsSelector(context),
        optionalCat(context),
        speedSlider(context),
        optSpeedSeparator(context),
        waveHold(context),
        slotsCat(context),
        slotTarget(context),
        slotWtBtn(context),
        slotWt2Btn(context),
        slotClearBtn(context),
        slotFileNameLabel(context),
        slotPreviews(context),
        modComponent(context),
        ampModComponent(context),
        utilityCat(context),
        pathRoot(context),
        ampMajorCat(context),
        pitchMajorCat(context),
        broadcastLevelButton(context),
        ieLfo(context),
        ieAmpEnv(context),
        iePitchEnv(context),
        ieSsgHwEnv(context),
        ieSsgHwPEnv(context),
        ieWtMod(context),
        ieWtAmpMod(context),
        ieSsgSwEnv(context),
        ieSsgSwEnv256(context),
        ieSsgSwPEnv256(context),
        ieDetune(context),
        ieUnison(context),
        ieQuality(context),
        ieChParam(context),
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphBtnSsg(context),
        graphBtnSsg256(context),
        graphBtnSsgP256(context),
        graphSeparator(context)
    {
        currentGraphMode = GraphMode::Amp; // 初期状態はAmp
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;

    // Ctrl + ← / → などで TARGET を動かす
    bool keyPressed(const juce::KeyPress& key) override;

    // 簡易表示モードで隠す区分への一括操作
    void bypassHiddenCategories() override;
    void openEnabledCategories() override;
    void closeBypassedCategories() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePresetName(const juce::String& name);
    void initParams();
    void setupGraph();
    void layoutWaveCat(Rectangle<int>& rect);
    void layoutSlotsCat(Rectangle<int>& rect);
    void layoutOptionalCat(Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutGraph(juce::Rectangle<int>& rect);
    void setLevel(float level);
    void importQualityParam();

    // ブラウザから直に読ませるための入口。
    void applyQualityParamFile(const juce::File& file);
    // 3.0.0 より前の形式を読む
    void setImportingQualityParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeQualityParams(Io::ParamWriter& writer);
    void exportQualityParam();
    // ブラウザから直に渡せるようにした入口。
    void writeQualityParamFile(const juce::File& file);
    void importChParam();

    // ブラウザから直に読ませるための入口。
    // importChParam() からダイアログを外したもの。
    void applyChParamFile(const juce::File& file) override;
    // 3.0.0 より前の形式を読む
    void setImportingChParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeChParams(Io::ParamWriter& writer);
    void exportChParam();

    // ブラウザから直に渡せるようにした入口。
    void writeChParamFile(const juce::File& file);
    // 波形メモリスロットのロード / クリア
    void importSlotWave(int slot, bool isWt2);
    void clearSlotWave(int slot);
    void updateSlotFileName(int slot);

    // 全スロットの波形と、対象のスロットの名前を読み直す。
    // プリセットを読んだあとなど、どのスロットも入れ替わっているときに使う。
    void refreshSlots();

    // 対象のスロットが変わったときに、ボタンと名前の指す先をそろえる。
    void applySlotTarget();

    // OPTIONAL はスロットごとに値を持つ。TARGET が変わったら
    // つまみの束ねる先をそのスロットのものへ差し替える。
    void rebindOptional();

    // いま読み込み・消去の対象になっているスロット。
    int targetSlot() const { return juce::jlimit(0, Global::WtPlus::slots - 1, (int)slotTarget.getValue()); }
    void updateSlotPreview(int slot);

    // HuC6280 モード用の変調波形
};
