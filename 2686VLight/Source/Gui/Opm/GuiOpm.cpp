#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiOpm.h"

#include "../../Core/Gui/GuiRefresh.h"

#include "../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat qualityFormat{ "quality", 1 };
	const Io::ParamFormat opmLfoFormat{ "opmLfo", 1 };
	const Io::ParamFormat opmFormat{ "opm", 1 };
	const Io::ParamFormat opmOpFormat{ "opmOp", 1 };
}

#include "../Components/WavePreview/WavePreviewSource.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Opm/ProcessorOpmKeys.h"
#include "../../Processor/Opm/ProcessorOpmValues.h"
#include "../../Effect/Envelope/Amp/FmRgAdddr/EnvFmRgAdddrParams.h"
#include "../../Core/Const/ConstMmlKeys.h"
#include "../../Core/Const/ConstMmlValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Fm/FmRegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiGraphValues.h"
#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiOpmValues.h"
#include "./GuiOpmText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> opmAlgItems = {
    {.name = "00: <OPM-00>", .value = 1 },
    {.name = "01: <OPM-01>", .value = 2 },
    {.name = "02: <OPM-02>", .value = 3 },
    {.name = "03: <OPM-03>", .value = 4 },
    {.name = "04: <OPM-04>", .value = 5 },
    {.name = "05: <OPM-05>", .value = 6 },
    {.name = "06: <OPM-06>", .value = 7 },
    {.name = "07: <OPM-07>", .value = 8 },
};

static std::vector<SelectItem> multems = {
    {.name = " 0: x  0.5", .value = 1 },
    {.name = " 1: x  1", .value = 2 },
    {.name = " 2: x  2", .value = 3 },
    {.name = " 3: x  3", .value = 4 },
    {.name = " 4: x  4", .value = 5 },
    {.name = " 5: x  5", .value = 6 },
    {.name = " 6: x  6", .value = 7 },
    {.name = " 7: x  7", .value = 8 },
    {.name = " 8: x  8", .value = 9 },
    {.name = " 9: x  9", .value = 10 },
    {.name = "10: x 10", .value = 11 },
    {.name = "11: x 11", .value = 12 },
    {.name = "12: x 12", .value = 13 },
    {.name = "13: x 13", .value = 14 },
    {.name = "14: x 14", .value = 15 },
    {.name = "15: x 15", .value = 16 },
    {.name = "16: Use Ratio", .value = 17 }
};

static std::vector<SelectItem> ksItems = {
    {.name = "0 OFF",      .value = 1},
    {.name = "1 (Weak)",   .value = 2},
    {.name = "2 (Mid)",    .value = 3},
    {.name = "3 (Strong)", .value = 4}
};

static std::vector<SelectItem> lfoShapeItems = {
    {.name = "0: Sine",     .value = 1 },
    {.name = "1: Saw Down", .value = 2 },
    {.name = "2: Square",   .value = 3 },
    {.name = "3: Triangle", .value = 4 },
    {.name = "4: Noise",    .value = 5 },
};

static std::vector<SelectItem> lfoPgShapeItems = {
    {.name = "0: Saw Up",   .value = 1 },
    {.name = "1: Square",   .value = 2 },
    {.name = "2: Triangle", .value = 3 },
    {.name = "3: Noise",    .value = 4 },
};

static std::vector<SelectItem> lfoEgShapeItems = {
    {.name = "0: Saw Down", .value = 1 },
    {.name = "1: Square",   .value = 2 },
    {.name = "2: Triangle", .value = 3 },
    {.name = "3: Noise",    .value = 4 },
};

static std::vector<SelectItem> pmsItems = {
    {.name = "0: 0cent", .value = 1 },
    {.name = "1: +-5cent", .value = 2 },
    {.name = "2: +-10cent", .value = 3 },
    {.name = "3: +-20cent", .value = 4 },
    {.name = "4: +-50cent", .value = 5 },
    {.name = "5: +-100cent", .value = 6 },
    {.name = "6: +-400cent", .value = 7 },
    {.name = "7: +-700cent", .value = 8 },
};

static std::vector<SelectItem> amsItems = {
    {.name = "1: 0dB", .value = 1 },
    {.name = "2: 23.9dB", .value = 2 },
    {.name = "3: 47.8dB", .value = 3 },
    {.name = "4: 95.6dB", .value = 4 },
};

static std::vector<SelectItem> ksModeItems = {
    {.name = "0: OPM", .value = 1 },
    {.name = "1: OPP", .value = 2 }
};

