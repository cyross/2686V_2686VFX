#pragma once

#include <JuceHeader.h>

#include "../../Core/Io/ParamFile.h"
#include <array>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Components/Fix/Fix.h"
#include "../../Gui/Components/MulDetune/MulDetune.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/AmpEnv/AmpEnv.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/LfoOpzx7/LfoOpzx7.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Processor/Rhythm/ProcessorRhythmValues.h"
#include "../../Gui/Components/PresetName/PresetName.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"
#include "../../Gui/Components/Level/Level.h"
#include "../../Gui/Components/CountButtons/CountButtons.h"
#include "../../Gui/Components/NudgeButtons/NudgeButtons.h"
#include "../../Gui/Components/NudgeSlider/NudgeSliderFloat.h"
#include "../../Gui/Components/NudgeSlider/NudgeSliderInt.h"
#include "../../Gui/Components/Separator/NormalSeparator.h"
#include "../../Gui/Components/Separator/ShortSeparator.h"
#include "../../Gui/Components/Quality/QualityPcm.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "../../Gui/Components/SsgHwPEnv/SsgHwPEnv.h"
#include "../../Gui/Components/WtMod/WtMod.h"
#include "../../Gui/Components/WtAmpMod/WtAmpMod.h"
#include "../../Gui/Components/WavePreview/WavePreview.h"

#include "../../Core/Gui/GuiCopyObj.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

// ==========================================================
// パッドの絵
// ==========================================================
// 波形とエンベロープを 1 枠にまとめたもの。つまみは持たない。
//
// パッドの数だけ区分をひとそろい並べるのをやめ、設定は下の 1 組へ
// 集めた。上に残るのはこの絵だけになる。
//
// 絵はパッドの数だけ同時に出るので、値をつまみから読むわけには
// いかない。パラメータから直に採る。
class RhythmPadCell : public GuiBase
{
    int m_padIndex = 0;

    // このパッドのパラメータの接頭辞
    juce::String m_code;

    GuiLabel titleLabel;
    GuiWavePreview samplePreview;
    GuiEnvelopeGraph graph;

    GuiToggleButton graphBtnAmp;
    GuiToggleButton graphBtnPitch;
    GuiToggleButton graphBtnSsg;
    GuiToggleButton graphBtnSsg11;
    GuiToggleButton graphBtnSsgP11;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };

    // どのエンベロープを映すかは枠ごとに決める。
    // 4 本を見比べたいことがあるので、まとめて 1 つにはしない。
    GraphMode currentGraphMode = GraphMode::Amp;

    // TARGET が指しているか。指している枠だけ濃い線で囲う。
    bool isActive = false;

    void setGraphMode(GraphMode mode);
public:
    RhythmPadCell(const GuiContext& context) :
        GuiBase(context),
        titleLabel(context),
        samplePreview(context),
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphBtnSsg(context),
        graphBtnSsg11(context),
        graphBtnSsgP11(context)
    {
    }

    // 枠を押したときに呼ぶ。TARGET をここへ動かすために使う。
    std::function<void(int)> onSelect;

    void setup(juce::Component& parent, int index, const juce::String& padName, int& tabOrder);
    void layout(juce::Rectangle<int> rect);

    // TARGET が指しているかを伝える。枠線の濃さが変わる。
    void setActive(bool active);

    void setVisibles(bool visible);

    void updateSamplePreview();
    void updateGraph();

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
};

class RhythmPadGui: public GuiBase
{
    GuiScrollGroup mainGroup;

    GuiLabel fileNameLabel;
    GuiTextButton loadButton;
    GuiTextButton clearButton;

    NormalSeparator formSeparator;

    // OPTIONAL の「One Shot」を挟む区切り線。
    // 上は再生位置・倍率、下はループの話で、まとまりが変わる。
    NormalSeparator optOneShotSepTop;
    NormalSeparator optOneShotSepBottom;

    GuiCategoryLabel formCat;
    GuiCategoryLabel optionalCat;

    // どのパッドかを覚えておく。プレビューで読むサンプルを選ぶのに使う。
    int m_padIndex = 0;

    // 読み込んだサンプルを見せるプレビュー。
    // P.OF / P.RT で切り出した範囲を描き、ループ位置を縦線で出す。
    GuiWavePreview samplePreview;

    // 押してから鳴り始めるまでの間 (秒)。OPTIONAL の一番上に置く。
    GuiComponentNudgeSliderFloat padDelaySlider;
    GuiComponentNudgeButtons padDelayNudge;
    NormalSeparator optDelaySeparator;

    // 再生速度。
    GuiSlider speedSlider;
    NormalSeparator optSpeedSeparator;

    // ループカウンター。SPEED のひとつ下に置く。
    GuiComponentNudgeSliderInt loopCountSlider;
    GuiComponentCountButtons loopCountButtons;
    NormalSeparator optCountSeparator;

    GuiSlider pcmOffsetSlider;
    GuiSlider pcmRatioSlider;
    GuiToggleButton loopPointEnableButton;
    GuiSlider loopPointStartSlider;
    GuiSlider loopPointEndSlider;

    QualityPcm qualityPcmComponent;

