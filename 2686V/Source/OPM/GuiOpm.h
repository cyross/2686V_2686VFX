#pragma once

#include <JuceHeader.h>
#include <array>

#include "../core/Global.h"
#include "../core/GuiValues.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpm : public GuiBase
{
    GuiGroup mainGroup;
    std::array<GuiGroup, Global::Fm::Op4> opGroups;

    // Global
    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
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
    std::array<GuiSlider, Global::Fm::Op4> mul;
    std::array<GuiComboBox, Global::Fm::Op4> dt1;
    std::array<GuiSlider, Global::Fm::Op4> dt2;
    std::array<GuiSlider, Global::Fm::Op4> tl;
    std::array<GuiSlider, Global::Fm::Op4> ar;
    std::array<GuiSlider, Global::Fm::Op4> d1r;
    std::array<GuiSlider, Global::Fm::Op4> d1l;
    std::array<GuiSlider, Global::Fm::Op4> d2r;
    std::array<GuiSlider, Global::Fm::Op4> rr;
    std::array<GuiComboBox, Global::Fm::Op4> ks;
    std::array<GuiToggleButton, Global::Fm::Op4> fix;
    std::array<GuiSlider, Global::Fm::Op4> freq;
    std::array<GuiTextButton, Global::Fm::Op4> freqToZero;
    std::array<GuiTextButton, Global::Fm::Op4> freqTo440;
    std::array<GuiToggleButton, Global::Fm::Op4> mask; // Mask
    std::array<GuiMmlButton, Global::Fm::Op4> mml;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
    GuiOpm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        algSelector(context),
        feedbackSlider(context),
        bitSelector(context),
        rateSelector(context),
        lfoFreqSlider(context),
        lfoPmsSelector(context),
        lfoAmsSelector(context),
        masterVolSlider(context),
        mul{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt1{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
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