void GuiOpm::setup()
{
    auto setupPanBtn = [this](GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            mainGroup.contentCanvas.addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    const juce::String code = OpmPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, OpmGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    qualityComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    algFbCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpmGuiText::Category::algFb });

    algSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::alg, .title = OpmGuiText::Fm::alg, .items = opmAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    mainGroup.contentCanvas.addAndMakeVisible(&algStaticGraphComp);

    algFbSep.setupComponent(mainGroup.contentCanvas);

    feedbackSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::fb, .title = OpmGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    panCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpmGuiText::Category::pan, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::pan, .title = OpmGuiText::Fm::pan, .isReset = true });
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    panToLBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = OpmGuiText::Fm::Pan::l, .isReset = false });
    panToLBtn.setWantsKeyboardFocus(true);
    panToLBtn.setExplicitFocusOrder(++tabOrder);
    panToLBtn.onClick = [this]() {
        panSlider.setValue(-1, juce::sendNotification);
        };

    panToCBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = OpmGuiText::Fm::Pan::c, .isReset = false });
    panToCBtn.setWantsKeyboardFocus(true);
    panToCBtn.setExplicitFocusOrder(++tabOrder);
    panToCBtn.onClick = [this]() {
        panSlider.setValue(0, juce::sendNotification);
        };

    panToRBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = OpmGuiText::Fm::Pan::r, .isReset = false });
    panToRBtn.setWantsKeyboardFocus(true);
    panToRBtn.setExplicitFocusOrder(++tabOrder);
    panToRBtn.onClick = [this]() {
        panSlider.setValue(1, juce::sendNotification);
        };

    lfoCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpmGuiText::Category::hwLfo, .enableChangeDetailVisible = true });

    lfoFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::OpmLfo::freq, .title = OpmGuiText::Fm::lfoFreq, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::OpmLfo::amSmoothRatio, .title = OpmGuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);


    lfoSyncDelaySlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::OpmLfo::syncDelay, .title = OpmGuiText::Fm::lfoSyncDelay, .isReset = true });
    lfoSyncDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoSyncDelaySlider.setWantsKeyboardFocus(true);
    lfoSyncDelaySlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelayToZeroBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Async", .isReset = false, .isResized = false });
    lfoSyncDelayToZeroBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToZeroBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToZeroBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(0.0f);
        };

    lfoSyncDelayToOneBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Sync", .isReset = false, .isResized = false });
    lfoSyncDelayToOneBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToOneBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToOneBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(1.0f);
        };

    lfoPmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::OpmLfo::pm, .title = OpmGuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPgShapeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::OpmLfo::pgShape, .title = OpmGuiText::Fm::lfoPgShape, .items = lfoPgShapeItems, .isReset = true });
    lfoPgShapeSelector.setWantsKeyboardFocus(true);
    lfoPgShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmsSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::OpmLfo::pms, .title = OpmGuiText::Fm::pms, .items = pmsItems, .isReset = true });
    lfoPmsSelector.setWantsKeyboardFocus(true);
    lfoPmsSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::OpmLfo::pmd, .title = OpmGuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::OpmLfo::am, .title = OpmGuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoEgShapeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::OpmLfo::egShape, .title = OpmGuiText::Fm::lfoEgShape, .items = lfoEgShapeItems, .isReset = true });
    lfoEgShapeSelector.setWantsKeyboardFocus(true);
    lfoEgShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmPreview.setup(mainGroup.contentCanvas, GuiColor::WavePreview::Lfo);
    lfoAmPreview.setup(mainGroup.contentCanvas, GuiColor::WavePreview::Lfo);

    // 各コンポーネントの setup() より後に付けること。setup() は APVTS との
    // 束縛を張り直すので、先に付けると束縛が壊れて操作できなくなる。
    auto refreshLfoPreviews = [this]() { this->updateLfoPreviews(); };

    lfoPgShapeSelector.onChange = refreshLfoPreviews;
    lfoEgShapeSelector.onChange = refreshLfoPreviews;
    lfoAmSmRtSlider.onValueChange = refreshLfoPreviews;

    updateLfoPreviews();

    lfoAmsSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::OpmLfo::ams, .title = OpmGuiText::Fm::ams, .items = amsItems, .isReset = true });
    lfoAmsSelector.setWantsKeyboardFocus(true);
    lfoAmsSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::OpmLfo::amd, .title = OpmGuiText::Fm::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

	lfoSep1.setupComponent(mainGroup.contentCanvas);
    lfoSep2.setupComponent(mainGroup.contentCanvas);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    modComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ampModComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgHwEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgHwPEnv.setupComponent(mainGroup.contentCanvas, code, tabOrder);
    ssgSwEnv11g.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwEnv11 + CPK::bypass, "Bypass");
    ssgSwPEnv11g.setupComponent(mainGroup.contentCanvas, code, tabOrder, CPK::ssgSwPEnv11 + CPK::bypass, "Bypass");

    setupGlobalGraph();

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = OpmGuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = OpmGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    copyParamsToOpnaBtn.setup({ .parent = mainGroup.contentCanvas, .title = "OP Params -> OPNA", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyParamsToOpnaBtn.setWantsKeyboardFocus(true);
    copyParamsToOpnaBtn.setExplicitFocusOrder(++tabOrder);
    copyParamsToOpnaBtn.onClick = [this] {
        ctx.editor.copyOpmParamsToOpna();
        };

    copyParamsToOpnBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Params -> OPN", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyParamsToOpnBtn.setWantsKeyboardFocus(true);
    copyParamsToOpnBtn.setExplicitFocusOrder(++tabOrder);
    copyParamsToOpnBtn.onClick = [this] {
        ctx.editor.copyOpmParamsToOpn();
        };

    uSep002.setupComponent(mainGroup.contentCanvas);

    copyOpParamBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Copy Op Params", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamBtn.setWantsKeyboardFocus(true);
    copyOpParamBtn.setExplicitFocusOrder(++tabOrder);
    copyOpParamBtn.onClick = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        ctx.editor.copyOpmOpParams(from, to);
        };

    copyOpFromSlider.setup({ .parent = mainGroup.contentCanvas, .title = "FROM", .isReset = false });
    copyOpFromSlider.setRange(1.0, 4.0, 1.0);
    copyOpFromSlider.setNumDecimalPlacesToDisplay(0);
    copyOpFromSlider.setValue(1, juce::sendNotification);
    copyOpFromSlider.setWantsKeyboardFocus(true);
    copyOpFromSlider.setExplicitFocusOrder(++tabOrder);
    copyOpFromSlider.onValueChange = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        copyOpParamBtn.setEnabled(from != to);
        };

    copyOpToSlider.setup({ .parent = mainGroup.contentCanvas, .title = "TO", .isReset = false });
    copyOpToSlider.setRange(1.0, 4.0, 1.0);
    copyOpToSlider.setNumDecimalPlacesToDisplay(0);
    copyOpToSlider.setValue(2, juce::sendNotification);
    copyOpToSlider.setWantsKeyboardFocus(true);
    copyOpToSlider.setExplicitFocusOrder(++tabOrder);
    copyOpToSlider.onValueChange = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        copyOpParamBtn.setEnabled(from != to);
        };

    uSep003.setupComponent(mainGroup.contentCanvas);

    ieOpPitchEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Pitch Env");
    ieOpPitchEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importPitchEnvParam(opIndex); };
    ieOpPitchEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportPitchEnvParam(opIndex); };

    ieOpSsgSwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW Env");
    ieOpSsgSwEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importSsgSwEnvParam(opIndex); };
    ieOpSsgSwEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportSsgSwEnvParam(opIndex); };

    ieOpSsgSwEnv11.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW E11");
    ieOpSsgSwEnv11.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importSsgSwEnv11Param(opIndex); };
    ieOpSsgSwEnv11.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportSsgSwEnv11Param(opIndex); };

    ieOpSsgSwPEnv11.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW P11");
    ieOpSsgSwPEnv11.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importSsgSwPEnv11Param(opIndex); };
    ieOpSsgSwPEnv11.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportSsgSwPEnv11Param(opIndex); };
    ieOpSsgHwPEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG HW PEnv");
    ieOpSsgHwPEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpSsgHwPEnvParam(opIndex); };
    ieOpSsgHwPEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpSsgHwPEnvParam(opIndex); };
    ieOpWtAmpMod.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Amp Mod");
    ieOpWtAmpMod.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpWtAmpModParam(opIndex); };
    ieOpWtAmpMod.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpWtAmpModParam(opIndex); };
    ieOpSsgHwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG HW Env");
    ieOpSsgHwEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpSsgHwEnvParam(opIndex); };
    ieOpSsgHwEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpSsgHwEnvParam(opIndex); };
    ieOpWtMod.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Modulation");
    ieOpWtMod.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpWtModParam(opIndex); };
    ieOpWtMod.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpWtModParam(opIndex); };

    ieOpChParam.setupComponentOp(mainGroup.contentCanvas, tabOrder, "OP Params");
    ieOpChParam.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpChParam(opIndex); };
    ieOpChParam.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpChParam(opIndex); };

    uSep004.setupComponent(mainGroup.contentCanvas);

    // TARGET。設定ひとそろいの帯へ置く。値が動いたら束縛と枠線を付け替える。
    targerOpSlider.setup({ .parent = *this, .title = "TARGET", .isReset = false });
    targerOpSlider.setRange(1.0, (double)OpmPrValue::ops, 1.0);
    targerOpSlider.setNumDecimalPlacesToDisplay(0);
    targerOpSlider.setWantsKeyboardFocus(true);
    targerOpSlider.setExplicitFocusOrder(++tabOrder);
    targerOpSlider.onValueChange = [this] { applyOpTarget(); };

    ieLfo.setupComponent(mainGroup.contentCanvas, tabOrder, "LFO");
    ieLfo.onClickImport = [this] { importLfoParam(); };
    ieLfo.onClickExport = [this] { exportLfoParam(); };

    ieAmpEnvG.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Env", ampEnvComponent);
    ieSsgHwEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW Env", ssgHwEnv);
    ieSsgHwPEnv.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG HW PEnv", ssgHwPEnv);
    ieWtMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Modulation", modComponent);
    ieWtAmpMod.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Amp Mod", ampModComponent);

    ieSsgSwEnv11.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW E11", ssgSwEnv11g);
    ieSsgSwPEnv11g.setupComponentFor(mainGroup.contentCanvas, tabOrder, "SSG SW P11", ssgSwPEnv11g);

    ieUnison.setupComponentFor(mainGroup.contentCanvas, tabOrder, "Unison", unisonComponent);

    ieQuality.setupComponent(mainGroup.contentCanvas, tabOrder, "Quality");
    ieQuality.onClickImport = [this] { importQualityParam(); };
    ieQuality.onClickExport = [this] { exportQualityParam(); };

    ieChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "CH Params");
    ieChParam.onClickImport = [this] { importChParam(); };
    ieChParam.onClickExport = [this] { exportChParam(); };

    // Operators
    const juce::String opCode = code + CPK::op;

    // ---- 上の絵と、下の設定 ----
    //
    // 絵はオペレータの数だけ。設定はひとそろいだけ置き、TARGET で
    // 指し先を切り替える。
    for (int i = 0; i < OpmPrValue::ops; ++i)
    {
        cells[(size_t)i].setup(*this, i, OpmGuiText::Group::opPrefix + juce::String(i + 1), false);
        cells[(size_t)i].onSelect = [this](int index) {
            targerOpSlider.setValue(index + 1, juce::sendNotification);
            };
    }

    // 区分は縦に積まず、横へ並べる。1 列 1 区分が基本。
    stripViewport.setViewedComponent(&stripCanvas, false);
    stripViewport.setScrollBarsShown(false, true);
    stripViewport.setOpaque(false);

    addAndMakeVisible(stripViewport);

    colAmp.setup(stripCanvas, juce::String("") + "AMP ENV / OPTIONAL");
    colSsgHwEnv.setup(stripCanvas, juce::String("") + "SSG HW AMP ENV");
    colSsgSwEnv.setup(stripCanvas, juce::String("") + "SSG SW AMP ENV");
    colSsgSwEnv11.setup(stripCanvas, juce::String("") + "SSG SW AMP ENV[11]");
    colAmpMod.setup(stripCanvas, juce::String("") + "WT AMP MOD");
    colPitchEnv.setup(stripCanvas, juce::String("") + "PITCH ENV");
    colSsgHwPEnv.setup(stripCanvas, juce::String("") + "SSG HW PITCH ENV");
    colSsgSwPEnv11.setup(stripCanvas, juce::String("") + "SSG SW PITCH ENV[11]");
    colMod.setup(stripCanvas, juce::String("") + "WT PITCH MOD");
    colKs.setup(stripCanvas, juce::String("") + "KEY SCALE / MUL/DET / FIX");
    colHwLfo.setup(stripCanvas, juce::String("") + "HW LFO");
    colMask.setup(stripCanvas, juce::String("") + "MASK / MML");

    // 今どのオペレータを触っているか。名前は rebind で入れ替える。
    opNameLabel.setup({ .parent = *this, .title = Io::empty,
        .justification = juce::Justification::centredLeft });

    opTargetSeparator.setupComponent(*this);

    // どのエンベロープを映すかの切り替え。TARGET のすぐ右へ置く。
    auto setupModeBtn = [this, &tabOrder](GuiToggleButton& btn, const juce::String& text,
        GuiEnvGraphMode mode) {
        btn.setup({ .parent = *this, .title = text, .isReset = false, .isResized = false });
        btn.setWantsKeyboardFocus(true);
        btn.setExplicitFocusOrder(++tabOrder);
        btn.onClick = [this, mode] { this->setGraphMode(mode); };
        };

    setupModeBtn(graphBtnAmp, "AMP", GuiEnvGraphMode::Amp);
    setupModeBtn(graphBtnPitch, "PIT", GuiEnvGraphMode::Pitch);
    setupModeBtn(graphBtnSsg, "SSG", GuiEnvGraphMode::SsgSw);
    setupModeBtn(graphBtnSsg11, "S11", GuiEnvGraphMode::SsgSw11);
    setupModeBtn(graphBtnSsgP11, "P11", GuiEnvGraphMode::SsgSwP11);

    graphBtnAmp.setToggleState(true, juce::dontSendNotification);

    // 組み立ての間だけ 1 番のオペレータへ繋いでおく。
    // 実際の指し先は、組み上がったあと rebind で入れ替える。
    const int i = 0;

    juce::String paramPrefix = opCode + juce::String(i);

    catDet.setupHwCategory({ .parent = colKs.contentCanvas, .title = OpmGuiText::Category::detune, .enableChangeDetailVisible = true });
    catAmp.setupHwCategory({ .parent = colAmp.contentCanvas, .title = OpmGuiText::Category::ampEnv, .detailVisible = true, .enableChangeDetailVisible = true });

    mul.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::mul, .title = OpmGuiText::Fm::Op::Mul, .items = multems, .isReset = true });
    mul.setWantsKeyboardFocus(true);
    mul.setExplicitFocusOrder(++tabOrder);
    mul.onChange = [this] {
        updateMulRatioEnable();
        };

    mulRatio.setupComponent(colKs.contentCanvas, paramPrefix + CPK::mulRatio, OpmGuiText::Fm::Op::MulRatio, tabOrder, std::nullopt);

    mulRatioTo001.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "0.01", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
		mulRatioTo001.setWantsKeyboardFocus(true);
		mulRatioTo001.setExplicitFocusOrder(++tabOrder);
		mulRatioTo001.onClick = [this, i]() {
			mulRatio.setValue(0.01f, juce::sendNotification);
			};

    mulRatioTo005.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "0.05", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    mulRatioTo005.setWantsKeyboardFocus(true);
    mulRatioTo005.setExplicitFocusOrder(++tabOrder);
    mulRatioTo005.onClick = [this, i]() {
        mulRatio.setValue(0.05f, juce::sendNotification);
        };

    mulRatioTo1.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "1", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    mulRatioTo1.setWantsKeyboardFocus(true);
    mulRatioTo1.setExplicitFocusOrder(++tabOrder);
    mulRatioTo1.onClick = [this, i]() {
        mulRatio.setValue(1.0f, juce::sendNotification);
        };

    mulRatioTo10.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "10", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    mulRatioTo10.setWantsKeyboardFocus(true);
    mulRatioTo10.setExplicitFocusOrder(++tabOrder);
    mulRatioTo10.onClick = [this, i]() {
        mulRatio.setValue(10.0f, juce::sendNotification);
        };

    mulRatioTo2757.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "27.57", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
    mulRatioTo2757.setWantsKeyboardFocus(true);
    mulRatioTo2757.setExplicitFocusOrder(++tabOrder);
    mulRatioTo2757.onClick = [this, i]() {
        mulRatio.setValue(27.57f, juce::sendNotification);
        };

    mulRatioTo02.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "0.2", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    mulRatioTo02.setWantsKeyboardFocus(true);
    mulRatioTo02.setExplicitFocusOrder(++tabOrder);
    mulRatioTo02.onClick = [this, i]() {
        mulRatio.setValue(0.2f, juce::sendNotification);
        };

    mulRatioTo025.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "0.25", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    mulRatioTo025.setWantsKeyboardFocus(true);
    mulRatioTo025.setExplicitFocusOrder(++tabOrder);
    mulRatioTo025.onClick = [this, i]() {
        mulRatio.setValue(0.25f, juce::sendNotification);
        };

    mulRatioTo04.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "0.4", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    mulRatioTo04.setWantsKeyboardFocus(true);
    mulRatioTo04.setExplicitFocusOrder(++tabOrder);
    mulRatioTo04.onClick = [this, i]() {
        mulRatio.setValue(0.4f, juce::sendNotification);
        };

    mulRatioTo05.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "0.5", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    mulRatioTo05.setWantsKeyboardFocus(true);
    mulRatioTo05.setExplicitFocusOrder(++tabOrder);
    mulRatioTo05.onClick = [this, i]() {
        mulRatio.setValue(0.5f, juce::sendNotification);
        };

    mulRatioTo06.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "0.6", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    mulRatioTo06.setWantsKeyboardFocus(true);
    mulRatioTo06.setExplicitFocusOrder(++tabOrder);
    mulRatioTo06.onClick = [this, i]() {
        mulRatio.setValue(0.6f, juce::sendNotification);
        };

    mulRatioTo075.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "0.75", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
    mulRatioTo075.setWantsKeyboardFocus(true);
    mulRatioTo075.setExplicitFocusOrder(++tabOrder);
    mulRatioTo075.onClick = [this, i]() {
        mulRatio.setValue(0.75f, juce::sendNotification);
        };

    mulRatioTo08.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "0.8", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
    mulRatioTo08.setWantsKeyboardFocus(true);
    mulRatioTo08.setExplicitFocusOrder(++tabOrder);
    mulRatioTo08.onClick = [this, i]() {
        mulRatio.setValue(0.8f, juce::sendNotification);
        };

    mulRatioPM10.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "-10", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioPM10.setWantsKeyboardFocus(true);
    mulRatioPM10.setExplicitFocusOrder(++tabOrder);
    mulRatioPM10.onClick = [this, i]() {
        mulRatio.setValue(mulRatio.getValue() - 10.0f, juce::sendNotification);
        };

    mulRatioP10.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "+10", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioP10.setWantsKeyboardFocus(true);
    mulRatioP10.setExplicitFocusOrder(++tabOrder);
    mulRatioP10.onClick = [this, i]() {
        mulRatio.setValue(mulRatio.getValue() + 10.0f, juce::sendNotification);
        };

    mulRatioPM1.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "-1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioPM1.setWantsKeyboardFocus(true);
    mulRatioPM1.setExplicitFocusOrder(++tabOrder);
    mulRatioPM1.onClick = [this, i]() {
        mulRatio.setValue(mulRatio.getValue() - 1.0f, juce::sendNotification);
        };

    mulRatioP1.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "+1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioP1.setWantsKeyboardFocus(true);
    mulRatioP1.setExplicitFocusOrder(++tabOrder);
    mulRatioP1.onClick = [this, i]() {
        mulRatio.setValue(mulRatio.getValue() + 1.0f, juce::sendNotification);
        };

    mulRatioPM01.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "-0.1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioPM01.setWantsKeyboardFocus(true);
    mulRatioPM01.setExplicitFocusOrder(++tabOrder);
    mulRatioPM01.onClick = [this, i]() {
        mulRatio.setValue(mulRatio.getValue() - 0.1f, juce::sendNotification);
        };

    mulRatioP01.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "+0.1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioP01.setWantsKeyboardFocus(true);
    mulRatioP01.setExplicitFocusOrder(++tabOrder);
    mulRatioP01.onClick = [this, i]() {
        mulRatio.setValue(mulRatio.getValue() + 0.1f, juce::sendNotification);
        };

    mulRatioPM001.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "-0.01", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioPM001.setWantsKeyboardFocus(true);
    mulRatioPM001.setExplicitFocusOrder(++tabOrder);
    mulRatioPM001.onClick = [this, i]() {
        mulRatio.setValue(mulRatio.getValue() - 0.01f, juce::sendNotification);
        };

    mulRatioP001.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .id = "", .title = "+0.01", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
    mulRatioP001.setWantsKeyboardFocus(true);
    mulRatioP001.setExplicitFocusOrder(++tabOrder);
    mulRatioP001.onClick = [this, i]() {
        mulRatio.setValue(mulRatio.getValue() + 0.01f, juce::sendNotification);
        };

    mulDetSep.setupComponent(colKs.contentCanvas);

    dt1.setup(GuiSlider::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::dt, .title = OpmGuiText::Fm::Op::Dt1, .isReset = true });
    dt1.setWantsKeyboardFocus(true);
    dt1.setExplicitFocusOrder(++tabOrder);

    dt2.setup(GuiSlider::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::dt2, .title = OpmGuiText::Fm::Op::Dt2, .isReset = true });
    dt2.setWantsKeyboardFocus(true);
    dt2.setExplicitFocusOrder(++tabOrder);

    rgAr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgAr, .title = OpmGuiText::Fm::Op::Ar, .isReset = true });
    rgAr.setWantsKeyboardFocus(true);
    rgAr.setExplicitFocusOrder(++tabOrder);

    rgD1r.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgD1r, .title = OpmGuiText::Fm::Op::D1r, .isReset = true });
    rgD1r.setWantsKeyboardFocus(true);
    rgD1r.setExplicitFocusOrder(++tabOrder);

    rgD1l.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgD1l, .title = OpmGuiText::Fm::Op::D1l, .isReset = true });
    rgD1l.setWantsKeyboardFocus(true);
    rgD1l.setExplicitFocusOrder(++tabOrder);

    rgD2r.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgD2r, .title = OpmGuiText::Fm::Op::D2r, .isReset = true });
    rgD2r.setWantsKeyboardFocus(true);
    rgD2r.setExplicitFocusOrder(++tabOrder);

    rgRr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgRr, .title = OpmGuiText::Fm::Op::Rr, .isReset = true });
    rgRr.setWantsKeyboardFocus(true);
    rgRr.setExplicitFocusOrder(++tabOrder);

    rgTl.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgTl, .title = OpmGuiText::Fm::Op::Tl, .isReset = true });
    rgTl.setWantsKeyboardFocus(true);
    rgTl.setExplicitFocusOrder(++tabOrder);

    ksCat.setupHwCategory({ .parent = colKs.contentCanvas, .title = OpmGuiText::Category::ks, .enableChangeDetailVisible = true });

    ksMode.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksMode, .title = "Mode", .items = ksModeItems, .isReset = true });
    ksMode.setWantsKeyboardFocus(true);
    ksMode.setExplicitFocusOrder(++tabOrder);
    ksMode.onChange = [this]() {
        ctx.editor.resized();
        };

    ks.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ks, .title = OpmGuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });
    ks.setWantsKeyboardFocus(true);
    ks.setExplicitFocusOrder(++tabOrder);

    ksrOPP.setup(GuiSlider::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksrOPP, .title = OpmGuiText::Fm::Op::Ksr, .isReset = true });
    ksrOPP.setWantsKeyboardFocus(true);
    ksrOPP.setExplicitFocusOrder(++tabOrder);

    kslOPP.setup(GuiSlider::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::kslOPP, .title = OpmGuiText::Fm::Op::Ksl, .isReset = true });
    kslOPP.setWantsKeyboardFocus(true);
    kslOPP.setExplicitFocusOrder(++tabOrder);

    catOptional.setupSwCategory({ .parent = colAmp.contentCanvas, .title = OpmGuiText::Category::optional, .enableChangeDetailVisible = true });

    opDelay.setupComponent(colAmp.contentCanvas, paramPrefix + CPK::delay, "DELAY", tabOrder, std::nullopt);

    opDelayNudge.setupComponent(colAmp.contentCanvas, opDelay.getSlider(), tabOrder);

    opDelaySeparator.setupComponent(colAmp.contentCanvas);

    xof.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::xof, .title = OpmGuiText::Fm::Op::xof, .isReset = true });
    xof.setWantsKeyboardFocus(true);
    xof.setExplicitFocusOrder(++tabOrder);

    kor.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::kor, .title = OpmGuiText::Fm::Op::kor, .isReset = true });
    kor.setWantsKeyboardFocus(true);
    kor.setExplicitFocusOrder(++tabOrder);

    bypass.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::bypass, .title = OpmGuiText::Fm::Op::bypass, .isReset = true });
    bypass.setWantsKeyboardFocus(true);
    bypass.setExplicitFocusOrder(++tabOrder);

    pitchEnv.setupComponent(colPitchEnv.contentCanvas, paramPrefix, tabOrder, CPK::PitchAdsr::enable, OpmGuiText::PitchAdsr::enable, true);

    ssgSwEnv.setupComponent(colSsgSwEnv.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv::enable, OpmGuiText::SsgSwEnv::enable, true);

    ssgSwEnv11.setupComponent(colSsgSwEnv11.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv11::enable, OpmGuiText::SsgSwEnv11::enable, true);

    ssgSwPEnv11.setupComponent(colSsgSwPEnv11.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwPEnv11::enable, OpmGuiText::SsgSwPEnv11::enable, true);
    ssgHwPEnvOp.setupComponent(colSsgHwPEnv.contentCanvas, paramPrefix, tabOrder);
    wtAmpModOp.setupComponent(colAmpMod.contentCanvas, paramPrefix, tabOrder);
    ssgHwEnvOp.setupComponent(colSsgHwEnv.contentCanvas, paramPrefix, tabOrder);
    wtModOp.setupComponent(colMod.contentCanvas, paramPrefix, tabOrder);

    cafLfo.setupHwCategory({ .parent = colHwLfo.contentCanvas, .title = OpmGuiText::Category::hwLfo, .enableChangeDetailVisible = true });

    amsEnable.setup(GuiToggleButton::Config{ .parent = colHwLfo.contentCanvas, .id = paramPrefix + CPK::OpmLfo::amsEn, .title = OpmGuiText::Fm::Op::AmsEn, .isReset = true });
    amsEnable.setWantsKeyboardFocus(true);
    amsEnable.setExplicitFocusOrder(++tabOrder);

    fix.setupComponent(colKs.contentCanvas, paramPrefix, tabOrder, OpmGuiText::Fm::Op::Opzx7FreqTo440, 440, true);

    catMask.setupHwCategory({ .parent = colMask.contentCanvas, .title = OpmGuiText::Category::mask, .enableChangeDetailVisible = true });

    mask.setup(GuiToggleButton::Config{ .parent = colMask.contentCanvas, .id = paramPrefix + CPK::Fm::mask, .title = OpmGuiText::Fm::Op::Mask, .isReset = true });
    mask.setWantsKeyboardFocus(true);
    mask.setExplicitFocusOrder(++tabOrder);

    mmlSeparator.setupComponent(colMask.contentCanvas);

    mml.setup({ .parent = colMask.contentCanvas, .title = juce::String("") + "MML風入力", .isReset = false, .isResized = false });
    mml.setWantsKeyboardFocus(true);
    mml.setExplicitFocusOrder(++tabOrder);
    // 札は TARGET が指しているオペレータへ入れる。番号は rebind で
    // 入れ替わるので、ここでは押されたときに引き直す。
    mml.onClick = [this] {
        const int op = currentOp();

        GuiMml::openDialog(this, op, mmlHint,
            [this](juce::String text) { this->applyMmlString(text); });
        };

    setupOpGraphWiring();

    // 区分の中身は最初から開いておく。1 列 1 区分にしたので、
    // 畳んだままだと見出しだけの列が並ぶことになる。
    for (auto* group : {
        &colAmp, &colSsgHwEnv, &colSsgSwEnv, &colSsgSwEnv11,
        &colAmpMod, &colPitchEnv, &colSsgHwPEnv, &colSsgSwPEnv11,
        &colMod, &colKs, &colHwLfo, &colMask,
        })
    {
        for (auto* child : group->contentCanvas.getChildren())
        {
            if (auto* cat = dynamic_cast<GuiCategoryLabel*>(child)) cat->setDetailVisible(true);
        }
    }

    // 前に開いていたときの指し先から始める。
    const int saved = (int)ctx.audioProcessor.apvts.state.getProperty(ProcessorStateKey::opmTarget, 0);

    targerOpSlider.setValue(juce::jlimit(0, OpmPrValue::ops - 1, saved) + 1, juce::dontSendNotification);

    applyOpTarget();
}

