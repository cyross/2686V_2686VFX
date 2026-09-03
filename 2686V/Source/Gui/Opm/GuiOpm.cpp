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

    auto updateMulRatioEnable = [this](int idx) {
        int mulIndex = mul[idx].getSelectedId() - 1;
        bool enableMulRatio = mulIndex == 16; // mul = Ratio

        mulRatio[idx].setEnabledWithLabel(enableMulRatio);
        };

    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    const juce::String code = OpmPrKey::prefix;
    int tabOrder = 1;

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

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

    targerOpSlider.setup({ .parent = mainGroup.contentCanvas, .title = "Op", .isReset = false });
    targerOpSlider.setRange(1.0, 4.0, 1.0);
    targerOpSlider.setNumDecimalPlacesToDisplay(0);
    targerOpSlider.setValue(1, juce::sendNotification);
    targerOpSlider.setWantsKeyboardFocus(true);
    targerOpSlider.setExplicitFocusOrder(++tabOrder);

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

    for (int i = 0; i < OpmPrValue::ops; ++i)
    {
        opGroups[i].setup(*this, OpmGuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        catDet[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpmGuiText::Category::detune, .enableChangeDetailVisible = true });
        catAmp[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpmGuiText::Category::ampEnv, .detailVisible = true, .enableChangeDetailVisible = true });

        mul[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::mul, .title = OpmGuiText::Fm::Op::Mul, .items = multems, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);
        mul[i].onChange = [this, i, updateMulRatioEnable] {
            updateMulRatioEnable(i);
            };

        mulRatio[i].setupComponent(opGroups[i].contentCanvas, paramPrefix + CPK::mulRatio, OpmGuiText::Fm::Op::MulRatio, tabOrder, std::nullopt);

        mulRatioTo001[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "0.01", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
		mulRatioTo001[i].setWantsKeyboardFocus(true);
		mulRatioTo001[i].setExplicitFocusOrder(++tabOrder);
		mulRatioTo001[i].onClick = [this, i]() {
			mulRatio[i].setValue(0.01f, juce::sendNotification);
			};

        mulRatioTo005[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "0.05", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
        mulRatioTo005[i].setWantsKeyboardFocus(true);
        mulRatioTo005[i].setExplicitFocusOrder(++tabOrder);
        mulRatioTo005[i].onClick = [this, i]() {
            mulRatio[i].setValue(0.05f, juce::sendNotification);
            };

        mulRatioTo1[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "1", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
        mulRatioTo1[i].setWantsKeyboardFocus(true);
        mulRatioTo1[i].setExplicitFocusOrder(++tabOrder);
        mulRatioTo1[i].onClick = [this, i]() {
            mulRatio[i].setValue(1.0f, juce::sendNotification);
            };

        mulRatioTo10[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "10", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
        mulRatioTo10[i].setWantsKeyboardFocus(true);
        mulRatioTo10[i].setExplicitFocusOrder(++tabOrder);
        mulRatioTo10[i].onClick = [this, i]() {
            mulRatio[i].setValue(10.0f, juce::sendNotification);
            };

        mulRatioTo2757[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "27.57", .bgColor = juce::Colours::lightseagreen.brighter(0.3f), .isReset = false });
        mulRatioTo2757[i].setWantsKeyboardFocus(true);
        mulRatioTo2757[i].setExplicitFocusOrder(++tabOrder);
        mulRatioTo2757[i].onClick = [this, i]() {
            mulRatio[i].setValue(27.57f, juce::sendNotification);
            };

        mulRatioTo02[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "0.2", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
        mulRatioTo02[i].setWantsKeyboardFocus(true);
        mulRatioTo02[i].setExplicitFocusOrder(++tabOrder);
        mulRatioTo02[i].onClick = [this, i]() {
            mulRatio[i].setValue(0.2f, juce::sendNotification);
            };

        mulRatioTo025[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "0.25", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
        mulRatioTo025[i].setWantsKeyboardFocus(true);
        mulRatioTo025[i].setExplicitFocusOrder(++tabOrder);
        mulRatioTo025[i].onClick = [this, i]() {
            mulRatio[i].setValue(0.25f, juce::sendNotification);
            };

        mulRatioTo04[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "0.4", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
        mulRatioTo04[i].setWantsKeyboardFocus(true);
        mulRatioTo04[i].setExplicitFocusOrder(++tabOrder);
        mulRatioTo04[i].onClick = [this, i]() {
            mulRatio[i].setValue(0.4f, juce::sendNotification);
            };

        mulRatioTo05[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "0.5", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
        mulRatioTo05[i].setWantsKeyboardFocus(true);
        mulRatioTo05[i].setExplicitFocusOrder(++tabOrder);
        mulRatioTo05[i].onClick = [this, i]() {
            mulRatio[i].setValue(0.5f, juce::sendNotification);
            };

        mulRatioTo06[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "0.6", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
        mulRatioTo06[i].setWantsKeyboardFocus(true);
        mulRatioTo06[i].setExplicitFocusOrder(++tabOrder);
        mulRatioTo06[i].onClick = [this, i]() {
            mulRatio[i].setValue(0.6f, juce::sendNotification);
            };

        mulRatioTo075[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "0.75", .bgColor = juce::Colours::lightgreen.brighter(0.5f), .isReset = false });
        mulRatioTo075[i].setWantsKeyboardFocus(true);
        mulRatioTo075[i].setExplicitFocusOrder(++tabOrder);
        mulRatioTo075[i].onClick = [this, i]() {
            mulRatio[i].setValue(0.75f, juce::sendNotification);
            };

        mulRatioTo08[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "0.8", .bgColor = juce::Colours::lightsalmon.brighter(0.5f), .isReset = false });
        mulRatioTo08[i].setWantsKeyboardFocus(true);
        mulRatioTo08[i].setExplicitFocusOrder(++tabOrder);
        mulRatioTo08[i].onClick = [this, i]() {
            mulRatio[i].setValue(0.8f, juce::sendNotification);
            };

        mulRatioPM10[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "-10", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
        mulRatioPM10[i].setWantsKeyboardFocus(true);
        mulRatioPM10[i].setExplicitFocusOrder(++tabOrder);
        mulRatioPM10[i].onClick = [this, i]() {
            mulRatio[i].setValue(mulRatio[i].getValue() - 10.0f, juce::sendNotification);
            };

        mulRatioP10[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "+10", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
        mulRatioP10[i].setWantsKeyboardFocus(true);
        mulRatioP10[i].setExplicitFocusOrder(++tabOrder);
        mulRatioP10[i].onClick = [this, i]() {
            mulRatio[i].setValue(mulRatio[i].getValue() + 10.0f, juce::sendNotification);
            };

        mulRatioPM1[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "-1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
        mulRatioPM1[i].setWantsKeyboardFocus(true);
        mulRatioPM1[i].setExplicitFocusOrder(++tabOrder);
        mulRatioPM1[i].onClick = [this, i]() {
            mulRatio[i].setValue(mulRatio[i].getValue() - 1.0f, juce::sendNotification);
            };

        mulRatioP1[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "+1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
        mulRatioP1[i].setWantsKeyboardFocus(true);
        mulRatioP1[i].setExplicitFocusOrder(++tabOrder);
        mulRatioP1[i].onClick = [this, i]() {
            mulRatio[i].setValue(mulRatio[i].getValue() + 1.0f, juce::sendNotification);
            };

        mulRatioPM01[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "-0.1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
        mulRatioPM01[i].setWantsKeyboardFocus(true);
        mulRatioPM01[i].setExplicitFocusOrder(++tabOrder);
        mulRatioPM01[i].onClick = [this, i]() {
            mulRatio[i].setValue(mulRatio[i].getValue() - 0.1f, juce::sendNotification);
            };

        mulRatioP01[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "+0.1", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
        mulRatioP01[i].setWantsKeyboardFocus(true);
        mulRatioP01[i].setExplicitFocusOrder(++tabOrder);
        mulRatioP01[i].onClick = [this, i]() {
            mulRatio[i].setValue(mulRatio[i].getValue() + 0.1f, juce::sendNotification);
            };

        mulRatioPM001[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "-0.01", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
        mulRatioPM001[i].setWantsKeyboardFocus(true);
        mulRatioPM001[i].setExplicitFocusOrder(++tabOrder);
        mulRatioPM001[i].onClick = [this, i]() {
            mulRatio[i].setValue(mulRatio[i].getValue() - 0.01f, juce::sendNotification);
            };

        mulRatioP001[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .id = "", .title = "+0.01", .bgColor = juce::Colours::lightblue.brighter(0.5f), .isReset = false });
        mulRatioP001[i].setWantsKeyboardFocus(true);
        mulRatioP001[i].setExplicitFocusOrder(++tabOrder);
        mulRatioP001[i].onClick = [this, i]() {
            mulRatio[i].setValue(mulRatio[i].getValue() + 0.01f, juce::sendNotification);
            };

        mulDetSep[i].setupComponent(opGroups[i].contentCanvas);

        dt1[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::dt, .title = OpmGuiText::Fm::Op::Dt1, .isReset = true, .regType = RegisterType::FmDt });
        dt1[i].setWantsKeyboardFocus(true);
        dt1[i].setExplicitFocusOrder(++tabOrder);

        dt2[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::dt2, .title = OpmGuiText::Fm::Op::Dt2, .isReset = true, .regType = RegisterType::FmDt2 });
        dt2[i].setWantsKeyboardFocus(true);
        dt2[i].setExplicitFocusOrder(++tabOrder);

        rgAr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgAr, .title = OpmGuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgD1r[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgD1r, .title = OpmGuiText::Fm::Op::D1r, .isReset = true });
        rgD1r[i].setWantsKeyboardFocus(true);
        rgD1r[i].setExplicitFocusOrder(++tabOrder);

        rgD1l[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgD1l, .title = OpmGuiText::Fm::Op::D1l, .isReset = true });
        rgD1l[i].setWantsKeyboardFocus(true);
        rgD1l[i].setExplicitFocusOrder(++tabOrder);

        rgD2r[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgD2r, .title = OpmGuiText::Fm::Op::D2r, .isReset = true });
        rgD2r[i].setWantsKeyboardFocus(true);
        rgD2r[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgRr, .title = OpmGuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgTl, .title = OpmGuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        ksCat[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpmGuiText::Category::ks, .enableChangeDetailVisible = true });

        ksMode[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksMode, .title = "Mode", .items = ksModeItems, .isReset = true });
        ksMode[i].setWantsKeyboardFocus(true);
        ksMode[i].setExplicitFocusOrder(++tabOrder);
        ksMode[i].onChange = [this]() {
            ctx.editor.resized();
            };

        ks[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ks, .title = OpmGuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });
        ks[i].setWantsKeyboardFocus(true);
        ks[i].setExplicitFocusOrder(++tabOrder);

        ksrOPP[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksrOPP, .title = OpmGuiText::Fm::Op::Ksr, .isReset = true });
        ksrOPP[i].setWantsKeyboardFocus(true);
        ksrOPP[i].setExplicitFocusOrder(++tabOrder);

        kslOPP[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::kslOPP, .title = OpmGuiText::Fm::Op::Ksl, .isReset = true });
        kslOPP[i].setWantsKeyboardFocus(true);
        kslOPP[i].setExplicitFocusOrder(++tabOrder);

        catOptional[i].setupSwCategory({ .parent = opGroups[i].contentCanvas, .title = OpmGuiText::Category::optional, .enableChangeDetailVisible = true });

        xof[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::xof, .title = OpmGuiText::Fm::Op::xof, .isReset = true });
        xof[i].setWantsKeyboardFocus(true);
        xof[i].setExplicitFocusOrder(++tabOrder);

        kor[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::kor, .title = OpmGuiText::Fm::Op::kor, .isReset = true });
        kor[i].setWantsKeyboardFocus(true);
        kor[i].setExplicitFocusOrder(++tabOrder);

        bypass[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::bypass, .title = OpmGuiText::Fm::Op::bypass, .isReset = true });
        bypass[i].setWantsKeyboardFocus(true);
        bypass[i].setExplicitFocusOrder(++tabOrder);

        pitchEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::PitchAdsr::enable, OpmGuiText::PitchAdsr::enable, true);

        ssgSwEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv::enable, OpmGuiText::SsgSwEnv::enable, true);

        ssgSwEnv11[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv11::enable, OpmGuiText::SsgSwEnv11::enable, true);

        ssgSwPEnv11[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwPEnv11::enable, OpmGuiText::SsgSwPEnv11::enable, true);
        ssgHwPEnvOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);
        wtAmpModOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);
        ssgHwEnvOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);
        wtModOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);

        cafLfo[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpmGuiText::Category::hwLfo, .enableChangeDetailVisible = true });

        amsEnable[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::OpmLfo::amsEn, .title = OpmGuiText::Fm::Op::AmsEn, .isReset = true });
        amsEnable[i].setWantsKeyboardFocus(true);
        amsEnable[i].setExplicitFocusOrder(++tabOrder);

        fix[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, OpmGuiText::Fm::Op::Opzx7FreqTo440, 440, true);

        catMask[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpmGuiText::Category::mask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::mask, .title = OpmGuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        mmlSeparator[i].setupComponent(opGroups[i].contentCanvas);

        mml[i].setup({ .parent = opGroups[i].contentCanvas, .title = juce::String("") + "MML風入力", .isReset = false, .isResized = false });
        mml[i].setWantsKeyboardFocus(true);
        mml[i].setExplicitFocusOrder(++tabOrder);
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = juce::String("") + "MML風にパラメータを入力してください。 例: AR:31 AR31 DT-1 等",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

        updateMulRatioEnable(i);

        setupGraph(i);
        updateOpGraph(i);
    }
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

    int opWidth = pageArea.getWidth() / 4;

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < OpmPrValue::ops; ++i)
    {
        auto opArea = pageArea.removeFromLeft(opWidth);

        // 枠線
        opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto iinnerRect = opArea.reduced(OpmGuiValue::Fm::Op::Padding::width, OpmGuiValue::Fm::Op::Padding::height);
        iinnerRect.removeFromTop(OpmGuiValue::Group::TitlePaddingTop);

        // グラフ用の区画を確保
        layoutOpGraph(i, iinnerRect);
        updateOpGraph(i);

        // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
        // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
        opGroups[i].setViewportCustomBounds(iinnerRect.translated(-opArea.getX(), -opArea.getY()));

        // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
        juce::Rectangle<int> innerRect(0, 0, opGroups[i].getContentWidth(), 2000);

        updateRgDisplayAsOp(i, true);

        layoutOpAmpCat(i, innerRect);

        layoutOpOptionalCat(i, innerRect);

        ssgHwEnvOp[i].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv));
        ssgHwEnvOp[i].layoutComponentRow(innerRect);
        ssgSwEnv[i].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv));
        ssgSwEnv[i].layoutComponentRow(innerRect);
        ssgSwEnv11[i].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11));
        ssgSwEnv11[i].layoutComponentRow(innerRect);
        wtAmpModOp[i].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));
        wtAmpModOp[i].layoutComponent(innerRect);

        pitchEnv[i].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::PitchEnv));
        pitchEnv[i].layoutComponentRow(innerRect);
        ssgHwPEnvOp[i].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv));
        ssgHwPEnvOp[i].layoutComponentRow(innerRect);
        ssgSwPEnv11[i].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11));
        ssgSwPEnv11[i].layoutComponentRow(innerRect);
        wtModOp[i].setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));
        wtModOp[i].layoutComponent(innerRect);

        layoutOpKsCat(i, innerRect);

        layoutOpDetCat(i, innerRect);

        layoutOpHwLfoCat(i, innerRect);

        fix[i].layoutComponentRow(innerRect);

        layoutOpMaskCat(i, innerRect);

        mmlSeparator[i].layoutComponent(innerRect);

        layoutRow({ .rowRect = innerRect, .component = &mml[i], .paddingBottom = 0 });

        int usedHeight = 2000 - innerRect.getHeight();

        // 下部の余白を足して、キャンバスの最終的な高さをセット
        opGroups[i].setContentHeight(usedHeight + 20);
    }

    updateAlgorithmDisplay();
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpm::applyMmlString(const juce::String& mml, int opIndex)
{
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMl,   [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixDt,   [&](int v) { dt1[opIndex].setValue(RegisterConverter::convertMmlDtToReg(v), juce::sendNotification); } },
        { mmlPrefixDt1,   [&](int v) { dt1[opIndex].setValue(RegisterConverter::convertMmlDtToReg(v), juce::sendNotification); } },
        { mmlPrefixDto,   [&](int v) { dt1[opIndex].setValue(RegisterConverter::convertMmlDtToReg(v), juce::sendNotification); } },
        { mmlPrefixDt2,  [&](int v) { dt2[opIndex].setValue(RegisterConverter::convertMmlDt2ToReg(v), juce::sendNotification); } },
        { mmlPrefixDtt,  [&](int v) { dt2[opIndex].setValue(RegisterConverter::convertMmlDt2ToReg(v), juce::sendNotification); } },
        { mmlPrefixKs,   [&](int v) { ks[opIndex].setSelectedItemIndex(RegisterConverter::convertFmKs(v), juce::sendNotification); } },
        { mmlPrefixMask, [&](int v) { mask[opIndex].setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },

        // --- TL系 (RGモードで分岐) ---
        { mmlPrefixTl,   [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixO,    [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixOl,   [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},

        // --- エンベロープ系 (RGモードで分岐) ---
        { mmlPrefixAr,   [&](int v) { rgAr[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixD1r,   [&](int v) { rgD1r[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixD2r,   [&](int v) { rgD2r[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixD1l,   [&](int v) { rgD1l[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
        { mmlPrefixRr,   [&](int v) { rgRr[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }}
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

void GuiOpm::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    mul[idx].setEnabledWithLabel(enable);
    dt1[idx].setEnabledWithLabel(enable);
    dt2[idx].setEnabledWithLabel(enable);
    ks[idx].setEnabledWithLabel(enable);
    catOptional[idx].setEnabled(enable);
    xof[idx].setEnabled(enable);
    kor[idx].setEnabled(enable);
    bypass[idx].setEnabled(enable);
    fix[idx].setEnables(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    mmlSeparator[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
    pitchEnv[idx].setEnabled(enable);
    ssgSwEnv[idx].setEnabled(enable);
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
        updateOpEnable(i, activeOps[i]);
    }
}

void GuiOpm::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].setVisibleWithLabel(rgMode);
    rgD1r[idx].setVisibleWithLabel(rgMode);
    rgD1l[idx].setVisibleWithLabel(rgMode);
    rgD2r[idx].setVisibleWithLabel(rgMode);
    rgRr[idx].setVisibleWithLabel(rgMode);
    rgTl[idx].setVisibleWithLabel(rgMode);
}

void GuiOpm::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

// ==============================================================================
// Keyboard Shortcut Logic
// ==============================================================================
bool GuiOpm::keyPressed(const juce::KeyPress& key)
{
    int opIndex = -1;
    int code = key.getKeyCode();
    juce::ModifierKeys metaKeys = key.getModifiers();

    // 通常の 1〜4キー、または テンキーの 1〜4 を判定
    if (code == '1' || code == juce::KeyPress::numberPad1) opIndex = 0;
    else if (code == '2' || code == juce::KeyPress::numberPad2) opIndex = 1;
    else if (code == '3' || code == juce::KeyPress::numberPad3) opIndex = 2;
    else if (code == '4' || code == juce::KeyPress::numberPad4) opIndex = 3;

    // 対応するキーが押されていたら、該当する処理を実行
    if (opIndex != -1)
    {
        // Altキーを押しながら -> マスクのON/OFF、それ以外 -> MMLボタンクリック
        // 該当オペレータが有効(Enabled)な時のみ反応させる
        if (metaKeys.isAltDown() && mask[opIndex].isEnabled()) {
            mask[opIndex].setToggleState(!mask[opIndex].getToggleState(), juce::sendNotification);
        }
        else if (mml[opIndex].isEnabled()) {
            mml[opIndex].triggerClick();
        }

        return true; // キー入力を消費したことをJUCEに伝える
    }

    return false; // 他のキーなら無視（通常処理へ）
}

void GuiOpm::copyFmParamsToString()
{
    int mask1 = FmMml::bool2Int(!mask[0].getToggleState());
    int mask2 = FmMml::bool2Int(!mask[1].getToggleState());
    int mask3 = FmMml::bool2Int(!mask[2].getToggleState());
    int mask4 = FmMml::bool2Int(!mask[3].getToggleState());
    int maskVal = FmMml::genMask4(mask1, mask2, mask2, mask3);

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
    auto formatOpBasic = [this](int index) {
        return juce::String::formatted(
            // ' MUL     DT   DT2   AR  D1R  D1L  D2R   RR   TL   KS   AMSEN
            u8"   %2d, %+1d,  %1d, %2d, %2d, %2d, %2d, %2d, %3d, %1d,    %1d\n",
            (int)this->mul[index].getSelectedId() - 1,               // MUL
            this->dt1[index].getValue() - 1,                         // DT
            (int)this->dt2[index].getValue(),                        // DT2
            (int)this->rgAr[index].getValue(),                       // AR
            (int)this->rgD1r[index].getValue(),                      // D1R
            (int)this->rgD1l[index].getValue(),                      // D1L
            (int)this->rgD2r[index].getValue(),                      // D2R
            (int)this->rgRr[index].getValue(),                       // RR
            (int)this->rgTl[index].getValue(),                       // TL
            this->ks[index].getSelectedId() - 1,                     // KS
            FmMml::bool2Int(this->amsEnable[index].getToggleState()) // AM
        );
        };
    auto formatOpsBasic = [this, formatOpBasic]() {
        return formatOpBasic(0) + formatOpBasic(1) + formatOpBasic(2) + formatOpBasic(3);
        };
    auto formatOpExt = [this](int index) {
        // ' MUL AR DR SL RR TL KSR KSL
        return juce::String::formatted(
            u8"MUL%d DT1%+d DT2+%d AR%d D1R%d D1L%d D2R%d RR%d TL%d KS%d\n",
            (int)this->mul[index].getSelectedId() - 1,
            this->dt1[index].getValue() - 1,
            (int)this->dt2[index].getValue(),
            (int)this->rgAr[index].getValue(),
            (int)this->rgD1r[index].getValue(),
            (int)this->rgD1l[index].getValue(),
            (int)this->rgD2r[index].getValue(),
            (int)this->rgRr[index].getValue(),
            (int)this->rgTl[index].getValue(),
            this->ks[index].getSelectedId() - 1
        );
        };
    auto formatOpsExt = [this, formatOpExt]() {
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
    targerOpSlider.setVisibleWithLabel(visible);
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
        layoutMain({ .mainRect = rect, .label = &targerOpSlider.label, .component = &targerOpSlider });

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

void GuiOpm::layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask[opIndex] });

    bool visibleMask = catMask[opIndex].isDetailVisible();

    mask[opIndex].setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask[opIndex] });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }

    // MASK の後ろには分類の外の行 (セパレータや MML) が続くので、
    // 板をここで閉じないと 1 行ぶん下まで伸びてしまう。
    closeCategoryBackdrops(catMask[opIndex].getParentComponent(), rect.getY());
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

void GuiOpm::layoutOpHwLfoCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &cafLfo[opIndex] });

    bool visible = cafLfo[opIndex].isDetailVisible();

    amsEnable[opIndex].setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &amsEnable[opIndex] });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpm::layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &ksCat[opIndex] });

    bool visible = ksCat[opIndex].isDetailVisible();
    FmRgAdddrKeyScaleMode mode = (FmRgAdddrKeyScaleMode)(ksMode[opIndex].getSelectedItemIndex());

    ksMode[opIndex].setVisibleWithLabel(visible);
    ks[opIndex].setVisibleWithLabel(visible && mode == FmRgAdddrKeyScaleMode::OPM);
    ksrOPP[opIndex].setVisibleWithLabel(visible && mode == FmRgAdddrKeyScaleMode::OPP);
    kslOPP[opIndex].setVisibleWithLabel(visible && mode == FmRgAdddrKeyScaleMode::OPP);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &ksMode[opIndex].label, .component = &ksMode[opIndex] });

        switch (mode) {
        case FmRgAdddrKeyScaleMode::OPM:
            layoutRow({ .rowRect = rect, .label = &ks[opIndex].label, .component = &ks[opIndex] });
            break;
        case FmRgAdddrKeyScaleMode::OPP:
            layoutRow({ .rowRect = rect, .label = &ksrOPP[opIndex].label, .component = &ksrOPP[opIndex] });
            layoutRow({ .rowRect = rect, .label = &kslOPP[opIndex].label, .component = &kslOPP[opIndex] });
            break;
        }

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpm::layoutOpDetCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catDet[opIndex] });

    bool visible = catDet[opIndex].isDetailVisible();

    mul[opIndex].setVisibleWithLabel(visible);
    mulRatio[opIndex].setVisibleWithLabel(visible);
    mulRatioTo001[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioTo005[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioTo1[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioTo10[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioTo2757[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioTo02[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioTo025[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioTo04[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioTo05[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioTo06[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioTo075[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioTo08[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioPM10[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioP10[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioPM1[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioP1[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioPM01[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioP01[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioPM001[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulRatioP001[opIndex].setVisible(visible && mulRatio[opIndex].isVisibleNudge());
    mulDetSep[opIndex].setVisible(visible);
    dt1[opIndex].setVisibleWithLabel(visible);
    dt2[opIndex].setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &mul[opIndex].label, .component = &mul[opIndex] });
        mulRatio[opIndex].layoutComponentRow(rect);
		if (mulRatio[opIndex].isVisibleNudge())
		{
            layoutRowFiveComps({ .rect = rect, .comp1 = &mulRatioTo001[opIndex], .comp2 = &mulRatioTo005[opIndex], .comp3 = &mulRatioTo1[opIndex], .comp4 = &mulRatioTo10[opIndex], .comp5 = &mulRatioTo2757[opIndex] });
            layoutRowThreeComps({ .rect = rect, .comp1 = &mulRatioTo025[opIndex], .comp2 = &mulRatioTo05[opIndex], .comp3 = &mulRatioTo075[opIndex] });
            layoutRowFourComps({ .rect = rect, .comp1 = &mulRatioTo02[opIndex], .comp2 = &mulRatioTo04[opIndex], .comp3 = &mulRatioTo06[opIndex], .comp4 = &mulRatioTo08[opIndex] });
            layoutRowFourComps({ .rect = rect, .comp1 = &mulRatioPM10[opIndex], .comp2 = &mulRatioPM1[opIndex], .comp3 = &mulRatioP1[opIndex], .comp4 = &mulRatioP10[opIndex] });
            layoutRowFourComps({ .rect = rect, .comp1 = &mulRatioPM001[opIndex], .comp2 = &mulRatioPM01[opIndex], .comp3 = &mulRatioP01[opIndex], .comp4 = &mulRatioP001[opIndex] });
		}
        mulDetSep[opIndex].layoutComponent(rect);
        layoutRow({ .rowRect = rect, .label = &dt1[opIndex].label, .component = &dt1[opIndex] });
        layoutRow({ .rowRect = rect, .label = &dt2[opIndex].label, .component = &dt2[opIndex] });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpm::layoutOpAmpCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catAmp[opIndex] });

    bool visible = catAmp[opIndex].isDetailVisible();

    rgAr[opIndex].setVisibleWithLabel(visible);
    rgD1r[opIndex].setVisibleWithLabel(visible);
    rgD2r[opIndex].setVisibleWithLabel(visible);
    rgD1l[opIndex].setVisibleWithLabel(visible);
    rgRr[opIndex].setVisibleWithLabel(visible);
    rgTl[opIndex].setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &rgAr[opIndex].label, .component = &rgAr[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgD1r[opIndex].label, .component = &rgD1r[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgD1l[opIndex].label, .component = &rgD1l[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgD2r[opIndex].label, .component = &rgD2r[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgRr[opIndex].label, .component = &rgRr[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgTl[opIndex].label, .component = &rgTl[opIndex] });

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
    // カーブモードが有効かどうかを判定
    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();
    if (currentGlobalGraphMode == GlobalGraphMode::SsgSw11) {
        ssgSwEnv11g.updateGraph(gGraph, p_curveCore, isCurveMode, 0);
    }
    else if (currentGlobalGraphMode == GlobalGraphMode::SsgSwP11) {
        ssgSwPEnv11g.updateGraph(gGraph, p_curveCore, isCurveMode, 0);
    }
    else {
        ampEnvComponent.updateGraph(gGraph, p_curveCore, isCurveMode, 0);
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

void GuiOpm::setupGraph(int opIndex)
{
    addAndMakeVisible(&opGraphs[opIndex]); // グラフを追加

    graphBtnAmp[opIndex].setup({ .parent = *this, .title = "AMP", .isReset = false, .isResized = false });
    graphBtnAmp[opIndex].setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::Amp); };

    graphBtnPitch[opIndex].setup({ .parent = *this, .title = "PIT", .isReset = false, .isResized = false });
    graphBtnPitch[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::Pitch); };

    graphBtnSsg[opIndex].setup({ .parent = *this, .title = "SSG", .isReset = false, .isResized = false });
    graphBtnSsg[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::SsgSw); };

    graphBtnSsg11[opIndex].setup({ .parent = *this, .title = "S11", .isReset = false, .isResized = false });
    graphBtnSsg11[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::SsgSw11); };

    graphBtnSsgP11[opIndex].setup({ .parent = *this, .title = "P11", .isReset = false, .isResized = false });
    graphBtnSsgP11[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::SsgSwP11); };

    auto repaintGraph = [this, opIndex]() {
        if (this->isUpdatingGraph) return; // 既に更新中なら無視

        // 旗は必ず下ろす。途中で抜けたときに立ちっぱなしになると、
        // 以後グラフの更新が全部素通りしてしまうため。
        const juce::ScopedValueSetter<bool> guard(this->isUpdatingGraph, true);

        this->updateOpGraph(opIndex);
        };

    bypass[opIndex].onStateChange = repaintGraph;
    xof[opIndex].onStateChange = repaintGraph;
    kor[opIndex].onStateChange = repaintGraph;

    rgAr[opIndex].onValueChange = repaintGraph;
    rgD1r[opIndex].onValueChange = repaintGraph;
    rgD2r[opIndex].onValueChange = repaintGraph;
    rgD1l[opIndex].onValueChange = repaintGraph;
    rgRr[opIndex].onValueChange = repaintGraph;
    rgTl[opIndex].onValueChange = repaintGraph;

    pitchEnv[opIndex].setupGraph(repaintGraph);
    ssgSwEnv[opIndex].setupGraph(repaintGraph);
    ssgSwEnv11[opIndex].setupGraph(repaintGraph);
    ssgSwPEnv11[opIndex].setupGraph(repaintGraph);

    graphSeparator[opIndex].setupComponent(*this);
}

void GuiOpm::setGraphMode(int opIndex, GraphMode mode)
{
    currentGraphMode[opIndex] = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp[opIndex].setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch[opIndex].setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg[opIndex].setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);
    graphBtnSsg11[opIndex].setToggleState(mode == GraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11[opIndex].setToggleState(mode == GraphMode::SsgSwP11, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateOpGraph(opIndex);
}

void GuiOpm::layoutOpGraph(int opIndex, juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(OpmGuiValue::ParamGroup::Graph::height + NormalSeparator::getHeight());

    graphSeparator[opIndex].layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(OpmGuiValue::ParamGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 5;

    graphBtnAmp[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg11[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsgP11[opIndex].setBounds(btnArea);

    // 残りをグラフエリアにする
    opGraphs[opIndex].setBounds(mainArea);
}

// グラフを再計算して描画
void GuiOpm::updateOpGraph(int opIndex)
{
    GraphMode mode = currentGraphMode[opIndex];

    // カーブモードが有効かどうかを判定
    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();

    // -------------------------------------------------------------
    // Helper: 幅の計算 (Amp 用)
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return maxWidth;

        if (rateValue <= 0.0f) return maxWidth;

        float norm = 1.0f - (rateValue / maxRate);

        return maxWidth * norm;
        };

    // -------------------------------------------------------------
    // Helper: カーブ関数を生成する
    // -------------------------------------------------------------
    auto getCurveFunc = [this, isCurveMode](int posIdx, int targetIdx, int prmIdx) {
        return [this, isCurveMode, posIdx, targetIdx, prmIdx](float progress) -> float {
            if (!isCurveMode || p_curveCore == nullptr) return progress;
            return p_curveCore->process(posIdx, targetIdx, prmIdx, progress);
            };
        };

    int posIdx = opIndex + 1; // Position::Op1 = 1, Op2 = 2 ... (Common=0) に合わせる

    // =============================================================
    // Pitch Env
    // =============================================================
    if (mode == GraphMode::Pitch) {
        pitchEnv[opIndex].updateGraph(opGraphs[opIndex], p_curveCore, isCurveMode, posIdx);
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        ssgSwEnv[opIndex].updateGraph(opGraphs[opIndex], p_curveCore, isCurveMode, posIdx);
    }
    // =============================================================
    // SSG SW Env 11
    // =============================================================
    else if (mode == GraphMode::SsgSw11) {
        ssgSwEnv11[opIndex].updateGraph(opGraphs[opIndex], p_curveCore, isCurveMode, posIdx);
    }
    // =============================================================
    // SSG SW PEnv 11
    // =============================================================
    else if (mode == GraphMode::SsgSwP11) {
        ssgSwPEnv11[opIndex].updateGraph(opGraphs[opIndex], p_curveCore, isCurveMode, posIdx);
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        bool isBypass = bypass[opIndex].getToggleState();

        opGraphs[opIndex].updateBypass(isBypass);

        if (isBypass) {
            return;
        }

        bool isXof = xof[opIndex].getToggleState();
        bool isKor = kor[opIndex].getToggleState();

        float arMax = (float)rgAr[opIndex].getMaximum();
        float d1rMax = (float)rgD1r[opIndex].getMaximum();
        float d2rMax = (float)rgD2r[opIndex].getMaximum();
        float d1lMax = (float)rgD1l[opIndex].getMaximum();
        float rrMax = (float)rgRr[opIndex].getMaximum();
        float tlMax = (float)rgTl[opIndex].getMaximum();

        float arVal = (float)rgAr[opIndex].getValue();
        float d1rVal = (float)rgD1r[opIndex].getValue();
        float d2rVal = (float)rgD2r[opIndex].getValue();
        float d1lVal = (float)rgD1l[opIndex].getValue();
        float rrVal = (float)rgRr[opIndex].getValue();
        float tlVal = (float)rgTl[opIndex].getValue();

        float sl = (d1lMax - d1lVal) / d1lMax; // 15=0.0, 0=1.0
        float tlScale = 1.0f - (tlVal / tlMax); // TL=127で無音

        std::vector<GuiEnvelopeGraph::PhaseDef> phases;
        auto color = juce::Colours::cyan;
        int targetIdx = (int)CurveParams::Target::AmpEnv; // または RegValue

        float currentTotalWidth = 0.0f;

        // 1. Attack
        float attackWidth = rateToWidth(arVal, arMax);
        phases.push_back({
            .widthPx = attackWidth, .startLevel = 0.0f, .endLevel = 1.0f * tlScale, .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Ar),
            .phaseLineColor = juce::Colours::red
            });
        currentTotalWidth += attackWidth;

        // 2. Decay 1
        float decayWidth = rateToWidth(d1rVal, d1rMax);
        phases.push_back({
            .widthPx = decayWidth, .startLevel = 1.0f * tlScale, .endLevel = sl * tlScale, .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Dr),
            .phaseLineColor = juce::Colours::blue
            });
        currentTotalWidth += decayWidth;

        // 3. Sustain (D2R) の実線部分 (キーオン中)
        float releaseStartLevel = sl;
        float keyOnWidth = 60.0f;
        float sustainTotalWidth = 0.0f;

        if (d2rVal > 0.0f && d2rVal < d2rMax) {
            sustainTotalWidth = rateToWidth(d2rVal, d2rMax, 300.0f);

            // カーブを加味したレベル計算
            float decayRatio = sustainTotalWidth / 300.0f;
            auto curveFunce = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Sr);
            float drCurvedRatio = curveFunce(1.0f);
            float rrCurvedRatio = curveFunce(0.5f);
            releaseStartLevel = sl - (sl * rrCurvedRatio);

            phases.push_back({
                .widthPx = sustainTotalWidth, .startLevel = sl * tlScale, .endLevel = 0.0f, .color = color,
                .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Sr),
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
                .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Rr),
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX,
                .isMax = (rrVal == rrMax)
                });
        }

        opGraphs[opIndex].setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
    }
}

void GuiOpm::layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catOptional[opIndex] });

    bool visible = catOptional[opIndex].isDetailVisible();

    xof[opIndex].setVisible(visible);
    kor[opIndex].setVisible(visible);
    bypass[opIndex].setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &xof[opIndex] });
        layoutRow({ .rowRect = rect, .component = &kor[opIndex] });
        layoutRow({ .rowRect = rect, .component = &bypass[opIndex] });

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
    copyObj.detune.mul = mul[p].getSelectedId();
    copyObj.detune.mulRatio = mulRatio[p].getValue();
    copyObj.detune.dt = dt1[p].getValue();
    copyObj.detune.dt2 = dt2[p].getValue();
    copyObj.aAdsr.ar = rgAr[p].getValue();
    copyObj.aAdsr.d1r = rgD1r[p].getValue();
    copyObj.aAdsr.d1l = rgD1l[p].getValue();
    copyObj.aAdsr.d2r = rgD2r[p].getValue();
    copyObj.aAdsr.rr = rgRr[p].getValue();
    copyObj.aAdsr.tl = rgTl[p].getValue();
    copyObj.aAdsr.bypass = bypass[p].getToggleState();
    copyObj.aAdsr.kor = kor[p].getToggleState();
    copyObj.aAdsr.xof = xof[p].getToggleState();
    copyObj.aAdsr.mode = (FmRgAdddrKeyScaleMode)ksMode[p].getSelectedId();
    copyObj.aAdsr.ks = ks[p].getSelectedId();
    copyObj.aAdsr.ksrOPP = ksrOPP[p].getValue();
    copyObj.aAdsr.kslOPP = kslOPP[p].getValue();
    copyObj.lfo.amsEn = amsEnable[p].getToggleState();

    copyObj.mask.mask = mask[p].getToggleState();

    fix[p].copyParams(copyObj.fix);
    pitchEnv[p].copyParams(copyObj.pAdsr);
    ssgSwEnv[p].copyParams(copyObj.aSsgSw);
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
    mul[p].setSelectedId(copyObj.detune.mul, juce::sendNotification);
    mulRatio[p].setValue(copyObj.detune.mulRatio, juce::sendNotification);
    dt1[p].setValue(copyObj.detune.dt, juce::sendNotification);
    dt2[p].setValue(copyObj.detune.dt2, juce::sendNotification);
    rgAr[p].setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgD1r[p].setValue(copyObj.aAdsr.d1r, juce::sendNotification);
    rgD1l[p].setValue(copyObj.aAdsr.d1l, juce::sendNotification);
    rgD2r[p].setValue(copyObj.aAdsr.d2r, juce::sendNotification);
    rgRr[p].setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl[p].setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass[p].setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor[p].setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof[p].setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ksMode[p].setSelectedId((int)copyObj.aAdsr.mode, juce::sendNotification);
    ks[p].setSelectedId(copyObj.aAdsr.ks, juce::sendNotification);
    ksrOPP[p].setValue(copyObj.aAdsr.ksrOPP, juce::sendNotification);
    kslOPP[p].setValue(copyObj.aAdsr.kslOPP, juce::sendNotification);
    amsEnable[p].setToggleState(copyObj.lfo.amsEn, juce::sendNotification);

    mask[p].setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix[p].pasteParams(copyObj.fix);
    pitchEnv[p].pasteParams(copyObj.pAdsr);
    ssgSwEnv[p].pasteParams(copyObj.aSsgSw);
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
    copyObj.detune.mul = mul[p].getSelectedId();
    copyObj.detune.dt = dt1[p].getValue();
    copyObj.aAdsr.ar = rgAr[p].getValue();
    copyObj.aAdsr.dr = rgD1r[p].getValue();
    copyObj.aAdsr.sl = rgD1l[p].getValue();
    copyObj.aAdsr.sr = rgD2r[p].getValue();
    copyObj.aAdsr.rr = rgRr[p].getValue();
    copyObj.aAdsr.tl = rgTl[p].getValue();
    copyObj.aAdsr.bypass = bypass[p].getToggleState();
    copyObj.aAdsr.kor = kor[p].getToggleState();
    copyObj.aAdsr.xof = xof[p].getToggleState();
    copyObj.aAdsr.ks = ks[p].getSelectedId();

    copyObj.mask.mask = mask[p].getToggleState();

    fix[p].copyParams(copyObj.fix);
    pitchEnv[p].copyParams(copyObj.pAdsr);
    ssgSwEnv[p].copyParams(copyObj.aSsgSw);
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
    mul[p].setSelectedId(copyObj.detune.mul, juce::sendNotification);
    dt1[p].setValue(copyObj.detune.dt, juce::sendNotification);
    rgAr[p].setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgD1r[p].setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgD1l[p].setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgD2r[p].setValue(copyObj.aAdsr.sr, juce::sendNotification);
    rgRr[p].setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl[p].setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass[p].setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor[p].setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof[p].setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ks[p].setSelectedId(copyObj.aAdsr.ks, juce::sendNotification);

    mask[p].setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix[p].pasteParams(copyObj.fix);
    pitchEnv[p].pasteParams(copyObj.pAdsr);
    ssgSwEnv[p].pasteParams(copyObj.aSsgSw);
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
    copyObj.detune.mul = mul[p].getSelectedItemIndex();
    copyObj.detune.dt = dt1[p].getValue();
    copyObj.aAdsr.ar = rgAr[p].getValue();
    copyObj.aAdsr.dr = rgD1r[p].getValue();
    copyObj.aAdsr.sl = rgD1l[p].getValue();
    copyObj.aAdsr.sr = rgD2r[p].getValue();
    copyObj.aAdsr.rr = rgRr[p].getValue();
    copyObj.aAdsr.tl = rgTl[p].getValue();
    copyObj.aAdsr.bypass = bypass[p].getToggleState();
    copyObj.aAdsr.kor = kor[p].getToggleState();
    copyObj.aAdsr.xof = xof[p].getToggleState();
    copyObj.aAdsr.ks = ks[p].getSelectedId();

    copyObj.mask.mask = mask[p].getToggleState();

    fix[p].copyParams(copyObj.fix);
    pitchEnv[p].copyParams(copyObj.pAdsr);
    ssgSwEnv[p].copyParams(copyObj.aSsgSw);
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
    mul[p].setSelectedItemIndex(copyObj.detune.mul, juce::sendNotification);
    dt1[p].setValue(copyObj.detune.dt, juce::sendNotification);
    rgAr[p].setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgD1r[p].setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgD1l[p].setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgD2r[p].setValue(copyObj.aAdsr.sr, juce::sendNotification);
    rgRr[p].setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl[p].setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass[p].setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor[p].setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof[p].setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ks[p].setSelectedId(copyObj.aAdsr.ks, juce::sendNotification);

    mask[p].setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix[p].pasteParams(copyObj.fix);
    pitchEnv[p].pasteParams(copyObj.pAdsr);
    ssgSwEnv[p].pasteParams(copyObj.aSsgSw);
}

void GuiOpm::importPitchEnvParam(int opIndex) {
    pitchEnv[opIndex].importParams();
}

void GuiOpm::exportPitchEnvParam(int opIndex) {
    pitchEnv[opIndex].exportParams();
}

void GuiOpm::importSsgSwEnvParam(int opIndex) {
    ssgSwEnv[opIndex].importParams();
}

void GuiOpm::exportSsgSwEnvParam(int opIndex) {
    ssgSwEnv[opIndex].exportParams();
}

void GuiOpm::importLfoParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultLfoParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importLfoParamFile, defaultDir, Io::ExtensionGlob::OpmLfoParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

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
                    amsEnable[i].setToggleState(amsValues[(size_t)i] != 0, juce::sendNotification);
                }
            }
        });
}

void GuiOpm::exportLfoParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultLfoParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportLfoParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::OpmLfoParam)), Io::saveGlob(Io::Extension::OpmLfoParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(opmLfoFormat);
                writeLfoParams(writer);

                writer.writeTo(file);
            }
        });
}

