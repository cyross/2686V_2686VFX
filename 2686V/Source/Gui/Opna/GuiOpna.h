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
#include "../../Gui/Components/Import/Import.h"
#include "../../Gui/Components/Level/Level.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"
#include "../../Gui/Components/Quality/Quality.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/AlgMatrix/GuiFmAlgRouting.h"
#include "../../Gui/Components/AmpEnv/AmpEnv.h"
#include "../../Gui/Components/WtMod/WtMod.h"
#include "../../Gui/Components/SsgHwEnv/SsgHwEnv.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiOpna : public GuiBase
{
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

    GuiCategoryLabel algFbCat;

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
    NormalSeparator lfoSep1;
    NormalSeparator lfoSep2;

    GuiComponentMidi midiComponent;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiSlider copyHwLfoFromSlider;
    GuiTextButton copyHwLfoParamsBtn;
    NormalSeparator uSep002;
    GuiTextButton copyParamsToOpnBtn;
    GuiTextButton copyParamsToOpmBtn;
    NormalSeparator uSep003;
    GuiTextButton copyOpParamBtn;
    GuiSlider copyOpFromSlider;
    GuiSlider copyOpToSlider;
    NormalSeparator uSep004;

    GuiComponentImportExport ieHwLfo;
    GuiComponentImportExport ieOpPitchEnv;
    GuiComponentImportExport ieOpSsgSwEnv;
    GuiComponentImportExport ieOpSsgSwEnv11;
    GuiComponentImportExport ieOpSsgSwPEnv11;
    GuiComponentImportExport ieOpChParam;
    GuiComponentImport imOpnOpChParam;
    GuiSlider targerOpSlider;
    NormalSeparator uSep005;
    GuiComponentImportExport ieAmpEnvG;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieWtMod;
    GuiComponentImportExport ieSsgSwEnv11;
    GuiComponentImportExport ieSsgSwPEnv11g;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    GuiComponentImport imOpnChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

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

    std::array<GuiComponentSsgSwEnv11, OpnaPrValue::ops> ssgSwEnv11;
    std::array<GuiComponentSsgSwPEnv11, OpnaPrValue::ops> ssgSwPEnv11;

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
    std::array<NormalSeparator, OpnaPrValue::ops> mmlSeparator;
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

    // チップ全体のエンベロープを見るグラフ
    GuiEnvelopeGraph gGraph;
    GuiToggleButton gGraphBtnAmp;
    GuiToggleButton gGraphBtnSsg11;
    GuiToggleButton gGraphBtnSsgP11;
    NormalSeparator gGraphSeparator;

    enum class GlobalGraphMode { Amp, SsgSw11, SsgSwP11 };
    GlobalGraphMode currentGlobalGraphMode = GlobalGraphMode::Amp;

    std::array<GuiEnvelopeGraph, OpnaPrValue::ops> opGraphs;
    std::array<GuiToggleButton, OpnaPrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, OpnaPrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, OpnaPrValue::ops> graphBtnSsg;
    std::array<GuiToggleButton, OpnaPrValue::ops> graphBtnSsg11;
    std::array<GuiToggleButton, OpnaPrValue::ops> graphBtnSsgP11;
    std::array<NormalSeparator, OpnaPrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };
    std::array<GraphMode, OpnaPrValue::ops> currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    bool isUpdatingGraph = false;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
	GuiOpna(const GuiContext& context) :
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
        lfoSep1(context),
        lfoSep2(context),
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
        ieHwLfo(context),
        ieOpPitchEnv(context),
        ieOpSsgSwEnv(context),
        ieOpSsgSwEnv11(context),
        ieOpSsgSwPEnv11(context),
        ieOpChParam(context),
		imOpnOpChParam(context),
        targerOpSlider(context),
        uSep005(context),
        ieAmpEnvG(context),
        ieSsgHwEnv(context),
        ieWtMod(context),
        ieSsgSwEnv11(context),
        ieSsgSwPEnv11g(context),
        ieLfo(context),
        ieUnison(context),
        ieQuality(context),
        ieChParam(context),
		imOpnChParam(context),
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
        ssgSwEnv11{ GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context) },
        ssgSwPEnv11{ GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context) },
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
        mmlSeparator{ NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context) },
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
    void layoutN88LfoCat(juce::Rectangle<int>& rect);
    void layoutOpSsgEnvelopeCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpHwLfoCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpN88LfoCat(int opIndex, juce::Rectangle<int>& rect);
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
    void importSsgSwEnv11Param(int opIndex);
    void exportSsgSwEnv11Param(int opIndex);
    void importSsgSwPEnv11Param(int opIndex);
    void exportSsgSwPEnv11Param(int opIndex);
    void importLfoParam();
    void exportLfoParam();
    void importQualityParam();
    void exportQualityParam();
    void importChParam();
    void exportChParam();
    void importOpChParam(int opIndex);
    void exportOpChParam(int opIndex);
    void getImportingOpParams(int opIndex, juce::StringArray& lines, int& index);
    juce::String setExportedOpParams(int opIndex);
    void importOpnChParam();
    void importOpnOpChParam(int opIndex);
    void getImportingOpnOpParams(int opIndex, juce::StringArray& lines, int& index);
};