void GuiOpm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    auto mainArea = pageArea.removeFromLeft(OpmGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(OpmGuiValue::Group::Padding::width, OpmGuiValue::Group::Padding::height);
    mmRect.removeFromTop(OpmGuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    // グラフはスクロールさせない固定ヘッダに置く。
    // 他のチャンネルやオペレータと同じく、プリセット名の下に来る。
    layoutGlobalGraph(mmRect);
    updateGlobalGraph();

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.getContentWidth(), 2000);

    levelComponent.layoutComponent(mRect);

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });

    mRect.removeFromTop(OpmGuiValue::Category::paddingTop);

    auto graphArea = mRect.removeFromTop(150); // 描画領域確保
    algStaticGraphComp.setBounds(graphArea.reduced(10));

    algFbSep.layoutComponent(mRect);

    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    mRect.removeFromTop(CoreGuiValue::Category::gapBelow);

    ampEnvComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv));
    ampEnvComponent.layoutComponent(mRect);

    ssgHwEnv.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv));
    ssgHwEnv.layoutComponent(mRect);
    ssgSwEnv11g.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11));
    ssgSwEnv11g.layoutComponent(mRect);
    ampModComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));
    ampModComponent.layoutComponent(mRect);

    ssgHwPEnv.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv));
    ssgHwPEnv.layoutComponent(mRect);
    ssgSwPEnv11g.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11));
    ssgSwPEnv11g.layoutComponent(mRect);
    modComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));
    modComponent.layoutComponent(mRect);

    layoutHwLfoCat(mRect);

    layoutPanCat(mRect);

    unisonComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::Unison));
    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    // ---- 上は絵、下は設定 ----
    //
    // 絵は横 4 枚ずつ並べる。オペレータが 4 の倍数でないときは、
    // 足りないところを空けたままにする (詰めると番号と場所がずれる)。
    constexpr int cols = 4;
    constexpr int rows = (OpmPrValue::ops + cols - 1) / cols;

    const int cellW = pageArea.getWidth() / cols;
    const int cellH = GuiTargetCell::naturalHeight(false);

    auto cellsArea = pageArea.removeFromTop(cellH * rows);

    for (int r = 0; r < rows; ++r) {
        auto rowArea = cellsArea.removeFromTop(cellH);

        for (int col = 0; col < cols; ++col) {
            const int i = r * cols + col;

            auto cellArea = rowArea.removeFromLeft(cellW);

            if (i >= OpmPrValue::ops) continue;

            cells[(size_t)i].layout(cellArea);

            updateOpGraph(i);
        }
    }

    // 残りはすべて設定。
    layoutOpPanel(pageArea);
    updateAlgorithmDisplay();
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpm::applyMmlString(const juce::String& mml)
{
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mul.setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMl,   [&](int v) { mul.setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixDt,   [&](int v) { dt1.setValue(RegisterConverter::convertMmlDtToReg(v), juce::sendNotification); } },
        { mmlPrefixDt1,   [&](int v) { dt1.setValue(RegisterConverter::convertMmlDtToReg(v), juce::sendNotification); } },
        { mmlPrefixDto,   [&](int v) { dt1.setValue(RegisterConverter::convertMmlDtToReg(v), juce::sendNotification); } },
        { mmlPrefixDt2,  [&](int v) { dt2.setValue(RegisterConverter::convertMmlDt2ToReg(v), juce::sendNotification); } },
        { mmlPrefixDtt,  [&](int v) { dt2.setValue(RegisterConverter::convertMmlDt2ToReg(v), juce::sendNotification); } },
        { mmlPrefixKs,   [&](int v) { ks.setSelectedItemIndex(RegisterConverter::convertFmKs(v), juce::sendNotification); } },
        { mmlPrefixMask, [&](int v) { mask.setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },

        // --- TL系 (RGモードで分岐) ---
        { mmlPrefixTl,   [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixO,    [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixOl,   [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},

        // --- エンベロープ系 (RGモードで分岐) ---
        { mmlPrefixAr,   [&](int v) { rgAr.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixD1r,   [&](int v) { rgD1r.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixD2r,   [&](int v) { rgD2r.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixD1l,   [&](int v) { rgD1l.setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
        { mmlPrefixRr,   [&](int v) { rgRr.setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }}
    };

    for (const auto& rUnit : units)
    {
        // actionMapの中に rUnit.key と一致するものがあるか検索
        auto it = actionMap.find(rUnit.key);

        if (it != actionMap.end())
        {
            // 見つかったら、そこに登録されたラムダ式(関数)に value を渡して実行
            it->second(rUnit.value);
        }
    }
}

// 鳴っていないオペレータは触れないようにする。
//
// つまみは TARGET のぶんしか無いので、指しているオペレータの話で
// なければ何もしない。指し先を変えたときに引き直す。
void GuiOpm::updateOpEnable(int idx, bool enable)
{
    if (idx != currentOp()) return;

    mul.setEnabledWithLabel(enable);
    dt1.setEnabledWithLabel(enable);
    dt2.setEnabledWithLabel(enable);
    ks.setEnabledWithLabel(enable);
    catOptional.setEnabled(enable);
    opDelay.setEnabled(enable);
    opDelayNudge.setEnables(enable);
    xof.setEnabled(enable);
    kor.setEnabled(enable);
    bypass.setEnabled(enable);
    fix.setEnables(enable);
    catMask.setEnabled(enable);
    mask.setEnabled(enable);
    mmlSeparator.setEnabled(enable);
    mml.setEnabled(enable);
    pitchEnv.setEnabled(enable);
    ssgSwEnv.setEnabled(enable);
}

// MUL が Ratio のときだけ、比率のつまみを触れるようにする。
//
// つまみは TARGET のぶんしか無いので、指し先を変えるたびに引き直す。
void GuiOpm::updateMulRatioEnable()
{
    const bool enableMulRatio = (mul.getSelectedId() - 1) == 16; // mul = Ratio

    mulRatio.setEnabledWithLabel(enableMulRatio);
}

void GuiOpm::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > OpmPrValue::Alg::max) return;

    // Coreのルーティング情報から FmAlgState を生成
    const auto& routing = OpmCore::routings[algIndex];
    FmAlgState s;
    s.numOps = OpmPrValue::ops;

    for (int i = 0; i < s.numOps; ++i) { // i = src
        s.isCarrier[i] = (routing.out[i] > 0.0f);
        for (int j = 0; j < s.numOps; ++j) { // j = dest
            s.mod[i][j] = (routing.mod[j][i] > 0.0f);
            s.fbMod[i][j] = (routing.fbMod[j][i] > 0.0f);
        }
    }

    // 生成したステートでグラフを描画
    algStaticGraphComp.updateState(s);

    // AlgRouting から出力に到達可能なオペレータを動的に判定
    auto activeOps = s.getActiveOperators();

    for (int i = 0; i < OpmPrValue::ops; ++i)
    {
        // 配列を使わず、到達可能性から判定したフラグをセット
        opActive[(size_t)i] = activeOps[i];

        updateOpEnable(i, activeOps[i]);
    }
}

void GuiOpm::updateRgDisplayAsOp(bool rgMode)
{
    rgAr.setVisibleWithLabel(rgMode);
    rgD1r.setVisibleWithLabel(rgMode);
    rgD1l.setVisibleWithLabel(rgMode);
    rgD2r.setVisibleWithLabel(rgMode);
    rgRr.setVisibleWithLabel(rgMode);
    rgTl.setVisibleWithLabel(rgMode);
}

// TARGET が今どのオペレータを指しているか (0 から数える)。
int GuiOpm::currentOp() const
{
    return juce::jlimit(0, OpmPrValue::ops - 1, (int)targerOpSlider.getValue() - 1);
}

// TARGET が指すオペレータへ、設定の束縛を丸ごと移す。
//
// オペレータごとに部品を並べるのをやめたので、指し先はここで差し替える。
// 見た目は組み直さない。
void GuiOpm::rebind(int opIndex)
{
    const juce::String code = OpmPrKey::prefix + CPK::op + juce::String(opIndex);

    opNameLabel.setText(OpmGuiText::Group::opPrefix + juce::String(opIndex + 1), juce::dontSendNotification);

    mul.rebind(code + CPK::mul);
    mulRatio.getSlider().rebind(code + CPK::mulRatio);
    dt1.rebind(code + CPK::dt);
    dt2.rebind(code + CPK::dt2);

    ksMode.rebind(code + CPK::Fm::ksMode);
    ks.rebind(code + CPK::Fm::ks);
    ksrOPP.rebind(code + CPK::Fm::ksrOPP);
    kslOPP.rebind(code + CPK::Fm::kslOPP);

    rgAr.rebind(code + CPK::Fm::rgAr);
    rgD1r.rebind(code + CPK::Fm::rgD1r);
    rgD1l.rebind(code + CPK::Fm::rgD1l);
    rgD2r.rebind(code + CPK::Fm::rgD2r);
    rgRr.rebind(code + CPK::Fm::rgRr);
    rgTl.rebind(code + CPK::Fm::rgTl);

    opDelay.getSlider().rebind(code + CPK::delay);

    xof.rebind(code + CPK::Fm::xof);
    kor.rebind(code + CPK::Fm::kor);
    bypass.rebind(code + CPK::Fm::bypass);

    amsEnable.rebind(code + CPK::OpmLfo::amsEn);

    mask.rebind(code + CPK::Fm::mask);

    pitchEnv.rebind(code);
    ssgSwEnv.rebind(code);
    ssgSwEnv11.rebind(code);
    ssgSwPEnv11.rebind(code);
    ssgHwPEnvOp.rebind(code);
    wtAmpModOp.rebind(code);
    ssgHwEnvOp.rebind(code);
    wtModOp.rebind(code);
    fix.rebind(code);

    updateMulRatioEnable();
}

// 指し先を切り替える。
//
// 設定の束縛を移し、枠線を付け替える。開き直したときに続きから
// 触れるよう、指し先は覚えておく。
void GuiOpm::applyOpTarget()
{
    const int op = currentOp();

    ctx.audioProcessor.apvts.state.setProperty(ProcessorStateKey::opmTarget, op, nullptr);

    rebind(op);

    for (int i = 0; i < OpmPrValue::ops; ++i) cells[(size_t)i].setActive(i == op);

    // 鳴っていないオペレータは触れないようにしてある。指し先が変わったので
    // 入り切りを引き直す。
    updateAlgorithmDisplay();
}

// 指し先を一時的に動かして何かをする。
//
// 設定はひとそろいしか無いので、TARGET が指していないオペレータを
// 読み書きするには、いったんそこへ繋ぎ替えるしかない。
// 終わったら必ず元へ戻す。
void GuiOpm::withOp(int opIndex, const std::function<void()>& fn)
{
    const int keep = currentOp();

    if (opIndex != keep) rebind(opIndex);

    fn();

    if (opIndex != keep) rebind(keep);
}

// 絵をまとめて切り替える。
//
// どれを映すかは絵ごとではなく、全部そろえる。並べた絵を見比べる
// ためのものなので、一枚ずつ違うものを映しても比べようがない。
void GuiOpm::setGraphMode(GuiEnvGraphMode mode)
{
    currentGraphMode = mode;

    // 札は排他。押したものだけを入れる。
    graphBtnAmp.setToggleState(mode == GuiEnvGraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GuiEnvGraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GuiEnvGraphMode::SsgSw, juce::dontSendNotification);
    graphBtnSsg11.setToggleState(mode == GuiEnvGraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11.setToggleState(mode == GuiEnvGraphMode::SsgSwP11, juce::dontSendNotification);

    for (int i = 0; i < OpmPrValue::ops; ++i) updateOpGraph(i);
}

void GuiOpm::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

bool GuiOpm::keyPressed(const juce::KeyPress& key)
{
    // Ctrl を押しているときは TARGET を動かす。数字だけを押したときの
    // MML より先に見ること。あちらは修飾キーを見ていないので、
    // 後ろへ置くと Ctrl + 数字まで MML が食べてしまう。
    if (moveTargetByKey(targerOpSlider, key)) return true;

    int opIndex = -1;
    int code = key.getKeyCode();
    juce::ModifierKeys metaKeys = key.getModifiers();

    // 通常の 1〜4キー、または テンキーの 1〜4 を判定
    if (code == '1' || code == juce::KeyPress::numberPad1) opIndex = 0;
    else if (code == '2' || code == juce::KeyPress::numberPad2) opIndex = 1;
    else if (code == '3' || code == juce::KeyPress::numberPad3) opIndex = 2;
    else if (code == '4' || code == juce::KeyPress::numberPad4) opIndex = 3;

    if (opIndex == -1) return false; // 他のキーなら無視（通常処理へ）

    // 数字はそのままオペレータ番号を指す。TARGET は動かさない。
    //
    // 設定はひとそろいしか無いので、ここで指し先を動かすと、MML を
    // 入れるつもりで押しただけで今見ているオペレータが入れ替わる。
    //
    // 鳴っていないオペレータには何もしない。以前はつまみが押せるかどうかで
    // 見ていたが、つまみは TARGET のぶんしか無くなったので、
    // アルゴリズムから出した表を見る。
    if (!opActive[(size_t)opIndex]) return true;

    const juce::String opPrefix = OpmPrKey::prefix + CPK::op + juce::String(opIndex);

    // Altキーを押しながら -> マスクのON/OFF、それ以外 -> MML の窓
    if (metaKeys.isAltDown()) {
        const juce::String id = opPrefix + CPK::Fm::mask;

        setParamValue(id, getParamValue(id) >= 0.5f ? 0.0f : 1.0f);
    }
    else {
        GuiMml::openDialog(this, opIndex, mmlHint,
            [this, opIndex](juce::String mml) {
                // 書き込む先は押された番号のオペレータ。TARGET は元へ戻す。
                withOp(opIndex, [this, mml] { this->applyMmlString(mml); });
            });
    }

    return true; // キー入力を消費したことをJUCEに伝える
}

void GuiOpm::copyFmParamsToString()
{
    // オペレータごとに部品を並べるのをやめたので、値はつまみからではなく
    // パラメータから直に採る。画面へ出ているのは TARGET の 1 本だけで、
    // MML には 4 本ぶんが要る。
    //
    // 選ぶ形のパラメータは、つまみの選択番号から 1 を引いたものと同じ値を
    // 持っている。以前 getSelectedId() - 1 と書いていたところがこれに当たる。
    auto& apvts = ctx.audioProcessor.apvts;

    auto opVal = [&apvts](int index, const juce::String& key) {
        return (int)GuiGraphValues::value(apvts, OpmPrKey::prefix + CPK::op + juce::String(index) + key);
        };

    auto opMask = [&apvts](int index) {
        return FmMml::bool2Int(!GuiGraphValues::flag(apvts,
            OpmPrKey::prefix + CPK::op + juce::String(index) + CPK::Fm::mask));
        };

    auto opFlag = [&apvts](int index, const juce::String& key) {
        return FmMml::bool2Int(GuiGraphValues::flag(apvts,
            OpmPrKey::prefix + CPK::op + juce::String(index) + key));
        };

    int maskVal = FmMml::genMask4(opMask(0), opMask(1), opMask(2), opMask(3));

    auto formatCoreBasic = [this, maskVal]() {
        return juce::String::formatted(
            //     ALG    FB   MSK  FRQ  SYC   PG   EG  PMS  PMD  AMS  AMD
            u8"    %1d,  %1d,  %2d, %3d, %3d, %1d, %1d, %1d, %3d, %1d, %3d\n",
            algSelector.getSelectedId() - 1,        // ALG
            (int)feedbackSlider.getValue(),         // FB
            maskVal,                                // MASK
            (int)lfoFreqSlider.getValue(),          // FREQ
            (int)lfoSyncDelaySlider.getValue(),     // SYNC
            lfoPgShapeSelector.getSelectedId() - 1, // PG
            lfoEgShapeSelector.getSelectedId() - 1, // EG
            lfoPmsSelector.getSelectedId() - 1,     // PMS
            (int)lfoPmdSlider.getValue(),           // PMD
            lfoAmsSelector.getSelectedId() - 1,     // AMS
            (int)lfoAmdSlider.getValue()            // AMD
            );
        };
    auto formatOpBasic = [&opVal, &opFlag](int index) {
        return juce::String::formatted(
            // ' MUL     DT   DT2   AR  D1R  D1L  D2R   RR   TL   KS   AMSEN
            u8"   %2d, %+1d,  %1d, %2d, %2d, %2d, %2d, %2d, %3d, %1d,    %1d\n",
            opVal(index, CPK::mul),                 // MUL
            opVal(index, CPK::dt) - 1,              // DT
            opVal(index, CPK::dt2),                 // DT2
            opVal(index, CPK::Fm::rgAr),            // AR
            opVal(index, CPK::Fm::rgD1r),           // D1R
            opVal(index, CPK::Fm::rgD1l),           // D1L
            opVal(index, CPK::Fm::rgD2r),           // D2R
            opVal(index, CPK::Fm::rgRr),            // RR
            opVal(index, CPK::Fm::rgTl),            // TL
            opVal(index, CPK::Fm::ks),              // KS
            opFlag(index, CPK::OpmLfo::amsEn)       // AM
        );
        };
    auto formatOpsBasic = [&formatOpBasic]() {
        return formatOpBasic(0) + formatOpBasic(1) + formatOpBasic(2) + formatOpBasic(3);
        };
    auto formatOpExt = [&opVal](int index) {
        // ' MUL AR DR SL RR TL KSR KSL
        return juce::String::formatted(
            u8"MUL%d DT1%+d DT2+%d AR%d D1R%d D1L%d D2R%d RR%d TL%d KS%d\n",
            opVal(index, CPK::mul),
            opVal(index, CPK::dt) - 1,
            opVal(index, CPK::dt2),
            opVal(index, CPK::Fm::rgAr),
            opVal(index, CPK::Fm::rgD1r),
            opVal(index, CPK::Fm::rgD1l),
            opVal(index, CPK::Fm::rgD2r),
            opVal(index, CPK::Fm::rgRr),
            opVal(index, CPK::Fm::rgTl),
            opVal(index, CPK::Fm::ks)
        );
        };
    auto formatOpsExt = [&formatOpExt]() {
        return formatOpExt(0) + formatOpExt(1) + formatOpExt(2) + formatOpExt(3);
        };

    juce::String mml = juce::String("[OPM]\n")
        + FmMml::basicMmlHeader
        + juce::String(u8"' ALG FB0 FB2 MASK FREQ SYNC PG EG PMS PMD AMS AMD\n")
        + formatCoreBasic()
        + juce::String(u8"' MUL  DT  DT2  AR D1R D1L D2R  RR   TL KS AMSEN\n")
        + formatOpsBasic()
        + juce::String(u8"\n")
        + FmMml::extMmlHeader
        + formatOpsExt();
    juce::SystemClipboard::copyTextToClipboard(mml);
}

void GuiOpm::copyFmParamsToObject()
{

}

void GuiOpm::pasteFmParamsFromObject()
{

}

void GuiOpm::initParams()
{
    this->ctx.audioProcessor.initParams("OPM_");
}

// 設定ひとそろいの置き方。
//
// 一番上が TARGET の帯、その下に区切り線、そこから下が区分。
// 区分は縦に積まず横へ並べ、あふれたぶんは横の送り棒で送る。
void GuiOpm::layoutOpPanel(juce::Rectangle<int> area)
{
    {
        auto row = area.removeFromTop(20);
        auto slot = row.removeFromLeft(OpmGuiValue::Fm::Op::Col::width);

        layoutMain({ .mainRect = slot, .label = &targerOpSlider.label, .component = &targerOpSlider });

        row.removeFromLeft(OpmGuiValue::Fm::Op::Col::gap);

        // TARGET のすぐ右が、どのエンベロープを映すかの切り替え。
        {
            const int w = OpmGuiValue::ParamGroup::Graph::ModeButtonWidth;

            auto btnRow = row.removeFromLeft(w * 5).withHeight(OpmGuiValue::ParamGroup::Graph::ButtonHeight);

            graphBtnAmp.setBounds(btnRow.removeFromLeft(w));
            graphBtnPitch.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg11.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsgP11.setBounds(btnRow);
        }

        row.removeFromLeft(OpmGuiValue::Fm::Op::Col::gap);

        opNameLabel.setBounds(row.removeFromLeft(OpmGuiValue::Fm::Op::Col::width).withHeight(18));
    }

    opTargetSeparator.layoutComponent(area);

    stripViewport.setBounds(area);

    const int colW = OpmGuiValue::Fm::Op::Col::width;
    const int colGap = OpmGuiValue::Fm::Op::Col::gap;

    // 横の送り棒が出るぶんだけ、中身の丈を短くする。
    const int colH = juce::jmax(1, area.getHeight() - stripViewport.getScrollBarThickness());

    int x = 0;

    // 1 列を切り出して、中身を上から積む。
    // 簡易表示モードで隠す区分は、列ごと出さない。隠したぶんだけ
    // 右の列が左へ詰まる。
    auto layoutCol = [&](GuiScrollGroup& group, bool show, auto&& body)
        {
            if (!show) {
                group.setVisible(false);

                return;
            }

            group.setVisible(true);

            juce::Rectangle<int> colArea(x, 0, colW, colH);

            x += colW + colGap;

            group.setBounds(colArea);

            auto inner = colArea.reduced(OpmGuiValue::Group::Padding::width, OpmGuiValue::Group::Padding::height);

            inner.removeFromTop(OpmGuiValue::Group::TitlePaddingTop);

            group.setViewportCustomBounds(inner.translated(-colArea.getX(), -colArea.getY()));

            juce::Rectangle<int> rect(0, 0, group.getContentWidth(), 20000);

            body(rect);

            group.setContentHeight(rect.getY() + 20);
        };

    const auto shown = [this](SimpleView::Cat cat) { return ctx.audioProcessor.isSimpleShown(cat); };

    // 1 区分ずつでは丈が余るので、いくつかの区分は 1 列へ積んである。
    layoutCol(colAmp, true, [&](juce::Rectangle<int>& rect) {
        updateRgDisplayAsOp(true);
        layoutOpAmpCat(rect);
        layoutOpOptionalCat(rect);
        });

    layoutCol(colSsgHwEnv, shown(SimpleView::SsgHwAmpEnv), [&](juce::Rectangle<int>& rect) {
        ssgHwEnvOp.setCategoryVisible(true);
        ssgHwEnvOp.layoutComponent(rect);
        });

    layoutCol(colSsgSwEnv, shown(SimpleView::SsgSwAmpEnv), [&](juce::Rectangle<int>& rect) {
        ssgSwEnv.setCategoryVisible(true);
        ssgSwEnv.layoutComponent(rect);
        });

    layoutCol(colSsgSwEnv11, shown(SimpleView::SsgSwAmpEnv11), [&](juce::Rectangle<int>& rect) {
        ssgSwEnv11.setCategoryVisible(true);
        ssgSwEnv11.layoutComponent(rect);
        });

    layoutCol(colAmpMod, shown(SimpleView::WtAmpMod), [&](juce::Rectangle<int>& rect) {
        wtAmpModOp.setCategoryVisible(true);
        wtAmpModOp.layoutComponent(rect);
        });

    layoutCol(colPitchEnv, shown(SimpleView::PitchEnv), [&](juce::Rectangle<int>& rect) {
        pitchEnv.setCategoryVisible(true);
        pitchEnv.layoutComponent(rect);
        });

    layoutCol(colSsgHwPEnv, shown(SimpleView::SsgHwPitchEnv), [&](juce::Rectangle<int>& rect) {
        ssgHwPEnvOp.setCategoryVisible(true);
        ssgHwPEnvOp.layoutComponent(rect);
        });

    layoutCol(colSsgSwPEnv11, shown(SimpleView::SsgSwPitchEnv11), [&](juce::Rectangle<int>& rect) {
        ssgSwPEnv11.setCategoryVisible(true);
        ssgSwPEnv11.layoutComponent(rect);
        });

    layoutCol(colMod, shown(SimpleView::WtPitchMod), [&](juce::Rectangle<int>& rect) {
        wtModOp.setCategoryVisible(true);
        wtModOp.layoutComponent(rect);
        });

    layoutCol(colKs, true, [&](juce::Rectangle<int>& rect) {
        layoutOpKsCat(rect);
        layoutOpDetCat(rect);

        fix.setCategoryVisible(shown(SimpleView::Fix));
        fix.layoutComponent(rect);
        });

    // OPM に N88 の LFO は無いので、この列は音源側のぶんだけ。
    layoutCol(colHwLfo, true, [&](juce::Rectangle<int>& rect) { layoutOpHwLfoCat(rect); });

    // MASK と MML の札も 1 列へまとめてある。
    layoutCol(colMask, true, [&](juce::Rectangle<int>& rect) {
        layoutOpMaskCat(rect);

        mmlSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .component = &mml, .paddingBottom = 0 });
        });

    stripCanvas.setSize(juce::jmax(1, x - colGap), colH);
}

void GuiOpm::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    copyParamsToOpnaBtn.setVisible(visible);
    copyParamsToOpnBtn.setVisible(visible);
    uSep002.setVisible(visible);
    copyOpParamBtn.setVisible(visible);
    copyOpFromSlider.setVisibleWithLabel(visible);
    copyOpToSlider.setVisibleWithLabel(visible);
    uSep003.setVisible(visible);
    ieOpPitchEnv.setVisible(visible);
    ieOpSsgSwEnv.setVisible(visible);
    ieOpSsgSwEnv11.setVisible(visible);
    ieOpSsgSwPEnv11.setVisible(visible);
    ieOpSsgHwPEnv.setVisible(visible);
    ieOpWtAmpMod.setVisible(visible);
    ieOpSsgHwEnv.setVisible(visible);
    ieOpWtMod.setVisible(visible);
    ieOpChParam.setVisible(visible);
    uSep004.setVisible(visible);
    ieAmpEnvG.setVisible(visible);
    ieSsgHwEnv.setVisible(visible);
    ieSsgHwPEnv.setVisible(visible);
    ieWtMod.setVisible(visible);
    ieWtAmpMod.setVisible(visible);
    ieSsgSwEnv11.setVisible(visible);
    ieSsgSwPEnv11g.setVisible(visible);
    ieLfo.setVisible(visible);
    ieUnison.setVisible(visible);
    ieQuality.setVisible(visible);
    ieChParam.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        uSep001.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyParamsToOpnaBtn });
        layoutMain({ .mainRect = rect, .component = &copyParamsToOpnBtn });

        uSep002.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyOpParamBtn });
        layoutMain({ .mainRect = rect, .label = &copyOpFromSlider.label, .component = &copyOpFromSlider });
        layoutMain({ .mainRect = rect, .label = &copyOpToSlider.label, .component = &copyOpToSlider });

        uSep003.layoutComponent(rect);

        ieOpPitchEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwEnv11.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwPEnv11.layoutComponent(rect);
        ieOpSsgHwPEnv.layoutComponent(rect);
        ieOpWtAmpMod.layoutComponent(rect);
        ieOpSsgHwEnv.layoutComponent(rect);
        ieOpWtMod.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpChParam.layoutComponent(rect);
        rect.removeFromTop(4);

        uSep004.layoutComponent(rect);

        ieAmpEnvG.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgHwEnv.layoutComponent(rect);
        ieSsgHwPEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieWtMod.layoutComponent(rect);
        ieWtAmpMod.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwEnv11.layoutComponent(rect);
        rect.removeFromTop(4);
        ieSsgSwPEnv11g.layoutComponent(rect);
        rect.removeFromTop(4);
        ieLfo.layoutComponent(rect);
        rect.removeFromTop(4);
        ieUnison.layoutComponent(rect);
        rect.removeFromTop(4);
        ieQuality.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpm::layoutOpMaskCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask });

    bool visibleMask = catMask.isDetailVisible();

    mask.setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }

    // MASK の後ろには分類の外の行 (セパレータや MML) が続くので、
    // 板をここで閉じないと 1 行ぶん下まで伸びてしまう。
    closeCategoryBackdrops(catMask.getParentComponent(), rect.getY());
}

