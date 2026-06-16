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
#include "../../Processor/Opn/ProcessorOpnValues.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiOpn : public GuiBase
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
    static inline const std::array<std::array<juce::String, OpnPrValue::ops>, OpnPrValue::algorithms> algOpPrefix = { {
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

    GuiSlider levelSlider;

    GuiCategoryLabel qualityCat;
    GuiCategoryLabel algFbCat;

    GuiComboBox algSelector;
    GuiFbSlider feedbackSlider;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

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

    // プリセット名ラベル
    GuiLabel presetNameLabel;
    GuiSeparator presetNameSeparator;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    GuiSeparator uSep001;
    GuiTextButton copyParamsToOpnaBtn;

    juce::ImageComponent algImageComp;
    std::array<juce::Image, OpnPrValue::algorithms> algImages;

    std::array<GuiComboBox, OpnPrValue::ops> mul;
    std::array<GuiComboBox, OpnPrValue::ops> dt;

    std::array<GuiCategoryLabel, OpnPrValue::ops> ksCat;
    std::array<GuiComboBox, OpnPrValue::ops> ks; // Key Scale (0-3)

    std::array<GuiScrollGroup, OpnPrValue::ops> opGroups;
    // Pitch ADSR
    std::array<GuiComponentPitchEnv, OpnPrValue::ops> pitchEnv;
    // SSG SW Env
    std::array<GuiComponentSsgSwEnv, OpnPrValue::ops> ssgSwEnv;

    std::array<GuiCategoryLabel, OpnPrValue::ops> catShape;
    std::array<GuiComboBox, OpnPrValue::ops> se; // SSG-EG Shape Selector
    std::array<GuiSlider, OpnPrValue::ops> seFreq;
    std::array<GuiComponentFix, OpnPrValue::ops> fix;
    std::array<GuiCategoryLabel, OpnPrValue::ops> catN88Lfo;
    std::array<GuiSlider, OpnPrValue::ops> n88Ams;
    std::array<GuiCategoryLabel, OpnPrValue::ops> catMask;
    std::array<GuiToggleButton, OpnPrValue::ops> mask; // Mask
    std::array<GuiSeparator, OpnPrValue::ops> mmlSeparator;
    std::array<GuiMmlButton, OpnPrValue::ops> mml;

    std::array<GuiSlider, OpnPrValue::ops> rgAr;
    std::array<GuiSlider, OpnPrValue::ops> rgDr;
    std::array<GuiSlider, OpnPrValue::ops> rgSr;
    std::array<GuiSlider, OpnPrValue::ops> rgSl;
    std::array<GuiSlider, OpnPrValue::ops> rgRr;
    std::array<GuiSlider, OpnPrValue::ops> rgTl;
    std::array<GuiCategoryLabel, OpnPrValue::ops> catOptional;
    std::array<GuiToggleButton, OpnPrValue::ops> xof;
    std::array<GuiToggleButton, OpnPrValue::ops> kor;
    std::array<GuiToggleButton, OpnPrValue::ops> bypass;

    void applyMmlString(const juce::String& mml, int opIndex);

    std::array<GuiEnvelopeGraph, OpnPrValue::ops> opGraphs;
    std::array<GuiToggleButton, OpnPrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, OpnPrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, OpnPrValue::ops> graphBtnSsg;
    std::array<GuiSeparator, OpnPrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw };
    std::array<GraphMode, OpnPrValue::ops> currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
	GuiOpn(const GuiContext& context) :
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
        copyParamsToOpnaBtn(context),
        opGroups{ GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context) },
        mul{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        dt{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        ksCat{ GuiCategoryLabel(context), GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        ks{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        pitchEnv{ GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context) },
        ssgSwEnv{ GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context) },
        catShape{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        se{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        seFreq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        fix{ GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context) },
        catN88Lfo{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        n88Ams{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
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
    void layoutN88LfoCat(juce::Rectangle<int>& rect);
    void layoutOpN88LfoCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect);
    void setupGraph(int opIndex);
    void layoutOpGraph(int opIndex, juce::Rectangle<int>& rect);
    void setLevel(float level);
    void copyParams(CopyOpn& copyObj);
    void copyOpParams(int p, CopyOpnOp& copyObj);
    void pasteParams(CopyOpn& copyObj);
    void pasteOpParams(int p, CopyOpnOp& copyObj);
};
