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
    GuiGroup envGroup;

    GuiCategoryLabel monoPolyCat;
    GuiCategoryLabel presetNameCat;
    GuiCategoryLabel qualityCat;
    GuiCategoryLabel shapeCat;
    GuiCategoryLabel toneCat;
    GuiCategoryLabel noiseCat;
    GuiCategoryLabel mixCat;
    GuiCategoryLabel pulseDutyCat;
    GuiCategoryLabel pulseInvCat;
    GuiCategoryLabel triCat;
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

    GuiCategoryLabel adsrCat;

    // SSG ADSR
    GuiToggleButton adsrBypassButton; // ADSR Bypass Switch

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

    // Detune
    GuiCategoryLabel detuneCat;
    GuiComboBox dt1;
    GuiSlider dt2;

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
        envGroup(context),
        monoPolyCat(context),
        presetNameCat(context),
        qualityCat(context),
        shapeCat(context),
        toneCat(context),
        noiseCat(context),
        mixCat(context),
        pulseDutyCat(context),
        pulseInvCat(context),
        triCat(context),
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
		detuneCat(context),
		dt1(context),
		dt2(context),
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
};