void GuiOpm::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityComponent.layoutComponent(rect);
}

void GuiOpm::layoutPanCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &panCat });

    bool visible = panCat.isDetailVisible();

    panSlider.setVisibleWithLabel(visible);
    panToLBtn.setVisible(visible);
    panToCBtn.setVisible(visible);
    panToRBtn.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &panSlider.label, .component = &panSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &panToLBtn, .comp2 = &panToCBtn, .comp3 = &panToRBtn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// 選んだ Shape を実際の LFO で走らせ、折れ線にして渡す。
// 値が変わったときだけ通るので、常時の負荷は無い。
void GuiOpm::updateLfoPreviews()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this] { updateLfoPreviews(); })) return;

    // PM は -1.0〜1.0 の両振り
    lfoPmPreview.setPoints(WavePreviewSource::opmLfoPm(lfoPgShapeSelector.getSelectedItemIndex()), true);

    // AM は 0.0〜1.0 の片側。スムースの効きも見えるよう実際の値を渡す。
    lfoAmPreview.setPoints(WavePreviewSource::opmLfoAm(lfoEgShapeSelector.getSelectedItemIndex(), (float)lfoAmSmRtSlider.getValue()), false);
}

void GuiOpm::layoutHwLfoCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &lfoCat });

    bool visible = lfoCat.isDetailVisible();

    lfoFreqSlider.setVisibleWithLabel(visible);
    lfoAmSmRtSlider.setVisibleWithLabel(visible);
    lfoSyncDelaySlider.setVisibleWithLabel(visible);
    lfoSyncDelayToZeroBtn.setVisible(visible);
    lfoSyncDelayToOneBtn.setVisible(visible);
    lfoSep1.setVisible(visible);
    lfoPmToggle.setVisible(visible);
    lfoPgShapeSelector.setVisibleWithLabel(visible);
    lfoPmPreview.setVisible(visible);
    lfoPmsSelector.setVisibleWithLabel(visible);
    lfoPmdSlider.setVisibleWithLabel(visible);
    lfoSep2.setVisible(visible);
    lfoAmToggle.setVisible(visible);
    lfoEgShapeSelector.setVisibleWithLabel(visible);
    lfoAmPreview.setVisible(visible);
    lfoAmsSelector.setVisibleWithLabel(visible);
    lfoAmdSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &lfoFreqSlider.label, .component = &lfoFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
        layoutMain({ .mainRect = rect, .label = &lfoSyncDelaySlider.label, .component = &lfoSyncDelaySlider });
        layoutMainTwoComps({ .rect = rect, .comp1 = &lfoSyncDelayToZeroBtn, .comp2 = &lfoSyncDelayToOneBtn });
		lfoSep1.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &lfoPmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoPgShapeSelector.label, .component = &lfoPgShapeSelector });
        lfoPmPreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        layoutMain({ .mainRect = rect, .label = &lfoPmsSelector.label, .component = &lfoPmsSelector });
        layoutMain({ .mainRect = rect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
        lfoSep2.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &lfoAmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoEgShapeSelector.label, .component = &lfoEgShapeSelector });
        lfoAmPreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        layoutMain({ .mainRect = rect, .label = &lfoAmsSelector.label, .component = &lfoAmsSelector });
        layoutMain({ .mainRect = rect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpm::layoutOpHwLfoCat(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &cafLfo });

    bool visible = cafLfo.isDetailVisible();

    amsEnable.setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &amsEnable });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpm::layoutOpKsCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &ksCat });

    bool visible = ksCat.isDetailVisible();
    FmRgAdddrKeyScaleMode mode = (FmRgAdddrKeyScaleMode)(ksMode.getSelectedItemIndex());

    ksMode.setVisibleWithLabel(visible);
    ks.setVisibleWithLabel(visible && mode == FmRgAdddrKeyScaleMode::OPM);
    ksrOPP.setVisibleWithLabel(visible && mode == FmRgAdddrKeyScaleMode::OPP);
    kslOPP.setVisibleWithLabel(visible && mode == FmRgAdddrKeyScaleMode::OPP);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &ksMode.label, .component = &ksMode });

        switch (mode) {
        case FmRgAdddrKeyScaleMode::OPM:
            layoutRow({ .rowRect = rect, .label = &ks.label, .component = &ks });
            break;
        case FmRgAdddrKeyScaleMode::OPP:
            layoutRow({ .rowRect = rect, .label = &ksrOPP.label, .component = &ksrOPP });
            layoutRow({ .rowRect = rect, .label = &kslOPP.label, .component = &kslOPP });
            break;
        }

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpm::layoutOpDetCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catDet });

    bool visible = catDet.isDetailVisible();

    mul.setVisibleWithLabel(visible);
    mulRatio.setVisibleWithLabel(visible);
    mulRatioTo001.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo005.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo1.setVisible(visible && mulRatio.isVisibleNudge());
	mulRatioTo10.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo2757.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo02.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo025.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo04.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo05.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo06.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo075.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioTo08.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM10.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP10.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM1.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP1.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM01.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP01.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioPM001.setVisible(visible && mulRatio.isVisibleNudge());
    mulRatioP001.setVisible(visible && mulRatio.isVisibleNudge());
    mulDetSep.setVisible(visible);
    dt1.setVisibleWithLabel(visible);
    dt2.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &mul.label, .component = &mul });
        mulRatio.layoutComponentRow(rect);
		if (mulRatio.isVisibleNudge())
		{
            layoutRowFiveComps({ .rect = rect, .comp1 = &mulRatioTo001, .comp2 = &mulRatioTo005, .comp3 = &mulRatioTo1, .comp4 = &mulRatioTo10, .comp5 = &mulRatioTo2757 });
            layoutRowThreeComps({ .rect = rect, .comp1 = &mulRatioTo025, .comp2 = &mulRatioTo05, .comp3 = &mulRatioTo075 });
            layoutRowFourComps({ .rect = rect, .comp1 = &mulRatioTo02, .comp2 = &mulRatioTo04, .comp3 = &mulRatioTo06, .comp4 = &mulRatioTo08 });
            layoutRowFourComps({ .rect = rect, .comp1 = &mulRatioPM10, .comp2 = &mulRatioPM1, .comp3 = &mulRatioP1, .comp4 = &mulRatioP10 });
            layoutRowFourComps({ .rect = rect, .comp1 = &mulRatioPM001, .comp2 = &mulRatioPM01, .comp3 = &mulRatioP01, .comp4 = &mulRatioP001 });
		}
        mulDetSep.layoutComponent(rect);
        layoutRow({ .rowRect = rect, .label = &dt1.label, .component = &dt1 });
        layoutRow({ .rowRect = rect, .label = &dt2.label, .component = &dt2 });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpm::layoutOpAmpCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catAmp });

    bool visible = catAmp.isDetailVisible();

    rgAr.setVisibleWithLabel(visible);
    rgD1r.setVisibleWithLabel(visible);
    rgD2r.setVisibleWithLabel(visible);
    rgD1l.setVisibleWithLabel(visible);
    rgRr.setVisibleWithLabel(visible);
    rgTl.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &rgAr.label, .component = &rgAr });
        layoutRow({ .rowRect = rect, .label = &rgD1r.label, .component = &rgD1r });
        layoutRow({ .rowRect = rect, .label = &rgD1l.label, .component = &rgD1l });
        layoutRow({ .rowRect = rect, .label = &rgD2r.label, .component = &rgD2r });
        layoutRow({ .rowRect = rect, .label = &rgRr.label, .component = &rgRr });
        layoutRow({ .rowRect = rect, .label = &rgTl.label, .component = &rgTl });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// ============================================================================
