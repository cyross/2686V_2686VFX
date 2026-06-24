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
#include "../../Gui/Components/MulDetune/MulDetune.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/AmpEnv/AmpEnv.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/LfoOpzx7/LfoOpzx7.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Processor/Rhythm/ProcessorRhythmValues.h"
#include "../../Gui/Components/PresetName/PresetName.h"
#include "../../Gui/Components/ViewMode/ViewMode.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class RhythmPadGui: public GuiBase
{
    GuiScrollGroup mainGroup;

    GuiLabel fileNameLabel;
    GuiTextButton loadButton;
    GuiTextButton clearButton;

    GuiCategoryLabel formCat;
    GuiCategoryLabel optionalCat;

    GuiSlider pcmOffsetSlider;
    GuiSlider pcmRatioSlider;
    GuiToggleButton loopPointEnableButton;
    GuiSlider loopPointStartSlider;
    GuiSlider loopPointEndSlider;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel panCat;
    GuiComboBox interpSelector;

    GuiSlider noteSlider;

    GuiComboBox modeSelector;

    GuiComboBox rateSelector;

    GuiSlider panSlider;

    GuiTextButton panToLBtn;
    GuiTextButton panToCBtn;
    GuiTextButton panToRBtn;

    GuiSlider volSlider;
    GuiSlider toneSlider;
    GuiSlider noiseSlider;
    GuiSlider noiseFreqSlider;
    GuiSlider mixSlider;
    GuiTextButton mixSetTone;  // 0.0
    GuiTextButton mixSetMix;   // 0.5
    GuiTextButton mixSetNoise; // 1.0

    GuiToggleButton oneShotButton;

    GuiComponentFix fixComponent;

    // Amp ADSR
    GuiComponentAmpEnv ampEnvComponent;

    // Pitch ADSR
    GuiComponentPitchEnv pitchEnvComponent;

    // SSG SW Env
    GuiComponentSsgSwEnv ssgSwEnvComponent;

    // Detune
    GuiComponentMulDetune mulDetuneComponent;

    GuiComponentLfoOpzx7 lfoComponent;
    std::unique_ptr<juce::FileChooser> fileChooser;

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
    RhythmPadGui(const GuiContext& context) :
		GuiBase(context),
        mainGroup(context),
        fileNameLabel(context),
        loadButton(context),
        clearButton(context),
        formCat(context),
        optionalCat(context),
        pcmOffsetSlider(context),
        pcmRatioSlider(context),
        loopPointEnableButton(context),
        loopPointStartSlider(context),
        loopPointEndSlider(context),
        qualityCat(context),
        interpSelector(context),
        panCat(context),
        noteSlider(context),
        modeSelector(context),
        rateSelector(context),
        panSlider(context),
        panToLBtn(context),
        panToCBtn(context),
        panToRBtn(context),
        volSlider(context),
        toneSlider(context),
        noiseSlider(context),
        noiseFreqSlider(context),
        mixSlider(context),
        mixSetTone(context),
        mixSetMix(context),
        mixSetNoise(context),
        oneShotButton(context),
        fixComponent(context),
        ampEnvComponent(context),
        pitchEnvComponent(context),
        ssgSwEnvComponent(context),
        mulDetuneComponent(context),
        lfoComponent(context),
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphBtnSsg(context),
        graphSeparator(context)
    {
        currentGraphMode = GraphMode::Amp; // 初期状態はAmp
    }

    void updatePadFileName(const juce::String& fileName);
    void updatePadVisible(bool visible);
    void setup(juce::Component& parent, int index, juce::String padName, int& tabOrder);
	void layout(juce::Rectangle<int> content);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    bool isThis(juce::Button* button);
    void layoutFormCat(Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanCat(juce::Rectangle<int>& rect);
    void layoutOptionalCat(juce::Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
    void copyParams(CopyRhythmPad& copyObj);
    void pasteParams(CopyRhythmPad& copyObj);
    void importToneNoiseParam();
    void exportToneNoiseParam();
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
    void importQualityParam();
    void exportQualityParam();
    void importPcmPlayParam();
    void exportPcmPlayParam();
};

class GuiRhythm : public GuiBase
{
    GuiComponentViewModes viewMode = GuiComponentViewModes::Twin;

    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;
    GuiComponentViewMode viewModeComp;

    // Master Level
	GuiSlider levelSlider;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    GuiComponentMidi midiComponent;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    GuiSeparator uSep001;
    GuiTextButton copyPadParamBtn;
    GuiSlider copyPadFromSlider;
    GuiSlider copyPadToSlider;
    GuiSeparator uSep002;
    GuiTextButton importToneNoiseParamButton;
    GuiTextButton exportToneNoiseParamButton;
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
    GuiTextButton importQualityParamButton;
    GuiTextButton exportQualityParamButton;
    GuiTextButton importPcmPlayParamButton;
    GuiTextButton exportPcmPlayParamButton;
    GuiSlider targerPadSlider;
    GuiSeparator uSep003;
    GuiTextButton importUnisonParamButton;
    GuiTextButton exportUnisonParamButton;

    // 8 Pads
    std::array<RhythmPadGui, RhythmPrValue::pads> pads;
public:
	GuiRhythm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        presetName(context),
        viewModeComp(context),
        levelSlider(context),
        unisonComponent(context),
        midiComponent(context),
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        copyPadParamBtn(context),
        copyPadFromSlider(context),
        copyPadToSlider(context),
        uSep002(context),
        importToneNoiseParamButton(context),
        exportToneNoiseParamButton(context),
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
        importQualityParamButton(context),
        exportQualityParamButton(context),
        importPcmPlayParamButton(context),
        exportPcmPlayParamButton(context),
        targerPadSlider(context),
        uSep003(context),
        importUnisonParamButton(context),
        exportUnisonParamButton(context),
        pads{ { {context}, {context}, {context}, {context}, {context}, {context}, {context}, {context} } }
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }
                     
    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void layoutPad(int padIndex, juce::Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    void buttonClicked(juce::Button* button, juce::AudioFormatManager &formatManager, std::unique_ptr<juce::FileChooser>& fileChooser);
	void updatePadFileName(int padIndex, const juce::String& fileName);
    bool isThis(int padIndex, juce::Button* button);
    void updatePadVisible(int idx, bool visible);
    void updatePresetName(const juce::String& name);
    void initParams();
    void setLevel(float level);
    void copyPadParams(int p, CopyRhythmPad& copyObj);
    void pastePadParams(int p, CopyRhythmPad& copyObj);
    void importToneNoiseParam(int p);
    void exportToneNoiseParam(int p);
    void importLfoParam(int p);
    void exportLfoParam(int p);
    void importAmpEnvParam(int p);
    void exportAmpEnvParam(int p);
    void importPitchEnvParam(int p);
    void exportPitchEnvParam(int p);
    void importSsgSwEnvParam(int p);
    void exportSsgSwEnvParam(int p);
    void importDetuneParam(int p);
    void exportDetuneParam(int p);
    void importQualityParam(int p);
    void exportQualityParam(int p);
    void importPcmPlayParam(int p);
    void exportPcmPlayParam(int p);
    void importUnisonParam();
    void exportUnisonParam();
};
