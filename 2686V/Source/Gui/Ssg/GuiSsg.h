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

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiSsg : public GuiBase
{
    // Pane Groups
    GuiScrollGroup mainGroup;
    GuiGroup dutyGroup;
    GuiGroup triGroup;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel shapeCat;
    GuiCategoryLabel toneCat;
    GuiCategoryLabel noiseCat;
    GuiCategoryLabel mixCat;
    GuiCategoryLabel pulseInvCat;
    GuiCategoryLabel triPeakCat;
    GuiCategoryLabel hwEnvCat;

    GuiSlider levelSlider;

    GuiSlider toneSlider;

    GuiSlider noiseSlider;

    GuiSlider noiseFreqSlider;

    GuiToggleButton noiseOnNoteButton;

    GuiSlider mixSlider;

    GuiTextButton mixSetTone;  // 0.0

    GuiTextButton mixSetMix;   // 0.5

    GuiTextButton mixSetNoise; // 1.0

    GuiComboBox waveSelector;

    GuiComboBox bitSelector;

    GuiComboBox rateSelector;

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

    // HW Env
    GuiToggleButton envEnableButton;
    GuiComboBox shapeSelector;
    GuiSlider periodSlider;

    GuiComponentMidi midiComponent;

    // Preset Name Label
    GuiLabel presetNameLabel;
    GuiSeparator presetNameSeparator;

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
	GuiSsg(const GuiContext& context) :
        GuiBase(context), 
        mainGroup(context),
        dutyGroup(context),
        triGroup(context),
        qualityCat(context),
        shapeCat(context),
        toneCat(context),
        noiseCat(context),
        mixCat(context),
        pulseInvCat(context),
        triPeakCat(context),
        hwEnvCat(context),
        levelSlider(context),
        toneSlider(context),
        noiseSlider(context),
        noiseFreqSlider(context),
        noiseOnNoteButton(context),
        mixSlider(context),
        mixSetTone(context),
        mixSetMix(context),
        mixSetNoise(context),
        waveSelector(context),
        bitSelector(context),
        rateSelector(context),
        fixComponent(context),
        unisonComponent(context),
        ampEnvComponent(context),
        pitchEnvComponent(context),
        ssgSwEnvComponent(context),
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
        envEnableButton(context),
        shapeSelector(context),
        periodSlider(context),
        midiComponent(context),
        presetNameLabel(context),
        presetNameSeparator(context),
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
    void updatePresetName(const juce::String& presetName);
    void initParams();
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutHwEnvCat(juce::Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
};
