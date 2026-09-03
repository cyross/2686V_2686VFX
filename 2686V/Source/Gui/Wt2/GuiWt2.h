#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Gui/GuiColor.h"
#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Gui/Components/ParamBarEditor/ParamBarEditor.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../Gui/Curve/GuiCurve.h"
#include "../../Advanced/Curve/AdvancedCurve.h"
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
#include "../../Gui/Components/Quality/Quality.h"
#include "../../Gui/Components/WtMod/WtMod.h"
#include "../../Gui/Components/WtAmpMod/WtAmpMod.h"
#include "../../Gui/Components/SsgSwEnv11/SsgSwEnv11.h"
#include "../../Gui/Components/SsgSwPEnv11/SsgSwPEnv11.h"
#include "../../Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "../../Gui/Components/SsgHwPEnv/SsgHwPEnv.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

// ==========================================================
// Waveform Drawing Container (Super Lightweight Custom Paint)
// ==========================================================
template <size_t tableSize>
class Waveform2Container : public ParamBarEditorBase
{
    int hoveredY = -1; // Y軸のホバー位置

public:
    Waveform2Container(const GuiContext& context);

    int resolution = 16;
    int resCenter = 8;

    struct Config {
        juce::Component& parent;
        juce::String idPrefix;
    };

    void setup(const Config& c);
    void setAllValues(int val);
    void setValues(const std::vector<int>& values);
    std::vector<int> getValues();
    void paint(juce::Graphics& g) override;

    void updateSliderValue(const juce::MouseEvent& e) override;
    void updateHoverState(const juce::MouseEvent& e) override;
    void paintOverChildren(juce::Graphics& g) override;
};

class GuiWt2 : public GuiBase
{
    int resolution = 16;
    int resCenter = 8;

    juce::Font labelFont = juce::Font(juce::FontOptions(16.0f));

    // Groups
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiScrollGroup customWaveGroup;

    Waveform2Container<32> customSliders32;
    Waveform2Container<64> customSliders64;
    Waveform2Container<128> customSliders128;
    Waveform2Container<256> customSliders256;

    GuiComponentLevel levelComponent;

    Quality qualityComponent;

    GuiCategoryLabel formCat;
    GuiComponentWtMod modComponent;
    GuiComponentWtAmpMod ampModComponent;

    GuiComponentFix fixComponent;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

    // Amp ADSR
    GuiComponentAmpEnv ampEnvComponent;

    // Pitch ADSR
    GuiComponentPitchEnv pitchEnvComponent;

    // SSG SW Env
    GuiComponentSsgSwEnv ssgSwEnvComponent;

    GuiComponentSsgSwEnv11 ssgSwEnv11Component;
    GuiComponentSsgSwPEnv11 ssgSwPEnv11Component;

    // Detune
    GuiComponentMulDetune mulDetuneComponent;

    // LFO
    GuiComponentLfoOpzx7 lfo;

    // SSG Hw Env
    GuiComponentSsgHwEnv ssgHwEnv;
    GuiComponentSsgHwPEnv ssgHwPEnv;

    GuiComboBox sizeSelector;
    GuiComboBox resoSelector;

    GuiComboBox waveSelector;

    NormalSeparator formSeparator;



	// Custom Waveform Reset
    GuiTextButton customWaveResetToCenterBtn;
    GuiTextButton customWaveResetToMaxBtn;
    GuiTextButton customWaveResetTo0Btn;

    GuiCategoryLabel utilityCat;
    GuiTextButton broadcastLevelButton;
    NormalSeparator uSep001;
    GuiTextButton customWaveImportBtn;
    GuiTextButton customWaveExportBtn;
    NormalSeparator uSep002;
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
    GuiComponentImportExport ieUnison;
    GuiComponentImportExport ieQuality;
    GuiComponentImportExport ieChParam;
    std::unique_ptr<juce::FileChooser> fileChooser;

    GuiComponentMidi midiComponent;

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
	GuiWt2(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        presetName(context),
        customWaveGroup(context),
        customSliders32(context),
        customSliders64(context),
        customSliders128(context),
        customSliders256(context),
        levelComponent(context),
		qualityComponent(context),
        formCat(context),
        modComponent(context),
        ampModComponent(context),
        fixComponent(context),
        ssgHwEnv(context),
        ssgHwPEnv(context),
        unisonComponent(context),
        ampEnvComponent(context),
        pitchEnvComponent(context),
        ssgSwEnvComponent(context),
        ssgSwEnv11Component(context),
        ssgSwPEnv11Component(context),
        mulDetuneComponent(context),
        lfo(context),
		utilityCat(context),
        broadcastLevelButton(context),
        uSep001(context),
        customWaveImportBtn(context),
        customWaveExportBtn(context),
        uSep002(context),
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
        ieUnison(context),
        ieQuality(context),
        ieChParam(context),
        sizeSelector(context),
        resoSelector(context),
        waveSelector(context),
        formSeparator(context),
        customWaveResetToCenterBtn(context),
        customWaveResetToMaxBtn(context),
        customWaveResetTo0Btn(context),
        midiComponent(context),
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

    // 簡易表示モードで隠す区分への一括操作
    void bypassHiddenCategories() override;
    void openEnabledCategories() override;
    void closeBypassedCategories() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePresetName(const juce::String& name);
    void importWavetable();
    void exportWavetable();
    void initParams();
    void layoutFormCat(Rectangle<int>& rect);
    void layoutQualityCat(juce::Rectangle<int>& rect);
    void layoutUtilityCat(juce::Rectangle<int>& rect);
    void setupGraph();
    void layoutGraph(juce::Rectangle<int>& rect);
    void setLevel(float level);
    void importQualityParam();

    // 3.0.0 より前の形式を読む
    void setImportingQualityParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeQualityParams(Io::ParamWriter& writer);
    void exportQualityParam();
    void importChParam();

    // 3.0.0 より前の形式を読む
    void setImportingChParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeChParams(Io::ParamWriter& writer);
    void exportChParam();
};