void GuiOpm::importQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importQualityParamFile, defaultDir, Io::ExtensionGlob::QualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

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
        });
}

void GuiOpm::exportQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportQualityParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::QualityParam)), Io::saveGlob(Io::Extension::QualityParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(qualityFormat);
                writeQualityParams(writer);

                writer.writeTo(file);
            }
        });
}

void GuiOpm::importSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11[opIndex].importParams();
}

void GuiOpm::exportSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11[opIndex].exportParams();
}

void GuiOpm::importSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11[opIndex].importParams();
}

void GuiOpm::exportSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11[opIndex].exportParams();
}

void GuiOpm::importOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp[opIndex].importParams();
}

void GuiOpm::exportOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp[opIndex].exportParams();
}

void GuiOpm::importOpWtAmpModParam(int opIndex) {
    wtAmpModOp[opIndex].importParams();
}

void GuiOpm::exportOpWtAmpModParam(int opIndex) {
    wtAmpModOp[opIndex].exportParams();
}

void GuiOpm::importOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp[opIndex].importParams();
}

void GuiOpm::exportOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp[opIndex].exportParams();
}

void GuiOpm::importOpWtModParam(int opIndex) {
    wtModOp[opIndex].importParams();
}

void GuiOpm::exportOpWtModParam(int opIndex) {
    wtModOp[opIndex].exportParams();
}

