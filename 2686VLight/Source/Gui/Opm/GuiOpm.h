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
#include "../../Gui/Components/NudgeButtons/NudgeButtons.h"
#include "../../Gui/Components/NudgeSlider/NudgeSliderFloat.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"
#include "../../Gui/Components/WavePreview/WavePreview.h"
#include "../../Gui/Components/TargetCell/TargetCell.h"
#include "../../Gui/Components/Quality/Quality.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/AlgMatrix/GuiFmAlgRouting.h"
#include "../../Gui/Components/AmpEnv/AmpEnv.h"
#include "../../Gui/Components/WtMod/WtMod.h"
#include "../../Gui/Components/WtAmpMod/WtAmpMod.h"
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
    GuiComponentWtAmpMod ampModComponent;
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
    GuiComponentImportExport ieOpWtAmpMod;
    GuiComponentImportExport ieOpSsgHwEnv;
    GuiComponentImportExport ieOpWtMod;
    GuiComponentImportExport ieOpChParam;
    GuiSlider targerOpSlider;
    NormalSeparator uSep004;
    GuiComponentImportExport ieAmpEnvG;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgHwPEnv;
    GuiComponentImportExport ieWtMod;
    GuiComponentImportExport ieWtAmpMod;
    GuiComponentImportExport ieSsgSwEnv11;
    GuiComponentImportExport ieSsgSwPEnv11g;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // 今指しているオペレータの名前。TARGET の隣に出す。
    GuiLabel opNameLabel;
    NormalSeparator opTargetSeparator;

    // 区分を横へ並べる送り台。縦へは送らず、あふれたぶんは横へ送る。
    juce::Viewport stripViewport;
    juce::Component stripCanvas;

    // 区分ごとの枠。基本は 1 列 1 区分。
    GuiScrollGroup colAmp;
    GuiScrollGroup colSsgHwEnv;
    GuiScrollGroup colSsgSwEnv;
    GuiScrollGroup colSsgSwEnv11;
    GuiScrollGroup colAmpMod;
    GuiScrollGroup colPitchEnv;
    GuiScrollGroup colSsgHwPEnv;
    GuiScrollGroup colSsgSwPEnv11;
    GuiScrollGroup colMod;
    GuiScrollGroup colKs;
    GuiScrollGroup colMask;

    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    GuiCategoryLabel catDet;
    GuiCategoryLabel catAmp;
    GuiComboBox mul;
    GuiComponentNudgeSliderFloat mulRatio;
    GuiTextButton mulRatioTo001;
    GuiTextButton mulRatioTo005;
    GuiTextButton mulRatioTo1;
    GuiTextButton mulRatioTo10;
    GuiTextButton mulRatioTo2757;
    GuiTextButton mulRatioTo02;
    GuiTextButton mulRatioTo025;
    GuiTextButton mulRatioTo04;
    GuiTextButton mulRatioTo05;
    GuiTextButton mulRatioTo06;
    GuiTextButton mulRatioTo075;
    GuiTextButton mulRatioTo08;
    GuiTextButton mulRatioPM10;
    GuiTextButton mulRatioPM1;
    GuiTextButton mulRatioPM01;
    GuiTextButton mulRatioPM001;
    GuiTextButton mulRatioP001;
    GuiTextButton mulRatioP01;
    GuiTextButton mulRatioP1;
    GuiTextButton mulRatioP10;
    NormalSeparator mulDetSep;
    GuiSlider dt1;
    GuiSlider dt2;

    GuiCategoryLabel ksCat;
    GuiComboBox ksMode;
    GuiComboBox ks;
    GuiSlider ksrOPP;
    GuiSlider kslOPP;

    // Pitch ADSR
    GuiComponentPitchEnv pitchEnv;
    // SSG SW Env
    GuiComponentSsgSwEnv ssgSwEnv;

    GuiComponentSsgSwEnv11 ssgSwEnv11;
    GuiComponentSsgSwPEnv11 ssgSwPEnv11;
    GuiComponentSsgHwPEnv ssgHwPEnvOp;
    GuiComponentWtAmpMod wtAmpModOp;
    GuiComponentSsgHwEnv ssgHwEnvOp;
    GuiComponentWtMod wtModOp;

    GuiCategoryLabel cafLfo;
    GuiToggleButton amsEnable;
    GuiComponentFix fix;
    GuiCategoryLabel catMask;
    GuiToggleButton mask; // Mask
    NormalSeparator mmlSeparator;
    GuiMmlButton mml;

    GuiSlider rgAr;
    GuiSlider rgD1r;
    GuiSlider rgD2r;
    GuiSlider rgD1l;
    GuiSlider rgRr;
    GuiSlider rgTl;
    GuiCategoryLabel catOptional;
    // 押してから鳴り始めるまでの間 (秒)。OPTIONAL の一番上に置く。
    GuiComponentNudgeSliderFloat opDelay;
    GuiComponentNudgeButtons opDelayNudge;
    NormalSeparator opDelaySeparator;

    GuiToggleButton xof;
    GuiToggleButton kor;
    GuiToggleButton bypass;

    void applyMmlString(const juce::String& mml);

    // チップ全体のエンベロープを見るグラフ
    GuiEnvelopeGraph gGraph;
    GuiToggleButton gGraphBtnAmp;
    GuiToggleButton gGraphBtnSsg11;
    GuiToggleButton gGraphBtnSsgP11;
    NormalSeparator gGraphSeparator;

    enum class GlobalGraphMode { Amp, SsgSw11, SsgSwP11 };
    GlobalGraphMode currentGlobalGraphMode = GlobalGraphMode::Amp;

    // 上に並ぶ絵。オペレータの数だけ置く。
    std::array<GuiTargetCell, OpmPrValue::ops> cells;

    // どのエンベロープを映すかの切り替え。絵ごとではなく、まとめて 1 つ。
    // TARGET のすぐ右へ置く。
    GuiToggleButton graphBtnAmp;
    GuiToggleButton graphBtnPitch;
    GuiToggleButton graphBtnSsg;
    GuiToggleButton graphBtnSsg11;
    GuiToggleButton graphBtnSsgP11;

    GuiEnvGraphMode currentGraphMode = GuiEnvGraphMode::Amp;

    bool isUpdatingGraph = false;

    // アルゴリズムから出した「鳴っているオペレータ」の表。
    //
    // つまみは TARGET のぶんしか無くなったので、押せるかどうかで
    // 判じられない。鍵で番号を押したときの入り口で見る。
    std::array<bool, OpmPrValue::ops> opActive{};

    // MML の窓へ出す案内。札からも鍵からも同じものを使う。
    static inline const juce::String mmlHint =
        juce::String("") + "MML風にパラメータを入力してください。 例: AR:31 AR31 DT-1 等";

    void updateOpGraph(int opIndex);
    void setGraphMode(GuiEnvGraphMode mode);

    // TARGET が今どのオペレータを指しているか (0 から数える)。
    int currentOp() const;

    // TARGET が指すオペレータへ、設定の束縛を丸ごと移す。
    void rebind(int opIndex);

    // 指し先を切り替える。束縛と枠線の付け替えをまとめて行う。
    void applyOpTarget();

    // 指し先を一時的に動かして何かをする。
    //
    // 設定はひとそろいしか無いので、TARGET が指していないオペレータを
    // 読み書きするには、いったんそこへ繋ぎ替えるしかない。
    void withOp(int opIndex, const std::function<void()>& fn);

    // 設定ひとそろいの置き方
    void layoutOpPanel(juce::Rectangle<int> area);
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
        ampModComponent(context),
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
        ieOpWtAmpMod(context),
        ieOpSsgHwEnv(context),
        ieOpWtMod(context),
		ieOpChParam(context),
        targerOpSlider(context),
        uSep004(context),
        ieAmpEnvG(context),
        ieSsgHwEnv(context),
        ieSsgHwPEnv(context),
        ieWtMod(context),
        ieWtAmpMod(context),
        ieSsgSwEnv11(context),
        ieSsgSwPEnv11g(context),
        ieLfo(context),
        ieUnison(context),
        ieQuality(context),
        ieChParam(context),
        opNameLabel(context),
        opTargetSeparator(context),
        colAmp(context),
        colSsgHwEnv(context),
        colSsgSwEnv(context),
        colSsgSwEnv11(context),
        colAmpMod(context),
        colPitchEnv(context),
        colSsgHwPEnv(context),
        colSsgSwPEnv11(context),
        colMod(context),
        colKs(context),
        colMask(context),
        catDet(context),
        catAmp(context),
        mul(context),
        mulRatio(context),
        mulRatioTo001(context),
        mulRatioTo005(context),
        mulRatioTo1(context),
        mulRatioTo10(context),
        mulRatioTo2757(context),
        mulRatioTo02(context),
        mulRatioTo025(context),
        mulRatioTo04(context),
        mulRatioTo05(context),
        mulRatioTo06(context),
        mulRatioTo075(context),
        mulRatioTo08(context),
        mulRatioPM10(context),
        mulRatioPM1(context),
        mulRatioPM01(context),
        mulRatioPM001(context),
        mulRatioP001(context),
        mulRatioP01(context),
        mulRatioP1(context),
        mulRatioP10(context),
        mulDetSep(context),
        dt1(context),
        dt2(context),
        ksCat(context),
        ksMode(context),
        ks(context),
        ksrOPP(context),
        kslOPP(context),
        pitchEnv(context),
        ssgSwEnv(context),
        ssgSwEnv11(context),
        ssgSwPEnv11(context),
        ssgHwPEnvOp(context),
        wtAmpModOp(context),
        ssgHwEnvOp(context),
        wtModOp(context),
        cafLfo(context),
        amsEnable(context),
        fix(context),
        catMask(context),
        mask(context),
        mmlSeparator(context),
        mml(context),
        rgAr(context),
        rgD1r(context),
        rgD2r(context),
        rgD1l(context),
        rgRr(context),
        rgTl(context),
        catOptional(context),
        opDelay(context),
        opDelayNudge(context),
        opDelaySeparator(context),
        xof(context),
        kor(context),
        bypass(context),
        midiComponent(context),
        gGraph(),
        gGraphBtnAmp(context),
        gGraphBtnSsg11(context),
        gGraphBtnSsgP11(context),
        gGraphSeparator(context),
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphBtnSsg(context),
        graphBtnSsg11(context),
        graphBtnSsgP11(context),
        cells{ GuiTargetCell(context), GuiTargetCell(context), GuiTargetCell(context), GuiTargetCell(context) }
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updateOpEnable(int idx, bool enable);
    void updateAlgorithmDisplay();

    // MUL が Ratio のときだけ、比率のつまみを触れるようにする。
    void updateMulRatioEnable();
    void updateRgDisplayAsOp(bool rgMode);
    void updatePresetName(const juce::String& name);
    bool keyPressed(const juce::KeyPress& key) override;
    void copyFmParamsToString();
    void copyFmParamsToObject();
    void pasteFmParamsFromObject();
    void initParams();
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutOpMaskCat(juce::Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanCat(juce::Rectangle<int>& rect);
    void layoutHwLfoCat(juce::Rectangle<int>& rect);
    void updateLfoPreviews();
    void layoutOpHwLfoCat(juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(juce::Rectangle<int>& rect);
    void layoutOpKsCat(juce::Rectangle<int>& rect);
    void layoutOpDetCat(juce::Rectangle<int>& rect);
    void layoutOpAmpCat(juce::Rectangle<int>& rect);
    void setupOpGraphWiring();

    // チップ全体のグラフ
    void setupGlobalGraph();
    void layoutGlobalGraph(juce::Rectangle<int>& rect);
    void updateGlobalGraph();
    void setGlobalGraphMode(GlobalGraphMode mode);
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
    void importOpWtAmpModParam(int opIndex);
    void exportOpWtAmpModParam(int opIndex);
    void importOpSsgHwEnvParam(int opIndex);
    void exportOpSsgHwEnvParam(int opIndex);
    void importOpWtModParam(int opIndex);
    void exportOpWtModParam(int opIndex);
    void importLfoParam();

    // ブラウザから直に読ませるための入口。
    void applyLfoParamFile(const juce::File& file);
    // 3.0.0 より前の形式を読む
    void setImportingLfoParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeLfoParams(Io::ParamWriter& writer);
    void exportLfoParam();
    // ブラウザから直に渡せるようにした入口。
    void writeLfoParamFile(const juce::File& file);
    void importQualityParam();

    // ブラウザから直に読ませるための入口。
    void applyQualityParamFile(const juce::File& file);
    // 3.0.0 より前の形式を読む
    void setImportingQualityParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeQualityParams(Io::ParamWriter& writer);
    void exportQualityParam();
    // ブラウザから直に渡せるようにした入口。
    void writeQualityParamFile(const juce::File& file);
    void importChParam();

    // ブラウザから直に読ませるための入口。
    // importChParam() からダイアログを外したもの。
    void applyChParamFile(const juce::File& file) override;
    // 3.0.0 より前の形式を読む
    void getImportingOpParams(int opIndex, juce::StringArray& lines, int& index);
    void setImportingChParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeChParams(Io::ParamWriter& writer);
    void exportChParam();
    // ブラウザから直に渡せるようにした入口。
    void writeChParamFile(const juce::File& file);
    void importOpChParam(int opIndex);

    // ブラウザから直に読ませるための入口。
    void applyOpChParamFile(int opIndex, const juce::File& file);
    // 3.0.0 より前の形式を読む
    void setImportingOpChFileParams(int opIndex, juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeOpChFileParams(int opIndex, Io::ParamWriter& writer);
    void exportOpChParam(int opIndex);
    // ブラウザから直に渡せるようにした入口。
    void writeOpChParamFile(int opIndex, const juce::File& file);
    // 名前で受け渡す。オペレータは並びの中のひとつを渡す。
    void readOpParams(int opIndex, const Io::ParamReader& r);
    void writeOpParams(int opIndex, Io::ParamWriter& w);

    // チャンネル 1 つぶん。
    void readChParams(const Io::ParamReader& reader);
};
