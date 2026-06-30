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

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiAdpcm : public GuiBase
{
    // --- ADPCM Page ---
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiCategoryLabel formCat;

    GuiComboBox modeSelector;

    GuiTextButton loadButton;
    GuiTextButton clearButton;
    GuiLabel fileNameLabel;

    GuiCategoryLabel optionalCat;

    GuiSlider pcmOffsetSlider;
    GuiSlider pcmRatioSlider;

    GuiSlider levelSlider;
    GuiTextButton levelPM1;
    GuiTextButton levelPM01;
    GuiTextButton levelTo1;
    GuiTextButton levelP01;
    GuiTextButton levelP1;

    GuiSlider toneSlider;
    GuiSlider noiseSlider;
    GuiSlider noiseFreqSlider;
    GuiSlider mixSlider;
    GuiTextButton mixSetTone;  // 0.0
    GuiTextButton mixSetMix;   // 0.5
    GuiTextButton mixSetNoise; // 1.0
    GuiComboBox interpSelector;
    GuiSlider panSlider;

    GuiTextButton panToLBtn;
    GuiTextButton panToCBtn;
    GuiTextButton panToRBtn;

    // Loop Button
    GuiToggleButton loopButton;
    GuiToggleButton loopPointEnableButton;
    GuiSlider loopPointStartSlider;
    GuiSlider loopPointEndSlider;

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

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    GuiSeparator uSep001;
    GuiComponentImportExport ieToneNoise;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieAmpEnv;
    GuiComponentImportExport iePitchEnv;
    GuiComponentImportExport ieSsgSwEnv;
    GuiComponentImportExport ieDetune;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport iePcmPlay;
    GuiComponentImportExport ieUnison;
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
    GuiAdpcm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        presetName(context),
        formCat(context),
        modeSelector(context),
        loadButton(context),
        clearButton(context),
        fileNameLabel(context),
        optionalCat(context),
        pcmOffsetSlider(context),
        pcmRatioSlider(context),
        levelSlider(context),
        levelPM1(context),
        levelPM01(context),
        levelTo1(context),
        levelP01(context),
        levelP1(context),
        toneSlider(context),
        noiseSlider(context),
        noiseFreqSlider(context),
        mixSlider(context),
        mixSetTone(context),
        mixSetMix(context),
        mixSetNoise(context),
        interpSelector(context),
        panSlider(context),
        panToLBtn(context),
        panToCBtn(context),
        panToRBtn(context),
        loopButton(context),
        loopPointEnableButton(context),
        loopPointStartSlider(context),
        loopPointEndSlider(context),
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
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        ieToneNoise(context),
        ieLfo(context),
        ieAmpEnv(context),
        iePitchEnv(context),
        ieSsgSwEnv(context),
        ieDetune(context),
        ieQuality(context),
        iePcmPlay(context),
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
    void updateFileName(const juce::String& fileName);
    bool isThis(juce::Button* button);
    bool isBtnPanL(juce::Button* button);
    bool isBtnPanC(juce::Button* button);
    bool isBtnPanR(juce::Button* button);
    void setPan(float pan);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    void updatePresetName(const juce::String& name);
    void initParams();
    void layoutFormCat(Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanCat(juce::Rectangle<int>& rect);
    void layoutOptionalCat(juce::Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
    void setLevel(float level);
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
    void importUnisonParam();
    void exportUnisonParam();
    void importQualityParam();
    void exportQualityParam();
    void importPcmPlayParam();
    void exportPcmPlayParam();
};
