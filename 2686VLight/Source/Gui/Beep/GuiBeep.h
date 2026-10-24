#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
#include "Shared/Gui/Components/TapSeq/TapSeq.h"
#include "Shared/Gui/Components/TapParam/TapParam.h"
#include "Shared/Gui/Components/RatioEnv/RatioEnv.h"
#include "Shared/Gui/Components/Unison/Unison.h"
#include "Shared/Gui/Components/Fix/Fix.h"
#include "Shared/Gui/Components/MulDetune/MulDetune.h"
#include "Shared/Gui/Components/AmpEnv/AmpEnv.h"
#include "Shared/Gui/Components/PitchEnv/PitchEnv.h"
#include "Shared/Gui/Components/Midi/Midi.h"
#include "Shared/Gui/Components/Edit/Edit.h"
#include "Shared/Gui/Components/LfoOpzx7/LfoOpzx7.h"
#include "Shared/Gui/Components/PresetName/PresetName.h"
#include "Shared/Gui/Components/ImportExport/ImportExport.h"
#include "Shared/Gui/Components/Level/Level.h"
#include "Shared/Gui/Components/Separator/NormalSeparator.h"
#include "Shared/Gui/Components/Separator/ShortSeparator.h"
#include "Shared/Gui/Components/SsgSwEnv256/SsgSwEnv256.h"
#include "Shared/Gui/Components/SsgSwPEnv256/SsgSwPEnv256.h"
#include "Shared/Gui/Components/SsgHwEnv/SsgHwEnv.h"
#include "Shared/Gui/Components/SsgHwPEnv/SsgHwPEnv.h"
#include "Shared/Gui/Components/WtMod/WtMod.h"
#include "Shared/Gui/Components/WtAmpMod/WtAmpMod.h"

class AudioPlugin2686V;
class AudioPlugin2686VEditor;

class GuiBeep : public GuiBase {
    GuiScrollGroup mainGroup;

    GuiComponentPresetName presetName;

    GuiComponentLevel levelComponent;

	GuiComponentFix fixComponent;

    // UNISON/HARMONY
    GuiComponentUnison unisonComponent;

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

    // SSG Hw Env
    GuiComponentSsgHwEnv ssgHwEnv;
    GuiComponentSsgHwPEnv ssgHwPEnv;

    GuiComponentMidi midiComponent;

    // EDIT 区分 (切り取り・コピー・貼り付け・初期化)
    GuiComponentEdit editComponent;

    GuiCategoryLabel optionalCat;
    GuiToggleButton antiAliasButton;
    GuiComboBox timerClockSelector;
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
    GuiTextButton broadcastLevelButton;
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

    bool isUpdatingGraph = false;

    void updateGraph();
    void setGraphMode(GraphMode mode);
public:
    GuiBeep(const GuiContext& context) : GuiBase(context),
        mainGroup(context),
        presetName(context),
        levelComponent(context),
        fixComponent(context),
        ssgHwEnv(context),
        ssgHwPEnv(context),
        unisonComponent(context),
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
        optionalCat(context),
        antiAliasButton(context),
        timerClockSelector(context),
        utilityCat(context),
        ampMajorCat(context),
        pitchMajorCat(context),
        tapMajorCat(context),
        tapAmpSeq(context, GuiComponentTapSeq::Kind::Amp),
        tapPitchSeq(context, GuiComponentTapSeq::Kind::Pitch),
        tapParamSeq(context, ParamModCatalog::beep()),
        paramEnvMajorCat(context),
        paramEnv(context, GuiComponentParamEnv::channelSpec(ParamModCatalog::beep(), 0)),
        broadcastLevelButton(context),
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

    // 簡易表示モードで隠す区分への一括操作
    void bypassHiddenCategories() override;
    void openEnabledCategories() override;
    void closeBypassedCategories() override;
    void layout(juce::Rectangle<int> content) override;
    void updatePresetName(const juce::String& name);
    void initParams();
    void setupGraph();
    void layoutOptionalCat(Rectangle<int>& rect);
    void layoutUtilityCat(Rectangle<int>& rect);
    void layoutGraph(juce::Rectangle<int>& rect);
    void setLevel(float level);
    void importChParam();

    // ブラウザから直に読ませるための入口。
    // importChParam() からダイアログを外したもの。
    void applyChParamFile(const juce::File& file) override;
    // 3.0.0 より前の形式を読む
    void setImportingChParams(juce::StringArray& lines, int& index);

    // 書き出す中身。エクスポートと変換の両方から使う。
    void writeChParams(Io::ParamWriter& writer);
    void exportChParam();
    // ブラウザから直に渡せるようにした入口。
    void writeChParamFile(const juce::File& file);
};
