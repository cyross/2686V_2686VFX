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
#include "../../Gui/Components/PresetName/PresetName.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiBeep : public GuiBase {
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiSlider levelSlider;
    GuiTextButton levelPM1;
    GuiTextButton levelPM01;
    GuiTextButton levelTo1;
    GuiTextButton levelP01;
    GuiTextButton levelP1;

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

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    GuiSeparator uSep001;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieAmpEnv;
    GuiComponentImportExport iePitchEnv;
    GuiComponentImportExport ieSsgSwEnv;
    GuiComponentImportExport ieDetune;
    GuiComponentImportExport ieUnison;

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
        presetName(context),
        levelSlider(context),
        levelPM1(context),
        levelPM01(context),
        levelTo1(context),
        levelP01(context),
        levelP1(context),
        fixComponent(context),
        unisonComponent(context),
        ampEnvComponent(context),
        pitchEnvComponent(context),
        ssgSwEnvComponent(context),
		mulDetuneComponent(context),
        lfoComponent(context),
        midiComponent(context),
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        ieLfo(context),
        ieAmpEnv(context),
        iePitchEnv(context),
        ieSsgSwEnv(context),
        ieDetune(context),
        ieUnison(context),
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
    void initParams();
    void setupGraph();
    void layoutUtilityCat(Rectangle<int>& rect);
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
};
