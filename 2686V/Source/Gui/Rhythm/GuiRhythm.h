#pragma once

#include <JuceHeader.h>

#include "../../Core/Io/ParamFile.h"
#include <array>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
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
#include "../../Gui/Components/ViewMode/ViewMode.h"
#include "../../Gui/Components/ImportExport/ImportExport.h"
#include "../../Gui/Components/Level/Level.h"
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

class RhythmPadGui: public GuiBase
{
    GuiScrollGroup mainGroup;

    GuiLabel fileNameLabel;
    GuiTextButton loadButton;
    GuiTextButton clearButton;

    NormalSeparator formSeparator;

    GuiCategoryLabel formCat;
    GuiCategoryLabel optionalCat;

    // どのパッドかを覚えておく。プレビューで読むサンプルを選ぶのに使う。
    int m_padIndex = 0;

    // 読み込んだサンプルを見せるプレビュー。
    // P.OF / P.RT で切り出した範囲を描き、ループ位置を縦線で出す。
    GuiWavePreview samplePreview;

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

    GuiEnvelopeGraph graph;
    GuiToggleButton graphBtnAmp;
    GuiToggleButton graphBtnPitch;
    GuiToggleButton graphBtnSsg;
    GuiToggleButton graphBtnSsg11;
    GuiToggleButton graphBtnSsgP11;
    NormalSeparator graphSeparator;

    enum class GraphMode { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };
    GraphMode currentGraphMode;

    CurveCore* p_curveCore = nullptr;
    GuiCurve* p_guiCurve = nullptr;

    bool isUpdatingGraph = false;

    void updateGraph();
    void setGraphMode(GraphMode mode);
public:
    void setImportingParams(int p, juce::StringArray& lines, int& index);
    RhythmPadGui(const GuiContext& context) :
		GuiBase(context),
        mainGroup(context),
        fileNameLabel(context),
        loadButton(context),
        clearButton(context),
        formSeparator(context),
        formCat(context),
        optionalCat(context),
        samplePreview(context),
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
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphBtnSsg(context),
        graphBtnSsg11(context),
        graphBtnSsgP11(context),
        graphSeparator(context)
    {
        currentGraphMode = GraphMode::Amp; // 初期状態はAmp
    }

    void updatePadFileName(const juce::String& fileName);
    void updateSamplePreview();
    void updatePadVisible(bool visible);
    void setup(juce::Component& parent, int index, juce::String padName, int& tabOrder);
	void layout(juce::Rectangle<int> content);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    bool isThis(juce::Button* button);
    void layoutFormCat(Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutPanCat(juce::Rectangle<int>& rect);
    void layoutOptionalCat(juce::Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
    void copyParams(CopyRhythmPad& copyObj);
    void pasteParams(CopyRhythmPad& copyObj);
    void importToneNoiseParam();

    // 3.0.0 より前の形式を読む
    void setImportingToneNoiseParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeToneNoiseParams(Io::ParamWriter& writer);
    void exportToneNoiseParam();
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

    // 3.0.0 より前の形式を読む
    void setImportingQualityParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeQualityParams(Io::ParamWriter& writer);
    void exportQualityParam();
    void importPcmPlayParam();

    // 3.0.0 より前の形式を読む
    void setImportingPcmPlayParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writePcmPlayParams(Io::ParamWriter& writer);
    void exportPcmPlayParam();
    // 名前で受け渡す。パッドは並びの中のひとつを渡す。
    void readParams(int p, const Io::ParamReader& r);
    void writeParams(int p, Io::ParamWriter& w);
};

class GuiRhythm : public GuiBase
{
    GuiComponentViewModes viewMode = GuiComponentViewModes::Twin;

    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;
    GuiComponentViewMode viewModeComp;

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
    GuiSlider targerPadSlider;
    NormalSeparator uSep003;
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    // 8 Pads
    std::array<RhythmPadGui, RhythmPrValue::pads> pads;
public:
    GuiRhythm(const GuiContext& context);
                     
    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void layoutPad(int padIndex, juce::Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void removeLoadButtonListener(AudioPlugin2686VEditor* editor);
    void buttonClicked(juce::Button* button, juce::AudioFormatManager &formatManager, std::unique_ptr<juce::FileChooser>& fileChooser);
	void updatePadFileName(int padIndex, const juce::String& fileName);
    bool isThis(int padIndex, juce::Button* button);
    void updatePadVisible(int idx, bool visible);
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

    // 3.0.0 より前の形式を読む
    void getImportingPadParams(int p, juce::StringArray& lines, int& index);
    void setImportingChParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeChParams(Io::ParamWriter& writer);
    void exportChParam();
    void importPadChParam(int p);

    // 3.0.0 より前の形式を読む
    void setImportingPadChParams(int p, juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writePadChParams(int p, Io::ParamWriter& writer);
    void exportPadChParam(int p);
};
