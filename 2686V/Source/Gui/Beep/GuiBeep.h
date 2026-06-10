#pragma once

#include <JuceHeader.h>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/Fix/Fix.h"
#include "../../Gui/Components/MulDetune/MulDetune.h"
#include "../../Gui/Components/AmpEnv/AmpEnv.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Gui/Components/LfoOpzx7/LfoOpzx7.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiBeep : public GuiBase {
    GuiScrollGroup mainGroup;

    GuiSlider volSlider;

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

    GuiComponentLfoOpzx7 lfoComponent;

    GuiComponentMidi midiComponent;

    // プリセット名ラベル
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
    GuiBeep(const GuiContext& context) : GuiBase(context),
        mainGroup(context),
        volSlider(context),
		fixComponent(context),
        unisonComponent(context),
        ampEnvComponent(context),
        pitchEnvComponent(context),
        ssgSwEnvComponent(context),
		mulDetuneComponent(context),
        lfoComponent(context),
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
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
};
