#pragma once

#include <JuceHeader.h>
#include <array>
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

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

    GuiComboBox dutyModeSelector; // Preset / Variable

    GuiComboBox dutyPresetSelector;

    GuiSlider dutyVarSlider;

    GuiToggleButton dutyInvertButton;

    // Triangle Controls
    GuiToggleButton triKeyTrackButton;

    GuiSlider triFreqSlider;

    GuiSlider triPeakSlider;

    // Peak Preset Buttons
    GuiTextButton triSetSawDown; // 0.0

    GuiTextButton triSetTri;     // 0.5

    GuiTextButton triSetSawUp;   // 1.0

    // HW Env Controls
    GuiToggleButton envEnableButton;

    GuiComboBox shapeSelector;

    GuiSlider periodSlider;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // Mono/Poly切り替えスイッチ
    GuiToggleButton monoModeToggle;

    // プリセット名ラベル
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
    {}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePresetName(const juce::String& presetName);
};