// チップ全体のエンベロープを見るグラフ
// ============================================================================
// オペレータごとのグラフと同じ作りで、対象がチップ全体の 3 つになる。
// 値が変わったときだけ描き直すので、常時の負荷は無い。
void GuiOpm::setupGlobalGraph()
{
    addAndMakeVisible(&gGraph);

    gGraphBtnAmp.setup({ .parent = *this, .title = "AMP", .isReset = false, .isResized = false });
    gGraphBtnAmp.setToggleState(true, juce::dontSendNotification);
    gGraphBtnAmp.onClick = [this] { setGlobalGraphMode(GlobalGraphMode::Amp); };

    gGraphBtnSsg11.setup({ .parent = *this, .title = "S11", .isReset = false, .isResized = false });
    gGraphBtnSsg11.onClick = [this] { setGlobalGraphMode(GlobalGraphMode::SsgSw11); };

    gGraphBtnSsgP11.setup({ .parent = *this, .title = "P11", .isReset = false, .isResized = false });
    gGraphBtnSsgP11.onClick = [this] { setGlobalGraphMode(GlobalGraphMode::SsgSwP11); };

    auto repaintGraph = [this]() {
        if (this->isUpdatingGraph) return;

        // 旗は必ず下ろす。途中で抜けたときに立ちっぱなしになると、
        // 以後グラフの更新が全部素通りしてしまうため。
        const juce::ScopedValueSetter<bool> guard(this->isUpdatingGraph, true);

        this->updateGlobalGraph();
        };

    ampEnvComponent.setupGraph(repaintGraph);
    ssgSwEnv11g.setupGraph(repaintGraph);
    ssgSwPEnv11g.setupGraph(repaintGraph);

    gGraphSeparator.setupComponent(*this);
}

