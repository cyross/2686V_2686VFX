#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Const/ConstGlobal.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/Fix/Fix.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Processor/Opna/ProcessorOpnaValues.h"
#include "../../Gui/Components/PresetName/PresetName.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

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
    static inline const std::array<std::array<juce::String, OpnaPrValue::ops>, OpnaPrValue::algorithms> algOpPrefix = {{
        {{"([M:FB->2])", "([M->3])", "([M->4])", "([C])"}}, // 00
        {{"([M:FB->3])", "([M->3])", "([M->4])", "([C])"}}, // 01
        {{"([M:FB->4])", "([M->3])", "([M->4])", "([C])"}}, // 02
        {{"([M:FB->2])", "([M->4])", "([M->4])", "([C])"}}, // 03
        {{"([M:FB->2])", "([C])", "([M->4])", "([C])"}},    // 04
        {{"([M:FB->2/3/4])", "([C])", "([C])", "([C])"}},   // 05
        {{"([M:FB->2])", "([C])", "([C])", "([C])"}},       // 06
        {{"([C:FB])", "([C])", "([C])", "([C])"}}           // 07
    }};

    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiSlider levelSlider;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel algFbCat;

	GuiComboBox algSelector;
	GuiFbSlider feedbackSlider;
	GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    GuiCategoryLabel panCat;
    GuiSlider panSlider;
    GuiTextButton panToLBtn;
    GuiTextButton panToCBtn;
    GuiTextButton panToRBtn;

    GuiCategoryLabel lfoCat;

    GuiSlider lfoFreqSlider;
    GuiComboBox lfoShapeSelector;
    GuiSlider lfoAmSmRtSlider;
    GuiSlider lfoSyncDelaySlider;
    GuiTextButton lfoSyncDelayToZeroBtn;
    GuiTextButton lfoSyncDelayToOneBtn;
    GuiToggleButton lfoPmToggle;
    GuiToggleButton lfoAmToggle;
    GuiSlider lfoPmdSlider;
    GuiSlider lfoPmsSlider;
    GuiSlider lfoAmdSlider;

    GuiComponentMidi midiComponent;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    GuiSeparator uSep001;
    GuiSlider copyHwLfoFromSlider;
    GuiTextButton copyHwLfoParamsBtn;
    GuiSeparator uSep002;
    GuiTextButton copyParamsToOpnBtn;
    GuiTextButton copyParamsToOpmBtn;
    GuiSeparator uSep003;
    GuiTextButton copyOpParamBtn;
    GuiSlider copyOpFromSlider;
    GuiSlider copyOpToSlider;
    GuiSeparator uSep004;
    GuiTextButton importHwLfoParamButton;
    GuiTextButton exportHwLfoParamButton;
    GuiTextButton importPitchEnvParamButton;
    GuiTextButton exportPitchEnvParamButton;
    GuiTextButton importSsgSwEnvParamButton;
    GuiTextButton exportSsgSwEnvParamButton;
    GuiSlider targerOpSlider;
    GuiSeparator uSep005;
    GuiTextButton importLfoParamButton;
    GuiTextButton exportLfoParamButton;
    GuiTextButton importUnisonParamButton;
    GuiTextButton exportUnisonParamButton;
    GuiTextButton importQualityParamButton;
    GuiTextButton exportQualityParamButton;
    std::unique_ptr<juce::FileChooser> fileChooser;

    juce::ImageComponent algImageComp;
    std::array<juce::Image, OpnaPrValue::algorithms> algImages;

    std::array<GuiScrollGroup, OpnaPrValue::ops> opGroups;

    std::array<GuiComboBox, OpnaPrValue::ops> mul;
    std::array<GuiComboBox, OpnaPrValue::ops> dt;

    std::array<GuiCategoryLabel, OpnaPrValue::ops> ksCat;
    std::array<GuiComboBox, OpnaPrValue::ops> ks;

    std::array<GuiCategoryLabel, OpnaPrValue::ops> catSsgEnv;
    std::array<GuiComboBox, OpnaPrValue::ops> se;
    std::array<GuiSlider, OpnaPrValue::ops> seFreq;

    // Pitch ADSR
    std::array<GuiComponentPitchEnv, OpnaPrValue::ops> pitchEnv;
    // SSG SW Env
    std::array<GuiComponentSsgSwEnv, OpnaPrValue::ops> ssgSwEnv;

    std::array<GuiComponentFix, OpnaPrValue::ops> fix;
    std::array<GuiCategoryLabel, OpnaPrValue::ops> catDet;
    std::array<GuiCategoryLabel, OpnaPrValue::ops> catAmp;
    std::array<GuiCategoryLabel, OpnaPrValue::ops> catLfo;
    std::array<GuiComboBox, OpnaPrValue::ops> freqs;
    std::array<GuiSlider, OpnaPrValue::ops> syncDelay;
    std::array<GuiTextButton, OpnaPrValue::ops> syncDelayToZero;
    std::array<GuiTextButton, OpnaPrValue::ops> syncDelayToOne;
    std::array<GuiToggleButton, OpnaPrValue::ops> pm;  // OPLの vib に相当)
    std::array<GuiComboBox, OpnaPrValue::ops> pms;
    std::array<GuiToggleButton, OpnaPrValue::ops> am;  // OPMでは AMS-EN に相当)
    std::array<GuiComboBox, OpnaPrValue::ops> ams;
    std::array<GuiCategoryLabel, OpnaPrValue::ops> catN88Lfo;
    std::array<GuiSlider, OpnaPrValue::ops> n88Ams;
    std::array<GuiCategoryLabel, OpnaPrValue::ops> catMask;
    std::array<GuiToggleButton, OpnaPrValue::ops> mask;
    std::array<GuiSeparator, OpnaPrValue::ops> mmlSeparator;
    std::array<GuiMmlButton, OpnaPrValue::ops> mml;

    std::array<GuiSlider, OpnaPrValue::ops> rgAr;
    std::array<GuiSlider, OpnaPrValue::ops> rgDr;
    std::array<GuiSlider, OpnaPrValue::ops> rgSr;
    std::array<GuiSlider, OpnaPrValue::ops> rgSl;
    std::array<GuiSlider, OpnaPrValue::ops> rgRr;
    std::array<GuiSlider, OpnaPrValue::ops> rgTl;
    std::array<GuiCategoryLabel, OpnaPrValue::ops> catOptional;
    std::array<GuiToggleButton, OpnaPrValue::ops> xof;
    std::array<GuiToggleButton, OpnaPrValue::ops> kor;
    std::array<GuiToggleButton, OpnaPrValue::ops> bypass;

    void applyMmlString(const juce::String& mml, int opIndex);

    std::array<GuiEnvelopeGraph, OpnaPrValue::ops> opGraphs;
    std::array<GuiToggleButton, OpnaPrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, OpnaPrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, OpnaPrValue::ops> graphBtnSsg;
    std::array<GuiSeparator, OpnaPrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw };
    std::array<GraphMode, OpnaPrValue::ops> currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
	GuiOpna(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        presetName(context),
        qualityCat(context),
        algFbCat(context),
        levelSlider(context),
        algSelector(context),
        feedbackSlider(context),
        bitSelector(context),
        rateSelector(context),
        unisonComponent(context),
        panCat(context),
        panSlider(context),
        panToLBtn(context),
        panToCBtn(context),
        panToRBtn(context),
        lfoCat(context),
        lfoFreqSlider(context),
        lfoShapeSelector(context),
        lfoAmSmRtSlider(context),
        lfoSyncDelaySlider(context),
        lfoSyncDelayToZeroBtn(context),
        lfoSyncDelayToOneBtn(context),
        lfoPmToggle(context),
        lfoAmToggle(context),
        lfoPmdSlider(context),
        lfoPmsSlider(context),
        lfoAmdSlider(context),
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        copyHwLfoFromSlider(context),
        copyHwLfoParamsBtn(context),
        uSep002(context),
        copyParamsToOpnBtn(context),
        copyParamsToOpmBtn(context),
        uSep003(context),
        copyOpParamBtn(context),
        copyOpFromSlider(context),
        copyOpToSlider(context),
        uSep004(context),
        importHwLfoParamButton(context),
        exportHwLfoParamButton(context),
        importPitchEnvParamButton(context),
        exportPitchEnvParamButton(context),
        importSsgSwEnvParamButton(context),
        exportSsgSwEnvParamButton(context),
        targerOpSlider(context),
        uSep005(context),
        importLfoParamButton(context),
        exportLfoParamButton(context),
        importUnisonParamButton(context),
        exportUnisonParamButton(context),
        importQualityParamButton(context),
        exportQualityParamButton(context),
        opGroups{ GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context) },
        catDet{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        catAmp{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        mul{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        dt{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        ksCat{ GuiCategoryLabel(context), GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        ks{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        catSsgEnv{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        se{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        seFreq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        pitchEnv{ GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context) },
        ssgSwEnv{ GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context) },
        fix{ GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context) },
        catLfo{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        freqs{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        syncDelay{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        syncDelayToZero{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        syncDelayToOne{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
        pm{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        pms{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        am{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        ams{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        catN88Lfo{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        n88Ams{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        mmlSeparator{ GuiSeparator(context), GuiSeparator(context), GuiSeparator(context), GuiSeparator(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) },
        rgAr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgDr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgSr{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        rgSl{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
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
    void layoutN88LfoCat(juce::Rectangle<int>& rect);
    void layoutOpSsgEnvelopeCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpHwLfoCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpN88LfoCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpDetCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpAmpCat(int opIndex, juce::Rectangle<int>& rect);
    void setupGraph(int opIndex);
    void layoutOpGraph(int opIndex, juce::Rectangle<int>& rect);
    void setLevel(float level);
    void copyParams(CopyOpna& copyObj);
    void copyOpParams(int p, CopyOpnaOp& copyObj);
    void pasteParams(CopyOpna& copyObj);
    void pasteOpParams(int p, CopyOpnaOp& copyObj);
    void copyParamsOpm(CopyOpnOpm& copyObj);
    void copyParamsOpnOpm(CopyOpnaOpnOpm& copyObj);
    void copyOpParamsOpm(int p, CopyOpnOpmOp& copyObj);
    void copyOpParamsOpnOpm(int p, CopyOpnaOpnOpmOp& copyObj);
    void pasteParamsOpm(CopyOpnOpm& copyObj);
    void pasteParamsOpnOpm(CopyOpnaOpnOpm& copyObj);
    void pasteOpParamsOpm(int p, CopyOpnOpmOp& copyObj);
    void pasteOpParamsOpnOpm(int p, CopyOpnaOpnOpmOp& copyObj);
    void importHwLfoParam(int opIndex);
    void exportHwLfoParam(int opIndex);
    void importPitchEnvParam(int opIndex);
    void exportPitchEnvParam(int opIndex);
    void importSsgSwEnvParam(int opIndex);
    void exportSsgSwEnvParam(int opIndex);
    void importLfoParam();
    void exportLfoParam();
    void importUnisonParam();
    void exportUnisonParam();
    void importQualityParam();
    void exportQualityParam();
};
