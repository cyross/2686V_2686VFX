#pragma once

#include <JuceHeader.h>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"

class GuiBeep : public GuiBase {
    GuiGroup mainGroup;

    GuiCategoryLabel catFix;
    GuiCategoryLabel adsrCat;
    GuiCategoryLabel monoPolyCat;
    GuiCategoryLabel mvolCat;

    GuiSlider volSlider;


    GuiToggleButton fixToggle;
    GuiSlider freqSlider;
    GuiTextButton freqTo2kBtn;


    GuiToggleButton bypassToggle;
    GuiSlider ar, dr, sl, rr, stl;

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

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // Mono/Poly切り替えスイッチ
    GuiToggleButton monoModeToggle;

    // プリセット名ラベル
    GuiLabel presetNameLabel;
public:
    GuiBeep(const GuiContext& context) : GuiBase(context),
        mainGroup(context),
        monoPolyCat(context),
        volSlider(context),
        catFix(context),
        fixToggle(context), freqSlider(context), freqTo2kBtn(context),
        adsrCat(context),
        bypassToggle(context),
        ar(context), dr(context), sl(context), rr(context), stl(context),
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
    void layoutFixCat(juce::Rectangle<int>& rect);
    void layoutMonoModeCat(juce::Rectangle<int>& rect);
    void layoutMvolCat(juce::Rectangle<int>& rect);
    void layoutAdsrCat(juce::Rectangle<int>& rect);
    void layoutPitchEnvCat(juce::Rectangle<int>& rect);
    void layoutDetuneCat(juce::Rectangle<int>& rect);
    void layoutSsgSwEnvCat(juce::Rectangle<int>& rect);
};