    GuiCategoryLabel panCat;

    GuiSlider noteSlider;

    GuiSlider panSlider;

    GuiTextButton panToLBtn;
    GuiTextButton panToCBtn;
    GuiTextButton panToRBtn;

    GuiSlider volSlider;
    GuiSlider toneSlider;
    GuiSlider noiseSlider;
    GuiSlider noiseFreqSlider;
    GuiSlider mixSlider;
    GuiTextButton mixSetTone;  // 0.0
    GuiTextButton mixSetMix;   // 0.5
    GuiTextButton mixSetNoise; // 1.0

    GuiToggleButton oneShotButton;

    GuiComponentFix fixComponent;

    // Amp ADSR
    GuiComponentAmpEnv ampEnvComponent;
    // パッドごとの MODULATION
    GuiComponentWtMod modComponent;
    GuiComponentWtAmpMod ampModComponent;

    // Pitch ADSR
    GuiComponentPitchEnv pitchEnvComponent;

    // SSG SW Env
    GuiComponentSsgSwEnv ssgSwEnvComponent;

    GuiComponentSsgSwEnv11 ssgSwEnv11Component;
    GuiComponentSsgSwPEnv11 ssgSwPEnv11Component;

    // Detune
    GuiComponentMulDetune mulDetuneComponent;

    GuiComponentLfoOpzx7 lfoComponent;

    // SSG Hw Env
    GuiComponentSsgHwEnv ssgHwEnv;
    GuiComponentSsgHwPEnv ssgHwPEnv;

    std::unique_ptr<juce::FileChooser> fileChooser;

    // TARGET のつまみ。見出しのすぐ下、区切り線の上に置く。
    //
    // 設定はひとそろいしか無いので、どのパッドを触っているのかは
    // ここでしか分からない。中身より先に目へ入る場所へ出す。
    GuiSlider padTarget;
    NormalSeparator padTargetSeparator;

    // 描き直しが入れ子で走らないようにする印。
    bool isUpdatingGraph = false;
public:
    void setImportingParams(int p, juce::StringArray& lines, int& index);
    RhythmPadGui(const GuiContext& context) :
		GuiBase(context),
        mainGroup(context),
        fileNameLabel(context),
        loadButton(context),
        clearButton(context),
        formSeparator(context),
        optOneShotSepTop(context),
        optOneShotSepBottom(context),
        formCat(context),
        optionalCat(context),
        samplePreview(context),
        padDelaySlider(context),
        padDelayNudge(context),
        optDelaySeparator(context),
        speedSlider(context),
        optSpeedSeparator(context),
        loopCountSlider(context),
        loopCountButtons(context),
        optCountSeparator(context),
        pcmOffsetSlider(context),
        pcmRatioSlider(context),
        loopPointEnableButton(context),
        loopPointStartSlider(context),
        loopPointEndSlider(context),
		qualityPcmComponent(context),
        panCat(context),
        noteSlider(context),
        panSlider(context),
        panToLBtn(context),
        panToCBtn(context),
        panToRBtn(context),
        volSlider(context),
        toneSlider(context),
        noiseSlider(context),
        noiseFreqSlider(context),
        mixSlider(context),
        mixSetTone(context),
        mixSetMix(context),
        mixSetNoise(context),
        oneShotButton(context),
        fixComponent(context),
        ampEnvComponent(context),
        modComponent(context),
        ampModComponent(context),
        pitchEnvComponent(context),
        ssgSwEnvComponent(context),
        ssgSwEnv11Component(context),
        ssgSwPEnv11Component(context),
        mulDetuneComponent(context),
        lfoComponent(context),
        ssgHwEnv(context),
        ssgHwPEnv(context),
        padTarget(context),
        padTargetSeparator(context)
    {
    }

    void updatePadFileName(const juce::String& fileName);
    void updateSamplePreview();
    void setup(juce::Component& parent, int& tabOrder);

    // TARGET が指すパッドへ、束縛を丸ごと移す。
    void rebind(int index);

    // 読んでいるファイルの名前を処理側から取り直す。
    void updateFileNameFromProcessor();

    // TARGET のつまみ。鍵で動かすために外から触れるようにしてある。
    GuiSlider& targetSlider() { return padTarget; }

    // TARGET の値 (1 から数える)。
    double getTargetValue() const { return padTarget.getValue(); }

    // TARGET が動いたときに呼ぶ。
    std::function<void()> onTargetChange;

