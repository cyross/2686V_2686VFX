#pragma once

#include <JuceHeader.h>
#include <array>

#include "../core/Global.h"
#include "../core/GuiValues.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpl : public GuiBase
{
    GuiGroup mainGroup;
    std::array<GuiGroup, Global::Fm::Op2> opGroups;

    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    std::array<GuiSlider, Global::Fm::Op2> mul;
    std::array<GuiComboBox, Global::Fm::Op2> dt;
    std::array<GuiSlider, Global::Fm::Op2> ar;
    std::array<GuiSlider, Global::Fm::Op2> dr;
    std::array<GuiSlider, Global::Fm::Op2> sl;
    std::array<GuiSlider, Global::Fm::Op2> rr;
    std::array<GuiSlider, Global::Fm::Op2> tl;
    std::array<GuiToggleButton, Global::Fm::Op2> am;
    std::array<GuiToggleButton, Global::Fm::Op2> vib;
    std::array<GuiToggleButton, Global::Fm::Op2> egType;
    std::array<GuiToggleButton, Global::Fm::Op2> ksr;
    std::array<GuiComboBox, Global::Fm::Op2> ksl; // Key Scale Level
    std::array<GuiComboBox, Global::Fm::Op2> eg; // Envlope Generator
    std::array<GuiToggleButton, Global::Fm::Op2> mask; // Mask
    std::array<GuiMmlButton, Global::Fm::Op2> mml;

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
        dt{ GuiComboBox(context), GuiComboBox(context) },
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
