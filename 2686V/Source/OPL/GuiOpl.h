#pragma once

#include <JuceHeader.h>
#include <array>

#include "../core/GuiConstants.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpl : public GuiBase
{
    GuiGroup mainGroup;
    std::array<GuiGroup, Fm2Ops> opGroups;

    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    std::array<GuiSlider, Fm2Ops> mul;
    std::array<GuiSlider, Fm2Ops> dt;
    std::array<GuiSlider, Fm2Ops> ar;
    std::array<GuiSlider, Fm2Ops> dr;
    std::array<GuiSlider, Fm2Ops> sl;
    std::array<GuiSlider, Fm2Ops> rr;
    std::array<GuiSlider, Fm2Ops> tl;
    std::array<GuiToggleButton, Fm2Ops> am;
    std::array<GuiToggleButton, Fm2Ops> vib;
    std::array<GuiToggleButton, Fm2Ops> egType;
    std::array<GuiToggleButton, Fm2Ops> ksr;
    std::array<GuiComboBox, Fm2Ops> ksl; // Key Scale Level
    std::array<GuiComboBox, Fm2Ops> eg; // Envlope Generator
    std::array<GuiToggleButton, Fm2Ops> mask; // Mask
    std::array<GuiMmlButton, Fm2Ops> mml;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
    GuiOpl(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context) },
        algSelector(context),
        feedbackSlider(context),
        bitSelector(context),
        rateSelector(context),
        masterVolSlider(context),
        mul{ GuiSlider(context), GuiSlider(context) },
        dt{ GuiSlider(context), GuiSlider(context) },
        ar{ GuiSlider(context), GuiSlider(context) },
        dr{ GuiSlider(context), GuiSlider(context) },
        sl{ GuiSlider(context), GuiSlider(context) },
        rr{ GuiSlider(context), GuiSlider(context) },
        tl{ GuiSlider(context), GuiSlider(context) },
        am{ GuiToggleButton(context),GuiToggleButton(context) },
        vib{ GuiToggleButton(context),GuiToggleButton(context) },
        egType{ GuiToggleButton(context),GuiToggleButton(context) },
        ksr{ GuiToggleButton(context),GuiToggleButton(context) },
        ksl{ GuiComboBox(context), GuiComboBox(context) },
        eg{ GuiComboBox(context), GuiComboBox(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context) }
    {
	}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
