#pragma once

#include "Shared/Gui/Components/TapSeq/TapSeq.h"
#include "Shared/Gui/Components/TapParam/TapParam.h"
#include "Shared/Gui/Components/RatioEnv/RatioEnv.h"
#include "Shared/Gui/Components/ToneNoiseValue/ToneNoiseValue.h"
#include <JuceHeader.h>
#include <array>

#include "Shared/Gui/Components/PathRoot/PathRoot.h"
#include "Shared/Core/Const/ConstGlobal.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "Shared/Advanced/Curve/AdvancedCurve.h"
#include "Shared/Gui/Components/Fix/Fix.h"
#include "Shared/Gui/Components/Unison/Unison.h"
#include "Shared/Gui/Components/MulDetune/MulDetune.h"
#include "Shared/Gui/Components/AmpEnv/AmpEnv.h"
#include "Shared/Gui/Components/PitchEnv/PitchEnv.h"
#include "Shared/Gui/Components/Midi/Midi.h"
#include "Shared/Gui/Components/Edit/Edit.h"
#include "Shared/Gui/Components/Edit/TargetsDialog.h"
#include "Shared/Gui/Components/LfoOpzx7/LfoOpzx7.h"
#include "Shared/Gui/Components/PresetName/PresetName.h"
#include "Shared/Gui/Components/ImportExport/ImportExport.h"
#include "Shared/Gui/Components/Level/Level.h"
#include "Shared/Gui/Components/CountButtons/CountButtons.h"
#include "Shared/Gui/Components/NudgeSlider/NudgeSliderInt.h"
#include "Shared/Gui/Components/Separator/NormalSeparator.h"
#include "Shared/Gui/Components/Separator/ShortSeparator.h"
#include "Shared/Gui/Components/Quality/QualityPcm.h"
#include "Shared/Gui/Components/SsgSwEnv256/SsgSwEnv256.h"
#include "Shared/Gui/Components/SsgSwPEnv256/SsgSwPEnv256.h"
#include "Shared/Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "Shared/Gui/Components/SsgHwPEnv/SsgHwPEnv.h"
#include "Shared/Gui/Components/WtMod/WtMod.h"
#include "Shared/Gui/Components/WtAmpMod/WtAmpMod.h"
#include "Shared/Gui/Components/WavePreview/WavePreview.h"
#include "Shared/Gui/Components/WavePreview/WavePreviewGrid.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiAdpcmPlus : public GuiBase
{
    // --- ADPCM Page ---
    GuiScrollGroup mainGroup;

    // FORM・OPTIONAL・PAN をまとめる専用グループ。
    // どれも PCM 1 本ごとの設定なので、チャンネル全体の設定とは分ける。
    GuiScrollGroup pcmGroup;

    GuiComponentPresetName presetName;

    GuiCategoryLabel formCat;

	QualityPcm qualityPcmComponent;

    // SLOT は鳴らす PCM。TARGET はいま画面へ出している PCM。
    // FORM・OPTIONAL・PAN の値は PCM ごとに分かれている。
    GuiSlider slotSlider;
    GuiSlider slotTarget;

    GuiTextButton loadButton;
    GuiTextButton clearButton;
    GuiLabel fileNameLabel;

    NormalSeparator formSeparator;

    // OPTIONAL の「Loop」を挟む区切り線。
    // 上は再生位置・倍率、下はループの話で、まとまりが変わる。
    NormalSeparator optLoopSepTop;
    NormalSeparator optLoopSepBottom;

    GuiCategoryLabel optionalCat;

    // 読み込んだサンプルを見せるプレビュー。
    // P.OF / P.RT で切り出した範囲を描き、ループ位置を縦線で出す。
    GuiWavePreview samplePreview;

    // スロット 32 個ぶんを並べたもの。どこに何が入っているかを一目で見せる。
    // 切り出す前の素材そのものを映すので、上の 1 枚とは役目が違う。
    GuiWavePreviewGrid slotPreviews;

    // 再生速度。OPTIONAL の一番上に置く。
    GuiSlider speedSlider;
    NormalSeparator optSpeedSeparator;

    // ループカウンター。SPEED のひとつ下に置く。
    GuiComponentNudgeSliderInt loopCountSlider;
    GuiComponentCountButtons loopCountButtons;
    NormalSeparator optCountSeparator;

    GuiSlider pcmOffsetSlider;
    GuiSlider pcmRatioSlider;

	GuiComponentLevel levelComponent;

    GuiSlider toneSlider;
    GuiSlider noiseSlider;
    GuiSlider noiseFreqSlider;

    // TONE / NOISE を実機の設定値で決める (N.FREQ の下)
    GuiComponentToneNoiseValue toneNoiseValue;
    GuiSlider mixSlider;
    GuiTextButton mixSetTone;  // 0.0
    GuiTextButton mixSetMix;   // 0.5
    GuiTextButton mixSetNoise; // 1.0
    GuiSlider panSlider;

    GuiTextButton panToLBtn;
    GuiTextButton panToCBtn;
    GuiTextButton panToRBtn;

    // Loop Button
    GuiToggleButton loopButton;
    GuiToggleButton loopPointEnableButton;
    GuiSlider loopPointStartSlider;
    GuiSlider loopPointEndSlider;

    // SSG Hw Env
    GuiComponentSsgHwEnv ssgHwEnv;
    GuiComponentSsgHwPEnv ssgHwPEnv;

    GuiComponentFix fixComponent;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    GuiCategoryLabel panCat;

    // Amp ADSR
    GuiComponentAmpEnv ampEnvComponent;
    // MODULATION (FDS / WonderSwan / HuC6280)
    GuiComponentWtMod modComponent;
    GuiComponentWtAmpMod ampModComponent;

    // Pitch ADSR
    GuiComponentPitchEnv pitchEnvComponent;

    // SSG SW Env

	GuiComponentSsgSwEnv256 ssgSwEnv256Component;
	GuiComponentSsgSwPEnv256 ssgSwPEnv256Component;

    // Detune
    GuiComponentMulDetune mulDetuneComponent;

    GuiComponentLfoOpzx7 lfoComponent;

    GuiComponentMidi midiComponent;

    // EDIT 区分 (切り取り・コピー・貼り付け・初期化)
    GuiComponentEdit editComponent;

    // TARGET の複写・消去のダイアログ。WAVE MEMORY / FORM の枠を隠さない
    // ところ (枠の右の空き) へ出す。
    GuiTargetsDialog targetsDialog;
    juce::Rectangle<int> targetsDialogArea;
    void openTargetsDialog(GuiTargetsDialog::Mode mode);

    GuiCategoryLabel utilityCat;
    GuiComponentPathRoot pathRoot;

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
    GuiTextButton broadcastLevelButton;
    GuiComponentImportExport ieToneNoise;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieAmpEnv;
    GuiComponentImportExport iePitchEnv;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgHwPEnv;
    GuiComponentImportExport ieWtMod;
    GuiComponentImportExport ieWtAmpMod;
    GuiComponentImportExport ieSsgSwEnv256;
    GuiComponentImportExport ieSsgSwPEnv256;
    GuiComponentImportExport ieDetune;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport iePcmPlay;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    GuiEnvelopeGraph graph;
    GuiToggleButton graphBtnAmp;
    GuiToggleButton graphBtnPitch;
    GuiToggleButton graphBtnSsg256;
    GuiToggleButton graphBtnSsgP256;
    NormalSeparator graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw256, SsgSwP256 };
    GraphMode currentGraphMode;

    CurveCore* p_curveCore = nullptr;

    bool isUpdatingGraph = false;

    void updateGraph();
    void setGraphMode(GraphMode mode);