void GuiOpm::layoutGlobalGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(OpmGuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    gGraphSeparator.layoutComponentBottom(mainArea);

    auto btnArea = mainArea.removeFromBottom(OpmGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    gGraphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsg11.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsgP11.setBounds(btnArea);

    gGraph.setBounds(mainArea);
}

void GuiOpm::updateGlobalGraph()
{

    if (currentGlobalGraphMode == GlobalGraphMode::SsgSw11) {
        ssgSwEnv11g.updateGraph(gGraph);
    }
    else if (currentGlobalGraphMode == GlobalGraphMode::SsgSwP11) {
        ssgSwPEnv11g.updateGraph(gGraph);
    }
    else {
        ampEnvComponent.updateGraph(gGraph);
    }
}

void GuiOpm::setGlobalGraphMode(GlobalGraphMode mode)
{
    currentGlobalGraphMode = mode;

    // ラジオボタン的な排他制御
    gGraphBtnAmp.setToggleState(mode == GlobalGraphMode::Amp, juce::dontSendNotification);
    gGraphBtnSsg11.setToggleState(mode == GlobalGraphMode::SsgSw11, juce::dontSendNotification);
    gGraphBtnSsgP11.setToggleState(mode == GlobalGraphMode::SsgSwP11, juce::dontSendNotification);

    updateGlobalGraph();
}

// つまみが動いたら、上の絵を描き直してもらうためのつなぎ。
//
// グラフそのものはここには無い。オペレータの数だけ同時に出るので、
// 設定の側で持つわけにいかない。描き直すのは TARGET が指している
// 1 枚だけでよい。ほかのオペレータの値は変わっていない。
void GuiOpm::setupOpGraphWiring()
{
    auto repaintGraph = [this]() {
        if (this->isUpdatingGraph) return; // 既に更新中なら無視

        // 旗は必ず下ろす。途中で抜けたときに立ちっぱなしになると、
        // 以後グラフの更新が全部素通りしてしまうため。
        const juce::ScopedValueSetter<bool> guard(this->isUpdatingGraph, true);

        this->updateOpGraph(this->currentOp());
        };

    bypass.onStateChange = repaintGraph;
    xof.onStateChange = repaintGraph;
    kor.onStateChange = repaintGraph;

    rgAr.onValueChange = repaintGraph;
    rgD1r.onValueChange = repaintGraph;
    rgD2r.onValueChange = repaintGraph;
    rgD1l.onValueChange = repaintGraph;
    rgRr.onValueChange = repaintGraph;
    rgTl.onValueChange = repaintGraph;

    pitchEnv.setupGraph(repaintGraph);
    ssgSwEnv.setupGraph(repaintGraph);
    ssgSwEnv11.setupGraph(repaintGraph);
    ssgSwPEnv11.setupGraph(repaintGraph);
}

// 枠に出すエンベロープを描き直す。
//
// 枠はオペレータの数だけ同時に出るので、値をつまみから読むわけには
// いかない。接頭辞を頼りにパラメータから直に採る。
void GuiOpm::updateOpGraph(int opIndex)
{
    auto& apvts = ctx.audioProcessor.apvts;
    auto& graph = cells[(size_t)opIndex].graph();

    const juce::String code = OpmPrKey::prefix + CPK::op + juce::String(opIndex);

    // -------------------------------------------------------------
    // Helper: 幅の計算 (Amp 用)
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return maxWidth;

        if (rateValue <= 0.0f) return maxWidth;

        float norm = 1.0f - (rateValue / maxRate);

        return maxWidth * norm;
        };

    // =============================================================
    // Pitch Env
    // =============================================================
    if (currentGraphMode == GuiEnvGraphMode::Pitch) {
        // オペレータの区分は「入れる」札なので、切ってあるときが素通し。
        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::PitchAdsr::enable));
        graph.setKeepLevels(GuiGraphValues::pitchEnvKeep(apvts, code));
        graph.updatePitchEnv(GuiGraphValues::pitchEnv(apvts, code));
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSw) {
        const auto v = GuiGraphValues::ssgSwEnv(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwEnv::enable));
        graph.updateSsgSwEnv(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    // =============================================================
    // SSG SW Env 11
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSw11) {
        const auto v = GuiGraphValues::ssgSwEnv11(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwEnv11::enable));
        graph.setKeepLevels(GuiGraphValues::ssgSwEnv11Keep(apvts, code));
        graph.updateSsgSwEnv11(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    // =============================================================
    // SSG SW PEnv 11
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSwP11) {
        const auto v = GuiGraphValues::ssgSwPEnv11(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwPEnv11::enable));
        graph.setKeepLevels(GuiGraphValues::ssgSwPEnv11Keep(apvts, code));
        graph.updateSsgSwPEnv11(v.head, v.rVal, v.rMax, v.lVal, v.lMax);
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        bool isBypass = GuiGraphValues::flag(apvts, code + CPK::Fm::bypass);

        graph.updateBypass(isBypass);

        if (isBypass) {
            graph.repaint();

            return;
        }

        bool isXof = GuiGraphValues::flag(apvts, code + CPK::Fm::xof);
        bool isKor = GuiGraphValues::flag(apvts, code + CPK::Fm::kor);

        float arMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgAr);
        float d1rMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgD1r);
        float d2rMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgD2r);
        float d1lMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgD1l);
        float rrMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgRr);
        float tlMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgTl);

        float arVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgAr);
        float d1rVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgD1r);
        float d2rVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgD2r);
        float d1lVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgD1l);
        float rrVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgRr);
        float tlVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgTl);

        float sl = (d1lMax - d1lVal) / d1lMax; // 15=0.0, 0=1.0
        float tlScale = 1.0f - (tlVal / tlMax); // TL=127で無音

        if (std::isnan(sl) || std::isinf(sl)) sl = 0.0f;
        if (std::isnan(tlScale) || std::isinf(tlScale)) tlScale = 1.0f;

        std::vector<GuiEnvelopeGraph::PhaseDef> phases;
        auto color = juce::Colours::cyan;

        float currentTotalWidth = 0.0f;

        // 1. Attack
        float attackWidth = rateToWidth(arVal, arMax);
        phases.push_back({
            .widthPx = attackWidth, .startLevel = 0.0f, .endLevel = 1.0f * tlScale, .color = color,
            .phaseLineColor = juce::Colours::red
            });
        currentTotalWidth += attackWidth;

        // 2. Decay 1
        float decayWidth = rateToWidth(d1rVal, d1rMax);
        phases.push_back({
            .widthPx = decayWidth, .startLevel = 1.0f * tlScale, .endLevel = sl * tlScale, .color = color,
            .phaseLineColor = juce::Colours::blue
            });
        currentTotalWidth += decayWidth;

        // 3. Sustain (D2R) の実線部分 (キーオン中)
        float releaseStartLevel = sl;
        float keyOnWidth = 60.0f;
        float sustainTotalWidth = 0.0f;

        if (d2rVal > 0.0f && d2rVal < d2rMax) {
            sustainTotalWidth = rateToWidth(d2rVal, d2rMax, 300.0f);

            releaseStartLevel = sl - (sl * 0.5f);

            phases.push_back({
                .widthPx = sustainTotalWidth, .startLevel = sl * tlScale, .endLevel = 0.0f, .color = color,
                .phaseLineColor = juce::Colours::green
                });

            currentTotalWidth += sustainTotalWidth * 0.5f;
        }
        else {
            phases.push_back({ .widthPx = keyOnWidth, .startLevel = sl * tlScale, .endLevel = sl * tlScale, .color = color, .phaseLineColor = juce::Colours::green });

            currentTotalWidth += keyOnWidth;
        }
        float noteOffPositionX = currentTotalWidth;

        // 4. Release
        if (isXof) {
            phases.push_back({
                .widthPx = 100.0f,
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = releaseStartLevel * tlScale,
                .isDashed = true,
                .color = juce::Colours::yellow,
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX
                });
        }
        else if (isKor) {
            phases.push_back({
                .widthPx = rateToWidth(rrVal, rrMax),
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = releaseStartLevel * tlScale,
                .color = juce::Colours::yellow,
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX
                });
        }
        else {
            phases.push_back({
                .widthPx = rateToWidth(rrVal, rrMax),
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = 0.0f,
                .color = d2rVal > 0.0f ? juce::Colours::yellow : color,
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX,
                .isMax = (rrVal == rrMax)
                });
        }

        graph.setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
    }

    graph.repaint();
}

