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

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel mainCat;
    GuiCategoryLabel mainVoiceCat;
    GuiCategoryLabel mainPulseCat;
    GuiCategoryLabel mainTriCat;
    GuiCategoryLabel mainHwEnvCat;

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
public:
	GuiSsg(const GuiContext& context) :
        GuiBase(context), 
        mainGroup(context),
        voiceGroup(context),
        dutyGroup(context),
        triGroup(context),
        envGroup(context),
        qualityCat(context),
        mainCat(context),
        mainVoiceCat(context),
        mainPulseCat(context),
        mainTriCat(context),
        mainHwEnvCat(context),
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
        masterVolSlider(context)
    {}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
