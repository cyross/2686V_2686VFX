#pragma once

#include <JuceHeader.h>
#include <array>

#include "../gui/GuiConstants.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpl3 : public GuiBase
{
    GuiGroup mainGroup;
    std::array<GuiGroup, Fm4Ops> opGroups;

    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiFbSlider feedback2Slider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    std::array<GuiSlider, Fm4Ops> mul;
    std::array<GuiSlider, Fm4Ops> tl;
    std::array<GuiSlider, Fm4Ops> ar;
    std::array<GuiSlider, Fm4Ops> dr;
    std::array<GuiSlider, Fm4Ops> sl;
    std::array<GuiSlider, Fm4Ops> rr;
    std::array<GuiToggleButton, Fm4Ops> am;
    std::array<GuiToggleButton, Fm4Ops> vib;
    std::array<GuiToggleButton, Fm4Ops> egType;
    std::array<GuiToggleButton, Fm4Ops> ksr;
    std::array<GuiComboBox, Fm4Ops> ksl; // Key Scale Level
    std::array<GuiComboBox, Fm4Ops> eg; // Envlope Generator
    std::array<GuiToggleButton, Fm4Ops> mask; // Mask
    std::array<GuiMmlButton, Fm4Ops> mml;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
    GuiOpl3(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        algSelector(context),
        feedbackSlider(context),
        feedback2Slider(context),
        bitSelector(context),
        rateSelector(context),
        masterVolSlider(context),
        mul{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        tl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ar{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        sl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        am{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        vib{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        egType{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        ksr{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        ksl{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        eg{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) }
    {
	}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