void GuiOpm::layoutOpOptionalCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catOptional });

    bool visible = catOptional.isDetailVisible();

    opDelay.setVisibles(visible);
    opDelayNudge.setVisibles(visible && opDelay.isVisibleNudge());
    opDelaySeparator.setVisible(visible);
    xof.setVisible(visible);
    kor.setVisible(visible);
    bypass.setVisible(visible);

    if (visible)
    {
        opDelay.layoutComponentRow(rect);

        if (opDelay.isVisibleNudge()) opDelayNudge.layoutComponentRow(rect);

        opDelaySeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .component = &xof });
        layoutRow({ .rowRect = rect, .component = &kor });
        layoutRow({ .rowRect = rect, .component = &bypass });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpm::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiOpm::copyParams(CopyOpm& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();
    copyObj.pan.pan = panSlider.getValue();

    copyObj.lfo.freq = lfoFreqSlider.getValue();
    copyObj.lfo.syncDelay = lfoSyncDelaySlider.getValue();
    copyObj.lfo.pgIndex = lfoPgShapeSelector.getSelectedId();
    copyObj.lfo.egIndex = lfoEgShapeSelector.getSelectedId();
    copyObj.lfo.amSmoothRate = lfoAmSmRtSlider.getValue();
    copyObj.lfo.pm = lfoPmToggle.getToggleState();
    copyObj.lfo.am = lfoAmToggle.getToggleState();
    copyObj.lfo.pmd = lfoPmdSlider.getValue();
    copyObj.lfo.pmsIndex = lfoPmsSelector.getSelectedId();
    copyObj.lfo.amd = lfoAmdSlider.getValue();
    copyObj.lfo.amsIndex = lfoAmsSelector.getSelectedId();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpm::copyOpParams(int p, CopyOpmOp& copyObj) {
    copyObj.detune.mul = mul.getSelectedId();
    copyObj.detune.mulRatio = mulRatio.getValue();
    copyObj.detune.dt = dt1.getValue();
    copyObj.detune.dt2 = dt2.getValue();
    copyObj.aAdsr.ar = rgAr.getValue();
    copyObj.aAdsr.d1r = rgD1r.getValue();
    copyObj.aAdsr.d1l = rgD1l.getValue();
    copyObj.aAdsr.d2r = rgD2r.getValue();
    copyObj.aAdsr.rr = rgRr.getValue();
    copyObj.aAdsr.tl = rgTl.getValue();
    copyObj.aAdsr.bypass = bypass.getToggleState();
    copyObj.aAdsr.kor = kor.getToggleState();
    copyObj.aAdsr.xof = xof.getToggleState();
    copyObj.aAdsr.mode = (FmRgAdddrKeyScaleMode)ksMode.getSelectedId();
    copyObj.aAdsr.ks = ks.getSelectedId();
    copyObj.aAdsr.ksrOPP = ksrOPP.getValue();
    copyObj.aAdsr.kslOPP = kslOPP.getValue();
    copyObj.lfo.amsEn = amsEnable.getToggleState();

    copyObj.mask.mask = mask.getToggleState();

    fix.copyParams(copyObj.fix);
    pitchEnv.copyParams(copyObj.pAdsr);
    ssgSwEnv.copyParams(copyObj.aSsgSw);
}

void GuiOpm::pasteParams(CopyOpm& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);
    panSlider.setValue(copyObj.pan.pan, juce::sendNotification);

    lfoFreqSlider.setValue(copyObj.lfo.freq, juce::sendNotification);
    lfoSyncDelaySlider.setValue(copyObj.lfo.syncDelay, juce::sendNotification);
    lfoPgShapeSelector.setSelectedId(copyObj.lfo.pgIndex, juce::sendNotification);
    lfoEgShapeSelector.setSelectedId(copyObj.lfo.egIndex, juce::sendNotification);
    lfoAmSmRtSlider.setValue(copyObj.lfo.amSmoothRate, juce::sendNotification);
    lfoPmToggle.setToggleState(copyObj.lfo.pm, juce::sendNotification);
    lfoAmToggle.setToggleState(copyObj.lfo.am, juce::sendNotification);
    lfoPmdSlider.setValue(copyObj.lfo.pmd, juce::sendNotification);
    lfoPmsSelector.setSelectedId(copyObj.lfo.pmsIndex, juce::sendNotification);
    lfoAmdSlider.setValue(copyObj.lfo.amd, juce::sendNotification);
    lfoAmsSelector.setSelectedId(copyObj.lfo.amsIndex, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpm::pasteOpParams(int p, CopyOpmOp& copyObj) {
    mul.setSelectedId(copyObj.detune.mul, juce::sendNotification);
    mulRatio.setValue(copyObj.detune.mulRatio, juce::sendNotification);
    dt1.setValue(copyObj.detune.dt, juce::sendNotification);
    dt2.setValue(copyObj.detune.dt2, juce::sendNotification);
    rgAr.setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgD1r.setValue(copyObj.aAdsr.d1r, juce::sendNotification);
    rgD1l.setValue(copyObj.aAdsr.d1l, juce::sendNotification);
    rgD2r.setValue(copyObj.aAdsr.d2r, juce::sendNotification);
    rgRr.setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl.setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass.setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor.setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof.setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ksMode.setSelectedId((int)copyObj.aAdsr.mode, juce::sendNotification);
    ks.setSelectedId(copyObj.aAdsr.ks, juce::sendNotification);
    ksrOPP.setValue(copyObj.aAdsr.ksrOPP, juce::sendNotification);
    kslOPP.setValue(copyObj.aAdsr.kslOPP, juce::sendNotification);
    amsEnable.setToggleState(copyObj.lfo.amsEn, juce::sendNotification);

    mask.setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix.pasteParams(copyObj.fix);
    pitchEnv.pasteParams(copyObj.pAdsr);
    ssgSwEnv.pasteParams(copyObj.aSsgSw);
}

void GuiOpm::copyParamsOpn(CopyOpnOpm& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpm::copyOpParamsOpn(int p, CopyOpnOpmOp& copyObj) {
    copyObj.detune.mul = mul.getSelectedId();
    copyObj.detune.dt = dt1.getValue();
    copyObj.aAdsr.ar = rgAr.getValue();
    copyObj.aAdsr.dr = rgD1r.getValue();
    copyObj.aAdsr.sl = rgD1l.getValue();
    copyObj.aAdsr.sr = rgD2r.getValue();
    copyObj.aAdsr.rr = rgRr.getValue();
    copyObj.aAdsr.tl = rgTl.getValue();
    copyObj.aAdsr.bypass = bypass.getToggleState();
    copyObj.aAdsr.kor = kor.getToggleState();
    copyObj.aAdsr.xof = xof.getToggleState();
    copyObj.aAdsr.ks = ks.getSelectedId();

    copyObj.mask.mask = mask.getToggleState();

    fix.copyParams(copyObj.fix);
    pitchEnv.copyParams(copyObj.pAdsr);
    ssgSwEnv.copyParams(copyObj.aSsgSw);
}

void GuiOpm::pasteParamsOpn(CopyOpnOpm& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpm::pasteOpParamsOpn(int p, CopyOpnOpmOp& copyObj) {
    mul.setSelectedId(copyObj.detune.mul, juce::sendNotification);
    dt1.setValue(copyObj.detune.dt, juce::sendNotification);
    rgAr.setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgD1r.setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgD1l.setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgD2r.setValue(copyObj.aAdsr.sr, juce::sendNotification);
    rgRr.setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl.setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass.setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor.setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof.setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ks.setSelectedId(copyObj.aAdsr.ks, juce::sendNotification);

    mask.setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix.pasteParams(copyObj.fix);
    pitchEnv.pasteParams(copyObj.pAdsr);
    ssgSwEnv.pasteParams(copyObj.aSsgSw);
}

void GuiOpm::copyParamsOpnaOpn(CopyOpnaOpnOpm& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpm::copyOpParamsOpnaOpn(int p, CopyOpnaOpnOpmOp& copyObj) {
    copyObj.detune.mul = mul.getSelectedItemIndex();
    copyObj.detune.dt = dt1.getValue();
    copyObj.aAdsr.ar = rgAr.getValue();
    copyObj.aAdsr.dr = rgD1r.getValue();
    copyObj.aAdsr.sl = rgD1l.getValue();
    copyObj.aAdsr.sr = rgD2r.getValue();
    copyObj.aAdsr.rr = rgRr.getValue();
    copyObj.aAdsr.tl = rgTl.getValue();
    copyObj.aAdsr.bypass = bypass.getToggleState();
    copyObj.aAdsr.kor = kor.getToggleState();
    copyObj.aAdsr.xof = xof.getToggleState();
    copyObj.aAdsr.ks = ks.getSelectedId();

    copyObj.mask.mask = mask.getToggleState();

    fix.copyParams(copyObj.fix);
    pitchEnv.copyParams(copyObj.pAdsr);
    ssgSwEnv.copyParams(copyObj.aSsgSw);
}

void GuiOpm::pasteParamsOpnaOpn(CopyOpnaOpnOpm& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpm::pasteOpParamsOpnaOpn(int p, CopyOpnaOpnOpmOp& copyObj) {
    mul.setSelectedItemIndex(copyObj.detune.mul, juce::sendNotification);
    dt1.setValue(copyObj.detune.dt, juce::sendNotification);
    rgAr.setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgD1r.setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgD1l.setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgD2r.setValue(copyObj.aAdsr.sr, juce::sendNotification);
    rgRr.setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl.setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass.setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor.setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof.setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ks.setSelectedId(copyObj.aAdsr.ks, juce::sendNotification);

    mask.setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix.pasteParams(copyObj.fix);
    pitchEnv.pasteParams(copyObj.pAdsr);
    ssgSwEnv.pasteParams(copyObj.aSsgSw);
}

void GuiOpm::importPitchEnvParam(int opIndex) {
    pitchEnv.importParams();
}

void GuiOpm::exportPitchEnvParam(int opIndex) {
    pitchEnv.exportParams();
}

void GuiOpm::importSsgSwEnvParam(int opIndex) {
    ssgSwEnv.importParams();
}

void GuiOpm::exportSsgSwEnvParam(int opIndex) {
    ssgSwEnv.exportParams();
}

void GuiOpm::importLfoParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultLfoParamDir,
        { EditorGuiText::ParamBrowser::kindLfoOpm },
        [this](const juce::File& file) { applyLfoParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpm::applyLfoParamFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingLfoParams(lines, index);
        }

        Io::ParamWriter writer(opmLfoFormat);

        writeLfoParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opmLfoFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    lfoFreqSlider.setValue(reader->getInt("lfoFreq", (int)lfoFreqSlider.getValue()), juce::sendNotification);
    lfoSyncDelaySlider.setValue(reader->getInt("lfoSyncDelay", (int)lfoSyncDelaySlider.getValue()), juce::sendNotification);
    lfoPmToggle.setToggleState(reader->getBool("lfoPm", lfoPmToggle.getToggleState()), juce::sendNotification);
    lfoPgShapeSelector.setSelectedItemIndex(reader->getInt("lfoPgShape", lfoPgShapeSelector.getSelectedItemIndex()), juce::sendNotification);
    lfoPmsSelector.setSelectedItemIndex(reader->getInt("lfoPms", lfoPmsSelector.getSelectedItemIndex()), juce::sendNotification);
    lfoPmdSlider.setValue(reader->getInt("lfoPmd", (int)lfoPmdSlider.getValue()), juce::sendNotification);
    lfoAmToggle.setToggleState(reader->getBool("lfoAm", lfoAmToggle.getToggleState()), juce::sendNotification);
    lfoAmSmRtSlider.setValue(reader->getFloat("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue()), juce::sendNotification);
    lfoEgShapeSelector.setSelectedItemIndex(reader->getInt("lfoEgShape", lfoEgShapeSelector.getSelectedItemIndex()), juce::sendNotification);
    lfoAmsSelector.setSelectedItemIndex(reader->getInt("lfoAms", lfoAmsSelector.getSelectedItemIndex()), juce::sendNotification);
    lfoAmdSlider.setValue(reader->getInt("lfoAmd", (int)lfoAmdSlider.getValue()), juce::sendNotification);

    auto amsValues = reader->getIntArray("amsEnable");

    for (int i = 0; i < OpmPrValue::ops && i < (int)amsValues.size(); i++) {
        amsEnable.setToggleState(amsValues[(size_t)i] != 0, juce::sendNotification);
    }
}

void GuiOpm::exportLfoParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultLfoParamDir,
        { EditorGuiText::ParamBrowser::kindLfoOpm }, Io::Extension::OpmLfoParam,
        [this](const juce::File& file) { writeLfoParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpm::writeLfoParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(opmLfoFormat);
    writeLfoParams(writer);

    writer.writeTo(file);
}

void GuiOpm::importQualityParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindQuality },
        [this](const juce::File& file) { applyQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpm::applyQualityParamFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingQualityParams(lines, index);
        }

        Io::ParamWriter writer(qualityFormat);

        writeQualityParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, qualityFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    qualityComponent.setBit(reader->getInt("bit", qualityComponent.getBit()));
    qualityComponent.setRate(reader->getInt("rate", qualityComponent.getRate()));
}

void GuiOpm::exportQualityParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindQuality }, Io::Extension::QualityParam,
        [this](const juce::File& file) { writeQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpm::writeQualityParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(qualityFormat);
    writeQualityParams(writer);

    writer.writeTo(file);
}

void GuiOpm::importSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11.importParams();
}

void GuiOpm::exportSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11.exportParams();
}

void GuiOpm::importSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11.importParams();
}

void GuiOpm::exportSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11.exportParams();
}

void GuiOpm::importOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp.importParams();
}

void GuiOpm::exportOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp.exportParams();
}

void GuiOpm::importOpWtAmpModParam(int opIndex) {
    wtAmpModOp.importParams();
}

void GuiOpm::exportOpWtAmpModParam(int opIndex) {
    wtAmpModOp.exportParams();
}

void GuiOpm::importOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp.importParams();
}

void GuiOpm::exportOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp.exportParams();
}

void GuiOpm::importOpWtModParam(int opIndex) {
    wtModOp.importParams();
}

void GuiOpm::exportOpWtModParam(int opIndex) {
    wtModOp.exportParams();
}

void GuiOpm::importChParam() {
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser({ "OPM" },
        [this](const juce::File& file) { applyChParamFile(file); });
}

// パラメータファイルのブラウザからも同じ読み込みを使うので、
// ダイアログを出すところと、読んで反映するところを分けてある。
void GuiOpm::applyChParamFile(const juce::File& file) {
    if (!file.existsAsFile()) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingChParams(lines, index);
        }

        Io::ParamWriter writer(opmFormat);

        writeChParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opmFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    readChParams(*reader);
}

void GuiOpm::exportChParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { "OPM" }, Io::Extension::opmParam,
        [this](const juce::File& file) { writeChParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpm::writeChParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(opmFormat);
    writeChParams(writer);

    writer.writeTo(file);
}

