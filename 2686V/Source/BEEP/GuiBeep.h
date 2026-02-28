#pragma once

#include <JuceHeader.h>
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"

class GuiBeep : public GuiBase {
    GuiGroup mainGroup;
    GuiSlider volSlider;
    GuiToggleButton bypassToggle;
    GuiSlider ar, dr, sl, rr;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;
public:
    GuiBeep(const GuiContext& context) : GuiBase(context), mainGroup(context),
        volSlider(context), bypassToggle(context),
        ar(context), dr(context), sl(context), rr(context), masterVolSlider(context) {
    }
    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
