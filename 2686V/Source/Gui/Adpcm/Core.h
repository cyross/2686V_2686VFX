#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/Core.h"
#include "../../Advanced/Curve/Core.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiAdpcm : public GuiBase
{
    // --- ADPCM Page ---
    GuiGroup mainGroup;

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

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel panCat;
    GuiCategoryLabel adsrCat;

    GuiToggleButton adsrBypassButton; // ADSR Bypass Switch
    GuiSlider startLevelSlider;
    GuiSlider attackSlider;
    GuiSlider decaySlider;
    GuiSlider sustainSlider;
    GuiSlider releaseSlider;

    // Pitch ADSR
    GuiCategoryLabel pitchAdsrCat;
    GuiToggleButton pitchAdsrBypassButton; // ADSR Bypass Switch
    GuiSlider pitchAttackSlider;
    GuiSlider pitchDecaySlider;
    GuiSlider pitchReleaseSlider;
    GuiSlider pitchStartLevelSlider;
    GuiSlider pitchAttackLevelSlider;
    GuiSlider pitchSustainLevelSlider;
    GuiSlider pitchReleaseLevelSlider;

    // SSG SW Env
    GuiCategoryLabel ssgSwEnvCat;
    GuiToggleButton ssgSwEnvBypassButton; // SSG SW Env Bypass Switch
    GuiSlider ssgSwStepsSlider;
    GuiToggleButton ssgSwEnvLoopButton;
    GuiSlider ssgSwLoopToSlider;
    GuiSlider ssgSwLoopCountSlider;
    GuiSlider ssgSwStartLevelSlider;
    GuiSlider ssgSwR1Slider;
    GuiSlider ssgSwL1Slider;
    GuiSlider ssgSwR2Slider;
    GuiSlider ssgSwL2Slider;
    GuiSlider ssgSwR3Slider;
    GuiSlider ssgSwL3Slider;
    GuiSlider ssgSwR4Slider;
    GuiSlider ssgSwL4Slider;
    GuiSlider ssgSwR5Slider;
    GuiSlider ssgSwL5Slider;
    GuiSlider ssgSwR6Slider;
    GuiSlider ssgSwL6Slider;

    // Detune
    GuiCategoryLabel detuneCat;
    GuiComboBox mul;
    GuiSlider mulRatio;
    GuiComboBox dt1;
    GuiSlider dt2;

    GuiComboBox rateSelector;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // プリセット名ラベル
    GuiLabel presetNameLabel;

    GuiEnvelopeGraph graph;
    GuiToggleButton graphBtnAmp;
    GuiToggleButton graphBtnPitch;
    GuiToggleButton graphBtnSsg;

    enum class GraphMode { Amp, Pitch, SsgSw };
    GraphMode currentGraphMode;

    CurveCore* p_curveCore;
    GuiCurve* p_guiCurve;

    void updateGraph();
    void setGraphMode(GraphMode mode);
public:
    GuiAdpcm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        mvolCat(context),
        masterVolSlider(context),
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
        qualityCat(context),
        panCat(context),
        adsrCat(context),
		adsrBypassButton(context),
		startLevelSlider(context),
        attackSlider(context),
        decaySlider(context),
        sustainSlider(context),
        releaseSlider(context),
        pitchAdsrCat(context),
        pitchAdsrBypassButton(context),
        pitchAttackSlider(context),
        pitchDecaySlider(context),
        pitchReleaseSlider(context),
        pitchStartLevelSlider(context),
        pitchAttackLevelSlider(context),
        pitchSustainLevelSlider(context),
        pitchReleaseLevelSlider(context),
        ssgSwEnvCat(context),
        ssgSwEnvBypassButton(context),
        ssgSwStepsSlider(context),
        ssgSwEnvLoopButton(context),
        ssgSwLoopToSlider(context),
        ssgSwLoopCountSlider(context),
        ssgSwStartLevelSlider(context),
        ssgSwR1Slider(context),
        ssgSwL1Slider(context),
        ssgSwR2Slider(context),
        ssgSwL2Slider(context),
        ssgSwR3Slider(context),
        ssgSwL3Slider(context),
        ssgSwR4Slider(context),
        ssgSwL4Slider(context),
        ssgSwR5Slider(context),
        ssgSwL5Slider(context),
        ssgSwR6Slider(context),
        ssgSwL6Slider(context),
        detuneCat(context),
        mul(context),
        mulRatio(context),
        dt1(context),
        dt2(context),
        rateSelector(context),
        presetNameLabel(context),
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphBtnSsg(context)
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
    void layoutMvolCat(juce::Rectangle<int>& rect);
    void layoutPanCat(juce::Rectangle<int>& rect);
    void layoutAdsrCat(juce::Rectangle<int>& rect);
    void layoutPitchEnvCat(juce::Rectangle<int>& rect);
    void layoutDetuneCat(juce::Rectangle<int>& rect);
    void layoutSsgSwEnvCat(juce::Rectangle<int>& rect);
    void layoutOptionalCat(juce::Rectangle<int>& rect);
    void applySsgSwEnvLoopValues(bool enabled);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
};
