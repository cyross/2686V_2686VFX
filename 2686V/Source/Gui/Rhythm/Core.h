#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/Core.h"
#include "../../Advanced/Curve/Core.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/AmpEnv/AmpEnv.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class RhythmPadGui: public GuiBase
{
    GuiGroup mainGroup;

    GuiLabel fileNameLabel;
    GuiTextButton loadButton;
    GuiTextButton clearButton;

    GuiCategoryLabel optionalCat;

    GuiSlider pcmOffsetSlider;
    GuiSlider pcmRatioSlider;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel panCat;

    GuiSlider noteSlider;

    GuiComboBox modeSelector;

    GuiComboBox rateSelector;

    GuiSlider panSlider;

    GuiTextButton panToLBtn;
    GuiTextButton panToCBtn;
    GuiTextButton panToRBtn;

    GuiSlider volSlider;

    GuiToggleButton oneShotButton;

    // Amp ADSR
    GuiComponentAmpEnv ampEnvComponent;

    // Pitch ADSR
    GuiComponentPitchEnv pitchEnvComponent;

    GuiEnvelopeGraph graph;
    GuiToggleButton graphBtnAmp;
    GuiToggleButton graphBtnPitch;
    GuiSeparator graphSeparator;

    enum class GraphMode { Amp, Pitch };
    GraphMode currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    void updateGraph();
    void setGraphMode(GraphMode mode);
public:
    RhythmPadGui(const GuiContext& context) :
		GuiBase(context),
        mainGroup(context),
        fileNameLabel(context),
        loadButton(context),
        clearButton(context),
        optionalCat(context),
        pcmOffsetSlider(context),
        pcmRatioSlider(context),
        qualityCat(context),
        panCat(context),
        noteSlider(context),
        modeSelector(context),
        rateSelector(context),
        panSlider(context),
        panToLBtn(context),
        panToCBtn(context),
        panToRBtn(context),
        volSlider(context),
        oneShotButton(context),
        ampEnvComponent(context),
        pitchEnvComponent(context),
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphSeparator(context)
    {
        currentGraphMode = GraphMode::Amp; // 初期状態はAmp
    }

    void updatePadFileName(const juce::String& fileName);
    void setup(juce::Component& parent, int index, juce::String padName, int& tabOrder);
	void layout(juce::Rectangle<int> content);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    bool isThis(juce::Button* button);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanCat(juce::Rectangle<int>& rect);
    void layoutAdsrCat(juce::Rectangle<int>& rect);
    void layoutOptionalCat(juce::Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
};

class GuiRhythm : public GuiBase
{
    GuiGroup mainGroup;

    // Master Level
	GuiSlider levelSlider;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    // プリセット名ラベル
    GuiLabel presetNameLabel;
    GuiSeparator presetNameSeparator;

    // 8 Pads
    std::array<RhythmPadGui, 8> pads;
public:
	GuiRhythm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        levelSlider(context),
        unisonComponent(context),
        presetNameLabel(context),
        presetNameSeparator(context),
        pads{ { {context}, {context}, {context}, {context}, {context}, {context}, {context}, {context} } }
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }
                     
    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    void buttonClicked(juce::Button* button, juce::AudioFormatManager &formatManager, std::unique_ptr<juce::FileChooser>& fileChooser);
	void updatePadFileName(int padIndex, const juce::String& fileName);
    bool isThis(int padIndex, juce::Button* button);
    void updatePresetName(const juce::String& presetName);
    void initParams();
};
