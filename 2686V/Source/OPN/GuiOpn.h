#pragma once

#include <JuceHeader.h>
#include <array>

#include "../gui/GuiConstants.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpn : public GuiBase
{
    GuiGroup mainGroup;
    std::array<GuiGroup, Fm4Ops> opGroups;
    std::array<GuiGroup, Fm4Ops> freqBtnGroup;

    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiFbSlider feedback2Slider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    std::array<GuiSlider, Fm4Ops> mul;
    std::array<GuiSlider, Fm4Ops> dt;
    std::array<GuiSlider, Fm4Ops> ar;
    std::array<GuiSlider, Fm4Ops> dr;
    std::array<GuiSlider, Fm4Ops> sr;
    std::array<GuiSlider, Fm4Ops> sl;
    std::array<GuiSlider, Fm4Ops> rr;
    std::array<GuiSlider, Fm4Ops> tl;  // Total Level
    std::array<GuiComboBox, Fm4Ops> ks; // Key Scale (0-3)
    std::array<GuiComboBox, Fm4Ops> se; // SSG-EG Shape Selector
    std::array<GuiSlider, Fm4Ops> seFreq;
    std::array<GuiToggleButton, Fm4Ops> fix;
    std::array<GuiSlider, Fm4Ops> freq;
    std::array<GuiTextButton, Fm4Ops> freqToZero;
    std::array<GuiTextButton, Fm4Ops> freqTo440;
    std::array<GuiToggleButton, Fm4Ops> mask; // Mask
    std::array<GuiMmlButton, Fm4Ops> mml;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
	GuiOpn(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        freqBtnGroup{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        algSelector(context),
        feedbackSlider(context),
        feedback2Slider(context),
        bitSelector(context),
        rateSelector(context),
        masterVolSlider(context),
        mul{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ar{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        sr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        sl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        tl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ks{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        se{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        seFreq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
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
