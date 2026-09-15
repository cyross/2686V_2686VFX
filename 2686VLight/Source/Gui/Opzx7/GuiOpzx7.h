#pragma once

#include <JuceHeader.h>

#include "../../Core/Io/ParamFile.h"
#include "../Components/FmToOpzx7/FmToOpzx7.h"
#include <array>

#include "../../Core/Const/ConstGlobal.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Components/StepValues/StepValues.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/Fix/Fix.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Gui/Components/PitchButtons/PitchButtons.h"
#include "../../Gui/Components/LfoOpzx7/LfoOpzx7.h"
#include "../../Gui/Components/MulDetune/MulDetune.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"
#include "../../Gui/Components/PresetName/PresetName.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"
#include "../../Gui/Components/Level/Level.h"
#include "../../Gui/Components/CountButtons/CountButtons.h"
#include "../../Gui/Components/NudgeSlider/NudgeSliderInt.h"
#include "../../Gui/Components/NudgeButtons/NudgeButtons.h"
#include "../../Gui/Components/NudgeSlider/NudgeSliderFloat.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"
#include "../../Gui/Components/Quality/Quality.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/AlgMatrix/GuiFmAlgRouting.h"
#include "../../Gui/Components/AmpEnv/AmpEnv.h"
#include "../../Gui/Components/WtMod/WtMod.h"
#include "../../Gui/Components/WtAmpMod/WtAmpMod.h"
#include "../../Gui/Components/WavePreview/WavePreview.h"
#include "../../Gui/Components/TargetCell/TargetCell.h"
#include "../../Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "../../Gui/Components/SsgHwPEnv/SsgHwPEnv.h"
#include "../../Gui/Components/NudgeButtons/NudgeButtons.h"
#include "../../Gui/Components/NudgeSlider/NudgeSliderFloat.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiOpzx7 : public GuiBase
{
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

    GuiCategoryLabel algFbCat;

    // AlgMatrix
    GuiComboBox algModeSelector;
    GuiFmAlgMatrix algMatrixComp;
    GuiFmAlgGraph algGraphComp;

    // 従来のアルゴリズム図用のグラフコンポーネント (画像から置き換え)
    GuiFmAlgGraph algStaticGraphComp;

    // Global
    GuiComboBox algSelector;
    NormalSeparator algFbSep;
    // オペレータごとにつまみを並べる代わりに、対象を選ぶつまみと値のつまみを
    // 1 組ずつ置く。選んでいない OP の値は帯へまとめて描く。
    GuiSlider feedbackTarget;
    GuiComponentNudgeSliderFloat feedbackSlider;
    GuiComponentNudgeButtons feedbackNudge;
    GuiStepValues feedbackValues;

    GuiCategoryLabel panCat;
    GuiToggleButton panpotEnableToggle;
    GuiSlider panpotSlider;
    GuiTextButton panToLBtn;
    GuiTextButton panToCBtn;
    GuiTextButton panToRBtn;

    // LFO
    GuiComponentLfoOpzx7 glLfo;

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

    // FM 音源のパラメータファイルを読み込む
    GuiTextButton importFmBtn;

    void importFmParams();
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiTextButton copyOpParamBtn;
    GuiSlider copyOpFromSlider;
    GuiSlider copyOpToSlider;
    NormalSeparator uSep002;
    GuiComponentImportExport ieOpLfo;
    GuiComponentImportExport ieOpDetune;
    GuiComponentImportExport ieOpPitchEnv;
    GuiComponentImportExport ieOpSsgSwEnv;
    GuiComponentImportExport ieOpSsgSwEnv11;
    GuiComponentImportExport ieOpSsgSwPEnv11;
    GuiComponentImportExport ieOpSsgHwPEnv;
    GuiComponentImportExport ieOpWtAmpMod;
    GuiComponentImportExport ieOpSsgHwEnv;
    GuiComponentImportExport ieOpWtMod;
    GuiComponentImportExport ieOpPcmPlay;
    GuiComponentImportExport ieOpChParam;
    GuiSlider targerOpSlider;
    NormalSeparator uSep003;
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
    GuiScrollGroup colWs;
    GuiScrollGroup colSsgHwEnv;
    GuiScrollGroup colSsgSwEnv;
    GuiScrollGroup colSsgSwEnv11;
    GuiScrollGroup colAmpMod;
    GuiScrollGroup colPitchEnv;
    GuiScrollGroup colSsgHwPEnv;
    GuiScrollGroup colSsgSwPEnv11;
    GuiScrollGroup colMod;
    GuiScrollGroup colKs;
    GuiScrollGroup colLfo;
    GuiScrollGroup colMask;
    // Operator Sliders
    // dr => d1r, sl => d1l, sr => d2r
    GuiComponentMulDetune mulDetune;
    GuiCategoryLabel catAmp;
    GuiSlider tl;
    GuiSlider ar;
    GuiSlider d1r;
    GuiSlider d1l;
    GuiSlider d2r;
    GuiSlider rr;

    GuiCategoryLabel ksCat;
    GuiToggleButton ksEn;
    GuiComboBox ksMode;
    GuiToggleButton ksrMA7;
    GuiComboBox kslMA7;
    GuiSlider ksrOPZ;
    GuiSlider kslOPZ;
    GuiSlider ksBp;
    GuiComboBox ksLc;
    GuiComboBox ksRc;
    GuiSlider ksLd;
    GuiSlider ksRd;
    GuiSlider ksRs;

    GuiCategoryLabel catOptional;
    GuiToggleButton bypass;
    GuiComponentFix fix;
    GuiCategoryLabel catWaveShape;
    GuiComboBox ws;
    NormalSeparator wsSeparator;

    // 選んでいる WS の波形を見せるプレビュー
    GuiWavePreview wsPreview;
    GuiTextButton loadPcmBtn;
    GuiTextButton clearPcmBtn;
    GuiLabel pcmFileNameLabel;
    // 再生速度。WS が PCM・WT・WT2 のときだけ効く。
    // ループカウンター。WS の POFF の上に置く。
    GuiComponentNudgeSliderInt loopCount;
    GuiComponentCountButtons loopCountButtons;

    GuiSlider speed;

    GuiSlider pcmOffset;
    GuiSlider pcmRatio;
    GuiToggleButton loopPointEnable;
    GuiSlider loopPointStart;
    GuiSlider loopPointEnd;
    GuiTextButton loadWtBtn;
    GuiTextButton clearWtBtn;
    GuiLabel wtFileNameLabel;
    GuiTextButton loadWt2Btn;
    GuiTextButton clearWt2Btn;
    GuiLabel wt2FileNameLabel;
    GuiCategoryLabel catSsgEnv;
    GuiComboBox se;
    GuiSlider seFreq;

    // 選んだ形を見せる。チャンネル全体のほうには前からあるが、
    // オペレータごとのものには無かった。
    GuiWavePreview sePreview;

    // LFO
    GuiComponentLfoOpzx7 lfo;

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

    GuiCategoryLabel catMask;
    GuiToggleButton mask; // Mask
    NormalSeparator mmlSeparator;
    GuiMmlButton mml;

    GuiToggleButton rgEn;
    NormalSeparator rgSeparator;
    GuiSlider rgAr;
    GuiSlider rgD1r;
    GuiSlider rgD2r;
    GuiSlider rgD1l;
    GuiSlider rgRr;
    GuiSlider rgTl;

    NormalSeparator optionalSeparator;
    GuiToggleButton sus; // Sus
    // 押してから鳴り始めるまでの間 (秒)。OPTIONAL の一番上に置く。
    GuiComponentNudgeSliderFloat opDelay;
    GuiComponentNudgeButtons opDelayNudge;
    NormalSeparator opDelaySeparator;

    GuiToggleButton xof; // Xof
    GuiToggleButton kor;

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
    std::array<GuiTargetCell, Opzx7PrValue::ops> cells;

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
    std::array<bool, Opzx7PrValue::ops> opActive{};

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
    GuiOpzx7(const GuiContext& context);

    void setup() override;

    // 簡易表示モードで隠す区分への一括操作
    void bypassHiddenCategories() override;
    void openEnabledCategories() override;
    void closeBypassedCategories() override;
    void layout(juce::Rectangle<int> content) override;

    // 選んだ OP へ FB のつまみを束縛し直す。帯の描き直しもここでやる。
    void rebindFeedback();
    void refreshFeedbackValues();

    // OP ごとの FB。つまみは 1 組しか束縛されていないので APVTS から直に読み書きする。
    float getFeedbackValue(int opIndex) const;
    void setFeedbackValue(int opIndex, float value);
    // 素材の名前を出す枠はひとそろいしか無い。指していないオペレータの
    // 名前が来ても、画面に出すところが無いので何もしない。
    // 指し先が変わったときは rebind が引き直す。
    void updatePcmFileName(int opIndex, const juce::String& fileName) {
        // 素材が変わったので、そのオペレータの枠の波形を描き直す。
        // 枠は TARGET と関係なく全部出ているので、名前より先に行う。
        updateCellWs(opIndex);

        if (opIndex != currentOp()) return;

        pcmFileNameLabel.setText(fileName, juce::dontSendNotification);

        // 波形メモリ / PCM が入れ替わったので、WS のプレビューも合わせる
        updateWsPreview();
    }
    void updateWtFileName(int opIndex, const juce::String& fileName) {
        // 素材が変わったので、そのオペレータの枠の波形を描き直す。
        // 枠は TARGET と関係なく全部出ているので、名前より先に行う。
        updateCellWs(opIndex);

        if (opIndex != currentOp()) return;

        wtFileNameLabel.setText(fileName, juce::dontSendNotification);

        // 波形メモリ / PCM が入れ替わったので、WS のプレビューも合わせる
        updateWsPreview();
    }
    void updateWt2FileName(int opIndex, const juce::String& fileName) {
        // 素材が変わったので、そのオペレータの枠の波形を描き直す。
        // 枠は TARGET と関係なく全部出ているので、名前より先に行う。
        updateCellWs(opIndex);

        if (opIndex != currentOp()) return;

        wt2FileNameLabel.setText(fileName, juce::dontSendNotification);

        // 波形メモリ / PCM が入れ替わったので、WS のプレビューも合わせる
        updateWsPreview();
    }
    void updateAllPcmFileName(const juce::String& fileName) {
        pcmFileNameLabel.setText(fileName, juce::dontSendNotification);
    }
    void updateAllWtFileName(const juce::String& fileName) {
        wtFileNameLabel.setText(fileName, juce::dontSendNotification);
    }
    void updateAllWt2FileName(const juce::String& fileName) {
        wt2FileNameLabel.setText(fileName, juce::dontSendNotification);
    }
    void updateOpEnable(int idx, bool enable);
    void updateOnWsChange();
    void updateWsPreview();
    void updateCellWs(int opIndex);
    void fillWsPreview(GuiWavePreview& preview, int opIndex);
    void updateAlgorithmDisplay();
    void updateRgDisplayAsOp(bool rgMode);
    void updateAlgorithmMatrixDisplay();
    void updatePresetName(const juce::String& name);
    bool keyPressed(const juce::KeyPress& key) override;
    void copyFmParamsToString();
    void copyFmParamsToObject();
    void pasteFmParamsFromObject();
    void initParams();
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutOpMaskCat(juce::Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanpotCat(juce::Rectangle<int>& rect);
    void updateSePreview();
    void layoutOpSsgEnvCat(juce::Rectangle<int>& rect);
    void layoutOpOptionalCat(juce::Rectangle<int>& rect);
    void layoutOpKsCat(juce::Rectangle<int>& rect, bool rgMode);
    void layoutOpAmpCat(juce::Rectangle<int>& rect, bool rgMode);
    void layoutOpWsCat(juce::Rectangle<int>& rect, int selectedWs);
    void setupOpGraphWiring();

    // チップ全体のグラフ
    void setupGlobalGraph();
    void layoutGlobalGraph(juce::Rectangle<int>& rect);
    void updateGlobalGraph();
    void setGlobalGraphMode(GlobalGraphMode mode);
    void setLevel(float level);
    void copyParams(CopyOpzx7& copyObj);
    void copyOpParams(int p, CopyOpzx7Op& copyObj);
    void pasteParams(CopyOpzx7& copyObj);
    void pasteOpParams(int p, CopyOpzx7Op& copyObj);
    void importOpLfoParam(int opIndex);
    void exportOpLfoParam(int opIndex);
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
    void importDetuneParam(int opIndex);
    void exportDetuneParam(int opIndex);
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
    void importOpPcmPlayParam(int opIndex);

    // ブラウザから直に読ませるための入口。
    void applyOpPcmPlayParamFile(int opIndex, const juce::File& file);
    // 3.0.0 より前の形式を読む
    void setImportingOpPcmPlayParams(int opIndex, juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeOpPcmPlayParams(int opIndex, Io::ParamWriter& writer);
    void exportOpPcmPlayParam(int opIndex);
    // ブラウザから直に渡せるようにした入口。
    void writeOpPcmPlayParamFile(int opIndex, const juce::File& file);
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
