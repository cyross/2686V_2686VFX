#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiColor.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Core/Const/ConstGlobal.h"
#include "../../Gui/Components/Fix/Fix.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/MulDetune/MulDetune.h"
#include "../../Gui/Components/AmpEnv/AmpEnv.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Gui/Components/LfoOpzx7/LfoOpzx7.h"
#include "../../Gui/Components/PresetName/PresetName.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"
#include "../../Gui/Components/Level/Level.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Quality/Quality.h"
#include "../../Gui/Components/WtMod/WtMod.h"
#include "../../Gui/Components/WtAmpMod/WtAmpMod.h"
#include "../../Gui/Components/WavePreview/WavePreview.h"
#include "../../Gui/Components/WavePreview/WavePreviewGrid.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "../../Gui/Components/SsgHwPEnv/SsgHwPEnv.h"

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
    GuiComponentSsgSwEnv11 ssgSwEnv11Component;
    GuiComponentSsgSwPEnv11 ssgSwPEnv11Component;

    GuiComponentMulDetune mulDetuneComponent;

    GuiComponentLfoOpzx7 lfoComponent;

    GuiComponentSsgHwEnv ssgHwEnv;
    GuiComponentSsgHwPEnv ssgHwPEnv;

    GuiComponentMidi midiComponent;

    // ---------------- WAVE ----------------
    GuiCategoryLabel waveCat;
    GuiSlider slotSlider;
    GuiToggleButton interpolateButton;
    GuiComboBox stepsSelector;

    // ---------------- WAVE MEMORY (32 スロット) ----------------
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
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieAmpEnv;
    GuiComponentImportExport iePitchEnv;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgHwPEnv;
    GuiComponentImportExport ieWtMod;
    GuiComponentImportExport ieWtAmpMod;
    GuiComponentImportExport ieSsgSwEnv;
    GuiComponentImportExport ieSsgSwEnv11;
    GuiComponentImportExport ieSsgSwPEnv11;
    GuiComponentImportExport ieDetune;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    GuiEnvelopeGraph graph;
    GuiToggleButton graphBtnAmp;
    GuiToggleButton graphBtnPitch;
    GuiToggleButton graphBtnSsg;
    GuiToggleButton graphBtnSsg11;
    GuiToggleButton graphBtnSsgP11;
    NormalSeparator graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };
    GraphMode currentGraphMode;

    bool isUpdatingGraph = false;

    void updateGraph();
    void setGraphMode(GraphMode mode);
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
        ssgSwEnv11Component(context),
        ssgSwPEnv11Component(context),
        mulDetuneComponent(context),
        lfoComponent(context),
        ssgHwEnv(context),
        ssgHwPEnv(context),
        midiComponent(context),
        waveCat(context),
        slotSlider(context),
        interpolateButton(context),
        stepsSelector(context),
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
        broadcastLevelButton(context),
        uSep001(context),
        ieLfo(context),
        ieAmpEnv(context),
        iePitchEnv(context),
        ieSsgHwEnv(context),
        ieSsgHwPEnv(context),
        ieWtMod(context),
        ieWtAmpMod(context),
        ieSsgSwEnv(context),
        ieSsgSwEnv11(context),
        ieSsgSwPEnv11(context),
        ieDetune(context),
        ieUnison(context),
        ieQuality(context),
        ieChParam(context),
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphBtnSsg(context),
        graphBtnSsg11(context),
        graphBtnSsgP11(context),
        graphSeparator(context)
    {
        currentGraphMode = GraphMode::Amp; // 初期状態はAmp
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePresetName(const juce::String& name);
    void initParams();
    void setupGraph();
    void layoutWaveCat(Rectangle<int>& rect);
    void layoutSlotsCat(Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutGraph(juce::Rectangle<int>& rect);
    void setLevel(float level);
    void importQualityParam();

    // 3.0.0 より前の形式を読む
    void setImportingQualityParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeQualityParams(Io::ParamWriter& writer);
    void exportQualityParam();
    void importChParam();

    // 3.0.0 より前の形式を読む
    void setImportingChParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeChParams(Io::ParamWriter& writer);
    void exportChParam();

    // 波形メモリスロットのロード / クリア
    void importSlotWave(int slot, bool isWt2);
    void clearSlotWave(int slot);
    void updateSlotFileName(int slot);

    // 対象のスロットが変わったときに、ボタンと名前の指す先をそろえる。
    void applySlotTarget();

    // いま読み込み・消去の対象になっているスロット。
    int targetSlot() const { return juce::jlimit(0, Global::WtPlus::slots - 1, (int)slotTarget.getValue()); }
    void updateSlotPreview(int slot);

    // HuC6280 モード用の変調波形
};
