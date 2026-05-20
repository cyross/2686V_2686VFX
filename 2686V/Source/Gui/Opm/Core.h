#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Const/Global.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiValues.h"

class GuiOpm : public GuiBase
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
    static inline const std::array<std::array<juce::String, 4>, 8> algOpPrefix = { {
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])"}}, // 00
        {{"([M:FB->3])", "([M->3])", "([M->4])", "([C])"}}, // 01
        {{"([M:FB->4])", "([M->3])", "([M->4])", "([C])"}}, // 02
        {{"([M:FB->2])", "([M->4])", "([M->4])", "([C])"}}, // 03
        {{"([M:FB->2])", "([C])", "([M->4])", "([C])"}},    // 04
        {{"([M:FB->2/3/4])", "([C])", "([C])", "([C])"}},   // 05
        {{"([M:FB->2])", "([C])", "([C])", "([C])"}},       // 06
        {{"([C:FB])", "([C])", "([C])", "([C])"}}           // 07
    } };

    GuiGroup mainGroup;
    std::array<GuiGroup, Global::Fm::Op4> opGroups;

    GuiCategoryLabel monoPolyCat;
    GuiCategoryLabel qualityCat;
    GuiCategoryLabel algFbCat;

    // Global
    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    GuiCategoryLabel panCat;
    GuiSlider panSlider;
    GuiTextButton panToLBtn;
    GuiTextButton panToCBtn;
    GuiTextButton panToRBtn;

    GuiCategoryLabel lfoCat;

    // OPM LFO
    GuiSlider lfoFreqSlider;
    GuiSlider lfoAmSmRtSlider;
    GuiSlider lfoSyncDelaySlider;
    GuiTextButton lfoSyncDelayToZeroBtn;
    GuiTextButton lfoSyncDelayToOneBtn;
    GuiComboBox lfoPgShapeSelector;
    GuiComboBox lfoEgShapeSelector;
    GuiToggleButton lfoPmToggle;
    GuiToggleButton lfoAmToggle;
    GuiComboBox lfoPmsSelector;
    GuiComboBox lfoAmsSelector;
    GuiSlider lfoPmdSlider;
    GuiSlider lfoAmdSlider;

    GuiCategoryLabel mvolCat;

    // マスターボリューム(全音源共通の最終出力)
    GuiMasterVolumeSlider masterVolSlider;

    // Mono/Poly切り替えスイッチ
    GuiToggleButton monoModeToggle;

    // プリセット名ラベル
    GuiLabel presetNameLabel;

    juce::ImageComponent algImageComp;
    std::array<juce::Image, 8> algImages;

    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    std::array<GuiComboBox, Global::Fm::Op4> mul;
    std::array<GuiSlider, Global::Fm::Op4> dt1;
    std::array<GuiSlider, Global::Fm::Op4> dt2;
    std::array<GuiComboBox, Global::Fm::Op4> ks;

    std::array<GuiCategoryLabel, Global::Fm::Op4> catPitchEnv;
    std::array<GuiToggleButton, Global::Fm::Op4> pitchEnvEnable;
    std::array<GuiSlider, Global::Fm::Op4> pitchAttack;
    std::array<GuiSlider, Global::Fm::Op4> pitchDecay;
    std::array<GuiSlider, Global::Fm::Op4> pitchRelease;
    std::array<GuiSlider, Global::Fm::Op4> pitchStartLevel;
    std::array<GuiSlider, Global::Fm::Op4> pitchAttackLevel;
    std::array<GuiSlider, Global::Fm::Op4> pitchSustainLevel;
    std::array<GuiSlider, Global::Fm::Op4> pitchReleaseLevel;

    std::array<GuiCategoryLabel, Global::Fm::Op4> catSsgSwEnv;
    std::array<GuiToggleButton, Global::Fm::Op4> ssgSwEnvEnable;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwSteps;
    std::array<GuiToggleButton, Global::Fm::Op4> ssgSwEnvLoop;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwLoopTo;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwLoopCount;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwStartLevel;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwR1;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwL1;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwR2;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwL2;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwR3;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwL3;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwR4;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwL4;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwR5;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwL5;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwR6;
    std::array<GuiSlider, Global::Fm::Op4> ssgSwL6;

    std::array<GuiCategoryLabel, Global::Fm::Op4> cafLfo;
    std::array<GuiToggleButton, Global::Fm::Op4> amsEnable;
    std::array<GuiCategoryLabel, Global::Fm::Op4> cafFix;
    std::array<GuiToggleButton, Global::Fm::Op4> fix;
    std::array<GuiSlider, Global::Fm::Op4> freq;
    std::array<GuiTextButton, Global::Fm::Op4> freqToZero;
    std::array<GuiTextButton, Global::Fm::Op4> freqTo440;
    std::array<GuiCategoryLabel, Global::Fm::Op4> catMask;
    std::array<GuiToggleButton, Global::Fm::Op4> mask; // Mask
    std::array<GuiCategoryLabel, Global::Fm::Op4> catMml;
    std::array<GuiMmlButton, Global::Fm::Op4> mml;

    std::array<GuiSlider, Global::Fm::Op4> rgAr;
    std::array<GuiSlider, Global::Fm::Op4> rgD1r;
    std::array<GuiSlider, Global::Fm::Op4> rgD2r;
    std::array<GuiSlider, Global::Fm::Op4> rgD1l;
    std::array<GuiSlider, Global::Fm::Op4> rgRr;
    std::array<GuiSlider, Global::Fm::Op4> rgTl;

    void applyMmlString(const juce::String& mml, int opIndex);
