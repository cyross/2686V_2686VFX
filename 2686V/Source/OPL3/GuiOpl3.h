#pragma once

#include <JuceHeader.h>
#include <array>

#include "../core/Global.h"
#include "../core/GuiValues.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpl3 : public GuiBase
{
    GuiGroup mainGroup;
    std::array<GuiGroup, Global::Fm::Op4> opGroups;

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
    std::array<GuiCategoryLabel, Global::Fm::Op4> catLfo;
    std::array<GuiSlider, Global::Fm::Op4> mul;
    std::array<GuiSlider, Global::Fm::Op4> tl;
    std::array<GuiSlider, Global::Fm::Op4> ar;
    std::array<GuiSlider, Global::Fm::Op4> dr;
    std::array<GuiSlider, Global::Fm::Op4> sl;
    std::array<GuiSlider, Global::Fm::Op4> rr;
    std::array<GuiToggleButton, Global::Fm::Op4> am;
    std::array<GuiToggleButton, Global::Fm::Op4> vib;
    std::array<GuiToggleButton, Global::Fm::Op4> egType;
    std::array<GuiToggleButton, Global::Fm::Op4> ksr;
    std::array<GuiComboBox, Global::Fm::Op4> ksl; // Key Scale Level
    std::array<GuiCategoryLabel, Global::Fm::Op4> catShape;
    std::array<GuiComboBox, Global::Fm::Op4> eg; // Envlope Generator
    std::array<GuiCategoryLabel, Global::Fm::Op4> catMask;
    std::array<GuiToggleButton, Global::Fm::Op4> mask; // Mask
    std::array<GuiCategoryLabel, Global::Fm::Op4> catMml;
    std::array<GuiMmlButton, Global::Fm::Op4> mml;
    std::array<GuiSlider, Global::Fm::Op4> ams;
    std::array<GuiSlider, Global::Fm::Op4> amd;
    std::array<GuiSlider, Global::Fm::Op4> pms;
    std::array<GuiSlider, Global::Fm::Op4> pmd;
    std::array<GuiTextButton, Global::Fm::Op4> amsTo37;
    std::array<GuiTextButton, Global::Fm::Op4> amdTo1;
    std::array<GuiTextButton, Global::Fm::Op4> amdTo48;
    std::array<GuiTextButton, Global::Fm::Op4> pmsTo64;
    std::array<GuiTextButton, Global::Fm::Op4> pmdTo7;
    std::array<GuiTextButton, Global::Fm::Op4> pmdTo14;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
    GuiOpl3(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
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
        catLfo{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
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
        catShape{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        eg{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        catMml{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) },
        ams{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        amd{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pms{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pmd{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        amsTo37{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        amdTo1{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        amdTo48{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        pmsTo64{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        pmdTo7{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        pmdTo14{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) }
    {
	}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
