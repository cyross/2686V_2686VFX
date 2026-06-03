#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Const/Global.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/Core.h"
#include "../../Advanced/Curve/Core.h"
#include "../../Gui/Components/Unison/Unison.h"

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

    GuiCategoryLabel monoPolyCat;
    GuiCategoryLabel qualityCat;
    GuiCategoryLabel algFbCat;
    GuiCategoryLabel initCat;

    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    GuiTextButton initLfoToOplBtn;
    GuiTextButton initLfoToOpllBtn;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    // Mono/Poly切り替えスイッチ
    GuiToggleButton monoModeToggle;

    // プリセット名ラベル
    GuiLabel presetNameLabel;
    GuiSeparator presetNameSeparator;

    juce::ImageComponent algImageComp;
    std::array<juce::Image, 2> algImages;

    std::array<GuiCategoryLabel, Global::Fm::Op2> catLfo;
    std::array<GuiComboBox, Global::Fm::Op2> mul;
    std::array<GuiToggleButton, Global::Fm::Op2> am;
    std::array<GuiToggleButton, Global::Fm::Op2> vib;
    std::array<GuiToggleButton, Global::Fm::Op2> egType;
    std::array<GuiToggleButton, Global::Fm::Op2> ksr;

    std::array<GuiCategoryLabel, Global::Fm::Op2> catPitchEnv;
    std::array<GuiToggleButton, Global::Fm::Op2> pitchEnvEnable;
    std::array<GuiSlider, Global::Fm::Op2> pitchAttack;
    std::array<GuiSlider, Global::Fm::Op2> pitchDecay;
    std::array<GuiSlider, Global::Fm::Op2> pitchRelease;
    std::array<GuiSlider, Global::Fm::Op2> pitchStartLevel;
    std::array<GuiSlider, Global::Fm::Op2> pitchAttackLevel;
    std::array<GuiSlider, Global::Fm::Op2> pitchSustainLevel;
    std::array<GuiSlider, Global::Fm::Op2> pitchReleaseLevel;

    std::array<GuiCategoryLabel, Global::Fm::Op2> catSsgSwEnv;
    std::array<GuiToggleButton, Global::Fm::Op2> ssgSwEnvEnable;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwSteps;
    std::array<GuiToggleButton, Global::Fm::Op2> ssgSwEnvLoop;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwLoopTo;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwLoopCount;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwStartLevel;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwR1;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwL1;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwR2;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwL2;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwR3;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwL3;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwR4;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwL4;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwR5;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwL5;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwR6;
    std::array<GuiSlider, Global::Fm::Op2> ssgSwL6;

    std::array<GuiComboBox, Global::Fm::Op2> ksl; // Key Scale Level
    std::array<GuiCategoryLabel, Global::Fm::Op2> catShape;
    std::array<GuiComboBox, Global::Fm::Op2> eg; // Envlope Generator
    std::array<GuiCategoryLabel, Global::Fm::Op2> catMask;
    std::array<GuiToggleButton, Global::Fm::Op2> mask; // Mask
    std::array<GuiSeparator, Global::Fm::Op2> mmlSeparator;
    std::array<GuiMmlButton, Global::Fm::Op2> mml;
    std::array<GuiSlider, Global::Fm::Op2> ams;
    std::array<GuiSlider, Global::Fm::Op2> amd;
    std::array<GuiSlider, Global::Fm::Op2> pms;
    std::array<GuiSlider, Global::Fm::Op2> pmd;
    std::array<GuiTextButton, Global::Fm::Op2> amsTo37;
    std::array<GuiTextButton, Global::Fm::Op2> amsTo606;
    std::array<GuiTextButton, Global::Fm::Op2> amdTo1;
    std::array<GuiTextButton, Global::Fm::Op2> amdTo12;
    std::array<GuiTextButton, Global::Fm::Op2> amdTo48;
    std::array<GuiTextButton, Global::Fm::Op2> pmsTo606;
    std::array<GuiTextButton, Global::Fm::Op2> pmsTo64;
    std::array<GuiTextButton, Global::Fm::Op2> pmdTo7;
    std::array<GuiTextButton, Global::Fm::Op2> pmdTo137;
    std::array<GuiTextButton, Global::Fm::Op2> pmdTo14;

    std::array<GuiSlider, Global::Fm::Op2> rgAr;
    std::array<GuiSlider, Global::Fm::Op2> rgDr;
    std::array<GuiSlider, Global::Fm::Op2> rgSl;
    std::array<GuiSlider, Global::Fm::Op2> rgRr;
    std::array<GuiSlider, Global::Fm::Op2> rgTl;
    std::array<GuiCategoryLabel, Global::Fm::Op4> catOptional;
    std::array<GuiToggleButton, Global::Fm::Op4> xof;
    std::array<GuiToggleButton, Global::Fm::Op4> bypass;

    std::array<GuiCategoryLabel, Global::Fm::Op2> adsrCat;
    std::array<GuiToggleButton, Global::Fm::Op2> sus;

    void applyMmlString(const juce::String& mml, int opIndex);

    std::array<GuiEnvelopeGraph, Global::Fm::Op2> opGraphs;
    std::array<GuiToggleButton, Global::Fm::Op2> graphBtnAmp;
    std::array<GuiToggleButton, Global::Fm::Op2> graphBtnPitch;
    std::array<GuiToggleButton, Global::Fm::Op2> graphBtnSsg;
    std::array<GuiSeparator, Global::Fm::Op2> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw };
    std::array<GraphMode, Global::Fm::Op2> currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
    GuiOpl(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        opGroups{ GuiGroup(context), GuiGroup(context) },
        monoPolyCat(context),
        qualityCat(context),
        algFbCat(context),
        initCat(context),
        algSelector(context),
        feedbackSlider(context),
        bitSelector(context),
        rateSelector(context),
        initLfoToOplBtn(context),
        initLfoToOpllBtn(context),
        unisonComponent(context),
        catLfo{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mul{ GuiComboBox(context), GuiComboBox(context) },
        am{ GuiToggleButton(context),GuiToggleButton(context) },
        vib{ GuiToggleButton(context),GuiToggleButton(context) },
        egType{ GuiToggleButton(context),GuiToggleButton(context) },
        ksr{ GuiToggleButton(context),GuiToggleButton(context) },
        ksl{ GuiComboBox(context), GuiComboBox(context) },
        catPitchEnv{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        pitchEnvEnable{ GuiToggleButton(context),GuiToggleButton(context) },
        pitchAttack{ GuiSlider(context), GuiSlider(context) },
        pitchDecay{ GuiSlider(context), GuiSlider(context) },
        pitchRelease{ GuiSlider(context), GuiSlider(context) },
        pitchStartLevel{ GuiSlider(context), GuiSlider(context) },
        pitchAttackLevel{ GuiSlider(context), GuiSlider(context) },
        pitchSustainLevel{ GuiSlider(context), GuiSlider(context) },
        pitchReleaseLevel{ GuiSlider(context), GuiSlider(context) },
        catSsgSwEnv{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        ssgSwEnvEnable{ GuiToggleButton(context),GuiToggleButton(context) },
        ssgSwSteps{ GuiSlider(context), GuiSlider(context) },
        ssgSwEnvLoop{ GuiToggleButton(context),GuiToggleButton(context) },
        ssgSwLoopTo{ GuiSlider(context), GuiSlider(context) },
        ssgSwLoopCount{ GuiSlider(context), GuiSlider(context) },
        ssgSwStartLevel{ GuiSlider(context), GuiSlider(context) },
        ssgSwR1{ GuiSlider(context), GuiSlider(context) },
        ssgSwL1{ GuiSlider(context), GuiSlider(context) },
        ssgSwR2{ GuiSlider(context), GuiSlider(context) },
        ssgSwL2{ GuiSlider(context), GuiSlider(context) },
        ssgSwR3{ GuiSlider(context), GuiSlider(context) },
        ssgSwL3{ GuiSlider(context), GuiSlider(context) },
        ssgSwR4{ GuiSlider(context), GuiSlider(context) },
        ssgSwL4{ GuiSlider(context), GuiSlider(context) },
        ssgSwR5{ GuiSlider(context), GuiSlider(context) },
        ssgSwL5{ GuiSlider(context), GuiSlider(context) },
        ssgSwR6{ GuiSlider(context), GuiSlider(context) },
        ssgSwL6{ GuiSlider(context), GuiSlider(context) },
        catShape{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        eg{ GuiComboBox(context), GuiComboBox(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context) },
        mmlSeparator{ GuiSeparator(context), GuiSeparator(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context) },
        ams{ GuiSlider(context), GuiSlider(context) },
        amd{ GuiSlider(context), GuiSlider(context) },
        pms{ GuiSlider(context), GuiSlider(context) },
        pmd{ GuiSlider(context), GuiSlider(context) },
        amsTo37{ GuiTextButton(context), GuiTextButton(context) },
        amsTo606{ GuiTextButton(context), GuiTextButton(context) },
        amdTo1{ GuiTextButton(context), GuiTextButton(context) },
        amdTo12{ GuiTextButton(context), GuiTextButton(context) },
        amdTo48{ GuiTextButton(context), GuiTextButton(context) },
        pmsTo606{ GuiTextButton(context), GuiTextButton(context) },
        pmsTo64{ GuiTextButton(context), GuiTextButton(context) },
        pmdTo7{ GuiTextButton(context), GuiTextButton(context) },
        pmdTo137{ GuiTextButton(context), GuiTextButton(context) },
        pmdTo14{ GuiTextButton(context), GuiTextButton(context) },
        rgAr{ GuiSlider(context), GuiSlider(context) },
        rgDr{ GuiSlider(context), GuiSlider(context) },
        rgSl{ GuiSlider(context), GuiSlider(context) },
        rgRr{ GuiSlider(context), GuiSlider(context) },
        rgTl{ GuiSlider(context), GuiSlider(context) },
        catOptional{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        xof{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        bypass{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        adsrCat{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        sus{ GuiToggleButton(context),GuiToggleButton(context) },
        monoModeToggle(context),
        presetNameLabel(context),
        presetNameSeparator(context),
        graphBtnAmp{ GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnPitch{ GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnSsg{ GuiToggleButton(context), GuiToggleButton(context) },
        graphSeparator{ GuiSeparator(context), GuiSeparator(context) }
    {
        currentGraphMode.fill(GraphMode::Amp); // 初期状態はすべてAmp
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
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutMonoModeCat(juce::Rectangle<int>& rect);
    void layoutInitializeCat(Rectangle<int>& rect);
    void layoutOpAdsrCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpLfoCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpPitchEnvCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpSsgSwEnvCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect);
    void applyOpSsgSwEnvLoopValues(int opIndex, bool enabled);
    void setupGraph(int opIndex);
    void layoutOpGraph(int opIndex, juce::Rectangle<int>& rect);
};
