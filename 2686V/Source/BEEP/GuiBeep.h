#pragma once

#include <JuceHeader.h>
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"

class GuiBeep : public GuiBase {
    GuiGroup mainGroup;

    GuiCategoryLabel mainCat;

    GuiSlider volSlider;

    GuiCategoryLabel catFix;

    GuiToggleButton fixToggle;
    GuiSlider freqSlider;
    GuiTextButton freqTo2kBtn;

    GuiCategoryLabel adsrCat;

    GuiToggleButton bypassToggle;
    GuiSlider ar, dr, sl, rr;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;
public:
    GuiBeep(const GuiContext& context) : GuiBase(context),
        mainGroup(context),
        mainCat(context),
        volSlider(context),
        catFix(context),
        fixToggle(context), freqSlider(context), freqTo2kBtn(context),
        adsrCat(context),
        bypassToggle(context),
        ar(context), dr(context), sl(context), rr(context),
        mvolCat(context),
        masterVolSlider(context)
    {
    }
    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
