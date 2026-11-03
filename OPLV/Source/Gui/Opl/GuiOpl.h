#pragma once

#include "Shared/Core/Gui/GuiI18n.h"
#include <JuceHeader.h>

#include "Shared/Gui/Components/TapSeq/TapSeq.h"
#include "Shared/Gui/Components/TapParam/TapParam.h"
#include "Shared/Gui/Components/FmToOpzx7/FmToOpzx7.h"

#include "Shared/Core/Io/ParamFile.h"
#include <array>

#include "Shared/Core/Const/ConstGlobal.h"
#include "Shared/Core/Gui/GuiValues.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiGroupFolds.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "Shared/Advanced/Curve/AdvancedCurve.h"
#include "Shared/Gui/Components/Unison/Unison.h"
#include "Shared/Gui/Components/PitchEnv/PitchEnv.h"
#include "Shared/Gui/Components/Midi/Midi.h"
#include "Shared/Gui/Components/Edit/Edit.h"
#include "Shared/Processor/Opl/ProcessorOplValues.h"
#include "Shared/Gui/Components/PresetName/PresetName.h"
#include "Shared/Gui/Components/ImportExport/ImportExport.h"
#include "Shared/Gui/Components/Import/Import.h"
#include "Shared/Gui/Components/Level/Level.h"
#include "Shared/Gui/Components/NudgeButtons/NudgeButtons.h"
#include "Shared/Gui/Components/NudgeSlider/NudgeSliderFloat.h"
#include "Shared/Gui/Components/Separator/NormalSeparator.h"
#include "Shared/Gui/Components/Separator/ShortSeparator.h"
#include "Shared/Gui/Components/TargetCell/TargetCell.h"
#include "Shared/Gui/Components/Quality/Quality.h"
#include "Shared/Gui/Components/SsgSwEnv256/SsgSwEnv256.h"
#include "Shared/Gui/Components/SsgSwPEnv256/SsgSwPEnv256.h"
#include "Shared/Gui/Components/SsgSwPEnv256/SsgSwPEnv256.h"
#include "Shared/Gui/Components/AlgMatrix/GuiFmAlgRouting.h"
#include "Shared/Gui/Components/AmpEnv/AmpEnv.h"
#include "Shared/Gui/Components/WtMod/WtMod.h"
#include "Shared/Gui/Components/WtAmpMod/WtAmpMod.h"
#include "Shared/Gui/Components/RatioEnv/RatioEnv.h"
#include "Shared/Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "Shared/Gui/Components/SsgHwPEnv/SsgHwPEnv.h"
#include "Shared/Gui/Components/NudgeSlider/NudgeSliderFloat.h"

