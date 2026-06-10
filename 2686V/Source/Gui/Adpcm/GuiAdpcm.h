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

class GuiAdpcm : public GuiBase
{
    // --- ADPCM Page ---
    GuiScrollGroup mainGroup;

    GuiComboBox modeSelector;

    GuiTextButton loadButton;
    GuiTextButton clearButton;
    GuiLabel fileNameLabel;

    GuiCategoryLabel optionalCat;

    GuiSlider pcmOffsetSlider;
    GuiSlider pcmRatioSlider;

    GuiSlider levelSlider;
    GuiSlider panSlider;

    GuiTextButton panToLBtn;
    GuiTextButton panToCBtn;
    GuiTextButton panToRBtn;

    // Loop Button
    GuiToggleButton loopButton;

    GuiComponentFix fixComponent;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel panCat;

    // Amp ADSR
    GuiComponentAmpEnv ampEnvComponent;

    // Pitch ADSR
    GuiComponentPitchEnv pitchEnvComponent;

    // SSG SW Env
    GuiComponentSsgSwEnv ssgSwEnvComponent;

    // Detune
    GuiComponentMulDetune mulDetuneComponent;

    GuiComponentLfoOpzx7 lfoComponent;

    GuiComboBox rateSelector;

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
    GuiAdpcm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        modeSelector(context),
        loadButton(context),
        clearButton(context),
        fileNameLabel(context),
        optionalCat(context),
        pcmOffsetSlider(context),
        pcmRatioSlider(context),
        levelSlider(context),
        panSlider(context),
        panToLBtn(context),
        panToCBtn(context),
        panToRBtn(context),
        loopButton(context),
        fixComponent(context),
        unisonComponent(context),
        qualityCat(context),
        panCat(context),
        ampEnvComponent(context),
        pitchEnvComponent(context),
        ssgSwEnvComponent(context),
		mulDetuneComponent(context),
        lfoComponent(context),
        rateSelector(context),
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
    void updateFileName(const juce::String& fileName);
    bool isThis(juce::Button* button);
    bool isBtnPanL(juce::Button* button);
    bool isBtnPanC(juce::Button* button);
    bool isBtnPanR(juce::Button* button);
    void setPan(float pan);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    void updatePresetName(const juce::String& presetName);
    void initParams();
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanCat(juce::Rectangle<int>& rect);
    void layoutOptionalCat(juce::Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
};
