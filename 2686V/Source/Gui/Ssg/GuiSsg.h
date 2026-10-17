#pragma once

#include "Shared/Gui/Components/ToneNoiseValue/ToneNoiseValue.h"
#include <JuceHeader.h>
#include <array>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "Shared/Advanced/Curve/AdvancedCurve.h"
#include "Shared/Gui/Components/Fix/Fix.h"
#include "Shared/Gui/Components/Unison/Unison.h"
#include "Shared/Gui/Components/MulDetune/MulDetune.h"
#include "Shared/Gui/Components/AmpEnv/AmpEnv.h"
#include "Shared/Gui/Components/PitchEnv/PitchEnv.h"
#include "Shared/Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "Shared/Gui/Components/Midi/Midi.h"
#include "Shared/Gui/Components/Edit/Edit.h"
#include "Shared/Gui/Components/LfoOpzx7/LfoOpzx7.h"
#include "Shared/Gui/Components/PresetName/PresetName.h"
#include "Shared/Gui/Components/ImportExport/ImportExport.h"
#include "Shared/Gui/Components/Level/Level.h"
#include "Shared/Gui/Components/Separator/NormalSeparator.h"
#include "Shared/Gui/Components/WaveHold/WaveHold.h"
#include "Shared/Gui/Components/Separator/ShortSeparator.h"
#include "Shared/Gui/Components/Quality/Quality.h"
#include "Shared/Gui/Components/SsgSwEnv256/SsgSwEnv256.h"
#include "Shared/Gui/Components/SsgSwPEnv256/SsgSwPEnv256.h"
#include "Shared/Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "Shared/Gui/Components/SsgHwPEnv/SsgHwPEnv.h"
#include "Shared/Gui/Components/WtMod/WtMod.h"
#include "Shared/Gui/Components/WtAmpMod/WtAmpMod.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiSsg : public GuiBase
{
    // Pane Groups
    GuiScrollGroup mainGroup;
    GuiGroup dutyGroup;
    GuiGroup triGroup;

    GuiComponentPresetName presetName;

    // OPTIONAL。いまは再生速度だけだが、この先もここへ足していく。
    GuiCategoryLabel optionalCat;
    GuiSlider speedSlider;
    NormalSeparator optSpeedSeparator;

    // ホールドと部分再生。SPEED の下へ置く。
    GuiComponentWaveHold waveHold;

    GuiCategoryLabel formCat;
    GuiCategoryLabel pulseInvCat;
    GuiCategoryLabel triPeakCat;
    GuiCategoryLabel hwEnvCat;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

    GuiSlider toneSlider;

    GuiSlider noiseSlider;

    GuiSlider noiseFreqSlider;

    // TONE / NOISE を実機の設定値で決める (N.FREQ の下)
    GuiComponentToneNoiseValue toneNoiseValue;

    GuiToggleButton noiseOnNoteButton;

    GuiSlider mixSlider;

    GuiTextButton mixSetTone;  // 0.0

    GuiTextButton mixSetMix;   // 0.5

    GuiTextButton mixSetNoise; // 1.0

    GuiComboBox waveSelector;

    NormalSeparator formSeparator;

    GuiComponentFix fixComponent;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    // Amp ADSR
    GuiComponentAmpEnv ampEnvComponent;
    // MODULATION (FDS / WonderSwan / HuC6280)
    GuiComponentWtMod modComponent;
    GuiComponentWtAmpMod ampModComponent;

    // Pitch ADSR
    GuiComponentPitchEnv pitchEnvComponent;

    // SSG SW Env
    GuiComponentSsgSwEnv ssgSwEnvComponent;

    GuiComponentSsgSwEnv256 ssgSwEnv256Component;
    GuiComponentSsgSwPEnv256 ssgSwPEnv256Component;
    GuiComponentSsgHwEnv ssgHwEnvComponent;
    GuiComponentSsgHwPEnv ssgHwPEnvComponent;

    // Detune
    GuiComponentMulDetune mulDetuneComponent;

    // LFO
    GuiComponentLfoOpzx7 lfo;

    // Duty
    GuiComboBox dutyModeSelector; // Preset / Variable
    GuiComboBox dutyPresetSelector;
    GuiSlider dutyVarSlider;
    GuiToggleButton dutyInvertButton;
    GuiToggleButton dutyFcButton;
    GuiSlider dutyFcFlucSlider;

    // ファミコン / NES の鳴り方。FC Mode と合わせて 1 つだけ入れられる。
    GuiToggleButton dutyMoreFcButton;
    GuiToggleButton dutyMoreNesButton;

    // Triangle
    GuiToggleButton triKeyTrackButton;
    GuiSlider triFreqSlider;
    GuiSlider triPeakSlider;
    GuiTextButton triSetSawDown; // 0.0
    GuiTextButton triSetTri;     // 0.5
    GuiTextButton triSetSawUp;   // 1.0

    GuiComponentMidi midiComponent;

    // EDIT 区分 (切り取り・コピー・貼り付け・初期化)
    GuiComponentEdit editComponent;

    GuiCategoryLabel utilityCat;

    // 大区分。[[AMP ENV]] と [[PITCH ENV]]
    GuiCategoryLabel ampMajorCat;
    GuiCategoryLabel pitchMajorCat;
    GuiTextButton broadcastLevelButton;
    GuiComponentImportExport ieToneNoise;
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
    GuiToggleButton graphBtnSsg11;
    GuiToggleButton graphBtnSsgP256;
    NormalSeparator graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw256, SsgSwP256 };
    GraphMode currentGraphMode;

    CurveCore* p_curveCore = nullptr;

    bool isUpdatingGraph = false;

    void updateGraph();
    void setGraphMode(GraphMode mode);
