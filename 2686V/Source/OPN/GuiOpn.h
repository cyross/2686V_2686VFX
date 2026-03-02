#pragma once

#include <JuceHeader.h>
#include <array>

#include "../core/Global.h"
#include "../core/GuiValues.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpn : public GuiBase
{
    GuiGroup mainGroup;
    std::array<GuiGroup, Global::Fm::Op4> opGroups;
    std::array<GuiGroup, Global::Fm::Op4> freqBtnGroup;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel algFbCat;

    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiFbSlider feedback2Slider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    std::array<GuiCategoryLabel, Global::Fm::Op4> catMain;
    std::array<GuiSlider, Global::Fm::Op4> mul;
    std::array<GuiComboBox, Global::Fm::Op4> dt;
    std::array<GuiSlider, Global::Fm::Op4> ar;
    std::array<GuiSlider, Global::Fm::Op4> dr;
    std::array<GuiSlider, Global::Fm::Op4> sr;
    std::array<GuiSlider, Global::Fm::Op4> sl;
    std::array<GuiSlider, Global::Fm::Op4> rr;
    std::array<GuiSlider, Global::Fm::Op4> tl;  // Total Level
    std::array<GuiComboBox, Global::Fm::Op4> ks; // Key Scale (0-3)
    std::array<GuiComboBox, Global::Fm::Op4> se; // SSG-EG Shape Selector
    std::array<GuiSlider, Global::Fm::Op4> seFreq;
    std::array<GuiCategoryLabel, Global::Fm::Op4> cafFix;
    std::array<GuiToggleButton, Global::Fm::Op4> fix;
    std::array<GuiSlider, Global::Fm::Op4> freq;
    std::array<GuiTextButton, Global::Fm::Op4> freqToZero;
    std::array<GuiTextButton, Global::Fm::Op4> freqTo440;
    std::array<GuiCategoryLabel, Global::Fm::Op4> catMask;
    std::array<GuiToggleButton, Global::Fm::Op4> mask; // Mask
    std::array<GuiCategoryLabel, Global::Fm::Op4> catMml;
    std::array<GuiMmlButton, Global::Fm::Op4> mml;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
	GuiOpn(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        freqBtnGroup{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        qualityCat(context),
        algFbCat(context),
        algSelector(context),
        feedbackSlider(context),
        feedback2Slider(context),
        bitSelector(context),
        rateSelector(context),
        mvolCat(context),
        masterVolSlider(context),
        catMain{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mul{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        ar{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        sr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        sl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        tl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ks{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        se{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        seFreq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        cafFix{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        fix{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        freq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        freqToZero{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        freqTo440{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        catMml{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) }
    {
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