public:
    // 区分の外に持っている素材 (ParamEdit の切り取り・コピー・貼り付け)
    void collectClipAssets(const juce::String& prefix, bool wholeChannel,
        std::map<juce::String, juce::String>& out) override;
    void applyClipAssets(const juce::String& prefix, bool wholeChannel,
        const std::map<juce::String, juce::String>& assets, bool clearOthers) override;

private:
    std::vector<ParamEdit::AssetSlot> clipAssetSlots(const juce::String& prefix, bool wholeChannel);
public:
    GuiAdpcmPlus(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        pcmGroup(context),
        presetName(context),
        formCat(context),
		qualityPcmComponent(context),
        slotSlider(context),
        slotTarget(context),
        loadButton(context),
        clearButton(context),
        fileNameLabel(context),
        formSeparator(context),
        optLoopSepTop(context),
        optLoopSepBottom(context),
        optionalCat(context),
        samplePreview(context),
        slotPreviews(context),
        speedSlider(context),
        optSpeedSeparator(context),
        loopCountSlider(context),
        loopCountButtons(context),
        optCountSeparator(context),
        pcmOffsetSlider(context),
        pcmRatioSlider(context),
        levelComponent(context),
        toneSlider(context),
        noiseSlider(context),
        noiseFreqSlider(context),
        toneNoiseValue(context),
        mixSlider(context),
        mixSetTone(context),
        mixSetMix(context),
        mixSetNoise(context),
        panSlider(context),
        panToLBtn(context),
        panToCBtn(context),
        panToRBtn(context),
        loopButton(context),
        loopPointEnableButton(context),
        loopPointStartSlider(context),
        loopPointEndSlider(context),
        fixComponent(context),
        ssgHwEnv(context),
        ssgHwPEnv(context),
        unisonComponent(context),
        panCat(context),
        ampEnvComponent(context),
        modComponent(context),
        ampModComponent(context),
        pitchEnvComponent(context),
		ssgSwEnv256Component(context),
		ssgSwPEnv256Component(context),
		mulDetuneComponent(context),
        lfoComponent(context),
        midiComponent(context),
        editComponent(context),
        targetsDialog(context),
        utilityCat(context),
        pathRoot(context),
        ampMajorCat(context),
        pitchMajorCat(context),
        tapMajorCat(context),
        tapAmpSeq(context, GuiComponentTapSeq::Kind::Amp),
        tapPitchSeq(context, GuiComponentTapSeq::Kind::Pitch),
        tapParamSeq(context, ParamModCatalog::adpcmPlus()),
        paramEnvMajorCat(context),
        paramEnv(context, GuiComponentParamEnv::channelSpec(ParamModCatalog::adpcmPlus(), 0)),
        broadcastLevelButton(context),
        ieToneNoise(context),
        ieLfo(context),
        ieAmpEnv(context),
        iePitchEnv(context),
        ieSsgHwEnv(context),
        ieSsgHwPEnv(context),
        ieWtMod(context),
        ieWtAmpMod(context),
        ieSsgSwEnv256(context),
        ieSsgSwPEnv256(context),
        ieDetune(context),
        ieQuality(context),
        iePcmPlay(context),
        ieUnison(context),
		ieChParam(context),
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphBtnSsg256(context),
        graphBtnSsgP256(context),
        graphSeparator(context)
    {
        currentGraphMode = GraphMode::Amp; // 初期状態はAmp
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;

    // Ctrl + ← / → などで TARGET を動かす
    bool keyPressed(const juce::KeyPress& key) override;

    // 簡易表示モードで隠す区分への一括操作
    void bypassHiddenCategories() override;
    void openEnabledCategories() override;
    void closeBypassedCategories() override;
    void layout(juce::Rectangle<int> content) override;
    void updateFileName(const juce::String& fileName);

    // いま画面へ出している PCM のスロット番号
    int targetSlot() const { return juce::jlimit(0, Global::AdpcmPlus::slots - 1, (int)slotTarget.getValue()); }

    // TARGET が動いたときに、つまみの指し先をそのスロットへ張り直す
    void applySlotTarget();
    void rebindSlot();
    void updateSlotFileName(int slot);
    void updateSlotPreview(int slot);

    // 32 個ぶんを作り直す。読み込み直しやプリセットの読み込みのあとに呼ぶ。
    void updateAllSlotPreviews();
    void updateSamplePreview();
    bool isThis(juce::Button* button);
    bool isBtnPanL(juce::Button* button);
    bool isBtnPanC(juce::Button* button);
    bool isBtnPanR(juce::Button* button);
    void setPan(float pan);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    void updatePresetName(const juce::String& name);
    void initParams();
    void layoutFormCat(Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanCat(juce::Rectangle<int>& rect);
    void layoutOptionalCat(juce::Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
    void setLevel(float level);
    void importToneNoiseParam();

    // ブラウザから直に読ませるための入口。
    void applyToneNoiseParamFile(const juce::File& file);
    // 3.0.0 より前の形式を読む
    void setImportingToneNoiseParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeToneNoiseParams(Io::ParamWriter& writer);
    void exportToneNoiseParam();
    // ブラウザから直に渡せるようにした入口。
    void writeToneNoiseParamFile(const juce::File& file);
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
    void importPcmPlayParam();

    // ブラウザから直に読ませるための入口。
    void applyPcmPlayParamFile(const juce::File& file);
    // 3.0.0 より前の形式を読む
    void setImportingPcmPlayParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writePcmPlayParams(Io::ParamWriter& writer);
    void exportPcmPlayParam();
    // ブラウザから直に渡せるようにした入口。
    void writePcmPlayParamFile(const juce::File& file);
    void importChParam();

    // ブラウザから直に読ませるための入口。
    // importChParam() からダイアログを外したもの。
    void applyChParamFile(const juce::File& file) override;

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeChParams(Io::ParamWriter& writer);
    void exportChParam();
    // ブラウザから直に渡せるようにした入口。
    void writeChParamFile(const juce::File& file);
};
