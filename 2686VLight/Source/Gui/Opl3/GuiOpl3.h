#pragma once

#include "../../Core/Gui/GuiI18n.h"
#include <JuceHeader.h>

#include "../Components/FmToOpzx7/FmToOpzx7.h"

#include "../../Core/Io/ParamFile.h"
#include <array>

#include "../../Core/Const/ConstGlobal.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Processor/Opl3/ProcessorOpl3Values.h"
#include "../../Gui/Components/PresetName/PresetName.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"
#include "../../Gui/Components/Import/Import.h"
#include "../../Gui/Components/Level/Level.h"
#include "../../Gui/Components/NudgeButtons/NudgeButtons.h"
#include "../../Gui/Components/NudgeSlider/NudgeSliderFloat.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"
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
    GuiComponentWtAmpMod ampModComponent;
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

    // 大区分。[[AMP ENV]] と [[PITCH ENV]]
    GuiCategoryLabel ampMajorCat;
    GuiCategoryLabel pitchMajorCat;

    // OPZX7S のパラメータファイルへ書き出す
    GuiTextButton exportOpzx7Btn;

    void exportOpzx7Params();
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
    GuiComponentImportExport ieOpWtAmpMod;
    GuiComponentImportExport ieOpSsgHwEnv;
    GuiComponentImportExport ieOpWtMod;
    GuiComponentImportExport ieOpChParam;
    GuiComponentImport imOplOpChParam;
    GuiSlider targerOpSlider;
    NormalSeparator uSep005;
    GuiComponentImportExport ieAmpEnvG;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgHwPEnv;
    GuiComponentImportExport ieWtMod;
    GuiComponentImportExport ieWtAmpMod;
    GuiComponentImportExport ieSsgSwEnv11;
    GuiComponentImportExport ieSsgSwPEnv11g;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    GuiComponentImport imOplChParam;
    GuiComponentImport imOplChAllOpParam;
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
    GuiComponentSsgSwEnv ssgSwEnv;

    GuiComponentSsgSwEnv11 ssgSwEnv11;
    GuiComponentSsgSwPEnv11 ssgSwPEnv11;
    GuiComponentSsgHwPEnv ssgHwPEnvOp;
    GuiComponentWtAmpMod wtAmpModOp;
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
    GuiTextButton amdTo1;
    GuiTextButton amdTo48;
    GuiTextButton pmsTo64;
    GuiTextButton pmdTo7;
    GuiTextButton pmdTo14;

    GuiSlider rgAr;
    GuiSlider rgDr;
    GuiSlider rgSl;
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
    std::array<GuiTargetCell, Opl3PrValue::ops> cells;

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
    std::array<bool, Opl3PrValue::ops> opActive{};

    // MML の窓へ出す案内。札からも鍵からも同じものを使う。
    static inline const juce::String mmlHint =
        I18n::pick(u8"MML風にパラメータを入力してください。 例: AR:31 AR31 DT-1 等", u8"Type the parameters MML-style. For example: AR:31 AR31 DT-1");

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
        ampModComponent(context),
        ssgHwEnv(context),
        ssgHwPEnv(context),
        ssgSwEnv11g(context),
        ssgSwPEnv11g(context),
        unisonComponent(context),
        utilityCat(context),
        ampMajorCat(context),
        pitchMajorCat(context),
        exportOpzx7Btn(context),
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
        ieOpWtAmpMod(context),
        ieOpSsgHwEnv(context),
        ieOpWtMod(context),
		ieOpChParam(context),
		imOplOpChParam(context),
        targerOpSlider(context),
        uSep005(context),
        ieAmpEnvG(context),
        ieSsgHwEnv(context),
        ieSsgHwPEnv(context),
        ieWtMod(context),
        ieWtAmpMod(context),
        ieSsgSwEnv11(context),
        ieSsgSwPEnv11g(context),
        ieUnison(context),
        ieQuality(context),
		ieChParam(context),
		imOplChParam(context),
        imOplChAllOpParam(context),
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
        ssgSwEnv(context),
		ssgSwEnv11(context),
        ssgSwPEnv11(context),
        ssgHwPEnvOp(context),
        wtAmpModOp(context),
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
        amdTo1(context),
        amdTo48(context),
        pmsTo64(context),
        pmdTo7(context),
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
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutOpMaskCat(juce::Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
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
    void copyParams(CopyOpl3& copyObj);
    void copyOpParams(int p, CopyOpl3Op& copyObj);
    void pasteParams(CopyOpl3& copyObj);
    void pasteOpParams(int p, CopyOpl3Op& copyObj);
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
    void importOplChParam();
    // ブラウザから直に読ませるための入口。
    // importOplChParam() からダイアログを外したもの。
    void applyOplChParamFile(const juce::File& file);
    void importOplChAllOpParam();
    // ブラウザから直に読ませるための入口。
    // importOplChAllOpParam() からダイアログを外したもの。
    void applyOplChAllOpParamFile(const juce::File& file);
    void importOplOpChParam(int opIndex);
    // ブラウザから直に読ませるための入口。
    void applyOplOpChParamFile(int opIndex, const juce::File& file);
};
