#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Components/Fix/Fix.h"
#include "../../Gui/Components/Unison/Unison.h"
#include "../../Gui/Components/MulDetune/MulDetune.h"
#include "../../Gui/Components/AmpEnv/AmpEnv.h"
#include "../../Gui/Components/PitchEnv/PitchEnv.h"
#include "../../Gui/Components/SsgSwEnv/SsgSwEnv.h"
#include "../../Gui/Components/Midi/Midi.h"
#include "../../Gui/Components/LfoOpzx7/LfoOpzx7.h"
#include "../../Gui/Components/PresetName/PresetName.h"
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

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiAdpcm : public GuiBase
{
    // --- ADPCM Page ---
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiCategoryLabel formCat;

	QualityPcm qualityPcmComponent;

    GuiTextButton loadButton;
    GuiTextButton clearButton;
    GuiLabel fileNameLabel;

    NormalSeparator formSeparator;

    GuiCategoryLabel optionalCat;

    // 読み込んだサンプルを見せるプレビュー。
    // P.OF / P.RT で切り出した範囲を描き、ループ位置を縦線で出す。
    GuiWavePreview samplePreview;

    GuiSlider pcmOffsetSlider;
    GuiSlider pcmRatioSlider;

	GuiComponentLevel levelComponent;

    GuiSlider toneSlider;
    GuiSlider noiseSlider;
    GuiSlider noiseFreqSlider;
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
    GuiComponentSsgSwEnv ssgSwEnvComponent;

	GuiComponentSsgSwEnv11 ssgSwEnv11Component;
	GuiComponentSsgSwPEnv11 ssgSwPEnv11Component;

    // Detune
    GuiComponentMulDetune mulDetuneComponent;

    GuiComponentLfoOpzx7 lfoComponent;

    GuiComponentMidi midiComponent;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
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
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieChParam;
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

    bool isUpdatingGraph = false;

    void updateGraph();
    void setGraphMode(GraphMode mode);
public:
    GuiAdpcm(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        presetName(context),
        formCat(context),
		qualityPcmComponent(context),
        loadButton(context),
        clearButton(context),
        fileNameLabel(context),
        formSeparator(context),
        optionalCat(context),
        samplePreview(context),
        pcmOffsetSlider(context),
        pcmRatioSlider(context),
        levelComponent(context),
        toneSlider(context),
        noiseSlider(context),
        noiseFreqSlider(context),
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
        ssgSwEnvComponent(context),
		ssgSwEnv11Component(context),
		ssgSwPEnv11Component(context),
		mulDetuneComponent(context),
        lfoComponent(context),
        midiComponent(context),
        utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        ieToneNoise(context),
        ieLfo(context),
        ieAmpEnv(context),
        iePitchEnv(context),
        ieSsgHwEnv(context),
        ieSsgHwPEnv(context),
        ieWtMod(context),
        ieWtAmpMod(context),
        ieSsgSwEnv(context),
        ieSsgSwEnv11(context),
        ieSsgSwPEnv11(context),
        ieDetune(context),
        ieQuality(context),
        iePcmPlay(context),
        ieUnison(context),
		ieChParam(context),
        graphBtnAmp(context),
        graphBtnPitch(context),
        graphBtnSsg(context),
        graphBtnSsg11(context),
        graphBtnSsgP11(context),
        graphSeparator(context)
    {
        currentGraphMode = GraphMode::Amp; // 初期状態はAmp
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void updateFileName(const juce::String& fileName);
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

    // 3.0.0 より前の形式を読む
    void setImportingToneNoiseParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeToneNoiseParams(Io::ParamWriter& writer);
    void exportToneNoiseParam();
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
    void importChParam();

    // 3.0.0 より前の形式を読む
    void setImportingChParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeChParams(Io::ParamWriter& writer);
    void exportChParam();
};
