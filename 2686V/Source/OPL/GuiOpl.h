#pragma once

#include <JuceHeader.h>
#include <array>

#include "../core/Global.h"
#include "../gui/GuiValues.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpl : public GuiBase
{
    /*
     * アルゴリズムのオペレータ表記凡例
     * 2026.3.7 CYROSS
     *
     * [C] : キャリアー(出力はオーディオ出力)
     * [M->n] : n番オペレータへ出力するモジュレーター
     * [C:FB] : 自身へフィードバックもするキャリアー
     * [M:FB->n] : 自身へフィードバックもする、n番オペレーターへ出力するモジュレーター
     * [C:FBm] : m番オペレータへフィードバックもするキャリア―
     * [M:FBm->n] : m番オペレータへフィードバックもする、n番オペレーターへ出力するモジュレーター
     * /を挟んでnが複数ある場合: それぞれのオペレータに出力する
     * 複数のnが存在する場合 : 各オペレーターからの出力を足し合わせて、n番のオペレータへ出力
     */
    static inline const std::array<std::array<juce::String, 2>, 2> algOpPrefix = { {
        {{"([M:FB->2])", "([C])"}}, // 00
        {{"([C:FB])", "([C])"}}     // 01
    } };

    GuiGroup mainGroup;
    std::array<GuiGroup, Global::Fm::Op2> opGroups;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel algFbCat;

    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    std::array<GuiCategoryLabel, Global::Fm::Op2> catMain;
    std::array<GuiCategoryLabel, Global::Fm::Op2> catLfo;
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
    std::array<GuiCategoryLabel, Global::Fm::Op2> catShape;
    std::array<GuiComboBox, Global::Fm::Op2> eg; // Envlope Generator
    std::array<GuiCategoryLabel, Global::Fm::Op2> catMask;
    std::array<GuiToggleButton, Global::Fm::Op2> mask; // Mask
    std::array<GuiCategoryLabel, Global::Fm::Op2> catMml;
    std::array<GuiMmlButton, Global::Fm::Op2> mml;
    std::array<GuiSlider, Global::Fm::Op2> ams;
    std::array<GuiSlider, Global::Fm::Op2> amd;
    std::array<GuiSlider, Global::Fm::Op2> pms;
    std::array<GuiSlider, Global::Fm::Op2> pmd;
    std::array<GuiTextButton, Global::Fm::Op2> amsTo37;
    std::array<GuiTextButton, Global::Fm::Op2> amdTo1;
    std::array<GuiTextButton, Global::Fm::Op2> amdTo48;
    std::array<GuiTextButton, Global::Fm::Op2> pmsTo64;
    std::array<GuiTextButton, Global::Fm::Op2> pmdTo7;
    std::array<GuiTextButton, Global::Fm::Op2> pmdTo14;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
    GuiOpl(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context) },
        qualityCat(context),
        algFbCat(context),
        algSelector(context),
        feedbackSlider(context),
        bitSelector(context),
        rateSelector(context),
        mvolCat(context),
        masterVolSlider(context),
        catMain{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        catLfo{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
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
        catShape{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        eg{ GuiComboBox(context), GuiComboBox(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context) },
        catMml{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context) },
        ams{ GuiSlider(context), GuiSlider(context) },
        amd{ GuiSlider(context), GuiSlider(context) },
        pms{ GuiSlider(context), GuiSlider(context) },
        pmd{ GuiSlider(context), GuiSlider(context) },
        amsTo37{ GuiTextButton(context), GuiTextButton(context) },
        amdTo1{ GuiTextButton(context), GuiTextButton(context) },
        amdTo48{ GuiTextButton(context), GuiTextButton(context) },
        pmsTo64{ GuiTextButton(context), GuiTextButton(context) },
        pmdTo7{ GuiTextButton(context), GuiTextButton(context) },
        pmdTo14{ GuiTextButton(context), GuiTextButton(context) }
    {
	}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updateOpEnable(int idx, bool enable);
    void updateAlgorithmDisplay();
};
