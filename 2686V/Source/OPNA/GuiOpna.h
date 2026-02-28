#pragma once

#include <JuceHeader.h>
#include <array>

#include "../core/Global.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpna : public GuiBase
{
    GuiGroup mainGroup;
    std::array<GuiGroup, Global::Fm::Op4> opGroups;
    std::array<GuiGroup, Global::Fm::Op4> freqBtnGroup;
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

    std::array<GuiSlider, Global::Fm::Op4> mul;
    std::array<GuiComboBox, Global::Fm::Op4> dt;
    std::array<GuiSlider, Global::Fm::Op4> ar;
    std::array<GuiSlider, Global::Fm::Op4> dr;
    std::array<GuiSlider, Global::Fm::Op4> sr;
    std::array<GuiSlider, Global::Fm::Op4> sl;
    std::array<GuiSlider, Global::Fm::Op4> rr;
    std::array<GuiSlider, Global::Fm::Op4> tl;
    std::array<GuiComboBox, Global::Fm::Op4> ks;
    std::array<GuiToggleButton, Global::Fm::Op4> am;
    std::array<GuiComboBox, Global::Fm::Op4> se;
    std::array<GuiSlider, Global::Fm::Op4> seFreq;
    std::array<GuiToggleButton, Global::Fm::Op4> fix;
    std::array<GuiSlider, Global::Fm::Op4> freq;
    std::array<GuiTextButton, Global::Fm::Op4> freqToZero;
    std::array<GuiTextButton, Global::Fm::Op4> freqTo440;
    std::array<GuiToggleButton, Global::Fm::Op4> mask;
    std::array<GuiMmlButton, Global::Fm::Op4> mml;

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
        dt{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
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
