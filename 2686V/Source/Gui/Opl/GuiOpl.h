#pragma once

#include <JuceHeader.h>

#include "../../Core/Io/ParamFile.h"
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
#include "../../Gui/Components/NudgeSlider/NudgeSliderFloat.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiOpl : public GuiBase
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

    // チップ全体へ掛かる AMP ENV
    GuiComponentAmpEnv ampEnvComponent;
    // チップ全体へ掛かる MODULATION
    GuiComponentWtMod modComponent;
    // SSG Hw Env
    GuiComponentSsgHwEnv ssgHwEnv;
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
    GuiTextButton copyOpParamToOpl3Btn;
    GuiTextButton copyOpParamToOpl312Btn;
    GuiTextButton copyOpParamToOpl334Btn;
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
    GuiComponentImportExport ieOpChParam;
    GuiComponentImport imOpl3OpChParam;
    GuiSlider targerOpSlider;
    NormalSeparator uSep005;
    GuiComponentImportExport ieAmpEnvG;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieWtMod;
    GuiComponentImportExport ieSsgSwEnv11;
    GuiComponentImportExport ieSsgSwPEnv11g;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    GuiComponentImport imOpl3ChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    std::array<GuiScrollGroup, OplPrValue::ops> opGroups;

    std::array<GuiCategoryLabel, OplPrValue::ops> catDet;
    std::array<GuiCategoryLabel, OplPrValue::ops> catAmp;
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

    std::array<GuiComponentSsgSwEnv11, OplPrValue::ops> ssgSwEnv11;
    std::array<GuiComponentSsgSwPEnv11, OplPrValue::ops> ssgSwPEnv11;

    std::array<GuiCategoryLabel, OplPrValue::ops> catShape;
    std::array<GuiComboBox, OplPrValue::ops> eg; // Envlope Generator
    std::array<GuiCategoryLabel, OplPrValue::ops> catMask;
    std::array<GuiToggleButton, OplPrValue::ops> mask; // Mask
    std::array<NormalSeparator, OplPrValue::ops> mmlSeparator;
    std::array<GuiMmlButton, OplPrValue::ops> mml;
	std::array<NormalSeparator, OplPrValue::ops> lfoSep;
    std::array<GuiComponentNudgeSliderFloat, OplPrValue::ops> ams;
    std::array<GuiComponentNudgeSliderFloat, OplPrValue::ops> amd;
    std::array<GuiComponentNudgeSliderFloat, OplPrValue::ops> pms;
    std::array<GuiComponentNudgeSliderFloat, OplPrValue::ops> pmd;
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
    std::array<GuiToggleButton, OplPrValue::ops> sus;
    std::array<GuiCategoryLabel, OplPrValue::ops> catOptional;
    std::array<GuiToggleButton, OplPrValue::ops> xof;
    std::array<GuiToggleButton, OplPrValue::ops> kor;
    std::array<GuiToggleButton, OplPrValue::ops> bypass;

    void applyMmlString(const juce::String& mml, int opIndex);

    // チップ全体のエンベロープを見るグラフ
    GuiEnvelopeGraph gGraph;
    GuiToggleButton gGraphBtnAmp;
    GuiToggleButton gGraphBtnSsg11;
    GuiToggleButton gGraphBtnSsgP11;
    NormalSeparator gGraphSeparator;

    enum class GlobalGraphMode { Amp, SsgSw11, SsgSwP11 };
    GlobalGraphMode currentGlobalGraphMode = GlobalGraphMode::Amp;

    std::array<GuiEnvelopeGraph, OplPrValue::ops> opGraphs;
    std::array<GuiToggleButton, OplPrValue::ops> graphBtnAmp;
    std::array<GuiToggleButton, OplPrValue::ops> graphBtnPitch;
    std::array<GuiToggleButton, OplPrValue::ops> graphBtnSsg;
    std::array<GuiToggleButton, OplPrValue::ops> graphBtnSsg11;
    std::array<GuiToggleButton, OplPrValue::ops> graphBtnSsgP11;
    std::array<NormalSeparator, OplPrValue::ops> graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };
    std::array<GraphMode, OplPrValue::ops> currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    bool isUpdatingGraph = false;

    void updateOpGraph(int opIndex);
    void setGraphMode(int opIndex, GraphMode mode);