void GuiOpm::importChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::opmParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

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
        });

}

void GuiOpm::exportChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::opmParam)), Io::saveGlob(Io::Extension::opmParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(opmFormat);
                writeChParams(writer);

                writer.writeTo(file);
            }
        });

}

void GuiOpm::importOpChParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::opmOpParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

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
        });

}

void GuiOpm::exportOpChParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::opmOpParam)), Io::saveGlob(Io::Extension::opmOpParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(opmOpFormat);
                writeOpChFileParams(opIndex, writer);

                writer.writeTo(file);
            }
        });

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
        readOpParams(i, reader.arrayItem(Io::ParamKey::ops, i));
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
    mul[opIndex].setSelectedId(r.getInt("mul", mul[opIndex].getSelectedId()), juce::sendNotification);
    mulRatio[opIndex].setValue(r.getFloat("mulRatio", (float)mulRatio[opIndex].getValue()), juce::sendNotification);
    dt1[opIndex].setValue(r.getFloat("dt1", (float)dt1[opIndex].getValue()), juce::sendNotification);
    dt2[opIndex].setValue(r.getFloat("dt2", (float)dt2[opIndex].getValue()), juce::sendNotification);

    // Env
    rgAr[opIndex].setValue(r.getFloat("ar", (float)rgAr[opIndex].getValue()), juce::sendNotification);
    rgD1r[opIndex].setValue(r.getFloat("d1r", (float)rgD1r[opIndex].getValue()), juce::sendNotification);
    rgD1l[opIndex].setValue(r.getFloat("d1l", (float)rgD1l[opIndex].getValue()), juce::sendNotification);
    rgD2r[opIndex].setValue(r.getFloat("d2r", (float)rgD2r[opIndex].getValue()), juce::sendNotification);
    rgRr[opIndex].setValue(r.getFloat("rr", (float)rgRr[opIndex].getValue()), juce::sendNotification);
    rgTl[opIndex].setValue(r.getFloat("tl", (float)rgTl[opIndex].getValue()), juce::sendNotification);

    // Key Scale
    ksMode[opIndex].setSelectedId(r.getInt("ksMode", ksMode[opIndex].getSelectedId()), juce::sendNotification);
    ks[opIndex].setSelectedId(r.getInt("ks", ks[opIndex].getSelectedId()), juce::sendNotification);
    ksrOPP[opIndex].setValue(r.getFloat("ksrOPP", (float)ksrOPP[opIndex].getValue()), juce::sendNotification);
    kslOPP[opIndex].setValue(r.getFloat("kslOPP", (float)kslOPP[opIndex].getValue()), juce::sendNotification);

    // Optional / AM / Mask
    amsEnable[opIndex].setToggleState(r.getBool("amsEnable", amsEnable[opIndex].getToggleState()), juce::sendNotification);
    bypass[opIndex].setToggleState(r.getBool("bypass", bypass[opIndex].getToggleState()), juce::sendNotification);
    xof[opIndex].setToggleState(r.getBool("xof", xof[opIndex].getToggleState()), juce::sendNotification);
    kor[opIndex].setToggleState(r.getBool("kor", kor[opIndex].getToggleState()), juce::sendNotification);
    mask[opIndex].setToggleState(r.getBool("mask", mask[opIndex].getToggleState()), juce::sendNotification);

    // Components
    fix[opIndex].readParams(r, "fix");
    pitchEnv[opIndex].readParams(r, "pitchEnv");
    ssgSwEnv[opIndex].readParams(r, "ssgSwEnv");
    ssgSwEnv11[opIndex].readParams(r, "ssgSwEnv11");
    ssgSwPEnv11[opIndex].readParams(r, "ssgSwPEnv11");
    ssgHwPEnvOp[opIndex].readParams(r, "ssgHwPEnv");
    wtAmpModOp[opIndex].readParams(r, "wtAmpMod");
    ssgHwEnvOp[opIndex].readParams(r, "ssgHwEnv");
    wtModOp[opIndex].readParams(r, "wtMod");
}

