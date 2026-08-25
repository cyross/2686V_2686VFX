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
#include "../../Gui/Components/Level/Level.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"
#include "../../Gui/Components/Quality/Quality.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/SsgHwEnv/SsgHwEnv.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

// ==========================================================
// Waveform Drawing Container (Super Lightweight Custom Paint)
// ==========================================================
template <size_t tableSize>
class WaveformContainer :
    public juce::Component,
    public GuiBaseComponent,
    public juce::AudioProcessorValueTreeState::Listener // リスナーを継承
{
    // APVTSのパラメータへの直接ポインタを保持して高速にアクセスする
    std::vector<juce::RangedAudioParameter*> m_params;
    juce::StringArray m_paramIds; // リスナー削除用にIDを格納

    bool isEnabledState = false;
    int hoveredIndex = -1;
    juce::Point<int> lastMousePos;
    juce::ModifierKeys lastModifiers;

public:
    int steps = 0;

    WaveformContainer(const GuiContext& context);
    ~WaveformContainer() override;

    struct Config {
        juce::Component& parent;
        juce::String idPrefix;
    };

    void parameterChanged(const juce::String& parameterID, float newValue) override;
    void setup(const Config& c);
    void applySmoothing();
    void setCustomEnabled(bool shouldBeEnabled);
    void setAllValues(float val);
    void setValues(const std::vector<float>& values);
    std::vector<float> getValues();
    void paint(juce::Graphics& g) override;

    void mouseMove(const juce::MouseEvent& e) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;

    void updateSliderValue(const juce::MouseEvent& e);
    void updateHoverState(const juce::MouseEvent& e);
    void paintOverChildren(juce::Graphics& g) override;
};

class GuiWt : public GuiBase
{
    juce::Font labelFont = juce::Font(juce::FontOptions(16.0f));

    // Groups
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiGroup customWaveGroup;

    WaveformContainer<32> customSliders32;
    WaveformContainer<64> customSliders64;
    WaveformContainer<128> customSliders128;
    WaveformContainer<256> customSliders256;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

    GuiCategoryLabel formCat;
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

    GuiComponentSsgSwEnv11 ssgSwEnv11Component;
    GuiComponentSsgSwPEnv11 ssgSwPEnv11Component;

    // Detune
    GuiComponentMulDetune mulDetuneComponent;

    // LFO
    GuiComponentLfoOpzx7 lfo;

    // SSG Hw Env
    GuiComponentSsgHwEnv ssgHwEnv;

    GuiComboBox sizeSelector;
    GuiComboBox stepsSelector;

    GuiComboBox waveSelector;

    NormalSeparator formSeparator;

    // Moduration
	GuiToggleButton modEnableButton;
    GuiSlider modDepthSlider;
    GuiSlider modSpeedSlider;

	// Custom Waveform Reset
    GuiTextButton customWaveResetTo0Btn;
    GuiTextButton customWaveResetTo1Btn;
    GuiTextButton customWaveResetToM1Btn;

	// Custom Waveform Smoothing
    GuiTextButton customWaveSmoothBtn;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiTextButton customWaveImportBtn;
    GuiTextButton customWaveExportBtn;
    NormalSeparator uSep002;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieAmpEnv;
    GuiComponentImportExport iePitchEnv;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgSwEnv;
    GuiComponentImportExport ieSsgSwEnv11;
    GuiComponentImportExport ieSsgSwPEnv11;
    GuiComponentImportExport ieDetune;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    GuiComponentMidi midiComponent;

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
	GuiWt(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        presetName(context),
        customWaveGroup(context),
        customSliders32(context),
        customSliders64(context),
        customSliders128(context),
        customSliders256(context),
        levelComponent(context),
        qualityComponent(context),
        formCat(context),
        modCat(context),
        fixComponent(context),
        ssgHwEnv(context),
        unisonComponent(context),
        ampEnvComponent(context),
        pitchEnvComponent(context),
        ssgSwEnvComponent(context),
        ssgSwEnv11Component(context),
        ssgSwPEnv11Component(context),
        mulDetuneComponent(context),
        lfo(context),
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        customWaveSmoothBtn(context),
        customWaveImportBtn(context),
        customWaveExportBtn(context),
        uSep002(context),
        ieLfo(context),
        ieAmpEnv(context),
        iePitchEnv(context),
        ieSsgHwEnv(context),
        ieSsgSwEnv(context),
        ieSsgSwEnv11(context),
        ieSsgSwPEnv11(context),
        ieDetune(context),
        ieUnison(context),
        ieQuality(context),
        ieChParam(context),
        sizeSelector(context),
        stepsSelector(context),
        waveSelector(context),
        formSeparator(context),
        modEnableButton(context),
        modDepthSlider(context),
        modSpeedSlider(context),
        customWaveResetTo0Btn(context),
        customWaveResetTo1Btn(context),
        customWaveResetToM1Btn(context),
        midiComponent(context),
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
    void importWavetable();
    void exportWavetable();
    void initParams();
    void layoutFormCat(Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutModulationCat(juce::Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
    void setLevel(float level);
    void importQualityParam();
    void exportQualityParam();
    void importChParam();
    void exportChParam();
};