public:
    GuiOpl(const GuiContext& context) :
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
        ieOpLfo(context),
        ieOpPitchEnv(context),
        ieOpSsgSwEnv(context),
        ieOpSsgSwEnv11(context),
        ieOpSsgSwPEnv11(context),
		ieOpChParam(context),
		imOpl3OpChParam(context),
        targerOpSlider(context),
        uSep005(context),
        ieAmpEnvG(context),
        ieSsgHwEnv(context),
        ieWtMod(context),
        ieSsgSwEnv11(context),
        ieSsgSwPEnv11g(context),
        ieUnison(context),
        ieQuality(context),
        ieChParam(context),
        imOpl3ChParam(context),
        opGroups{ GuiScrollGroup(context), GuiScrollGroup(context) },
        catLfo{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        catDet{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        catAmp{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mul{ GuiComboBox(context), GuiComboBox(context) },
        am{ GuiToggleButton(context),GuiToggleButton(context) },
        vib{ GuiToggleButton(context),GuiToggleButton(context) },
        egType{ GuiToggleButton(context),GuiToggleButton(context) },
        ksCat{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        ksr{ GuiToggleButton(context),GuiToggleButton(context) },
        ksl{ GuiComboBox(context), GuiComboBox(context) },
        pitchEnv{ GuiComponentPitchEnv(context), GuiComponentPitchEnv(context) },
        ssgSwEnv{ GuiComponentSsgSwEnv(context), GuiComponentSsgSwEnv(context) },
        ssgSwEnv11{ GuiComponentSsgSwEnv11(context), GuiComponentSsgSwEnv11(context) },
        ssgSwPEnv11{ GuiComponentSsgSwPEnv11(context), GuiComponentSsgSwPEnv11(context) },
        catShape{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        eg{ GuiComboBox(context), GuiComboBox(context) },
        catMask{ GuiCategoryLabel(context), GuiCategoryLabel(context) },
        mask{ GuiToggleButton(context),GuiToggleButton(context) },
        mmlSeparator{ NormalSeparator(context), NormalSeparator(context) },
        mml{ GuiMmlButton(context),GuiMmlButton(context) },
        lfoSep{ NormalSeparator(context), NormalSeparator(context) },
        ams{ GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context) },
        amd{ GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context) },
        pms{ GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context) },
        pmd{ GuiComponentNudgeSliderFloat(context), GuiComponentNudgeSliderFloat(context) },
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
        sus{ GuiToggleButton(context),GuiToggleButton(context) },
        kor{ GuiToggleButton(context),GuiToggleButton(context) },
        midiComponent(context),
        gGraph(),
        gGraphBtnAmp(context),
        gGraphBtnSsg11(context),
        gGraphBtnSsgP11(context),
        gGraphSeparator(context),
        graphBtnAmp{ GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnPitch{ GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnSsg{ GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnSsg11{ GuiToggleButton(context), GuiToggleButton(context) },
        graphBtnSsgP11{ GuiToggleButton(context), GuiToggleButton(context) },
        graphSeparator{ NormalSeparator(context), NormalSeparator(context) }
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
    void layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
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
    void importSsgSwEnv11Param(int opIndex);
    void exportSsgSwEnv11Param(int opIndex);
    void importSsgSwPEnv11Param(int opIndex);
    void exportSsgSwPEnv11Param(int opIndex);
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
    void exportOpChParam(int opIndex);
    // 名前で受け渡す。オペレータは並びの中のひとつを渡す。
    void readOpParams(int opIndex, const Io::ParamReader& r);
    void writeOpParams(int opIndex, Io::ParamWriter& w);

    // チャンネル 1 つぶん。相手の音源のファイルを読むときにも使う。
    void readChParams(const Io::ParamReader& reader);
    void importOpl3ChParam();
    void importOpl3OpChParam(int opIndex);
};