void GuiOpm::importOpChParam(int opIndex)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpmOp },
        [this, opIndex](const juce::File& file) { applyOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpm::applyOpChParamFile(int opIndex, const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingOpChFileParams(opIndex, lines, index);
        }

        Io::ParamWriter writer(opmOpFormat);

        writeOpChFileParams(opIndex, writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opmOpFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    readOpParams(opIndex, *reader);
}

void GuiOpm::exportOpChParam(int opIndex)
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpmOp }, Io::Extension::opmOpParam,
        [this, opIndex](const juce::File& file) { writeOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpm::writeOpChParamFile(int opIndex, const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(opmOpFormat);
    writeOpChFileParams(opIndex, writer);

    writer.writeTo(file);
}


// チャンネル 1 つぶん。
void GuiOpm::readChParams(const Io::ParamReader& reader) {
    // Level
    levelComponent.readParams(reader, "level");

    // Algorithm & Feedback
    algSelector.setSelectedId(reader.getInt("alg", algSelector.getSelectedId()), juce::sendNotification);
    feedbackSlider.setValue(reader.getInt("feedback", (int)feedbackSlider.getValue()), juce::sendNotification);

    updateAlgorithmDisplay();

    // Panpot
    panSlider.setValue(reader.getFloat("pan", (float)panSlider.getValue()), juce::sendNotification);

    // OPM LFO
    lfoFreqSlider.setValue(reader.getFloat("lfoFreq", (float)lfoFreqSlider.getValue()), juce::sendNotification);
    lfoSyncDelaySlider.setValue(reader.getFloat("lfoSyncDelay", (float)lfoSyncDelaySlider.getValue()), juce::sendNotification);
    lfoPmToggle.setToggleState(reader.getBool("lfoPm", lfoPmToggle.getToggleState()), juce::sendNotification);
    lfoPgShapeSelector.setSelectedId(reader.getInt("lfoPgShape", lfoPgShapeSelector.getSelectedId()), juce::sendNotification);
    lfoPmsSelector.setSelectedId(reader.getInt("lfoPms", lfoPmsSelector.getSelectedId()), juce::sendNotification);
    lfoPmdSlider.setValue(reader.getFloat("lfoPmd", (float)lfoPmdSlider.getValue()), juce::sendNotification);
    lfoAmToggle.setToggleState(reader.getBool("lfoAm", lfoAmToggle.getToggleState()), juce::sendNotification);
    lfoAmSmRtSlider.setValue(reader.getFloat("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue()), juce::sendNotification);
    lfoEgShapeSelector.setSelectedId(reader.getInt("lfoEgShape", lfoEgShapeSelector.getSelectedId()), juce::sendNotification);
    lfoAmsSelector.setSelectedId(reader.getInt("lfoAms", lfoAmsSelector.getSelectedId()), juce::sendNotification);
    lfoAmdSlider.setValue(reader.getFloat("lfoAmd", (float)lfoAmdSlider.getValue()), juce::sendNotification);

    // Components (Global)
    ssgHwEnv.readParams(reader, "ssgHwEnv");
    ssgHwPEnv.readParams(reader, "ssgHwPEnv");
    ssgSwEnv11g.readParams(reader, "ssgSwEnv11");
    qualityComponent.readParams(reader, "quality");
    unisonComponent.readParams(reader, "unison");

    for (int i = 0; i < OpmPrValue::ops; i++) {
        withOp(i, [this, &reader, i] { readOpParams(i, reader.arrayItem(Io::ParamKey::ops, i)); });
    }

    ampEnvComponent.readParams(reader, "ampEnv");

    ssgSwPEnv11g.readParams(reader, "ssgSwPEnv11");

    modComponent.readParams(reader, "wtMod");
    ampModComponent.readParams(reader, "wtAmpMod");
}

// オペレータ 1 つぶん。並びの中のひとつを渡してもらう。
//
// 名前で引くので、他の音源のファイルを読ませても、こちらに無い
// 項目は勝手に読み飛ばされる。行数を数えて飛ばす細工が要らない。
void GuiOpm::readOpParams(int opIndex, const Io::ParamReader& r) {
    // Mul / Dt
    mul.setSelectedId(r.getInt("mul", mul.getSelectedId()), juce::sendNotification);
    mulRatio.setValue(r.getFloat("mulRatio", (float)mulRatio.getValue()), juce::sendNotification);
    dt1.setValue(r.getFloat("dt1", (float)dt1.getValue()), juce::sendNotification);
    dt2.setValue(r.getFloat("dt2", (float)dt2.getValue()), juce::sendNotification);

    // Env
    rgAr.setValue(r.getFloat("ar", (float)rgAr.getValue()), juce::sendNotification);
    rgD1r.setValue(r.getFloat("d1r", (float)rgD1r.getValue()), juce::sendNotification);
    rgD1l.setValue(r.getFloat("d1l", (float)rgD1l.getValue()), juce::sendNotification);
    rgD2r.setValue(r.getFloat("d2r", (float)rgD2r.getValue()), juce::sendNotification);
    rgRr.setValue(r.getFloat("rr", (float)rgRr.getValue()), juce::sendNotification);
    rgTl.setValue(r.getFloat("tl", (float)rgTl.getValue()), juce::sendNotification);

    // Key Scale
    ksMode.setSelectedId(r.getInt("ksMode", ksMode.getSelectedId()), juce::sendNotification);
    ks.setSelectedId(r.getInt("ks", ks.getSelectedId()), juce::sendNotification);
    ksrOPP.setValue(r.getFloat("ksrOPP", (float)ksrOPP.getValue()), juce::sendNotification);
    kslOPP.setValue(r.getFloat("kslOPP", (float)kslOPP.getValue()), juce::sendNotification);

    // Optional / AM / Mask
    amsEnable.setToggleState(r.getBool("amsEnable", amsEnable.getToggleState()), juce::sendNotification);
    bypass.setToggleState(r.getBool("bypass", bypass.getToggleState()), juce::sendNotification);
    xof.setToggleState(r.getBool("xof", xof.getToggleState()), juce::sendNotification);
    kor.setToggleState(r.getBool("kor", kor.getToggleState()), juce::sendNotification);
    mask.setToggleState(r.getBool("mask", mask.getToggleState()), juce::sendNotification);

    // Components
    fix.readParams(r, "fix");
    pitchEnv.readParams(r, "pitchEnv");
    ssgSwEnv.readParams(r, "ssgSwEnv");
    ssgSwEnv11.readParams(r, "ssgSwEnv11");
    ssgSwPEnv11.readParams(r, "ssgSwPEnv11");
    ssgHwPEnvOp.readParams(r, "ssgHwPEnv");
    wtAmpModOp.readParams(r, "wtAmpMod");
    ssgHwEnvOp.readParams(r, "ssgHwEnv");
    wtModOp.readParams(r, "wtMod");
}

void GuiOpm::writeOpParams(int opIndex, Io::ParamWriter& w) {
    // Mul / Dt
    w.set("mul", mul.getSelectedId());
    w.set("mulRatio", (float)mulRatio.getValue());
    w.set("dt1", (float)dt1.getValue());
    w.set("dt2", (float)dt2.getValue());

    // Env
    w.set("ar", (float)rgAr.getValue());
    w.set("d1r", (float)rgD1r.getValue());
    w.set("d1l", (float)rgD1l.getValue());
    w.set("d2r", (float)rgD2r.getValue());
    w.set("rr", (float)rgRr.getValue());
    w.set("tl", (float)rgTl.getValue());

    // Key Scale
    w.set("ksMode", ksMode.getSelectedId());
    w.set("ks", ks.getSelectedId());
    w.set("ksrOPP", (float)ksrOPP.getValue());
    w.set("kslOPP", (float)kslOPP.getValue());

    // Optional / AM / Mask
    w.set("amsEnable", amsEnable.getToggleState());
    w.set("bypass", bypass.getToggleState());
    w.set("xof", xof.getToggleState());
    w.set("kor", kor.getToggleState());
    w.set("mask", mask.getToggleState());

    // Components
    fix.writeParams(w, "fix");
    pitchEnv.writeParams(w, "pitchEnv");
    ssgSwEnv.writeParams(w, "ssgSwEnv");
    ssgSwEnv11.writeParams(w, "ssgSwEnv11");
    ssgSwPEnv11.writeParams(w, "ssgSwPEnv11");
    ssgHwPEnvOp.writeParams(w, "ssgHwPEnv");
    wtAmpModOp.writeParams(w, "wtAmpMod");
    ssgHwEnvOp.writeParams(w, "ssgHwEnv");
    wtModOp.writeParams(w, "wtMod");
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。並び順を写し直すより確実で、
// 当時の互換の工夫もそのまま残る。
void GuiOpm::setImportingChParams(juce::StringArray& lines, int& index) {
	// Level
	levelComponent.setImportingParams(lines, index);

	// Algorithm & Feedback
	algSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
	feedbackSlider.setValue(lines[index++].getIntValue(), juce::sendNotification);

	updateAlgorithmDisplay();

	// Panpot
	panSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	// OPM LFO
	lfoFreqSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoSyncDelaySlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoPmToggle.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	lfoPgShapeSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
	lfoPmsSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
	lfoPmdSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoAmToggle.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	lfoAmSmRtSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoEgShapeSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
	lfoAmsSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
	lfoAmdSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	// Components (Global)
	ssgHwEnv.setImportingParams(lines, index);
	ssgSwEnv11g.setImportingParams(lines, index);
	qualityComponent.setImportingParams(lines, index);
	unisonComponent.setImportingParams(lines, index);

	for (int i = 0; i < OpmPrValue::ops; i++) {
	    getImportingOpParams(i, lines, index);
	}

	// AMP ENV は後から足したので、旧フォーマットとの互換のため
	// ファイル末尾から読む。行が無ければ既定のままにする。
	if (index < lines.size()) {
	    ampEnvComponent.setImportingParams(lines, index);
	}

	if (index < lines.size()) {
	    ssgSwPEnv11g.setImportingParams(lines, index);
	}

	if (index < lines.size()) {
	    modComponent.setImportingBaseParams(lines, index);
	    modComponent.setImportingShapeParam(lines, index);
	}

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpm::writeChParams(Io::ParamWriter& writer) {
	// Level
	levelComponent.writeParams(writer, "level");

	// Algorithm & Feedback
	writer.set("alg", algSelector.getSelectedId());
	writer.set("feedback", (float)feedbackSlider.getValue());

	// Panpot
	writer.set("pan", (float)panSlider.getValue());

	// OPM LFO
	writer.set("lfoFreq", (float)lfoFreqSlider.getValue());
	writer.set("lfoSyncDelay", (float)lfoSyncDelaySlider.getValue());
	writer.set("lfoPm", lfoPmToggle.getToggleState());
	writer.set("lfoPgShape", lfoPgShapeSelector.getSelectedId());
	writer.set("lfoPms", lfoPmsSelector.getSelectedId());
	writer.set("lfoPmd", (float)lfoPmdSlider.getValue());
	writer.set("lfoAm", lfoAmToggle.getToggleState());
	writer.set("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue());
	writer.set("lfoEgShape", lfoEgShapeSelector.getSelectedId());
	writer.set("lfoAms", lfoAmsSelector.getSelectedId());
	writer.set("lfoAmd", (float)lfoAmdSlider.getValue());

	// Components (Global)
	ssgHwEnv.writeParams(writer, "ssgHwEnv");
	ssgHwPEnv.writeParams(writer, "ssgHwPEnv");
	ssgSwEnv11g.writeParams(writer, "ssgSwEnv11");
	qualityComponent.writeParams(writer, "quality");
	unisonComponent.writeParams(writer, "unison");

	for (int i = 0; i < OpmPrValue::ops; i++) {
	    auto op = writer.arrayItem(Io::ParamKey::ops, i);

	    writeOpParams(i, op);
	}

	// 名前で持つので、置き場所に意味は無い
	ampEnvComponent.writeParams(writer, "ampEnv");
	ssgSwPEnv11g.writeParams(writer, "ssgSwPEnv11");
	modComponent.writeParams(writer, "wtMod");
	ampModComponent.writeParams(writer, "wtAmpMod");

	
}

// 3.0.0 より前の形式を読むための補助。履歴から戻したもの。
void GuiOpm::getImportingOpParams(int opIndex, juce::StringArray& lines, int& index) {
    // Mul / Dt
    mul.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    mulRatio.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    dt1.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    dt2.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Env
    rgAr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD1r.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD1l.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD2r.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgRr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgTl.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Key Scale
    ksMode.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ks.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ksrOPP.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    kslOPP.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Optional / AM / Mask
    amsEnable.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    bypass.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    xof.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    kor.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    mask.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

    // Components
    fix.setImportingParams(lines, index);
    pitchEnv.setImportingParams(lines, index);
    ssgSwEnv.setImportingParams(lines, index);
    ssgSwEnv11.setImportingParams(lines, index);
    ssgSwPEnv11.setImportingParams(lines, index);
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpm::setImportingLfoParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 15) return;

	lfoFreqSlider.setValue(lines[0].getIntValue(), juce::sendNotification);
	lfoSyncDelaySlider.setValue(lines[1].getIntValue(), juce::sendNotification);
	lfoPmToggle.setToggleState(lines[2].getIntValue() == 1, juce::sendNotification);
	lfoPgShapeSelector.setSelectedItemIndex(lines[3].getIntValue(), juce::sendNotification);
	lfoPmsSelector.setSelectedItemIndex(lines[4].getIntValue(), juce::sendNotification);
	lfoPmdSlider.setValue(lines[5].getIntValue(), juce::sendNotification);
	lfoAmToggle.setToggleState(lines[6].getIntValue() == 1, juce::sendNotification);
	lfoAmSmRtSlider.setValue(lines[7].getFloatValue(), juce::sendNotification);
	lfoEgShapeSelector.setSelectedItemIndex(lines[8].getIntValue(), juce::sendNotification);
	lfoAmsSelector.setSelectedItemIndex(lines[9].getIntValue(), juce::sendNotification);
	lfoAmdSlider.setValue(lines[10].getIntValue(), juce::sendNotification);

	for (int i = 0; i < OpmPrValue::ops; i++) {
	    amsEnable.setToggleState(lines[11 + i].getIntValue() == 1, juce::sendNotification);
	}

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpm::writeLfoParams(Io::ParamWriter& writer) {
	writer.set("lfoFreq", (int)lfoFreqSlider.getValue());
	writer.set("lfoSyncDelay", (int)lfoSyncDelaySlider.getValue());
	writer.set("lfoPm", lfoPmToggle.getToggleState());
	writer.set("lfoPgShape", lfoPgShapeSelector.getSelectedItemIndex());
	writer.set("lfoPms", lfoPmsSelector.getSelectedItemIndex());
	writer.set("lfoPmd", (int)lfoPmdSlider.getValue());
	writer.set("lfoAm", lfoAmToggle.getToggleState());
	writer.set("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue());
	writer.set("lfoEgShape", lfoEgShapeSelector.getSelectedItemIndex());
	writer.set("lfoAms", lfoAmsSelector.getSelectedItemIndex());
	writer.set("lfoAmd", (int)lfoAmdSlider.getValue());

	std::vector<int> amsValues;

	for (int i = 0; i < OpmPrValue::ops; i++) {
	    amsValues.push_back(amsEnable.getToggleState() ? 1 : 0);
	}

	writer.setArray("amsEnable", amsValues);

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpm::setImportingQualityParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 2) return;

	qualityComponent.setBit(lines[0].getIntValue());
	qualityComponent.setRate(lines[1].getIntValue());

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpm::writeQualityParams(Io::ParamWriter& writer) {
	writer.set("bit", qualityComponent.getBit());
	writer.set("rate", qualityComponent.getRate());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpm::setImportingOpChFileParams(int opIndex, juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	getImportingOpParams(opIndex, lines, index);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpm::writeOpChFileParams(int opIndex, Io::ParamWriter& writer) {
	writeOpParams(opIndex, writer);

	
}
