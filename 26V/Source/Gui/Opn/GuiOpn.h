#pragma once

#include <JuceHeader.h>

#include "../../Core/Io/ParamFile.h"
#include <array>

#include "../../Core/Const/ConstGlobal.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/Fix/Fix.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"
#include "../../Gui/Components/PresetName/PresetName.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"
#include "../../Gui/Components/Import/Import.h"
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

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiOpn : public GuiBase
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
    GuiComponentSsgHwPEnv ssgHwPEnv;
    // SSG Sw Env
    GuiComponentSsgSwEnv11 ssgSwEnv11g;
    // チップ全体へ掛かるピッチ側
    GuiComponentSsgSwPEnv11 ssgSwPEnv11g;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    GuiCategoryLabel lfoCat;

    GuiSlider lfoFreqSlider;
    GuiComboBox lfoShapeSelector;
    GuiSlider lfoAmSmRtSlider;

    // Shape がどんな形かを見せるプレビュー
    GuiWavePreview lfoPmPreview;
    GuiWavePreview lfoAmPreview;
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
    GuiComponentImport imOpnaOpChParam;
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
    GuiComponentImport imOpnaChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    std::array<GuiCategoryLabel, OpnPrValue::ops> catDet;
    std::array<GuiCategoryLabel, OpnPrValue::ops> catAmp;
    std::array<GuiComboBox, OpnPrValue::ops> mul;
    std::array<GuiComboBox, OpnPrValue::ops> dt;

    std::array<GuiCategoryLabel, OpnPrValue::ops> ksCat;
    std::array<GuiComboBox, OpnPrValue::ops> ks; // Key Scale (0-3)

    std::array<GuiScrollGroup, OpnPrValue::ops> opGroups;
    // Pitch ADSR
    std::array<GuiComponentPitchEnv, OpnPrValue::ops> pitchEnv;
    // SSG SW Env
    std::array<GuiComponentSsgSwEnv, OpnPrValue::ops> ssgSwEnv;

    std::array<GuiComponentSsgSwEnv11, OpnPrValue::ops> ssgSwEnv11;
    std::array<GuiComponentSsgSwPEnv11, OpnPrValue::ops> ssgSwPEnv11;
    std::array<GuiComponentSsgHwPEnv, OpnPrValue::ops> ssgHwPEnvOp;

    std::array<GuiCategoryLabel, OpnPrValue::ops> catShape;
    std::array<GuiComboBox, OpnPrValue::ops> se; // SSG-EG Shape Selector
    std::array<GuiSlider, OpnPrValue::ops> seFreq;
    std::array<GuiComponentFix, OpnPrValue::ops> fix;
    std::array<GuiCategoryLabel, OpnPrValue::ops> catN88Lfo;
    std::array<GuiSlider, OpnPrValue::ops> n88Ams;
    std::array<GuiCategoryLabel, OpnPrValue::ops> catMask;
    std::array<GuiToggleButton, OpnPrValue::ops> mask; // Mask
    std::array<NormalSeparator, OpnPrValue::ops> mmlSeparator;
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

    // チップ全体のエンベロープを見るグラフ
    GuiEnvelopeGraph gGraph;
    GuiToggleButton gGraphBtnAmp;
    GuiToggleButton gGraphBtnSsg11;
    GuiToggleButton gGraphBtnSsgP11;
    NormalSeparator gGraphSeparator;

    enum class GlobalGraphMode { Amp, SsgSw11, SsgSwP11 };
    GlobalGraphMode currentGlobalGraphMode = GlobalGraphMode::Amp;

    std::array<GuiEnvelopeGraph, OpnPrValue::ops> opGraphs;
    std::array<GuiToggleButton, OpnPrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, OpnPrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, OpnPrValue::ops> graphBtnSsg;
    std::array<GuiToggleButton, OpnPrValue::ops> graphBtnSsg11;
    std::array<GuiToggleButton, OpnPrValue::ops> graphBtnSsgP11;
    std::array<NormalSeparator, OpnPrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };
    std::array<GraphMode, OpnPrValue::ops> currentGraphMode;

    bool isUpdatingGraph = false;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
	GuiOpn(const GuiContext& context) :
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
        lfoCat(context),
        lfoFreqSlider(context),
        lfoShapeSelector(context),
        lfoAmSmRtSlider(context),
        lfoPmPreview(context),
        lfoAmPreview(context),
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
		imOpnaOpChParam(context),
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
		imOpnaChParam(context),
        opGroups{ GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context) },
        catDet{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        catAmp{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        mul{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        dt{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        ksCat{ GuiCategoryLabel(context), GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        ks{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        pitchEnv{ GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context) },
        ssgSwEnv{ GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context) },
        ssgSwEnv11{ GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context) },
        ssgSwPEnv11{ GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context) },
        ssgHwPEnvOp{ GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context) },
        catShape{ GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        se{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        seFreq{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        fix{ GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context),GuiComponentFix(context) },
        catN88Lfo{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        n88Ams{ GuiSlider(context), GuiSlider(context), GuiSlider(context), GuiSlider(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
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
    void layoutN88LfoCat(juce::Rectangle<int>& rect);
    void updateLfoPreviews();
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
    void copyParams(CopyOpn& copyObj);
    void copyOpParams(int p, CopyOpnOp& copyObj);
    void pasteParams(CopyOpn& copyObj);
    void pasteOpParams(int p, CopyOpnOp& copyObj);
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

    // チャンネル 1 つぶん。OPNA のファイルを読むときにも使う。
    void readChParams(const Io::ParamReader& reader);
    void importOpnaChParam();
    void importOpnaOpChParam(int opIndex);
};
