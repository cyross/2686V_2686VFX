#pragma once

#include <JuceHeader.h>
#include <array>

#include "../core/Global.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiOpna : public GuiBase
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
    static inline const std::array<std::array<juce::String, 4>, 8> algOpPrefix = {{
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])"}}, // 00
        {{"([M:FB->3])", "([M->3])", "([M->4])", "([C])"}}, // 01
        {{"([M:FB->4])", "([M->3])", "([M->4])", "([C])"}}, // 02
        {{"([M:FB->2])", "([M->4])", "([M->4])", "([C])"}}, // 03
        {{"([M:FB->2])", "([C])", "([M->4])", "([C])"}},    // 04
        {{"([M:FB->2/3/4])", "([C])", "([C])", "([C])"}},   // 05
        {{"([M:FB->2])", "([C])", "([C])", "([C])"}},       // 06
        {{"([C:FB])", "([C])", "([C])", "([C])"}}           // 07
    }};

    GuiGroup mainGroup;
    std::array<GuiGroup, Global::Fm::Op4> opGroups;
    std::array<GuiGroup, Global::Fm::Op4> freqBtnGroup;
    GuiGroup lfoGroup;

    GuiCategoryLabel monoPolyCat;
    GuiCategoryLabel presetNameCat;
    GuiCategoryLabel qualityCat;
    GuiCategoryLabel algFbCat;

	GuiComboBox algSelector;
	GuiFbSlider feedbackSlider;
	GuiFbSlider feedback2Slider;
	GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    GuiCategoryLabel lfoCat;

    GuiSlider lfoFreqSlider;
    GuiSlider lfoAmSmRtSlider;
    GuiToggleButton lfoPmToggle;
    GuiToggleButton lfoAmToggle;
    GuiComboBox lfoPmsSelector;
	GuiComboBox lfoAmsSelector;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // Mono/Poly切り替えスイッチ
    GuiToggleButton monoModeToggle;

    // プリセット名ラベル
    GuiLabel presetNameLabel;

    std::array<GuiCategoryLabel, Global::Fm::Op4> catMain;
    std::array<GuiSlider, Global::Fm::Op4> mul;
    std::array<GuiComboBox, Global::Fm::Op4> dt;
    std::array<GuiSlider, Global::Fm::Op4> ar;
    std::array<GuiSlider, Global::Fm::Op4> dr;
    std::array<GuiSlider, Global::Fm::Op4> sr;
    std::array<GuiSlider, Global::Fm::Op4> sl;
    std::array<GuiSlider, Global::Fm::Op4> rr;
    std::array<GuiSlider, Global::Fm::Op4> tl;
    std::array<GuiComboBox, Global::Fm::Op4> ks;
    std::array<GuiCategoryLabel, Global::Fm::Op4> catShape;
    std::array<GuiComboBox, Global::Fm::Op4> se;
    std::array<GuiSlider, Global::Fm::Op4> seFreq;
    std::array<GuiCategoryLabel, Global::Fm::Op4> cafFix;
    std::array<GuiToggleButton, Global::Fm::Op4> fix;
    std::array<GuiSlider, Global::Fm::Op4> freq;
    std::array<GuiTextButton, Global::Fm::Op4> freqToZero;
    std::array<GuiTextButton, Global::Fm::Op4> freqTo440;
    std::array<GuiCategoryLabel, Global::Fm::Op4> catLfo;
    std::array<GuiToggleButton, Global::Fm::Op4> pm;  // OPLの vib に相当)
    std::array<GuiComboBox, Global::Fm::Op4> pms;
    std::array<GuiToggleButton, Global::Fm::Op4> am;  // OPMでは AMS-EN に相当)
    std::array<GuiComboBox, Global::Fm::Op4> ams;
    std::array<GuiCategoryLabel, Global::Fm::Op4> catMask;
    std::array<GuiToggleButton, Global::Fm::Op4> mask;
    std::array<GuiCategoryLabel, Global::Fm::Op4> catMml;
    std::array<GuiMmlButton, Global::Fm::Op4> mml;

    std::array<GuiToggleButton, Global::Fm::Op4> rgEn;
    std::array<GuiSlider, Global::Fm::Op4> rgAr;
    std::array<GuiSlider, Global::Fm::Op4> rgDr;
    std::array<GuiSlider, Global::Fm::Op4> rgSr;
    std::array<GuiSlider, Global::Fm::Op4> rgSl;
    std::array<GuiSlider, Global::Fm::Op4> rgRr;
    std::array<GuiSlider, Global::Fm::Op4> rgTl;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
	GuiOpna(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        freqBtnGroup{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        lfoGroup(context),
        monoPolyCat(context),
        presetNameCat(context),
        qualityCat(context),
        algFbCat(context),
        algSelector(context),
        feedbackSlider(context),
        feedback2Slider(context),
        bitSelector(context),
        rateSelector(context),
        lfoCat(context),
        lfoFreqSlider(context),
        lfoAmSmRtSlider(context),
        lfoPmToggle(context),
        lfoAmToggle(context),
        lfoPmsSelector(context),
        lfoAmsSelector(context),
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
        catShape{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        se{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        seFreq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        fix{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        cafFix{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        freq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        freqToZero{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        freqTo440{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        catLfo{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        pm{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        pms{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        am{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        ams{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        catMml{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) },
        rgEn{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        rgAr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgDr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgSr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgSl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgRr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgTl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        monoModeToggle(context),
        presetNameLabel(context)
    {
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updateOpEnable(int idx, bool enable);
    void updateAlgorithmDisplay();
    void updateRgDisplayAsOp(int idx, bool rgMode);
    void updatePresetName(const juce::String& presetName);
};
