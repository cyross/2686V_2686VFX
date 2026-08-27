#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiColor.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Core/Const/ConstGlobal.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
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
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/SsgHwEnv/SsgHwEnv.h"

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

    GuiComponentMidi midiComponent;

    // ---------------- WAVE ----------------
    GuiCategoryLabel waveCat;
    GuiSlider slotSlider;
    GuiToggleButton interpolateButton;
    GuiComboBox stepsSelector;

    // ---------------- WAVE MEMORY (32 スロット) ----------------
    GuiCategoryLabel slotsCat;
    std::array<GuiTextButton, Global::WtPlus::slots> slotWtBtn;
    std::array<GuiTextButton, Global::WtPlus::slots> slotWt2Btn;
    std::array<GuiTextButton, Global::WtPlus::slots> slotClearBtn;
    std::array<GuiLabel, Global::WtPlus::slots> slotFileNameLabel;

    // ---------------- MODULATION ----------------
    GuiCategoryLabel modCat;
    GuiToggleButton modEnableButton;
    GuiSlider modDepthSlider;
    GuiSlider modSpeedSlider;
    GuiComboBox modShapeSelector;
    GuiTextButton modWaveWtBtn;
    GuiTextButton modWaveWt2Btn;
    GuiTextButton modWaveClearBtn;
    GuiLabel modWaveFileNameLabel;
    GuiToggleButton modWaveSmoothBtn;

    // HuC6280 モードの変調波形 (32 サンプル) を書き込むためのパラメータ
    std::array<juce::RangedAudioParameter*, 32> modWaveParams = { nullptr };

    // ---------------- UTILITY ----------------
    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieAmpEnv;
    GuiComponentImportExport iePitchEnv;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgSwEnv;
    GuiComponentImportExport ieSsgSwEnv11;
    GuiComponentImportExport ieSsgSwPEnv11;
    GuiComponentImportExport ieDetune;
    GuiComponentImportExport ieUnison;
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

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

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
        midiComponent(context),
        waveCat(context),
        slotSlider(context),
        interpolateButton(context),
        stepsSelector(context),
        slotsCat(context),
        slotWtBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                   GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                   GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                   GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                   GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                   GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                   GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                   GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        slotWt2Btn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                    GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                    GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                    GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                    GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                    GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                    GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                    GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        slotClearBtn{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                      GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                      GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                      GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                      GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                      GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                      GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context),
                      GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        slotFileNameLabel{ GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context),
                           GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context),
                           GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context),
                           GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context),
                           GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context),
                           GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context),
                           GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context),
                           GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context) },
        modCat(context),
        modEnableButton(context),
        modDepthSlider(context),
        modSpeedSlider(context),
        modShapeSelector(context),
        modWaveWtBtn(context),
        modWaveWt2Btn(context),
        modWaveClearBtn(context),
        modWaveFileNameLabel(context),
        modWaveSmoothBtn(context),
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        ieLfo(context),
        ieAmpEnv(context),
        iePitchEnv(context),
        ieSsgHwEnv(context),
        ieSsgSwEnv(context),
        ieSsgSwEnv11(context),
        ieSsgSwPEnv11(context),
        ieDetune(context),
        ieUnison(context),
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
    void layoutModulationCat(Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutGraph(juce::Rectangle<int>& rect);
    void setLevel(float level);
    void importChParam();
    void exportChParam();

    // 波形メモリスロットのロード / クリア
    void importSlotWave(int slot, bool isWt2);
    void clearSlotWave(int slot);
    void updateSlotFileName(int slot);

    // HuC6280 モード用の変調波形
    void importModWave(bool isWt2);
    bool applyModWaveFile(const juce::File& file, bool isWt2);
    void clearModWave();
    void updateModWaveFileName(const juce::String& fileName);
};
