#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"

class GuiSsg : public GuiBase
{
    // Pane Groups
    GuiGroup mainGroup;
    GuiGroup voiceGroup;
    GuiGroup dutyGroup;
    GuiGroup triGroup;

    GuiCategoryLabel monoPolyCat;
    GuiCategoryLabel qualityCat;
    GuiCategoryLabel shapeCat;
    GuiCategoryLabel toneCat;
    GuiCategoryLabel noiseCat;
    GuiCategoryLabel mixCat;
    GuiCategoryLabel pulseInvCat;
    GuiCategoryLabel triPeakCat;
    GuiCategoryLabel hwEnvCat;

    GuiSlider levelSlider;

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

    // Amp ADSR
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
    GuiComboBox dt1;
    GuiSlider dt2;

    // LFO
    GuiCategoryLabel lfoCat;
    GuiSlider lfoPmFreqSlider;
    GuiSlider lfoAmFreqSlider;
    GuiSlider lfoSyncDelaySlider;
    GuiTextButton lfoSyncDelayToZeroBtn;
    GuiTextButton lfoSyncDelayToOneBtn;
    GuiSlider lfoAmSmRtSlider;
    GuiComboBox lfoPmShapeSelector;
    GuiComboBox lfoAmShapeSelector;
    GuiToggleButton lfoPmToggle;
    GuiToggleButton lfoAmToggle;
    GuiSlider lfoPmsSlider;
    GuiSlider lfoAmsSlider;
    GuiSlider lfoPmdSlider;
    GuiSlider lfoAmdSlider;

    // Duty
    GuiComboBox dutyModeSelector; // Preset / Variable
    GuiComboBox dutyPresetSelector;
    GuiSlider dutyVarSlider;
    GuiToggleButton dutyInvertButton;

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

    // Master Volume
    GuiCategoryLabel mvolCat;
    GuiMasterVolumeSlider masterVolSlider;

    // Mono/Poly Switch
    GuiToggleButton monoModeToggle;

    // Preset Name Label
    GuiLabel presetNameLabel;
public:
	GuiSsg(const GuiContext& context) :
        GuiBase(context), 
        mainGroup(context),
        voiceGroup(context),
        dutyGroup(context),
        triGroup(context),
        monoPolyCat(context),
        qualityCat(context),
        shapeCat(context),
        toneCat(context),
        noiseCat(context),
        mixCat(context),
        pulseInvCat(context),
        triPeakCat(context),
        hwEnvCat(context),
        levelSlider(context),
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
		dt1(context),
		dt2(context),
        lfoCat(context),
        lfoPmFreqSlider(context),
        lfoAmFreqSlider(context),
        lfoSyncDelaySlider(context),
        lfoSyncDelayToZeroBtn(context),
        lfoSyncDelayToOneBtn(context),
        lfoAmSmRtSlider(context),
        lfoPmShapeSelector(context),
        lfoAmShapeSelector(context),
        lfoPmToggle(context),
        lfoAmToggle(context),
        lfoPmsSlider(context),
        lfoAmsSlider(context),
        lfoPmdSlider(context),
        lfoAmdSlider(context),
        dutyModeSelector(context),
        dutyPresetSelector(context),
        dutyVarSlider(context),
        dutyInvertButton(context),
        triKeyTrackButton(context),
        triFreqSlider(context),
        triPeakSlider(context),
        triSetSawDown(context),
        triSetTri(context),
        triSetSawUp(context),
        envEnableButton(context),
        shapeSelector(context),
        periodSlider(context),
        mvolCat(context),
        masterVolSlider(context),
        monoModeToggle(context),
        presetNameLabel(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePresetName(const juce::String& presetName);
    void initParams();
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutMonoModeCat(juce::Rectangle<int>& rect);
    void layoutMvolCat(juce::Rectangle<int>& rect);
    void layoutAdsrCat(juce::Rectangle<int>& rect);
    void layoutPitchEnvCat(juce::Rectangle<int>& rect);
    void layoutDetuneCat(juce::Rectangle<int>& rect);
    void layoutLfoCat(juce::Rectangle<int>& rect);
    void layoutHwEnvCat(juce::Rectangle<int>& rect);
    void layoutSwEnvCat(juce::Rectangle<int>& rect);
	void applySsgSwEnvLoopValues(bool enabled);
};