public:
	GuiSsg(const GuiContext& context) :
        GuiBase(context), 
        mainGroup(context),
        presetName(context),
        dutyGroup(context),
        triGroup(context),
        optionalCat(context),
        speedSlider(context),
        optSpeedSeparator(context),
        waveHold(context),
        formCat(context),
        pulseInvCat(context),
        triPeakCat(context),
        hwEnvCat(context),
        levelComponent(context),
        qualityComponent(context),
        toneSlider(context),
        noiseSlider(context),
        noiseFreqSlider(context),
        toneNoiseValue(context),
        noiseOnNoteButton(context),
        mixSlider(context),
        mixSetTone(context),
        mixSetMix(context),
        mixSetNoise(context),
        waveSelector(context),
        formSeparator(context),
        fixComponent(context),
        unisonComponent(context),
        ampEnvComponent(context),
        modComponent(context),
        ampModComponent(context),
        pitchEnvComponent(context),
        ssgSwEnvComponent(context),
        ssgSwEnv256Component(context),
        ssgSwPEnv256Component(context),
        ssgHwEnvComponent(context),
        ssgHwPEnvComponent(context),
        mulDetuneComponent(context),
        lfo(context),
        dutyModeSelector(context),
        dutyPresetSelector(context),
        dutyVarSlider(context),
        dutyInvertButton(context),
        dutyFcButton(context),
        dutyFcFlucSlider(context),
        dutyMoreFcButton(context),
        dutyMoreNesButton(context),
        triKeyTrackButton(context),
        triFreqSlider(context),
        triPeakSlider(context),
        triSetSawDown(context),
        triSetTri(context),
        triSetSawUp(context),
        midiComponent(context),
        editComponent(context),
        utilityCat(context),
        ampMajorCat(context),
        pitchMajorCat(context),
        broadcastLevelButton(context),
        ieToneNoise(context),
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
        graphBtnSsg11(context),
        graphBtnSsgP256(context),
        graphSeparator(context)
    {
        currentGraphMode = GraphMode::Amp; // 初期状態はAmp
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;

    // 簡易表示モードで隠す区分への一括操作
    void bypassHiddenCategories() override;
    void openEnabledCategories() override;
    void closeBypassedCategories() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePresetName(const juce::String& name);
    void initParams();
    void layoutFormCat(Rectangle<int>& rect);
    void layoutOptionalCat(Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
    void setLevel(float level);
    void importToneNoiseParam();

    // ブラウザから直に読ませるための入口。
    void applyToneNoiseParamFile(const juce::File& file);
    // 3.0.0 より前の形式を読む
    void setImportingToneNoiseParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeToneNoiseParams(Io::ParamWriter& writer);
    void exportToneNoiseParam();
    // ブラウザから直に渡せるようにした入口。
    void writeToneNoiseParamFile(const juce::File& file);
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
};