#include "Shared/Core/Gui/GuiCopyObj.h"

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
    GuiComponentWtAmpMod ampModComponent;
    // SSG Hw Env
    GuiComponentSsgHwEnv ssgHwEnv;
    GuiComponentSsgHwPEnv ssgHwPEnv;
    // SSG Sw Env
    GuiComponentSsgSwEnv256 ssgSwEnv256g;
    // チップ全体へ掛かるピッチ側
    GuiComponentSsgSwPEnv256 ssgSwPEnv256g;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    GuiComponentMidi midiComponent;

    // EDIT 区分 (切り取り・コピー・貼り付け・初期化)
    GuiComponentEdit editComponent;

    // オペレーター 1 つぶんの EDIT 区分 (MASK の下)
    GuiComponentUnitEdit opEditComponent;

    GuiCategoryLabel utilityCat;

    // 大区分。[[AMP ENV]] と [[PITCH ENV]]
    GuiCategoryLabel ampMajorCat;
    GuiCategoryLabel pitchMajorCat;

    // [[TAP SEQUENCER]]。[[PITCH ENV]] の下 (RHYTHM はグローバルの UNISON の下)
    GuiCategoryLabel tapMajorCat;
    GuiComponentTapSeq tapAmpSeq;
    GuiComponentTapSeq tapPitchSeq;

    // TAP PARAM SEQUENCER と、チャンネルの PARAM ENV[256] (4.3.0)
    GuiComponentTapParam tapParamSeq;
    GuiCategoryLabel paramEnvMajorCat;
    GuiComponentParamEnv paramEnv;

    // OPZX7S のパラメータファイルへ書き出す
    GuiTextButton exportOpzx7Btn;

    void exportOpzx7Params();
    GuiTextButton broadcastLevelButton;
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
    GuiComponentImportExport ieOpLfo;
    GuiComponentImportExport ieOpPitchEnv;
    GuiComponentImportExport ieOpSsgSwEnv256;
    GuiComponentImportExport ieOpSsgSwPEnv256;
    GuiComponentImportExport ieOpSsgHwPEnv;
    GuiComponentImportExport ieOpWtAmpMod;
    GuiComponentImportExport ieOpSsgHwEnv;
    GuiComponentImportExport ieOpWtMod;
    GuiComponentImportExport ieOpChParam;
    GuiComponentImport imOpl3OpChParam;
    GuiSlider targerOpSlider;
    NormalSeparator uSep005;
    GuiComponentImportExport ieAmpEnvG;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgHwPEnv;
    GuiComponentImportExport ieWtMod;
    GuiComponentImportExport ieWtAmpMod;
    GuiComponentImportExport ieSsgSwEnv256;
    GuiComponentImportExport ieSsgSwPEnv256g;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    GuiComponentImport imOpl3ChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // 今指しているオペレータの名前。TARGET の隣に出す。
    GuiLabel opNameLabel;
    NormalSeparator opTargetSeparator;

    // 一番上の帯の背後へ敷く板。壁紙の上でも読めるようにする
    GuiRowBoard opTargetBoard;

    // 区分を横へ並べる送り台。縦へは送らず、あふれたぶんは横へ送る。
    juce::Viewport stripViewport;
    juce::Component stripCanvas;

    // 区分ごとの枠。基本は 1 列 1 区分。
    GuiScrollGroup colAmp;
    GuiScrollGroup colSsgHwEnv;
    GuiScrollGroup colSsgSwEnv256;
    GuiScrollGroup colAmpMod;
    GuiScrollGroup colPitchEnv;
    GuiScrollGroup colSsgHwPEnv;
    GuiScrollGroup colSsgSwPEnv256;
    GuiScrollGroup colMod;
    GuiScrollGroup colKs;

    // PARAM ENV[256] (4.3.0)。KEY SCALE / MUL/DET / FIX の右に、独立した列で置く
    GuiScrollGroup colParamEnv;
    GuiScrollGroup colTapParam;
    GuiScrollGroup colMask;

    GuiCategoryLabel catDet;
    GuiCategoryLabel catAmp;
    GuiCategoryLabel catLfo;
    GuiComboBox mul;
    GuiToggleButton am;
    GuiToggleButton vib;
    GuiToggleButton egType;

    GuiCategoryLabel ksCat;
    GuiToggleButton ksr;
    GuiComboBox ksl; // Key Scale Level

    // Pitch ADSR
    GuiComponentPitchEnv pitchEnv;
    // SSG SW Env

    GuiComponentSsgSwEnv256 ssgSwEnv256;
    GuiComponentSsgSwPEnv256 ssgSwPEnv256;
    GuiComponentSsgHwPEnv ssgHwPEnvOp;
    GuiComponentWtAmpMod wtAmpModOp;

    // TL ENV。オペレーターの AMP ENV 区分のすぐ下 (同じ列)
    // PARAM ENV[256] (オペレーター)。4.0.0 の TL ENV の代わり。TARGET の初期値は TL
    GuiComponentParamEnv opParamEnv;

    // TAP PARAM SEQUENCER (オペレーター)。そのオペレーターのパラメータを打鍵ごとに置き換える
    GuiComponentTapParam opTapParam;

    // ALG ENV。ALGORITHM/FEEDBACK の FEEDBACK の下
    GuiComponentAlgEnv algEnv;
    GuiComponentSsgHwEnv ssgHwEnvOp;
    GuiComponentWtMod wtModOp;

    GuiCategoryLabel catShape;
    GuiComboBox eg; // Envlope Generator
    GuiCategoryLabel catMask;
    GuiToggleButton mask; // Mask
    NormalSeparator mmlSeparator;
    GuiMmlButton mml;
	NormalSeparator lfoSep;
    GuiComponentNudgeSliderFloat ams;
    GuiComponentNudgeSliderFloat amd;
    GuiComponentNudgeSliderFloat pms;
    GuiComponentNudgeSliderFloat pmd;
    GuiTextButton amsTo37;
    GuiTextButton amsTo606;
    GuiTextButton amdTo1;
    GuiTextButton amdTo12;
    GuiTextButton amdTo48;
    GuiTextButton pmsTo606;
    GuiTextButton pmsTo64;
    GuiTextButton pmdTo7;
    GuiTextButton pmdTo137;
    GuiTextButton pmdTo14;

    GuiSlider rgAr;
    GuiSlider rgDr;
    GuiSlider rgSl;
    GuiSlider rgRr;
    GuiSlider rgTl;
    GuiToggleButton sus;
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
    GuiToggleButton gGraphBtnSsg256;
    GuiToggleButton gGraphBtnSsgP256;
    GuiToggleButton gGraphBtnParam;
    NormalSeparator gGraphSeparator;

    enum class GlobalGraphMode { Amp, SsgSw256, SsgSwP256, ParamEnv256 };
    GlobalGraphMode currentGlobalGraphMode = GlobalGraphMode::Amp;

    // 上に並ぶ絵。オペレータの数だけ置く。
    std::array<GuiTargetCell, OplPrValue::ops> cells;

    // 4.0.0: TARGET の大きな枠。指している小さな枠を写す
    GuiTargetCell bigCell;

    // どのエンベロープを映すかの切り替え。絵ごとではなく、まとめて 1 つ。
    // TARGET のすぐ右へ置く。
    GuiToggleButton graphBtnAmp;
    GuiToggleButton graphBtnPitch;
    GuiToggleButton graphBtnSsg256;
    GuiToggleButton graphBtnSsgP256;
    GuiToggleButton graphBtnParam;

    GuiEnvGraphMode currentGraphMode = GuiEnvGraphMode::Amp;

    // 枠の幅の畳み (4.3.0)。見出しを押すと幅が 30px になる
    GuiGroupFolds groupFolds{ ctx, GuiGroupFoldKey::opl };

    CurveCore* p_curveCore = nullptr;

    bool isUpdatingGraph = false;

    // アルゴリズムから出した「鳴っているオペレータ」の表。
    //
    // つまみは TARGET のぶんしか無くなったので、押せるかどうかで
    // 判じられない。鍵で番号を押したときの入り口で見る。
    std::array<bool, OplPrValue::ops> opActive{};

    // MML の窓へ出す案内。札からも鍵からも同じものを使う。
    static inline const juce::String mmlHint =
        I18n::pick(u8"MML風にパラメータを入力してください。 例: AR:31 AR31 DT-1 等", u8"Type the parameters MML-style. For example: AR:31 AR31 DT-1");

    void updateOpGraph(int opIndex);
    void updateOpGraphs();
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
        ampModComponent(context),
        ssgHwEnv(context),
        ssgHwPEnv(context),
        ssgSwEnv256g(context),
        ssgSwPEnv256g(context),
        unisonComponent(context),
        utilityCat(context),
        ampMajorCat(context),
        pitchMajorCat(context),
        tapMajorCat(context),
        tapAmpSeq(context, GuiComponentTapSeq::Kind::Amp),
        tapPitchSeq(context, GuiComponentTapSeq::Kind::Pitch),
        tapParamSeq(context, ParamModCatalog::opl()),
        paramEnvMajorCat(context),
        paramEnv(context, GuiComponentParamEnv::channelSpec(ParamModCatalog::opl(), OplPrValue::ops)),
        exportOpzx7Btn(context),
        broadcastLevelButton(context),
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
        ieOpLfo(context),
        ieOpPitchEnv(context),
        ieOpSsgSwEnv256(context),
        ieOpSsgSwPEnv256(context),
        ieOpSsgHwPEnv(context),
        ieOpWtAmpMod(context),
        ieOpSsgHwEnv(context),
        ieOpWtMod(context),
		ieOpChParam(context),
		imOpl3OpChParam(context),
        targerOpSlider(context),
        uSep005(context),
        ieAmpEnvG(context),
        ieSsgHwEnv(context),
        ieSsgHwPEnv(context),
        ieWtMod(context),
        ieWtAmpMod(context),
        ieSsgSwEnv256(context),
        ieSsgSwPEnv256g(context),
        ieUnison(context),
        ieQuality(context),
        ieChParam(context),
        imOpl3ChParam(context),
        opNameLabel(context),
        opTargetSeparator(context),
        colAmp(context),
        colSsgHwEnv(context),
        colSsgSwEnv256(context),
        colAmpMod(context),
        colPitchEnv(context),
        colSsgHwPEnv(context),
        colSsgSwPEnv256(context),
        colMod(context),
        colKs(context),
        colParamEnv(context),
        colTapParam(context),
        colMask(context),
        catLfo(context),
        catDet(context),
        catAmp(context),
        mul(context),
        am(context),
        vib(context),
        egType(context),
        ksCat(context),
        ksr(context),
        ksl(context),
        pitchEnv(context),
        ssgSwEnv256(context),
        ssgSwPEnv256(context),
        ssgHwPEnvOp(context),
        wtAmpModOp(context),
        opParamEnv(context, GuiComponentParamEnv::opSpec(ParamModCatalog::opl(), 63.0f)),
        opTapParam(context, ParamModCatalog::opl(), true),
        algEnv(context, OplPrValue::algorithms),
        ssgHwEnvOp(context),
        wtModOp(context),
        catShape(context),
        eg(context),
        catMask(context),
        mask(context),
        mmlSeparator(context),
        mml(context),
        lfoSep(context),
        ams(context),
        amd(context),
        pms(context),
        pmd(context),
        amsTo37(context),
        amsTo606(context),
        amdTo1(context),
        amdTo12(context),
        amdTo48(context),
        pmsTo606(context),
        pmsTo64(context),
        pmdTo7(context),
        pmdTo137(context),
        pmdTo14(context),
        rgAr(context),
        rgDr(context),
        rgSl(context),
        rgRr(context),
        rgTl(context),
        catOptional(context),
        opDelay(context),
        opDelayNudge(context),
        opDelaySeparator(context),
        xof(context),
        bypass(context),
        sus(context),
        kor(context),
        midiComponent(context),
        editComponent(context),
        opEditComponent(context),
        gGraph(),
        gGraphBtnAmp(context),
        gGraphBtnSsg256(context),
        gGraphBtnSsgP256(context),
        gGraphBtnParam(context),
        gGraphSeparator(context),
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphBtnSsg256(context),
        graphBtnSsgP256(context),
        graphBtnParam(context),
        cells{ GuiTargetCell(context), GuiTargetCell(context) },
        bigCell(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;

    // 簡易表示モードで隠す区分への一括操作
    void bypassHiddenCategories() override;
    void openEnabledCategories() override;
    void closeBypassedCategories() override;
    void layout(juce::Rectangle<int> content) override;
    void updateOpEnable(int idx, bool enable);
    void updateAlgorithmDisplay();
    void updateRgDisplayAsOp(bool rgMode);
    void updatePresetName(const juce::String& name);
    bool keyPressed(const juce::KeyPress& key) override;
    void copyFmParamsToString();
    void copyFmParamsToObject();
    void pasteFmParamsFromObject();
    void initParams();
    void layoutOpMaskCat(juce::Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutOpLfoCat(juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(juce::Rectangle<int>& rect);
    void layoutOpKsCat(juce::Rectangle<int>& rect);
    void layoutOpDetCat(juce::Rectangle<int>& rect);
    void layoutOpAmpCat(juce::Rectangle<int>& rect);
    void layoutOpEgCat(juce::Rectangle<int>& rect);
    void setupOpGraphWiring();

    // チップ全体のグラフ
    void setupGlobalGraph();
    void layoutGlobalGraph(juce::Rectangle<int>& rect);
    void updateGlobalGraph();
    void setGlobalGraphMode(GlobalGraphMode mode);
    void setLevel(float level);
    void copyParams(CopyOpl& copyObj);
    void copyOpParams(int p, CopyOplOp& copyObj);
    void pasteParams(CopyOpl& copyObj);
    void pasteOpParams(int p, CopyOplOp& copyObj);
    void importLfoParam(int opIndex);

    // ブラウザから直に読ませるための入口。
    void applyLfoParamFile(int opIndex, const juce::File& file);
    // 3.0.0 より前の形式を読む
    void setImportingLfoParams(int opIndex, juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeLfoParams(int opIndex, Io::ParamWriter& writer);
    void exportLfoParam(int opIndex);
    // ブラウザから直に渡せるようにした入口。
    void writeLfoParamFile(int opIndex, const juce::File& file);
    void importPitchEnvParam(int opIndex);
    void exportPitchEnvParam(int opIndex);
    void importSsgSwEnv256Param(int opIndex);
    void exportSsgSwEnv256Param(int opIndex);
    void importSsgSwPEnv256Param(int opIndex);
    void exportSsgSwPEnv256Param(int opIndex);
    void importOpSsgHwPEnvParam(int opIndex);
    void exportOpSsgHwPEnvParam(int opIndex);
    void importOpWtAmpModParam(int opIndex);
    void exportOpWtAmpModParam(int opIndex);
    void importOpSsgHwEnvParam(int opIndex);
    void exportOpSsgHwEnvParam(int opIndex);
    void importOpWtModParam(int opIndex);
    void exportOpWtModParam(int opIndex);
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

    // チャンネル 1 つぶん。相手の音源のファイルを読むときにも使う。
    void readChParams(const Io::ParamReader& reader);
    void importOpl3ChParam();
    // ブラウザから直に読ませるための入口。
    // importOpl3ChParam() からダイアログを外したもの。
    void applyOpl3ChParamFile(const juce::File& file);
    void importOpl3OpChParam(int opIndex);
    // ブラウザから直に読ませるための入口。
    void applyOpl3OpChParamFile(int opIndex, const juce::File& file);
};