    // つまみが動いたときに呼ぶ。上の絵を描き直してもらう。
    std::function<void()> onParamsChanged;
	void layout(juce::Rectangle<int> content);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    bool isThis(juce::Button* button);
    void layoutFormCat(Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanCat(juce::Rectangle<int>& rect);
    void layoutOptionalCat(juce::Rectangle<int>& rect);
    void setupGraph();
    void copyParams(CopyRhythmPad& copyObj);
    void pasteParams(CopyRhythmPad& copyObj);
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
    void importLfoParam();
    void exportLfoParam();
    void importAmpEnvParam();
    void exportAmpEnvParam();
    void importPitchEnvParam();
    void exportPitchEnvParam();
    void importSsgHwEnvParam();
    void importSsgHwPEnvParam();
    void importWtModParam();
    void importWtAmpModParam();
    void exportSsgHwEnvParam();
    void exportSsgHwPEnvParam();
    void exportWtModParam();
    void exportWtAmpModParam();
    void importSsgSwEnvParam();
    void exportSsgSwEnvParam();
    void importSsgSwEnv11Param();
    void exportSsgSwEnv11Param();
    void importSsgSwPEnv11Param();
    void exportSsgSwPEnv11Param();
    void importDetuneParam();
    void exportDetuneParam();
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
    // 名前で受け渡す。パッドは並びの中のひとつを渡す。
    void readParams(int p, const Io::ParamReader& r);
    void writeParams(int p, Io::ParamWriter& w);
};

class GuiRhythm : public GuiBase
{
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiComponentLevel levelComponent;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    GuiComponentMidi midiComponent;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiTextButton copyPadParamBtn;
    GuiSlider copyPadFromSlider;
    GuiSlider copyPadToSlider;
    NormalSeparator uSep002;
    GuiComponentImportExport ieToneNoise;
    GuiComponentImportExport ieLfo;
    GuiComponentImportExport ieAmpEnv;
    GuiComponentImportExport iePitchEnv;
    GuiComponentImportExport ieSsgHwEnv;
    GuiComponentImportExport ieSsgHwPEnv;
    GuiComponentImportExport ieWtMod;
    GuiComponentImportExport ieWtAmpMod;
    GuiComponentImportExport ieSsgSwEnv;
    GuiComponentImportExport ieSsgSwEnv11;
    GuiComponentImportExport ieSsgSwPEnv11;
    GuiComponentImportExport ieDetune;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport iePcmPlay;
    GuiComponentImportExport ieChPadParam;
    NormalSeparator uSep003;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // 設定はひとそろいだけ。TARGET で指し先を切り替える。
    RhythmPadGui padPanel;

    // 上に並ぶ絵。こちらはパッドの数だけ置く。
    std::array<RhythmPadCell, RhythmPrValue::pads> cells;

    // 指し先を一時的に動かして何かをする。
    //
    // 設定はひとそろいしか無いので、TARGET が指していないパッドを
    // 読み書きするには、いったんそこへ繋ぎ替えるしかない。
    // 終わったら必ず元へ戻す。
    void withPad(int index, const std::function<void(RhythmPadGui&)>& fn);
public:
    GuiRhythm(const GuiContext& context);
                     
    void setup() override;

    // Ctrl + ← / → などで TARGET を動かす
    bool keyPressed(const juce::KeyPress& key) override;
    void layout(juce::Rectangle<int> content) override;

    // TARGET が今どのパッドを指しているか (0 から数える)。
    int currentPad() const;

    // 指し先を切り替える。設定の束縛と枠線の付け替えをまとめて行う。
    void applyPadTarget();
    void layoutUtilityCat(Rectangle<int>& rect);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    void buttonClicked(juce::Button* button);
	void updatePadFileName(int padIndex, const juce::String& fileName);
    bool isThis(int padIndex, juce::Button* button);
    void updatePresetName(const juce::String& name);
    void initParams();
    void setLevel(float level);
    void copyPadParams(int p, CopyRhythmPad& copyObj);
    void pastePadParams(int p, CopyRhythmPad& copyObj);
    void importToneNoiseParam(int p);
    void exportToneNoiseParam(int p);
    void importLfoParam(int p);
    void exportLfoParam(int p);
    void importAmpEnvParam(int p);
    void exportAmpEnvParam(int p);
    void importPitchEnvParam(int p);
    void exportPitchEnvParam(int p);
    void importSsgSwEnvParam(int p);
    void exportSsgSwEnvParam(int p);
    void importSsgSwEnv11Param(int p);
    void exportSsgSwEnv11Param(int p);
    void importSsgSwPEnv11Param(int p);
    void exportSsgSwPEnv11Param(int p);
    void importDetuneParam(int p);
    void exportDetuneParam(int p);
    void importQualityParam(int p);
    void exportQualityParam(int p);
    void importPcmPlayParam(int p);
    void exportPcmPlayParam(int p);
    void importChParam();

    // ブラウザから直に読ませるための入口。
    // importChParam() からダイアログを外したもの。
    void applyChParamFile(const juce::File& file) override;
    // 3.0.0 より前の形式を読む
    void getImportingPadParams(int p, juce::StringArray& lines, int& index);
    void setImportingChParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeChParams(Io::ParamWriter& writer);
    void exportChParam();
    // ブラウザから直に渡せるようにした入口。
    void writeChParamFile(const juce::File& file);
    void importPadChParam(int p);

    // ブラウザから直に読ませるための入口。
    void applyPadChParamFile(int p, const juce::File& file);
    // 3.0.0 より前の形式を読む
    void setImportingPadChParams(int p, juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writePadChParams(int p, Io::ParamWriter& writer);
    void exportPadChParam(int p);
    // ブラウザから直に渡せるようにした入口。
    void writePadChParamFile(int p, const juce::File& file);
};
