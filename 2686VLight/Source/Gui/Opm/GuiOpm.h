#pragma once

#include <JuceHeader.h>

#include "../../Core/Io/ParamFile.h"
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
#include "../../Gui/Components/WavePreview/WavePreview.h"
#include "../../Gui/Components/Quality/Quality.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/AlgMatrix/GuiFmAlgRouting.h"
#include "../../Gui/Components/AmpEnv/AmpEnv.h"
#include "../../Gui/Components/WtMod/WtMod.h"
#include "../../Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "../../Gui/Components/SsgHwPEnv/SsgHwPEnv.h"
#include "../../Gui/Components/NudgeSlider/NudgeSliderFloat.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiOpm : public GuiBase
{
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiCategoryLabel algFbCat;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

    // Global
    GuiComboBox algSelector;
    // 従来のアルゴリズム図用のグラフコンポーネント (画像から置き換え)
    GuiFmAlgGraph algStaticGraphComp;
    NormalSeparator algFbSep;
    GuiFbSlider feedbackSlider;

    // SSG Hw Env
    // チップ全体へ掛かる AMP ENV
    GuiComponentAmpEnv ampEnvComponent;
    // チップ全体へ掛かる MODULATION
    GuiComponentWtMod modComponent;
    GuiComponentSsgHwEnv ssgHwEnv;
    GuiComponentSsgHwPEnv ssgHwPEnv;
    // SSG Sw Env
    GuiComponentSsgSwEnv11 ssgSwEnv11g;
    // チップ全体へ掛かるピッチ側
    GuiComponentSsgSwPEnv11 ssgSwPEnv11g;

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

    // Shape がどんな形かを見せるプレビュー
    GuiWavePreview lfoPmPreview;
    GuiWavePreview lfoAmPreview;
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
    GuiComponentImportExport ieOpSsgHwPEnv;
    GuiComponentImportExport ieOpChParam;
    GuiSlider targerOpSlider;
    NormalSeparator uSep004;
    GuiComponentImportExport ieAmpEnvG;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgHwPEnv;
    GuiComponentImportExport ieWtMod;
    GuiComponentImportExport ieSsgSwEnv11;
    GuiComponentImportExport ieSsgSwPEnv11g;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    std::array<GuiScrollGroup, OpmPrValue::ops> opGroups;

    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    std::array<GuiCategoryLabel, OpmPrValue::ops> catDet;
    std::array<GuiCategoryLabel, OpmPrValue::ops> catAmp;
    std::array<GuiComboBox, OpmPrValue::ops> mul;
    std::array<GuiComponentNudgeSliderFloat, OpmPrValue::ops> mulRatio;
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
    std::array<GuiComponentSsgHwPEnv, OpmPrValue::ops> ssgHwPEnvOp;

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

    // チップ全体のエンベロープを見るグラフ
    GuiEnvelopeGraph gGraph;
    GuiToggleButton gGraphBtnAmp;
    GuiToggleButton gGraphBtnSsg11;
    GuiToggleButton gGraphBtnSsgP11;
    NormalSeparator gGraphSeparator;

    enum class GlobalGraphMode { Amp, SsgSw11, SsgSwP11 };
    GlobalGraphMode currentGlobalGraphMode = GlobalGraphMode::Amp;

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
        ampEnvComponent(context),
        modComponent(context),
        ssgHwEnv(context),
        ssgHwPEnv(context),
        ssgSwEnv11g(context),
        ssgSwPEnv11g(context),
        unisonComponent(context),
        panCat(context),
        panSlider(context),
        panToLBtn(context),
        panToCBtn(context),
        panToRBtn(context),
        lfoCat(context),
        lfoFreqSlider(context),
        lfoAmSmRtSlider(context),
        lfoPmPreview(context),
        lfoAmPreview(context),
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
        ieOpSsgHwPEnv(context),
		ieOpChParam(context),
        targerOpSlider(context),
        uSep004(context),
        ieAmpEnvG(context),
        ieSsgHwEnv(context),
        ieSsgHwPEnv(context),
        ieWtMod(context),
        ieSsgSwEnv11(context),
        ieSsgSwPEnv11g(context),
        ieLfo(context),
        ieUnison(context),
        ieQuality(context),
        ieChParam(context),
        opGroups{ GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context) },
        catDet{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        catAmp{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        mul{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        mulRatio{ GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context) },
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
        ssgHwPEnvOp{ GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context) },
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
        gGraph(),
        gGraphBtnAmp(context),
        gGraphBtnSsg11(context),
        gGraphBtnSsgP11(context),
        gGraphSeparator(context),
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
    void updateLfoPreviews();
    void layoutOpHwLfoCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpDetCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpAmpCat(int opIndex, juce::Rectangle<int>& rect);
    void setupGraph(int opIndex);

    // チップ全体のグラフ
    void setupGlobalGraph();
    void layoutGlobalGraph(juce::Rectangle<int>& rect);
    void updateGlobalGraph();
    void setGlobalGraphMode(GlobalGraphMode mode);
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
    void importOpSsgHwPEnvParam(int opIndex);
    void exportOpSsgHwPEnvParam(int opIndex);
    void importLfoParam();

    // 3.0.0 より前の形式を読む
    void setImportingLfoParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeLfoParams(Io::ParamWriter& writer);
    void exportLfoParam();
    void importQualityParam();

    // 3.0.0 より前の形式を読む
    void setImportingQualityParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeQualityParams(Io::ParamWriter& writer);
    void exportQualityParam();
    void importChParam();

    // 3.0.0 より前の形式を読む
    void getImportingOpParams(int opIndex, juce::StringArray& lines, int& index);
    void setImportingChParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeChParams(Io::ParamWriter& writer);
    void exportChParam();
    void importOpChParam(int opIndex);

    // 3.0.0 より前の形式を読む
    void setImportingOpChFileParams(int opIndex, juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeOpChFileParams(int opIndex, Io::ParamWriter& writer);
    void exportOpChParam(int opIndex);
    // 名前で受け渡す。オペレータは並びの中のひとつを渡す。
    void readOpParams(int opIndex, const Io::ParamReader& r);
    void writeOpParams(int opIndex, Io::ParamWriter& w);

    // チャンネル 1 つぶん。
    void readChParams(const Io::ParamReader& reader);
};
