#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Const/ConstGlobal.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/Fix/Fix.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Processor/Opm/ProcessorOpmValues.h"
#include "../../Gui/Components/PresetName/PresetName.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"
#include "../../Gui/Components/Level/Level.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"
#include "../../Gui/Components/Quality/Quality.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

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
    static inline const std::array<std::array<juce::String, OpmPrValue::ops>, OpmPrValue::algorithms> algOpPrefix = { {
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])"}}, // 00
        {{"([M:FB->3])", "([M->3])", "([M->4])", "([C])"}}, // 01
        {{"([M:FB->4])", "([M->3])", "([M->4])", "([C])"}}, // 02
        {{"([M:FB->2])", "([M->4])", "([M->4])", "([C])"}}, // 03
        {{"([M:FB->2])", "([C])", "([M->4])", "([C])"}},    // 04
        {{"([M:FB->2/3/4])", "([C])", "([C])", "([C])"}},   // 05
        {{"([M:FB->2])", "([C])", "([C])", "([C])"}},       // 06
        {{"([C:FB])", "([C])", "([C])", "([C])"}}           // 07
    } };

    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiCategoryLabel algFbCat;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

    // Global
    GuiComboBox algSelector;
    NormalSeparator algFbSep;
    GuiFbSlider feedbackSlider;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

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
    NormalSeparator lfoSep1;
    NormalSeparator lfoSep2;

    GuiComponentMidi midiComponent;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiTextButton copyParamsToOpnaBtn;
    GuiTextButton copyParamsToOpnBtn;
    NormalSeparator uSep002;
    GuiTextButton copyOpParamBtn;
    GuiSlider copyOpFromSlider;
    GuiSlider copyOpToSlider;
    NormalSeparator uSep003;
    GuiComponentImportExport ieOpPitchEnv;
    GuiComponentImportExport ieOpSsgSwEnv;
    GuiComponentImportExport ieOpSsgSwEnv11;
    GuiComponentImportExport ieOpSsgSwPEnv11;
    GuiSlider targerOpSlider;
    NormalSeparator uSep004;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::ImageComponent algImageComp;
    std::array<juce::Image, OpmPrValue::algorithms> algImages;

    std::array<GuiScrollGroup, OpmPrValue::ops> opGroups;

    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    std::array<GuiCategoryLabel, OpmPrValue::ops> catDet;
    std::array<GuiCategoryLabel, OpmPrValue::ops> catAmp;
    std::array<GuiComboBox, OpmPrValue::ops> mul;
    std::array<GuiSlider, OpmPrValue::ops> mulRatio;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo001;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo005;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo1;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo10;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo2757;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo02;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo025;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo04;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo05;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo06;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo075;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioTo08;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioPM10;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioPM1;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioPM01;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioPM001;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioP001;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioP01;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioP1;
    std::array<GuiTextButton, OpmPrValue::ops> mulRatioP10;
    std::array<NormalSeparator, OpmPrValue::ops> mulDetSep;
    std::array<GuiSlider, OpmPrValue::ops> dt1;
    std::array<GuiSlider, OpmPrValue::ops> dt2;

    std::array<GuiCategoryLabel, OpmPrValue::ops> ksCat;
    std::array<GuiComboBox, OpmPrValue::ops> ksMode;
    std::array<GuiComboBox, OpmPrValue::ops> ks;
    std::array<GuiSlider, OpmPrValue::ops> ksrOPP;
    std::array<GuiSlider, OpmPrValue::ops> kslOPP;

    // Pitch ADSR
    std::array<GuiComponentPitchEnv, OpmPrValue::ops> pitchEnv;
    // SSG SW Env
    std::array<GuiComponentSsgSwEnv, OpmPrValue::ops> ssgSwEnv;

    std::array<GuiComponentSsgSwEnv11, OpmPrValue::ops> ssgSwEnv11;
    std::array<GuiComponentSsgSwPEnv11, OpmPrValue::ops> ssgSwPEnv11;

    std::array<GuiCategoryLabel, OpmPrValue::ops> cafLfo;
    std::array<GuiToggleButton, OpmPrValue::ops> amsEnable;
    std::array<GuiComponentFix, OpmPrValue::ops> fix;
    std::array<GuiCategoryLabel, OpmPrValue::ops> catMask;
    std::array<GuiToggleButton, OpmPrValue::ops> mask; // Mask
    std::array<NormalSeparator, OpmPrValue::ops> mmlSeparator;
    std::array<GuiMmlButton, OpmPrValue::ops> mml;

    std::array<GuiSlider, OpmPrValue::ops> rgAr;
    std::array<GuiSlider, OpmPrValue::ops> rgD1r;
    std::array<GuiSlider, OpmPrValue::ops> rgD2r;
    std::array<GuiSlider, OpmPrValue::ops> rgD1l;
    std::array<GuiSlider, OpmPrValue::ops> rgRr;
    std::array<GuiSlider, OpmPrValue::ops> rgTl;
    std::array<GuiCategoryLabel, OpmPrValue::ops> catOptional;
    std::array<GuiToggleButton, OpmPrValue::ops> xof;
    std::array<GuiToggleButton, OpmPrValue::ops> kor;
    std::array<GuiToggleButton, OpmPrValue::ops> bypass;

    void applyMmlString(const juce::String& mml, int opIndex);

    std::array<GuiEnvelopeGraph, OpmPrValue::ops> opGraphs;
    std::array<GuiToggleButton, OpmPrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, OpmPrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, OpmPrValue::ops> graphBtnSsg;
    std::array<GuiToggleButton, OpmPrValue::ops> graphBtnSsg11;
    std::array<GuiToggleButton, OpmPrValue::ops> graphBtnSsgP11;
    std::array<NormalSeparator, OpmPrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };
    std::array<GraphMode, OpmPrValue::ops> currentGraphMode;

    bool isUpdatingGraph = false;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
    GuiOpm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        presetName(context),
        algFbCat(context),
        levelComponent(context),
        qualityComponent(context),
        algSelector(context),
        algFbSep(context),
        feedbackSlider(context),
        unisonComponent(context),
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
		lfoSep1(context),
        lfoSep2(context),
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        copyParamsToOpnaBtn(context),
        copyParamsToOpnBtn(context),
        uSep002(context),
        copyOpParamBtn(context),
        copyOpFromSlider(context),
        copyOpToSlider(context),
        uSep003(context),
        ieOpPitchEnv(context),
        ieOpSsgSwEnv(context),
        ieOpSsgSwEnv11(context),
        ieOpSsgSwPEnv11(context),
        targerOpSlider(context),
        uSep004(context),
        ieLfo(context),
        ieUnison(context),
        ieQuality(context),
        opGroups{ GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context) },
        catDet{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        catAmp{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        mul{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        mulRatio{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        mulRatioTo001{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioTo005{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioTo1{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioTo10{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioTo2757{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioTo02{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioTo025{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioTo04{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioTo05{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioTo06{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioTo075{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioTo08{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioPM10{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioPM1{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioPM01{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioPM001{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioP001{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioP01{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioP1{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulRatioP10{ GuiTextButton(context),GuiTextButton(context),GuiTextButton(context),GuiTextButton(context) },
        mulDetSep{ NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context) },
        dt1{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        dt2{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        ksCat{ GuiCategoryLabel(context), GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        ksMode{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        ks{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        ksrOPP{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        kslOPP{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pitchEnv{ GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context) },
        ssgSwEnv{ GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context) },
        ssgSwEnv11{ GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context) },
        ssgSwPEnv11{ GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context) },
        cafLfo{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        amsEnable{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        fix{ GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        mmlSeparator{ NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) },
        rgAr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgD1r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgD2r{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgD1l{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgRr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgTl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        catOptional{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        xof{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        kor{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        bypass{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        midiComponent(context),
        graphBtnAmp{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnPitch{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnSsg{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnSsg11{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnSsgP11{ GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context), GuiToggleButton(context) },
        graphSeparator{ NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context) }
    {
        currentGraphMode.fill(GraphMode::Amp); // 初期状態はすべてAmp
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updateOpEnable(int idx, bool enable);
    void updateAlgorithmDisplay();
    void updateRgDisplayAsOp(int idx, bool rgMode);
    void updatePresetName(const juce::String& name);
    bool keyPressed(const juce::KeyPress& key) override;
    void copyFmParamsToString();
    void copyFmParamsToObject();
    void pasteFmParamsFromObject();
    void initParams();
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanCat(juce::Rectangle<int>& rect);
    void layoutHwLfoCat(juce::Rectangle<int>& rect);
    void layoutOpHwLfoCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpDetCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpAmpCat(int opIndex, juce::Rectangle<int>& rect);
    void setupGraph(int opIndex);
    void layoutOpGraph(int opIndex, juce::Rectangle<int>& rect);
    void setLevel(float level);
    void copyParams(CopyOpm& copyObj);
    void copyOpParams(int p, CopyOpmOp& copyObj);
    void pasteParams(CopyOpm& copyObj);
    void pasteOpParams(int p, CopyOpmOp& copyObj);
    void copyParamsOpn(CopyOpnOpm& copyObj);
    void copyParamsOpnaOpn(CopyOpnaOpnOpm& copyObj);
    void copyOpParamsOpn(int p, CopyOpnOpmOp& copyObj);
    void copyOpParamsOpnaOpn(int p, CopyOpnaOpnOpmOp& copyObj);
    void pasteParamsOpn(CopyOpnOpm& copyObj);
    void pasteParamsOpnaOpn(CopyOpnaOpnOpm& copyObj);
    void pasteOpParamsOpn(int p, CopyOpnOpmOp& copyObj);
    void pasteOpParamsOpnaOpn(int p, CopyOpnaOpnOpmOp& copyObj);
    void importPitchEnvParam(int opIndex);
    void exportPitchEnvParam(int opIndex);
    void importSsgSwEnvParam(int opIndex);
    void exportSsgSwEnvParam(int opIndex);
    void importSsgSwEnv11Param(int opIndex);
    void exportSsgSwEnv11Param(int opIndex);
    void importSsgSwPEnv11Param(int opIndex);
    void exportSsgSwPEnv11Param(int opIndex);
    void importLfoParam();
    void exportLfoParam();
    void importUnisonParam();
    void exportUnisonParam();
    void importQualityParam();
    void exportQualityParam();
};
