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
#include "../../Gui/Curve/GuiCurve.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Processor/Opl3/ProcessorOpl3Values.h"
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
#include "../../Gui/Components/SsgHwPEnv/SsgHwPEnv.h"
#include "../../Gui/Components/NudgeSlider/NudgeSliderFloat.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiOpl3 : public GuiBase
{
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiCategoryLabel algFbCat;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

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

    GuiComponentMidi midiComponent;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiTextButton initLfoToOplBtn;
    GuiTextButton initLfoToOpllBtn;
    NormalSeparator uSep002;
    GuiTextButton copyOpParamToOplBtn;
    GuiTextButton copyOpParamToOpl12Btn;
    GuiTextButton copyOpParamToOpl34Btn;
    NormalSeparator uSep003;
    GuiTextButton copyOpParamBtn;
    GuiSlider copyOpFromSlider;
    GuiSlider copyOpToSlider;
    NormalSeparator uSep004;
    GuiComponentImportExport ieOpLfo;
    GuiComponentImportExport ieOpPitchEnv;
    GuiComponentImportExport ieOpSsgSwEnv;
    GuiComponentImportExport ieOpSsgSwEnv11;
    GuiComponentImportExport ieOpSsgSwPEnv11;
    GuiComponentImportExport ieOpSsgHwPEnv;
    GuiComponentImportExport ieOpChParam;
    GuiComponentImport imOplOpChParam;
    GuiSlider targerOpSlider;
    NormalSeparator uSep005;
    GuiComponentImportExport ieAmpEnvG;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgHwPEnv;
    GuiComponentImportExport ieWtMod;
    GuiComponentImportExport ieSsgSwEnv11;
    GuiComponentImportExport ieSsgSwPEnv11g;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    GuiComponentImport imOplChParam;
    GuiComponentImport imOplChAllOpParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    std::array<GuiScrollGroup, Opl3PrValue::ops> opGroups;
    std::array<GuiCategoryLabel, Opl3PrValue::ops> catDet;
    std::array<GuiCategoryLabel, Opl3PrValue::ops> catAmp;
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

    std::array<GuiComponentSsgSwEnv11, Opl3PrValue::ops> ssgSwEnv11;
    std::array<GuiComponentSsgSwPEnv11, Opl3PrValue::ops> ssgSwPEnv11;
    std::array<GuiComponentSsgHwPEnv, Opl3PrValue::ops> ssgHwPEnvOp;

    std::array<GuiCategoryLabel, Opl3PrValue::ops> catShape;
    std::array<GuiComboBox, Opl3PrValue::ops> eg; // Envlope Generator
    std::array<GuiCategoryLabel, Opl3PrValue::ops> catMask;
    std::array<GuiToggleButton, Opl3PrValue::ops> mask; // Mask
    std::array<NormalSeparator, Opl3PrValue::ops> mmlSeparator;
    std::array<GuiMmlButton, Opl3PrValue::ops> mml;
    std::array<NormalSeparator, Opl3PrValue::ops> lfoSep;
    std::array<GuiComponentNudgeSliderFloat, Opl3PrValue::ops> ams;
    std::array<GuiComponentNudgeSliderFloat, Opl3PrValue::ops> amd;
    std::array<GuiComponentNudgeSliderFloat, Opl3PrValue::ops> pms;
    std::array<GuiComponentNudgeSliderFloat, Opl3PrValue::ops> pmd;
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

    // チップ全体のエンベロープを見るグラフ
    GuiEnvelopeGraph gGraph;
    GuiToggleButton gGraphBtnAmp;
    GuiToggleButton gGraphBtnSsg11;
    GuiToggleButton gGraphBtnSsgP11;
    NormalSeparator gGraphSeparator;

    enum class GlobalGraphMode { Amp, SsgSw11, SsgSwP11 };
    GlobalGraphMode currentGlobalGraphMode = GlobalGraphMode::Amp;

    std::array<GuiEnvelopeGraph, Opl3PrValue::ops> opGraphs;
    std::array<GuiToggleButton, Opl3PrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, Opl3PrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, Opl3PrValue::ops> graphBtnSsg;
    std::array<GuiToggleButton, Opl3PrValue::ops> graphBtnSsg11;
    std::array<GuiToggleButton, Opl3PrValue::ops> graphBtnSsgP11;
    std::array<NormalSeparator, Opl3PrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };
    std::array<GraphMode, Opl3PrValue::ops> currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    bool isUpdatingGraph = false;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
    GuiOpl3(const GuiContext& context) :
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
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        initLfoToOplBtn(context),
        initLfoToOpllBtn(context),
        uSep002(context),
        copyOpParamToOplBtn(context),
        copyOpParamToOpl12Btn(context),
        copyOpParamToOpl34Btn(context),
        uSep003(context),
        copyOpParamBtn(context),
        copyOpFromSlider(context),
        copyOpToSlider(context),
        uSep004(context),
        ieOpLfo(context),
        ieOpPitchEnv(context),
        ieOpSsgSwEnv(context),
        ieOpSsgSwEnv11(context),
        ieOpSsgSwPEnv11(context),
        ieOpSsgHwPEnv(context),
        ieOpChParam(context),
		imOplOpChParam(context),
        targerOpSlider(context),
        uSep005(context),
        ieAmpEnvG(context),
        ieSsgHwEnv(context),
        ieSsgHwPEnv(context),
        ieWtMod(context),
        ieSsgSwEnv11(context),
        ieSsgSwPEnv11g(context),
        ieUnison(context),
        ieQuality(context),
        ieChParam(context),
		imOplChParam(context),
        imOplChAllOpParam(context),
        opGroups{ GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context), GuiScrollGroup(context) },
        catLfo{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        catDet{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        catAmp{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mul{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        am{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        vib{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        egType{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        ksCat{ GuiCategoryLabel(context), GuiCategoryLabel(context),GuiCategoryLabel(context),GuiCategoryLabel(context) },
        ksr{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        ksl{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        pitchEnv{ GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context), GuiComponentPitchEnv(context) },
        ssgSwEnv{ GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context) },
		ssgSwEnv11{ GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context) },
        ssgSwPEnv11{ GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context) },
        ssgHwPEnvOp{ GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context), GuiComponentSsgHwPEnv(context) },
        catShape{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        eg{ GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context),GuiToggleButton(context) },
        mmlSeparator{ NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context),GuiMmlButton(context) },
        lfoSep{ NormalSeparator(context), NormalSeparator(context), NormalSeparator(context), NormalSeparator(context) },
        ams{ GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context) },
        amd{ GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context) },
        pms{ GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context) },
        pmd{ GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context) },
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
    void layoutOpLfoCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpDetCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpAmpCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutOpEgCat(int opIndex, juce::Rectangle<int>& rect);
    void setupGraph(int opIndex);

    // チップ全体のグラフ
    void setupGlobalGraph();
    void layoutGlobalGraph(juce::Rectangle<int>& rect);
    void updateGlobalGraph();
    void setGlobalGraphMode(GlobalGraphMode mode);
    void layoutOpGraph(int opIndex, juce::Rectangle<int>& rect);
    void setLevel(float level);
    void copyParams(CopyOpl3& copyObj);
    void copyOpParams(int p, CopyOpl3Op& copyObj);
    void pasteParams(CopyOpl3& copyObj);
    void pasteOpParams(int p, CopyOpl3Op& copyObj);
    void importLfoParam(int opIndex);

    // 3.0.0 より前の形式を読む
    void setImportingLfoParams(int opIndex, juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeLfoParams(int opIndex, Io::ParamWriter& writer);
    void exportLfoParam(int opIndex);
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

    // チャンネル 1 つぶん。相手の音源のファイルを読むときにも使う。
    void readChParams(const Io::ParamReader& reader);
    void importOplChParam();
    void importOplChAllOpParam();
    void importOplOpChParam(int opIndex);
};
