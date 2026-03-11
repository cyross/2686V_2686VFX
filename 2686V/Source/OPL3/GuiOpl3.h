#pragma once

#include <JuceHeader.h>
#include <array>

#include "../core/Global.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"
#include "../gui/GuiValues.h"

class GuiOpl3 : public GuiBase
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
    static inline const std::array<std::array<juce::String, 4>, 4> algOpPrefix = { {
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])"}}, // 00
        {{"([C:FB])", "([M->3])", "([M->4])", "([C])"}},    // 01
        {{"([M:FB->2])", "([C])", "([M->4])", "([C])"}},    // 02
        {{"([C:FB])", "([M->3])", "([C])", "([C])"}}        // 03
    } };

    GuiGroup mainGroup;
    std::array<GuiGroup, Global::Fm::Op4> opGroups;

    GuiCategoryLabel monoPolyCat;
    GuiCategoryLabel presetNameCat;
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

    // Mono/Poly切り替えスイッチ
    GuiToggleButton monoModeToggle;

    // プリセット名ラベル
    GuiLabel presetNameLabel;

    juce::ImageComponent algImageComp;
    std::array<juce::Image, 4> algImages;

    std::array<GuiCategoryLabel, Global::Fm::Op4> catMain;
    std::array<GuiCategoryLabel, Global::Fm::Op4> catLfo;
    std::array<GuiSlider, Global::Fm::Op4> mul;
    std::array<GuiSlider, Global::Fm::Op4> tl;
    std::array<GuiSlider, Global::Fm::Op4> ar;
    std::array<GuiTextButton, Global::Fm::Op4> arTo000;
    std::array<GuiTextButton, Global::Fm::Op4> arTo003;
    std::array<GuiSlider, Global::Fm::Op4> dr;
    std::array<GuiSlider, Global::Fm::Op4> sl;
    std::array<GuiSlider, Global::Fm::Op4> rr;
    std::array<GuiTextButton, Global::Fm::Op4> rrTo000;
    std::array<GuiTextButton, Global::Fm::Op4> rrTo003;
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

    std::array<GuiToggleButton, Global::Fm::Op4> rgEn;
    std::array<GuiSlider, Global::Fm::Op4> rgAr;
    std::array<GuiSlider, Global::Fm::Op4> rgDr;
    std::array<GuiSlider, Global::Fm::Op4> rgSl;
    std::array<GuiSlider, Global::Fm::Op4> rgRr;
    std::array<GuiSlider, Global::Fm::Op4> rgTl;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
    GuiOpl3(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        monoPolyCat(context),
        presetNameCat(context),
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
        arTo000{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        arTo003{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        dr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        sl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rrTo000{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        rrTo003{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
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
        pmdTo14{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        rgEn{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        rgAr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgDr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgSl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgRr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgTl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        monoModeToggle(context),
        presetNameLabel(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updateOpEnable(int idx, bool enable);
    void updateAlgorithmDisplay();
    void updateRgDisplayAsOp(int idx, bool rgMode);
    void updatePresetName(const juce::String& presetName);
};
