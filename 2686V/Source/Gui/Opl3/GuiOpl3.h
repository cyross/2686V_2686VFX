#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Const/ConstGlobal.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Processor/Opl3/ProcessorOpl3Values.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

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
    static inline const std::array<std::array<juce::String, Opl3PrValue::ops>, Opl3PrValue::algorithms> algOpPrefix = { {
        { {"([M:FB->2])", "([M->3])", "([M->4])", "([C])"} },   // 00
        { {"([C:FB])", "([M->3])", "([M->4])", "([C])"} },      // 01
        { {"([M:FB->2])", "([C])", "([M->4])", "([C])"} },      // 02
        { {"([C:FB])", "([M->3])", "([C])", "([C])"} },         // 03
        { { "([C:FB])", "([C])", "([C])", "([C])" } },          // 04
        { { "([M:FB->2])", "([C])", "([M:FB->4])", "([C])" } }, // 05
        { { "([C:FB])", "([C])", "([C:FB])", "([C])" } }        // 06
    } };

    GuiScrollGroup mainGroup;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel algFbCat;

    GuiSlider levelSlider;

    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    GuiComponentMidi midiComponent;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    GuiSeparator uSep001;
    GuiTextButton initLfoToOplBtn;
    GuiTextButton initLfoToOpllBtn;
    GuiSeparator uSep002;
    GuiTextButton copyOpParamToOplBtn;
    GuiSeparator uSep003;
    GuiTextButton copyOpParamBtn;
    GuiSlider copyOpFromSlider;
    GuiSlider copyOpToSlider;

    // プリセット名ラベル
    GuiLabel presetNameLabel;
    GuiSeparator presetNameSeparator;

    juce::ImageComponent algImageComp;
    std::array<juce::Image, Opl3PrValue::algorithms> algImages;

    std::array<GuiScrollGroup, Opl3PrValue::ops> opGroups;
    std::array<GuiCategoryLabel, Opl3PrValue::ops> catLfo;
    std::array<GuiComboBox, Opl3PrValue::ops> mul;
    std::array<GuiToggleButton, Opl3PrValue::ops> am;
    std::array<GuiToggleButton, Opl3PrValue::ops> vib;
    std::array<GuiToggleButton, Opl3PrValue::ops> egType;

    std::array<GuiCategoryLabel, Opl3PrValue::ops> ksCat;
    std::array<GuiToggleButton, Opl3PrValue::ops> ksr;
    std::array<GuiComboBox, Opl3PrValue::ops> ksl; // Key Scale Level

    // Pitch ADSR
    std::array<GuiComponentPitchEnv, Opl3PrValue::ops> pitchEnv;
    // SSG SW Env
    std::array<GuiComponentSsgSwEnv, Opl3PrValue::ops> ssgSwEnv;

    std::array<GuiCategoryLabel, Opl3PrValue::ops> catShape;
    std::array<GuiComboBox, Opl3PrValue::ops> eg; // Envlope Generator
    std::array<GuiCategoryLabel, Opl3PrValue::ops> catMask;
    std::array<GuiToggleButton, Opl3PrValue::ops> mask; // Mask
    std::array<GuiSeparator, Opl3PrValue::ops> mmlSeparator;
    std::array<GuiMmlButton, Opl3PrValue::ops> mml;
    std::array<GuiSlider, Opl3PrValue::ops> ams;
    std::array<GuiSlider, Opl3PrValue::ops> amd;
    std::array<GuiSlider, Opl3PrValue::ops> pms;
    std::array<GuiSlider, Opl3PrValue::ops> pmd;
    std::array<GuiTextButton, Opl3PrValue::ops> amsTo37;
    std::array<GuiTextButton, Opl3PrValue::ops> amdTo1;
    std::array<GuiTextButton, Opl3PrValue::ops> amdTo48;
    std::array<GuiTextButton, Opl3PrValue::ops> pmsTo64;
    std::array<GuiTextButton, Opl3PrValue::ops> pmdTo7;
    std::array<GuiTextButton, Opl3PrValue::ops> pmdTo14;

    std::array<GuiSlider, Opl3PrValue::ops> rgAr;
    std::array<GuiSlider, Opl3PrValue::ops> rgDr;
    std::array<GuiSlider, Opl3PrValue::ops> rgSl;
    std::array<GuiSlider, Opl3PrValue::ops> rgRr;
    std::array<GuiSlider, Opl3PrValue::ops> rgTl;
    std::array<GuiCategoryLabel, Opl3PrValue::ops> catOptional;
    std::array<GuiToggleButton, Opl3PrValue::ops> xof;
    std::array<GuiToggleButton, Opl3PrValue::ops> kor;
    std::array<GuiToggleButton, Opl3PrValue::ops> bypass;

    void applyMmlString(const juce::String& mml, int opIndex);

    std::array<GuiEnvelopeGraph, Opl3PrValue::ops> opGraphs;
    std::array<GuiToggleButton, Opl3PrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, Opl3PrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, Opl3PrValue::ops> graphBtnSsg;
    std::array<GuiSeparator, Opl3PrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw };
    std::array<GraphMode, Opl3PrValue::ops> currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
    GuiOpl3(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        qualityCat(context),
        algFbCat(context),
        levelSlider(context),
        algSelector(context),
        feedbackSlider(context),
        bitSelector(context),
        rateSelector(context),
        unisonComponent(context),
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        initLfoToOplBtn(context),
        initLfoToOpllBtn(context),
        uSep002(context),
        copyOpParamToOplBtn(context),
        uSep003(context),
        copyOpParamBtn(context),
        copyOpFromSlider(context),
        copyOpToSlider(context),
        opGroups{ GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context) },
        catLfo{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mul{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        am{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        vib{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        egType{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        ksCat{ GuiCategoryLabel(context), GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        ksr{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        ksl{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        pitchEnv{ GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context) },
        ssgSwEnv{ GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context) },
        catShape{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        eg{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        mmlSeparator{ GuiSeparator(context), GuiSeparator(context), GuiSeparator(context), GuiSeparator(context) },
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
        rgAr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgDr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgSl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgRr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgTl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        catOptional{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        xof{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        kor{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        bypass{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        midiComponent(context),
        presetNameLabel(context),
        presetNameSeparator(context),
        graphBtnAmp{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnPitch{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnSsg{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        graphSeparator{ GuiSeparator(context), GuiSeparator(context), GuiSeparator(context), GuiSeparator(context) }
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
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutOpLfoCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect);
    void setupGraph(int opIndex);
    void layoutOpGraph(int opIndex, juce::Rectangle<int>& rect);
    void setLevel(float level);
    void copyParams(CopyOpl3& copyObj);
    void copyOpParams(int p, CopyOpl3Op& copyObj);
    void pasteParams(CopyOpl3& copyObj);
    void pasteOpParams(int p, CopyOpl3Op& copyObj);
};
