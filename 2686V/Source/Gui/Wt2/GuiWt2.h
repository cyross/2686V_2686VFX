#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiColor.h"
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

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

// ==========================================================
// Waveform Drawing Container (Super Lightweight Custom Paint)
// ==========================================================
template <size_t tableSize>
class Waveform2Container :
    public juce::Component,
    public GuiBaseComponent,
    public juce::AudioProcessorValueTreeState::Listener // リスナーを継承
{
    // APVTSのパラメータへの直接ポインタを保持して高速にアクセスする
    std::vector<juce::RangedAudioParameter*> m_params;
    juce::StringArray m_paramIds; // リスナー削除用にIDを格納

    bool isEnabledState = false;
    int hoveredIndex = -1;
    int hoveredY = -1; // Y軸のホバー位置
    juce::Point<int> lastMousePos;
    juce::ModifierKeys lastModifiers;

public:
    Waveform2Container(const GuiContext& context);
    ~Waveform2Container() override;

    int resolution = 16;
    int resCenter = 8;

    struct Config {
        juce::Component& parent;
        juce::String idPrefix;
    };

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void setup(const Config& c);
    void setCustomEnabled(bool shouldBeEnabled);
    void setAllValues(int val);
    void setValues(const std::vector<int>& values);
    std::vector<int> getValues();
    void paint(juce::Graphics& g) override;

    void mouseMove(const juce::MouseEvent& e) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;

    void updateSliderValue(const juce::MouseEvent& e);
    void updateHoverState(const juce::MouseEvent& e);
    void paintOverChildren(juce::Graphics& g) override;
};

class GuiWt2 : public GuiBase
{
    int resolution = 16;
    int resCenter = 8;

    // Groups
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiScrollGroup customWaveGroup;

    Waveform2Container<32> customSliders32;
    Waveform2Container<64> customSliders64;
    Waveform2Container<128> customSliders128;
    Waveform2Container<256> customSliders256;

    GuiSlider levelSlider;

    GuiCategoryLabel formCat;
    GuiCategoryLabel qualityCat;
    GuiCategoryLabel modCat;

    GuiComponentFix fixComponent;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    // Amp ADSR
    GuiComponentAmpEnv ampEnvComponent;

    // Pitch ADSR
    GuiComponentPitchEnv pitchEnvComponent;

    // SSG SW Env
    GuiComponentSsgSwEnv ssgSwEnvComponent;

    // Detune
    GuiComponentMulDetune mulDetuneComponent;

    // LFO
    GuiComponentLfoOpzx7 lfo;

    GuiComboBox bitSelector;
    GuiComboBox rateSelector;
    GuiComboBox sizeSelector;
    GuiComboBox resoSelector;

    GuiComboBox waveSelector;

    // Moduration
	GuiToggleButton modEnableButton;
    GuiSlider modDepthSlider;
    GuiSlider modSpeedSlider;

	// Custom Waveform Reset
    GuiTextButton customWaveResetToCenterBtn;
    GuiTextButton customWaveResetToMaxBtn;
    GuiTextButton customWaveResetTo0Btn;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    GuiSeparator uSep001;
    GuiTextButton customWaveImportBtn;
    GuiTextButton customWaveExportBtn;
    GuiSeparator uSep002;
    GuiTextButton importLfoParamButton;
    GuiTextButton exportLfoParamButton;
    GuiTextButton importAmpEnvParamButton;
    GuiTextButton exportAmpEnvParamButton;
    GuiTextButton importPitchEnvParamButton;
    GuiTextButton exportPitchEnvParamButton;
    GuiTextButton importSsgSwEnvParamButton;
    GuiTextButton exportSsgSwEnvParamButton;
    GuiTextButton importDetuneParamButton;
    GuiTextButton exportDetuneParamButton;
    GuiTextButton importUnisonParamButton;
    GuiTextButton exportUnisonParamButton;
    GuiTextButton importQualityParamButton;
    GuiTextButton exportQualityParamButton;
    std::unique_ptr<juce::FileChooser> fileChooser;

    GuiComponentMidi midiComponent;

    GuiEnvelopeGraph graph;
    GuiToggleButton graphBtnAmp;
    GuiToggleButton graphBtnPitch;
    GuiToggleButton graphBtnSsg;
    GuiSeparator graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw };
    GraphMode currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    void updateGraph();
    void setGraphMode(GraphMode mode);
public:
	GuiWt2(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        presetName(context),
        customWaveGroup(context),
        customSliders32(context),
        customSliders64(context),
        customSliders128(context),
        customSliders256(context),
        levelSlider(context),
        formCat(context),
        qualityCat(context),
        modCat(context),
        fixComponent(context),
        unisonComponent(context),
        ampEnvComponent(context),
        pitchEnvComponent(context),
        ssgSwEnvComponent(context),
		mulDetuneComponent(context),
        lfo(context),
        bitSelector(context),
        rateSelector(context),
		utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        customWaveImportBtn(context),
        customWaveExportBtn(context),
        uSep002(context),
        importLfoParamButton(context),
        exportLfoParamButton(context),
        importAmpEnvParamButton(context),
        exportAmpEnvParamButton(context),
        importPitchEnvParamButton(context),
        exportPitchEnvParamButton(context),
        importSsgSwEnvParamButton(context),
        exportSsgSwEnvParamButton(context),
        importDetuneParamButton(context),
        exportDetuneParamButton(context),
        importUnisonParamButton(context),
        exportUnisonParamButton(context),
        importQualityParamButton(context),
        exportQualityParamButton(context),
        sizeSelector(context),
        resoSelector(context),
        waveSelector(context),
        modEnableButton(context),
        modDepthSlider(context),
        modSpeedSlider(context),
        customWaveResetToCenterBtn(context),
        customWaveResetToMaxBtn(context),
        customWaveResetTo0Btn(context),
        midiComponent(context),
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphBtnSsg(context),
        graphSeparator(context)
    {
        currentGraphMode = GraphMode::Amp; // 初期状態はAmp
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePresetName(const juce::String& name);
    void importWavetable();
    void exportWavetable();
    void initParams();
    void layoutFormCat(Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutModulationCat(juce::Rectangle<int>& rect);
    void layoutUtilityCat(juce::Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
    void setLevel(float level);
    void importLfoParam();
    void exportLfoParam();
    void importAmpEnvParam();
    void exportAmpEnvParam();
    void importPitchEnvParam();
    void exportPitchEnvParam();
    void importSsgSwEnvParam();
    void exportSsgSwEnvParam();
    void importDetuneParam();
    void exportDetuneParam();
    void importUnisonParam();
    void exportUnisonParam();
    void importQualityParam();
    void exportQualityParam();
};