void GuiOpm::writeOpParams(int opIndex, Io::ParamWriter& w) {
    // Mul / Dt
    w.set("mul", mul[opIndex].getSelectedId());
    w.set("mulRatio", (float)mulRatio[opIndex].getValue());
    w.set("dt1", (float)dt1[opIndex].getValue());
    w.set("dt2", (float)dt2[opIndex].getValue());

    // Env
    w.set("ar", (float)rgAr[opIndex].getValue());
    w.set("d1r", (float)rgD1r[opIndex].getValue());
    w.set("d1l", (float)rgD1l[opIndex].getValue());
    w.set("d2r", (float)rgD2r[opIndex].getValue());
    w.set("rr", (float)rgRr[opIndex].getValue());
    w.set("tl", (float)rgTl[opIndex].getValue());

    // Key Scale
    w.set("ksMode", ksMode[opIndex].getSelectedId());
    w.set("ks", ks[opIndex].getSelectedId());
    w.set("ksrOPP", (float)ksrOPP[opIndex].getValue());
    w.set("kslOPP", (float)kslOPP[opIndex].getValue());

    // Optional / AM / Mask
    w.set("amsEnable", amsEnable[opIndex].getToggleState());
    w.set("bypass", bypass[opIndex].getToggleState());
    w.set("xof", xof[opIndex].getToggleState());
    w.set("kor", kor[opIndex].getToggleState());
    w.set("mask", mask[opIndex].getToggleState());

    // Components
    fix[opIndex].writeParams(w, "fix");
    pitchEnv[opIndex].writeParams(w, "pitchEnv");
    ssgSwEnv[opIndex].writeParams(w, "ssgSwEnv");
    ssgSwEnv11[opIndex].writeParams(w, "ssgSwEnv11");
    ssgSwPEnv11[opIndex].writeParams(w, "ssgSwPEnv11");
    ssgHwPEnvOp[opIndex].writeParams(w, "ssgHwPEnv");
    wtAmpModOp[opIndex].writeParams(w, "wtAmpMod");
    ssgHwEnvOp[opIndex].writeParams(w, "ssgHwEnv");
    wtModOp[opIndex].writeParams(w, "wtMod");
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
    mul[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    mulRatio[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    dt1[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    dt2[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Env
    rgAr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD1r[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD1l[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgD2r[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgRr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgTl[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Key Scale
    ksMode[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ks[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    ksrOPP[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    kslOPP[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Optional / AM / Mask
    amsEnable[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    bypass[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    xof[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    kor[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    mask[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

    // Components
    fix[opIndex].setImportingParams(lines, index);
    pitchEnv[opIndex].setImportingParams(lines, index);
    ssgSwEnv[opIndex].setImportingParams(lines, index);
    ssgSwEnv11[opIndex].setImportingParams(lines, index);
    ssgSwPEnv11[opIndex].setImportingParams(lines, index);
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
	    amsEnable[i].setToggleState(lines[11 + i].getIntValue() == 1, juce::sendNotification);
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
	    amsValues.push_back(amsEnable[i].getToggleState() ? 1 : 0);
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