public:
    GuiOpm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context), GuiGroup(context), GuiGroup(context) },
        monoPolyCat(context),
        qualityCat(context),
        algFbCat(context),
        algSelector(context),
        feedbackSlider(context),
        bitSelector(context),
        rateSelector(context),
        panCat(context),
        panSlider(context),
        panToLBtn(context),
        panToCBtn(context),
        panToRBtn(context),
        lfoCat(context),
        lfoFreqSlider(context),
        lfoAmSmRtSlider(context),
        lfoSyncDelaySlider(context),
        lfoSyncDelayToZeroBtn(context),
        lfoSyncDelayToOneBtn(context),
        lfoPgShapeSelector(context),
        lfoEgShapeSelector(context),
        lfoPmToggle(context),
        lfoAmToggle(context),
        lfoPmsSelector(context),
        lfoAmsSelector(context),
        lfoPmdSlider(context),
        lfoAmdSlider(context),
        mvolCat(context),
        masterVolSlider(context),
        mul{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        dt1{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt2{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ks{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        catPitchEnv{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        pitchEnvEnable{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        pitchAttack{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pitchDecay{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pitchRelease{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pitchStartLevel{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pitchAttackLevel{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pitchSustainLevel{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pitchReleaseLevel{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        catSsgSwEnv{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        ssgSwEnvEnable{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        ssgSwSteps{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwEnvLoop{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        ssgSwLoopTo{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwLoopCount{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwStartLevel{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwR1{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwL1{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwR2{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwL2{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwR3{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwL3{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwR4{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwL4{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwR5{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwL5{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwR6{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ssgSwL6{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        cafLfo{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        amsEnable{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        cafFix{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        fix{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        freq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        freqToZero{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        freqTo440{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        catMml{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) },
        rgAr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgD1r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgD2r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgD1l{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
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
    bool keyPressed(const juce::KeyPress& key) override;
    void copyFmParamsToString();
    void copyFmParamsToObject();
    void pasteFmParamsFromObject();
    void initParams();
    void layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpFixCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutMonoModeCat(juce::Rectangle<int>& rect);
    void layoutMvolCat(juce::Rectangle<int>& rect);
    void layoutPanCat(juce::Rectangle<int>& rect);
    void layoutHwLfoCat(juce::Rectangle<int>& rect);
    void layoutOpHwLfoCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpPitchEnvCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpSsgSwEnvCat(int opIndex, juce::Rectangle<int>& rect);
};
