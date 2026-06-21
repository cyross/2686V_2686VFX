#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Const/ConstGlobal.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Processor/Opl/ProcessorOplValues.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

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
    static inline const std::array<std::array<juce::String, OplPrValue::ops>, OplPrValue::algorithms> algOpPrefix = { {
        {{"([M:FB->2])", "([C])"}}, // 00
        {{"([C:FB])", "([C])"}}     // 01
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
    GuiTextButton copyOpParamToOpl3Btn;
    GuiTextButton copyOpParamToOpl312Btn;
    GuiTextButton copyOpParamToOpl334Btn;
    GuiSeparator uSep003;
    GuiTextButton copyOpParamBtn;
    GuiSlider copyOpFromSlider;
    GuiSlider copyOpToSlider;
    GuiSeparator uSep004;
    GuiTextButton importLfoParamButton;
    GuiTextButton exportLfoParamButton;
    GuiTextButton importPitchEnvParamButton;
    GuiTextButton exportPitchEnvParamButton;
    GuiTextButton importSsgSwEnvParamButton;
    GuiTextButton exportSsgSwEnvParamButton;
    GuiSlider targerOpSlider;
    GuiSeparator uSep005;
    GuiTextButton importUnisonParamButton;
    GuiTextButton exportUnisonParamButton;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // プリセット名ラベル
    GuiLabel presetNameLabel;
    GuiSeparator presetNameSeparator;

    juce::ImageComponent algImageComp;
    std::array<juce::Image, OplPrValue::algorithms> algImages;

    std::array<GuiScrollGroup, OplPrValue::ops> opGroups;

    std::array<GuiCategoryLabel, OplPrValue::ops> catLfo;
    std::array<GuiComboBox, OplPrValue::ops> mul;
    std::array<GuiToggleButton, OplPrValue::ops> am;
    std::array<GuiToggleButton, OplPrValue::ops> vib;
    std::array<GuiToggleButton, OplPrValue::ops> egType;

    std::array<GuiCategoryLabel, OplPrValue::ops> ksCat;
    std::array<GuiToggleButton, OplPrValue::ops> ksr;
    std::array<GuiComboBox, OplPrValue::ops> ksl; // Key Scale Level

    // Pitch ADSR
    std::array<GuiComponentPitchEnv, OplPrValue::ops> pitchEnv;
    // SSG SW Env
    std::array<GuiComponentSsgSwEnv, OplPrValue::ops> ssgSwEnv;

    std::array<GuiCategoryLabel, OplPrValue::ops> catShape;
    std::array<GuiComboBox, OplPrValue::ops> eg; // Envlope Generator
    std::array<GuiCategoryLabel, OplPrValue::ops> catMask;
    std::array<GuiToggleButton, OplPrValue::ops> mask; // Mask
    std::array<GuiSeparator, OplPrValue::ops> mmlSeparator;
    std::array<GuiMmlButton, OplPrValue::ops> mml;
    std::array<GuiSlider, OplPrValue::ops> ams;
    std::array<GuiSlider, OplPrValue::ops> amd;
    std::array<GuiSlider, OplPrValue::ops> pms;
    std::array<GuiSlider, OplPrValue::ops> pmd;
    std::array<GuiTextButton, OplPrValue::ops> amsTo37;
    std::array<GuiTextButton, OplPrValue::ops> amsTo606;
    std::array<GuiTextButton, OplPrValue::ops> amdTo1;
    std::array<GuiTextButton, OplPrValue::ops> amdTo12;
    std::array<GuiTextButton, OplPrValue::ops> amdTo48;
    std::array<GuiTextButton, OplPrValue::ops> pmsTo606;
    std::array<GuiTextButton, OplPrValue::ops> pmsTo64;
    std::array<GuiTextButton, OplPrValue::ops> pmdTo7;
    std::array<GuiTextButton, OplPrValue::ops> pmdTo137;
    std::array<GuiTextButton, OplPrValue::ops> pmdTo14;

    std::array<GuiSlider, OplPrValue::ops> rgAr;
    std::array<GuiSlider, OplPrValue::ops> rgDr;
    std::array<GuiSlider, OplPrValue::ops> rgSl;
    std::array<GuiSlider, OplPrValue::ops> rgRr;
    std::array<GuiSlider, OplPrValue::ops> rgTl;
    std::array<GuiCategoryLabel, OplPrValue::ops> catOptional;
    std::array<GuiToggleButton, OplPrValue::ops> xof;
    std::array<GuiToggleButton, OplPrValue::ops> kor;
    std::array<GuiToggleButton, OplPrValue::ops> bypass;

    std::array<GuiCategoryLabel, OplPrValue::ops> adsrCat;
    std::array<GuiToggleButton, OplPrValue::ops> sus;

    void applyMmlString(const juce::String& mml, int opIndex);

    std::array<GuiEnvelopeGraph, OplPrValue::ops> opGraphs;
    std::array<GuiToggleButton, OplPrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, OplPrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, OplPrValue::ops> graphBtnSsg;
    std::array<GuiSeparator, OplPrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw };
    std::array<GraphMode, OplPrValue::ops> currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
    GuiOpl(const GuiContext& context) :
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
        copyOpParamToOpl3Btn(context),
        copyOpParamToOpl312Btn(context),
        copyOpParamToOpl334Btn(context),
        uSep003(context),
        copyOpParamBtn(context),
        copyOpFromSlider(context),
        copyOpToSlider(context),
        uSep004(context),
        importLfoParamButton(context),
        exportLfoParamButton(context),
        importPitchEnvParamButton(context),
        exportPitchEnvParamButton(context),
        importSsgSwEnvParamButton(context),
        exportSsgSwEnvParamButton(context),
        targerOpSlider(context),
        uSep005(context),
        importUnisonParamButton(context),
        exportUnisonParamButton(context),
        opGroups{ GuiScrollGroup(context), GuiScrollGroup(context) },
        catLfo{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mul{ GuiComboBox(context), GuiComboBox(context) },
        am{ GuiToggleButton(context),GuiToggleButton(context) },
        vib{ GuiToggleButton(context),GuiToggleButton(context) },
        egType{ GuiToggleButton(context),GuiToggleButton(context) },
        ksCat{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        ksr{ GuiToggleButton(context),GuiToggleButton(context) },
        ksl{ GuiComboBox(context), GuiComboBox(context) },
        pitchEnv{ GuiComponentPitchEnv(context), GuiComponentPitchEnv(context) },
        ssgSwEnv{ GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context) },
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
        catOptional{ GuiCategoryLabel(context),GuiCategoryLabel(context) },
        xof{ GuiToggleButton(context),GuiToggleButton(context) },
        bypass{ GuiToggleButton(context),GuiToggleButton(context) },
        adsrCat{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        sus{ GuiToggleButton(context),GuiToggleButton(context) },
        kor{ GuiToggleButton(context),GuiToggleButton(context) },
        midiComponent(context),
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
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutOpAdsrCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpLfoCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect);
    void setupGraph(int opIndex);
    void layoutOpGraph(int opIndex, juce::Rectangle<int>& rect);
    void setLevel(float level);
    void copyParams(CopyOpl& copyObj);
    void copyOpParams(int p, CopyOplOp& copyObj);
    void pasteParams(CopyOpl& copyObj);
    void pasteOpParams(int p, CopyOplOp& copyObj);
    void importLfoParam(int opIndex);
    void exportLfoParam(int opIndex);
    void importPitchEnvParam(int opIndex);
    void exportPitchEnvParam(int opIndex);
    void importSsgSwEnvParam(int opIndex);
    void exportSsgSwEnvParam(int opIndex);
    void importUnisonParam();
    void exportUnisonParam();
};
