#pragma once

#include <JuceHeader.h>
#include <array>

#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpna : public GuiBase
{
    GuiGroup mainGroup;
    std::array<GuiGroup, Fm4Ops> opGroups;
    std::array<GuiGroup, Fm4Ops> freqBtnGroup;
    GuiGroup lfoGroup;

	GuiComboBox algSelector;
	GuiFbSlider feedbackSlider;
	GuiFbSlider feedback2Slider;
	GuiComboBox bitSelector;
    GuiComboBox rateSelector;
	GuiSlider lfoFreqSlider;
	GuiComboBox lfoPmsSelector;
	GuiComboBox lfoAmsSelector;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    std::array<GuiSlider, Fm4Ops> mul;
    std::array<GuiSlider, Fm4Ops> dt;
    std::array<GuiSlider, Fm4Ops> ar;
    std::array<GuiSlider, Fm4Ops> dr;
    std::array<GuiSlider, Fm4Ops> sr;
    std::array<GuiSlider, Fm4Ops> sl;
    std::array<GuiSlider, Fm4Ops> rr;
    std::array<GuiSlider, Fm4Ops> tl;
    std::array<GuiComboBox, Fm4Ops> ks;
    std::array<GuiToggleButton, Fm4Ops> am;
    std::array<GuiComboBox, Fm4Ops> se;
    std::array<GuiSlider, Fm4Ops> seFreq;
    std::array<GuiToggleButton, Fm4Ops> fix;
    std::array<GuiSlider, Fm4Ops> freq;
    std::array<GuiTextButton, Fm4Ops> freqToZero;
    std::array<GuiTextButton, Fm4Ops> freqTo440;
    std::array<GuiToggleButton, Fm4Ops> mask;
    std::array<GuiMmlButton, Fm4Ops> mml;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
	GuiOpna(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        freqBtnGroup{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        lfoGroup(context),
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
        dt{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ar{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        sr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        sl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        tl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ks{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        am{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
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
