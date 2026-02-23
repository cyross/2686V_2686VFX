#pragma once

#include <JuceHeader.h>
#include <array>

#include "../gui/GuiConstants.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpm : public GuiBase
{
    GuiGroup mainGroup;
    std::array<GuiGroup, Fm4Ops> opGroups;

    // Global
    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiFbSlider feedback2Slider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    // OPM LFO
    GuiSlider lfoFreqSlider;
    GuiComboBox lfoPmsSelector;
    GuiComboBox lfoAmsSelector;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    std::array<GuiSlider, Fm4Ops> mul;
    std::array<GuiSlider, Fm4Ops> dt1;
    std::array<GuiSlider, Fm4Ops> dt2;
    std::array<GuiSlider, Fm4Ops> tl;
    std::array<GuiSlider, Fm4Ops> ar;
    std::array<GuiSlider, Fm4Ops> d1r;
    std::array<GuiSlider, Fm4Ops> d1l;
    std::array<GuiSlider, Fm4Ops> d2r;
    std::array<GuiSlider, Fm4Ops> rr;
    std::array<GuiComboBox, Fm4Ops> ks;
    std::array<GuiToggleButton, Fm4Ops> fix;
    std::array<GuiSlider, Fm4Ops> freq;
    std::array<GuiTextButton, Fm4Ops> freqToZero;
    std::array<GuiTextButton, Fm4Ops> freqTo440;
    std::array<GuiToggleButton, Fm4Ops> mask; // Mask
    std::array<GuiMmlButton, Fm4Ops> mml;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
    GuiOpm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        algSelector(context),
        feedbackSlider(context),
        feedback2Slider(context),
        bitSelector(context),
        rateSelector(context),
        lfoFreqSlider(context),
        lfoPmsSelector(context),
        lfoAmsSelector(context),
        masterVolSlider(context),
        mul{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt1{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt2{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        tl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ar{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        d1r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        d1l{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        d2r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ks{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        fix{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        freq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        freqToZero{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        freqTo440{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) }
    {
	}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
