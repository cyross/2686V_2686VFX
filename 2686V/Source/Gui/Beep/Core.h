#pragma once

#include <JuceHeader.h>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"

class GuiBeep : public GuiBase {
    GuiGroup mainGroup;

    GuiCategoryLabel presetNameCat;
    GuiCategoryLabel mainCat;
    GuiCategoryLabel catFix;
    GuiCategoryLabel adsrCat;
    GuiCategoryLabel monoPolyCat;
    GuiCategoryLabel mvolCat;

    GuiSlider volSlider;


    GuiToggleButton fixToggle;
    GuiSlider freqSlider;
    GuiTextButton freqTo2kBtn;


    GuiToggleButton bypassToggle;
    GuiSlider ar, dr, sl, rr;

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
        presetNameCat(context),
        mainCat(context),
        volSlider(context),
        catFix(context),
        fixToggle(context), freqSlider(context), freqTo2kBtn(context),
        adsrCat(context),
        bypassToggle(context),
        ar(context), dr(context), sl(context), rr(context),
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
};
