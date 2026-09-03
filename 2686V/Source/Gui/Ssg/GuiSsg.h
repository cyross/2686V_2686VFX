#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
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
#include "../../Gui/Components/Separator/ShortSeparator.h"
#include "../../Gui/Components/Quality/Quality.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "../../Gui/Components/SsgHwPEnv/SsgHwPEnv.h"
#include "../../Gui/Components/WtMod/WtMod.h"
#include "../../Gui/Components/WtAmpMod/WtAmpMod.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiSsg : public GuiBase
{
    // Pane Groups
    GuiScrollGroup mainGroup;
    GuiGroup dutyGroup;
    GuiGroup triGroup;

    GuiComponentPresetName presetName;

    GuiCategoryLabel formCat;
    GuiCategoryLabel pulseInvCat;
    GuiCategoryLabel triPeakCat;
    GuiCategoryLabel hwEnvCat;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

    GuiSlider toneSlider;

    GuiSlider noiseSlider;

    GuiSlider noiseFreqSlider;

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

    GuiComponentSsgSwEnv11 ssgSwEnv11Component;
    GuiComponentSsgSwPEnv11 ssgSwPEnv11Component;
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

    // Triangle
    GuiToggleButton triKeyTrackButton;
    GuiSlider triFreqSlider;
    GuiSlider triPeakSlider;
    GuiTextButton triSetSawDown; // 0.0
    GuiTextButton triSetTri;     // 0.5
    GuiTextButton triSetSawUp;   // 1.0

    GuiComponentMidi midiComponent;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiComponentImportExport ieToneNoise;
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

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

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
        formCat(context),
        pulseInvCat(context),
        triPeakCat(context),
        hwEnvCat(context),
        levelComponent(context),
        qualityComponent(context),
        toneSlider(context),
        noiseSlider(context),
        noiseFreqSlider(context),
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
        ssgSwEnv11Component(context),
        ssgSwPEnv11Component(context),
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
        triKeyTrackButton(context),
        triFreqSlider(context),
        triPeakSlider(context),
        triSetSawDown(context),
        triSetTri(context),
        triSetSawUp(context),
        midiComponent(context),
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        ieToneNoise(context),
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

    // 簡易表示モードで隠す区分への一括操作
    void bypassHiddenCategories() override;
    void openEnabledCategories() override;
    void closeBypassedCategories() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePresetName(const juce::String& name);
    void initParams();
    void layoutFormCat(Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
    void setLevel(float level);
    void importToneNoiseParam();

    // 3.0.0 より前の形式を読む
    void setImportingToneNoiseParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeToneNoiseParams(Io::ParamWriter& writer);
    void exportToneNoiseParam();
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
};
