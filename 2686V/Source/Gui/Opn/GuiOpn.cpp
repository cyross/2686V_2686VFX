#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiOpn.h"

#include "../../Core/Gui/GuiRefresh.h"

#include "../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat opnFormat{ "opn", 1 };
	const Io::ParamFormat opnOpFormat{ "opnOp", 1 };

	// OPNA のファイルを読み込む口があるため、その印も要る
	const Io::ParamFormat opnaFormat{ "opna", 1 };
	const Io::ParamFormat opnaOpFormat{ "opnaOp", 1 };
	const Io::ParamFormat n88LfoFormat{ "n88Lfo", 1 };
	const Io::ParamFormat qualityFormat{ "quality", 1 };
}

#include "../Components/WavePreview/WavePreviewSource.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Opn/ProcessorOpnKeys.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"
#include "../../Processor/Opna/ProcessorOpnaValues.h"
#include "../../Core/Const/ConstMmlKeys.h"
#include "../../Core/Const/ConstMmlValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Fm/FmRegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiGraphValues.h"
#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiOpnValues.h"
#include "./GuiOpnText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> opnAlgItems = {
    {.name = "00: <OPN-00>", .value = 1 },
    {.name = "01: <OPN-01>", .value = 2 },
    {.name = "02: <OPN-02>", .value = 3 },
    {.name = "03: <OPN-03>", .value = 4 },
    {.name = "04: <OPN-04>", .value = 5 },
    {.name = "05: <OPN-05>", .value = 6 },
    {.name = "06: <OPN-06>", .value = 7 },
    {.name = "07: <OPN-07>", .value = 8 },
};

static std::vector<SelectItem> multems = {
    {.name = " 0:   0.5x", .value = 1 },
    {.name = " 1:   1x", .value = 2 },
    {.name = " 2:   2x", .value = 3 },
    {.name = " 3:   3x", .value = 4 },
    {.name = " 4:   4x", .value = 5 },
    {.name = " 5:   5x", .value = 6 },
    {.name = " 6:   6x", .value = 7 },
    {.name = " 7:   7x", .value = 8 },
    {.name = " 8:   8x", .value = 9 },
    {.name = " 9:   9x", .value = 10 },
    {.name = "10:  10x", .value = 11 },
    {.name = "11:  11x", .value = 12 },
    {.name = "12:  12x", .value = 13 },
    {.name = "13:  13x", .value = 14 },
    {.name = "14:  14x", .value = 15 },
    {.name = "15:  15x", .value = 16 }
};

// DT (デチューン1) 用のコンボボックスアイテム
// レジスタ仕様: 0=0, 1=+1, 2=+2, 3=+3, 4=0, 5=-1, 6=-2, 7=-3
static std::vector<SelectItem> dtItems = {
    {.name = " 0", .value = 1 },
    {.name = "-3", .value = 2 },
    {.name = "-2", .value = 3 },
    {.name = "-1", .value = 4 },
    {.name = " 0", .value = 5 }, // 実質0ですが、レジスタ4として一応用意
    {.name = "+1", .value = 6 },
    {.name = "+2", .value = 7 },
    {.name = "+3", .value = 8 }
};

static std::vector<SelectItem> ksItems = {
    {.name = "0 OFF",      .value = 1},
    {.name = "1 (Weak)",   .value = 2},
    {.name = "2 (Mid)",    .value = 3},
    {.name = "3 (Strong)", .value = 4}
};

static std::vector<SelectItem> lfoShapeItems = {
    {.name = "0: Saw Up",              .value = 1 },
    {.name = "1: Square",              .value = 2 },
    {.name = "2: Triangle",            .value = 3 },
    {.name = "3: Sample & Hold",       .value = 4 },
    {.name = "4: Saw Down & One Shot", .value = 5 },
    {.name = "5: Triangle & One Shot", .value = 6 },
};

void GuiOpn::setup()
{
    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    const juce::String code = OpnPrKey::prefix;
    int tabOrder = 1;

    p_curveCore = ctx.audioProcessor.getCurveCore();

    mainGroup.setup(*this, OpnGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    qualityComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    algFbCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Category::algFb });

    algSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::alg, .title = OpnGuiText::Fm::alg, .items = opnAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    mainGroup.contentCanvas.addAndMakeVisible(&algStaticGraphComp);

    algFbSep.setupComponent(mainGroup.contentCanvas);

    feedbackSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::fb, .title = OpnGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    lfoCat.setupSwLfoCategory({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Category::n88Lfo, .enableChangeDetailVisible = true });

    lfoFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::freq, .title = OpnGuiText::Fm::lfoSpeed, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoShapeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::shape, .title = OpnGuiText::Fm::lfoShape, .items = lfoShapeItems, .isReset = true });
    lfoShapeSelector.setWantsKeyboardFocus(true);
    lfoShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::amSmoothRatio, .title = OpnGuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmPreview.setup(mainGroup.contentCanvas, GuiColor::WavePreview::Lfo);
    lfoAmPreview.setup(mainGroup.contentCanvas, GuiColor::WavePreview::Lfo);

    auto refreshLfoPreviews = [this]() { this->updateLfoPreviews(); };

    lfoShapeSelector.onChange = refreshLfoPreviews;
    lfoAmSmRtSlider.onValueChange = refreshLfoPreviews;

    updateLfoPreviews();

    lfoSyncDelaySlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::syncDelay, .title = OpnGuiText::Fm::lfoSyncDelay, .isReset = true });
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

    lfoPmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::pm, .title = OpnGuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::pmd, .title = OpnGuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::pms, .title = OpnGuiText::Fm::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::am, .title = OpnGuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::amd, .title = OpnGuiText::Fm::amd, .isReset = true });
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

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Utility::bcLevel });
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
        ctx.editor.copyOpnParamsToOpna();
        };

    copyParamsToOpmBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Params -> OPM", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyParamsToOpmBtn.setWantsKeyboardFocus(true);
    copyParamsToOpmBtn.setExplicitFocusOrder(++tabOrder);
    copyParamsToOpmBtn.onClick = [this] {
        ctx.editor.copyOpnParamsToOpm();
        };

    uSep002.setupComponent(mainGroup.contentCanvas);

    copyOpParamBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Copy Op Params", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamBtn.setWantsKeyboardFocus(true);
    copyOpParamBtn.setExplicitFocusOrder(++tabOrder);
    copyOpParamBtn.onClick = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        ctx.editor.copyOpnOpParams(from, to);
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

    imOpnaOpChParam.setupComponentOp(mainGroup.contentCanvas, tabOrder, "OPNA OP Params");
    imOpnaOpChParam.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpnaOpChParam(opIndex); };

    // TARGET。設定ひとそろいの帯へ置く。値が動いたら束縛と枠線を付け替える。
    targerOpSlider.setup({ .parent = *this, .title = "TARGET", .isReset = false });
    targerOpSlider.setRange(1.0, (double)OpnPrValue::ops, 1.0);
    targerOpSlider.setNumDecimalPlacesToDisplay(0);
    targerOpSlider.setWantsKeyboardFocus(true);
    targerOpSlider.setExplicitFocusOrder(++tabOrder);
    targerOpSlider.onValueChange = [this] { applyOpTarget(); };

    uSep004.setupComponent(mainGroup.contentCanvas);

    ieLfo.setupComponent(mainGroup.contentCanvas, tabOrder, "N88 LFO");
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

    imOpnaChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "OPNA CH Params");
    imOpnaChParam.onClickImport = [this] { importOpnaChParam(); };

    // Operators
    const juce::String opCode = code + CPK::op;

    // ---- 上の絵と、下の設定 ----
    //
    // 絵はオペレータの数だけ。設定はひとそろいだけ置き、TARGET で
    // 指し先を切り替える。
    for (int i = 0; i < OpnPrValue::ops; ++i)
    {
        cells[(size_t)i].setup(*this, i, OpnGuiText::Group::opPrefix + juce::String(i + 1), false);
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
    colN88Lfo.setup(stripCanvas, juce::String("") + "N88 LFO");
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

    catDet.setupHwCategory({ .parent = colKs.contentCanvas, .title = OpnGuiText::Category::detune, .enableChangeDetailVisible = true });
    catAmp.setupHwCategory({ .parent = colAmp.contentCanvas, .title = OpnGuiText::Category::ampEnv, .detailVisible = true, .enableChangeDetailVisible = true });

    mul.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::mul, .title = OpnGuiText::Fm::Op::Mul, .items = multems, .isReset = true });
    mul.setWantsKeyboardFocus(true);
    mul.setExplicitFocusOrder(++tabOrder);

    dt.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::dt, .title = OpnGuiText::Fm::Op::Dt, .items = dtItems, .isReset = true });
    dt.setWantsKeyboardFocus(true);
    dt.setExplicitFocusOrder(++tabOrder);

    rgAr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgAr, .title = OpnGuiText::Fm::Op::Ar, .isReset = true });
    rgAr.setWantsKeyboardFocus(true);
    rgAr.setExplicitFocusOrder(++tabOrder);

    rgDr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgDr, .title = OpnGuiText::Fm::Op::Dr, .isReset = true });
    rgDr.setWantsKeyboardFocus(true);
    rgDr.setExplicitFocusOrder(++tabOrder);

    rgSl.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgSl, .title = OpnGuiText::Fm::Op::Sl, .isReset = true });
    rgSl.setWantsKeyboardFocus(true);
    rgSl.setExplicitFocusOrder(++tabOrder);

    rgSr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgSr, .title = OpnGuiText::Fm::Op::Sr, .isReset = true });
    rgSr.setWantsKeyboardFocus(true);
    rgSr.setExplicitFocusOrder(++tabOrder);

    rgRr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgRr, .title = OpnGuiText::Fm::Op::Rr, .isReset = true });
    rgRr.setWantsKeyboardFocus(true);
    rgRr.setExplicitFocusOrder(++tabOrder);

    rgTl.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgTl, .title = OpnGuiText::Fm::Op::Tl, .isReset = true });
    rgTl.setWantsKeyboardFocus(true);
    rgTl.setExplicitFocusOrder(++tabOrder);

    ksCat.setupHwCategory({ .parent = colKs.contentCanvas, .title = OpnGuiText::Category::ks, .enableChangeDetailVisible = true });

    ks.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ks, .title = OpnGuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });
    ks.setWantsKeyboardFocus(true);
    ks.setExplicitFocusOrder(++tabOrder);

    catOptional.setupSwCategory({ .parent = colAmp.contentCanvas, .title = OpnGuiText::Category::optional, .enableChangeDetailVisible = true });

    opDelay.setupComponent(colAmp.contentCanvas, paramPrefix + CPK::delay, "DELAY", tabOrder, std::nullopt);

    opDelayNudge.setupComponent(colAmp.contentCanvas, opDelay.getSlider(), tabOrder);

    opDelaySeparator.setupComponent(colAmp.contentCanvas);

    xof.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::xof, .title = OpnGuiText::Fm::Op::xof, .isReset = true });
    xof.setWantsKeyboardFocus(true);
    xof.setExplicitFocusOrder(++tabOrder);

    kor.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::kor, .title = OpnGuiText::Fm::Op::kor, .isReset = true });
    kor.setWantsKeyboardFocus(true);
    kor.setExplicitFocusOrder(++tabOrder);

    bypass.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::bypass, .title = OpnGuiText::Fm::Op::bypass, .isReset = true });
    bypass.setWantsKeyboardFocus(true);
    bypass.setExplicitFocusOrder(++tabOrder);

    pitchEnv.setupComponent(colPitchEnv.contentCanvas, paramPrefix, tabOrder, CPK::PitchAdsr::enable, OpnGuiText::PitchAdsr::enable, true);

    ssgSwEnv.setupComponent(colSsgSwEnv.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv::enable, OpnGuiText::SsgSwEnv::enable, true);

    ssgSwEnv11.setupComponent(colSsgSwEnv11.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv11::enable, OpnGuiText::SsgSwEnv11::enable, true);

    ssgSwPEnv11.setupComponent(colSsgSwPEnv11.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwPEnv11::enable, OpnGuiText::SsgSwPEnv11::enable, true);
    ssgHwPEnvOp.setupComponent(colSsgHwPEnv.contentCanvas, paramPrefix, tabOrder);
    wtAmpModOp.setupComponent(colAmpMod.contentCanvas, paramPrefix, tabOrder);
    ssgHwEnvOp.setupComponent(colSsgHwEnv.contentCanvas, paramPrefix, tabOrder);
    wtModOp.setupComponent(colMod.contentCanvas, paramPrefix, tabOrder);

    catN88Lfo.setupSwLfoCategory({ .parent = colN88Lfo.contentCanvas, .title = OpnGuiText::Category::n88Lfo, .enableChangeDetailVisible = true });

    n88Ams.setup(GuiSlider::Config{ .parent = colN88Lfo.contentCanvas, .id = paramPrefix + CPK::N88Lfo::ams, .title = OpnGuiText::Fm::Op::Ams, .isReset = true });
    n88Ams.setWantsKeyboardFocus(true);
    n88Ams.setExplicitFocusOrder(++tabOrder);

    fix.setupComponent(colKs.contentCanvas, paramPrefix, tabOrder, OpnGuiText::Fm::Op::Opzx7FreqTo440, 440, true);

    catMask.setupHwCategory({ .parent = colMask.contentCanvas, .title = OpnGuiText::Category::mask, .enableChangeDetailVisible = true });

    mask.setup(GuiToggleButton::Config{ .parent = colMask.contentCanvas, .id = paramPrefix + CPK::Fm::mask, .title = OpnGuiText::Fm::Op::Mask, .isReset = true });
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
        &colMod, &colKs, &colN88Lfo, &colMask,
        })
    {
        for (auto* child : group->contentCanvas.getChildren())
        {
            if (auto* cat = dynamic_cast<GuiCategoryLabel*>(child)) cat->setDetailVisible(true);
        }
    }

    // 前に開いていたときの指し先から始める。
    const int saved = (int)ctx.audioProcessor.apvts.state.getProperty(ProcessorStateKey::opnTarget, 0);

    targerOpSlider.setValue(juce::jlimit(0, OpnPrValue::ops - 1, saved) + 1, juce::dontSendNotification);

    applyOpTarget();
}

void GuiOpn::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    auto mainArea = pageArea.removeFromLeft(OpnGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(OpnGuiValue::Group::Padding::width, OpnGuiValue::Group::Padding::height);

    mmRect.removeFromTop(OpnGuiValue::Group::TitlePaddingTop);

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

    mRect.removeFromTop(OpnGuiValue::Category::paddingTop);

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

    layoutN88LfoCat(mRect);

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
    constexpr int rows = (OpnPrValue::ops + cols - 1) / cols;

    const int cellW = pageArea.getWidth() / cols;
    const int cellH = GuiTargetCell::naturalHeight(false);

    auto cellsArea = pageArea.removeFromTop(cellH * rows);

    for (int r = 0; r < rows; ++r) {
        auto rowArea = cellsArea.removeFromTop(cellH);

        for (int col = 0; col < cols; ++col) {
            const int i = r * cols + col;

            auto cellArea = rowArea.removeFromLeft(cellW);

            if (i >= OpnPrValue::ops) continue;

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
void GuiOpn::applyMmlString(const juce::String& mml)
{
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mul.setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMl,   [&](int v) { mul.setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixDt,   [&](int v) { dt.setSelectedItemIndex(RegisterConverter::convertMmlDtToReg(v), juce::sendNotification); } },
        { mmlPrefixKs,   [&](int v) { ks.setSelectedItemIndex(RegisterConverter::convertFmKs(v), juce::sendNotification); } },
        { mmlPrefixMask, [&](int v) { mask.setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },

        // --- TL系 (RGモードで分岐) ---
        { mmlPrefixTl,   [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixO,    [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixOl,   [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},

        // --- エンベロープ系 (RGモードで分岐) ---
        { mmlPrefixAr,   [&](int v) { rgAr.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixDr,   [&](int v) { rgDr.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixSr,   [&](int v) { rgSr.setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixSl,   [&](int v) { rgSl.setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
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
void GuiOpn::updateOpEnable(int idx, bool enable)
{
    if (idx != currentOp()) return;

    mul.setEnabledWithLabel(enable);
    dt.setEnabledWithLabel(enable);
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

void GuiOpn::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > OpnPrValue::Alg::max) return;

    // Coreのルーティング情報から FmAlgState を生成
    const auto& routing = OpnCore::routings[algIndex];
    FmAlgState s;
    s.numOps = OpnPrValue::ops;

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

    for (int i = 0; i < OpnPrValue::ops; ++i)
    {
        // 配列を使わず、到達可能性から判定したフラグをセット
        opActive[(size_t)i] = activeOps[i];

        updateOpEnable(i, activeOps[i]);
    }
}

void GuiOpn::updateRgDisplayAsOp(bool rgMode)
{
    rgAr.setVisibleWithLabel(rgMode);
    rgDr.setVisibleWithLabel(rgMode);
    rgSl.setVisibleWithLabel(rgMode);
    rgSr.setVisibleWithLabel(rgMode);
    rgRr.setVisibleWithLabel(rgMode);
    rgTl.setVisibleWithLabel(rgMode);
}

// TARGET が今どのオペレータを指しているか (0 から数える)。
int GuiOpn::currentOp() const
{
    return juce::jlimit(0, OpnPrValue::ops - 1, (int)targerOpSlider.getValue() - 1);
}

// TARGET が指すオペレータへ、設定の束縛を丸ごと移す。
//
// オペレータごとに部品を並べるのをやめたので、指し先はここで差し替える。
// 見た目は組み直さない。
void GuiOpn::rebind(int opIndex)
{
    const juce::String code = OpnPrKey::prefix + CPK::op + juce::String(opIndex);

    opNameLabel.setText(OpnGuiText::Group::opPrefix + juce::String(opIndex + 1), juce::dontSendNotification);

    mul.rebind(code + CPK::mul);
    dt.rebind(code + CPK::dt);
    ks.rebind(code + CPK::Fm::ks);

    rgAr.rebind(code + CPK::Fm::rgAr);
    rgDr.rebind(code + CPK::Fm::rgDr);
    rgSl.rebind(code + CPK::Fm::rgSl);
    rgSr.rebind(code + CPK::Fm::rgSr);
    rgRr.rebind(code + CPK::Fm::rgRr);
    rgTl.rebind(code + CPK::Fm::rgTl);

    opDelay.getSlider().rebind(code + CPK::delay);

    xof.rebind(code + CPK::Fm::xof);
    kor.rebind(code + CPK::Fm::kor);
    bypass.rebind(code + CPK::Fm::bypass);

    n88Ams.rebind(code + CPK::N88Lfo::ams);

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
}

// 指し先を切り替える。
//
// 設定の束縛を移し、枠線を付け替える。開き直したときに続きから
// 触れるよう、指し先は覚えておく。
void GuiOpn::applyOpTarget()
{
    const int op = currentOp();

    ctx.audioProcessor.apvts.state.setProperty(ProcessorStateKey::opnTarget, op, nullptr);

    rebind(op);

    for (int i = 0; i < OpnPrValue::ops; ++i) cells[(size_t)i].setActive(i == op);

    // 鳴っていないオペレータは触れないようにしてある。指し先が変わったので
    // 入り切りを引き直す。
    updateAlgorithmDisplay();
}

// 指し先を一時的に動かして何かをする。
//
// 設定はひとそろいしか無いので、TARGET が指していないオペレータを
// 読み書きするには、いったんそこへ繋ぎ替えるしかない。
// 終わったら必ず元へ戻す。
void GuiOpn::withOp(int opIndex, const std::function<void()>& fn)
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
void GuiOpn::setGraphMode(GuiEnvGraphMode mode)
{
    currentGraphMode = mode;

    // 札は排他。押したものだけを入れる。
    graphBtnAmp.setToggleState(mode == GuiEnvGraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GuiEnvGraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GuiEnvGraphMode::SsgSw, juce::dontSendNotification);
    graphBtnSsg11.setToggleState(mode == GuiEnvGraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11.setToggleState(mode == GuiEnvGraphMode::SsgSwP11, juce::dontSendNotification);

    for (int i = 0; i < OpnPrValue::ops; ++i) updateOpGraph(i);
}

void GuiOpn::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

bool GuiOpn::keyPressed(const juce::KeyPress& key)
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

    const juce::String opPrefix = OpnPrKey::prefix + CPK::op + juce::String(opIndex);

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

void GuiOpn::copyFmParamsToString()
{
    // オペレータごとに部品を並べるのをやめたので、値はつまみからではなく
    // パラメータから直に採る。画面へ出ているのは TARGET の 1 本だけで、
    // MML には 4 本ぶんが要る。
    //
    // 選ぶ形のパラメータは、つまみの選択番号から 1 を引いたものと同じ値を
    // 持っている。以前 getSelectedId() - 1 と書いていたところがこれに当たる。
    auto& apvts = ctx.audioProcessor.apvts;

    auto opVal = [&apvts](int index, const juce::String& key) {
        return (int)GuiGraphValues::value(apvts, OpnPrKey::prefix + CPK::op + juce::String(index) + key);
        };

    auto opMask = [&apvts](int index) {
        return FmMml::bool2Int(!GuiGraphValues::flag(apvts,
            OpnPrKey::prefix + CPK::op + juce::String(index) + CPK::Fm::mask));
        };

    int maskVal = FmMml::genMask4(opMask(0), opMask(1), opMask(2), opMask(3));

    auto formatCoreBasic = [this, maskVal]() {
        return juce::String::formatted(
            // '   ALG    FB   MSK    SHP  SPD  SYC  PMD  PMS  AMD
            u8"    %1d,  %1d,  %2d,   %2d, %5d, %3d,  %d, %+d,  %d\n",
            algSelector.getSelectedId() - 1,      // ALG
            (int)feedbackSlider.getValue(),       // FB
            maskVal,                              // MASK
            lfoShapeSelector.getSelectedId() - 1, // SHAPE
            (int)lfoFreqSlider.getValue(),        // SPEED
            (int)lfoSyncDelaySlider.getValue(),   // SYNC
            (int)lfoPmdSlider.getValue(),         // PMD
            (int)lfoPmsSlider.getValue(),         // PMS
            (int)lfoAmdSlider.getValue()          // AMD
        );
        };
    auto formatOpBasic = [&opVal](int index) {
        return juce::String::formatted(
            // ' MUL    DT   AR   DR   SL   RR   SR   TL   KS  AMS
            u8"  %2d, %+1d, %2d, %2d, %2d, %2d, %2d, %3d, %1d, %+d\n",
            opVal(index, CPK::mul),                    // MUL
            FmMml::int2dt(opVal(index, CPK::dt)),      // DT
            opVal(index, CPK::Fm::rgAr),               // AR
            opVal(index, CPK::Fm::rgDr),               // DR
            opVal(index, CPK::Fm::rgSl),               // SL
            opVal(index, CPK::Fm::rgSr),               // SR
            opVal(index, CPK::Fm::rgRr),               // RR
            opVal(index, CPK::Fm::rgTl),               // TL
            opVal(index, CPK::Fm::ks),                 // KS
            opVal(index, CPK::N88Lfo::ams)             // AMS
        );
        };
    auto formatOpsBasic = [&formatOpBasic]() {
        return formatOpBasic(0) + formatOpBasic(1) + formatOpBasic(2) + formatOpBasic(3);
        };
    auto formatOpExt = [&opVal](int index) {
        return juce::String::formatted(
            // ' MUL    DT   AR   DR   SL   SR   RR   TL   KS
            u8"MUL%d DT%+d AR%d DR%d SL%d SR%d RR%d TL%d KS%d\n",
            opVal(index, CPK::mul),
            FmMml::int2dt(opVal(index, CPK::dt)),
            opVal(index, CPK::Fm::rgAr),
            opVal(index, CPK::Fm::rgDr),
            opVal(index, CPK::Fm::rgSl),
            opVal(index, CPK::Fm::rgSr),
            opVal(index, CPK::Fm::rgRr),
            opVal(index, CPK::Fm::rgTl),
            opVal(index, CPK::Fm::ks)
        );
        };
    auto formatOpsExt = [&formatOpExt]() {
        return formatOpExt(0) + formatOpExt(1) + formatOpExt(2) + formatOpExt(3);
        };

    juce::String mml = juce::String("[OPN]\n")
        + FmMml::basicMmlHeader
        + juce::String(u8"' ALG FB0 FB2 MASK SHAPE  SPEED SYNC PMD PMS AMD\n")
        + formatCoreBasic()
        + juce::String(u8"' MUL DT  AR  DR  SL  SR  RR   TL KS AMS\n")
        + formatOpsBasic()
        + juce::String(u8"\n")
        + FmMml::extMmlHeader
        + formatOpsExt();
    juce::SystemClipboard::copyTextToClipboard(mml);
}

void GuiOpn::copyFmParamsToObject()
{

}

void GuiOpn::pasteFmParamsFromObject()
{

}

void GuiOpn::initParams()
{
    this->ctx.audioProcessor.initParams("OPN_");
}

void GuiOpn::layoutOpMaskCat(juce::Rectangle<int>& rect) {
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

// 設定ひとそろいの置き方。
//
// 一番上が TARGET の帯、その下に区切り線、そこから下が区分。
// 区分は縦に積まず横へ並べ、あふれたぶんは横の送り棒で送る。
void GuiOpn::layoutOpPanel(juce::Rectangle<int> area)
{
    {
        auto row = area.removeFromTop(20);
        auto slot = row.removeFromLeft(OpnGuiValue::Fm::Op::Col::width);

        layoutMain({ .mainRect = slot, .label = &targerOpSlider.label, .component = &targerOpSlider });

        row.removeFromLeft(OpnGuiValue::Fm::Op::Col::gap);

        // TARGET のすぐ右が、どのエンベロープを映すかの切り替え。
        {
            const int w = OpnGuiValue::ParamGroup::Graph::ModeButtonWidth;

            auto btnRow = row.removeFromLeft(w * 5).withHeight(OpnGuiValue::ParamGroup::Graph::ButtonHeight);

            graphBtnAmp.setBounds(btnRow.removeFromLeft(w));
            graphBtnPitch.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg11.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsgP11.setBounds(btnRow);
        }

        row.removeFromLeft(OpnGuiValue::Fm::Op::Col::gap);

        opNameLabel.setBounds(row.removeFromLeft(OpnGuiValue::Fm::Op::Col::width).withHeight(18));
    }

    opTargetSeparator.layoutComponent(area);

    stripViewport.setBounds(area);

    const int colW = OpnGuiValue::Fm::Op::Col::width;
    const int colGap = OpnGuiValue::Fm::Op::Col::gap;

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

            auto inner = colArea.reduced(OpnGuiValue::Group::Padding::width, OpnGuiValue::Group::Padding::height);

            inner.removeFromTop(OpnGuiValue::Group::TitlePaddingTop);

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

    // OPN には音源側の LFO が無いので、この列は N88 のぶんだけ。
    layoutCol(colN88Lfo, true, [&](juce::Rectangle<int>& rect) { layoutOpN88LfoCat(rect); });

    // MASK と MML の札も 1 列へまとめてある。
    layoutCol(colMask, true, [&](juce::Rectangle<int>& rect) {
        layoutOpMaskCat(rect);

        mmlSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .component = &mml, .paddingBottom = 0 });
        });

    stripCanvas.setSize(juce::jmax(1, x - colGap), colH);
}

void GuiOpn::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    copyParamsToOpnaBtn.setVisible(visible);
    copyParamsToOpmBtn.setVisible(visible);
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
    imOpnaOpChParam.setVisible(visible);
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
    imOpnaChParam.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        uSep001.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyParamsToOpnaBtn });
        layoutMain({ .mainRect = rect, .component = &copyParamsToOpmBtn });

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
        imOpnaOpChParam.layoutComponent(rect);
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
        rect.removeFromTop(4);
        imOpnaChParam.layoutComponent(rect);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpn::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityComponent.layoutComponent(rect);
}

// 選んだ Shape を実際の LFO で走らせ、折れ線にして渡す。
// 値が変わったときだけ通るので、常時の負荷は無い。
void GuiOpn::updateLfoPreviews()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this] { updateLfoPreviews(); })) return;

    // Shape は 1 つだが、同じ番号でも PM と AM で波形が違うので両方出す。
    int shape = lfoShapeSelector.getSelectedItemIndex();

    // PM は -1.0〜1.0 の両振り
    lfoPmPreview.setPoints(WavePreviewSource::n88LfoPm(shape), true);

    // AM は 0.0〜1.0 の片側。スムースの効きも見えるよう実際の値を渡す。
    lfoAmPreview.setPoints(WavePreviewSource::n88LfoAm(shape, (float)lfoAmSmRtSlider.getValue()), false);
}

void GuiOpn::layoutN88LfoCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &lfoCat });

    bool visible = lfoCat.isDetailVisible();

    lfoFreqSlider.setVisibleWithLabel(visible);
    lfoShapeSelector.setVisibleWithLabel(visible);
    lfoPmPreview.setVisible(visible);
    lfoAmPreview.setVisible(visible);
    lfoAmSmRtSlider.setVisibleWithLabel(visible);
    lfoSyncDelaySlider.setVisibleWithLabel(visible);
    lfoSyncDelayToZeroBtn.setVisible(visible);
    lfoSyncDelayToOneBtn.setVisible(visible);
    lfoSep1.setVisible(visible);
    lfoPmToggle.setVisible(visible);
    lfoPmsSlider.setVisibleWithLabel(visible);
    lfoPmdSlider.setVisibleWithLabel(visible);
    lfoSep2.setVisible(visible);
    lfoAmToggle.setVisible(visible);
    lfoAmdSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &lfoFreqSlider.label, .component = &lfoFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoShapeSelector.label, .component = &lfoShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
        layoutMain({ .mainRect = rect, .label = &lfoSyncDelaySlider.label, .component = &lfoSyncDelaySlider });
        layoutMainTwoComps({ .rect = rect, .comp1 = &lfoSyncDelayToZeroBtn, .comp2 = &lfoSyncDelayToOneBtn });
        lfoSep1.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &lfoPmToggle });
        lfoPmPreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        layoutMain({ .mainRect = rect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
        layoutMain({ .mainRect = rect, .label = &lfoPmsSlider.label, .component = &lfoPmsSlider });
        lfoSep2.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &lfoAmToggle });
        lfoAmPreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        layoutMain({ .mainRect = rect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpn::layoutOpN88LfoCat(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catN88Lfo });

    bool visible = catN88Lfo.isDetailVisible();

    n88Ams.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &n88Ams.label, .component = &n88Ams });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpn::layoutOpKsCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &ksCat });

    bool visible = ksCat.isDetailVisible();

    ks.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &ks.label, .component = &ks });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpn::layoutOpDetCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catDet });

    bool visible = catDet.isDetailVisible();

    mul.setVisibleWithLabel(visible);
    dt.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &mul.label, .component = &mul });
        layoutRow({ .rowRect = rect, .label = &dt.label, .component = &dt });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpn::layoutOpAmpCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catAmp });

    bool visible = catAmp.isDetailVisible();

    rgAr.setVisibleWithLabel(visible);
    rgDr.setVisibleWithLabel(visible);
    rgSr.setVisibleWithLabel(visible);
    rgSl.setVisibleWithLabel(visible);
    rgRr.setVisibleWithLabel(visible);
    rgTl.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &rgAr.label, .component = &rgAr });
        layoutRow({ .rowRect = rect, .label = &rgDr.label, .component = &rgDr });
        layoutRow({ .rowRect = rect, .label = &rgSr.label, .component = &rgSr });
        layoutRow({ .rowRect = rect, .label = &rgSl.label, .component = &rgSl });
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
void GuiOpn::setupGlobalGraph()
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

void GuiOpn::layoutGlobalGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(OpnGuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    gGraphSeparator.layoutComponentBottom(mainArea);

    auto btnArea = mainArea.removeFromBottom(OpnGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    gGraphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsg11.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsgP11.setBounds(btnArea);

    gGraph.setBounds(mainArea);
}

void GuiOpn::updateGlobalGraph()
{
    // カーブモードが有効かどうかを判定
    // カーブを使うかどうかは処理側が持っている。画面から引くと、
    // どのタブを開いても Curve タブまで一緒に組み上がってしまう。
    bool isCurveMode = ctx.audioProcessor.prCurve.getEnable();
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

void GuiOpn::setGlobalGraphMode(GlobalGraphMode mode)
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
void GuiOpn::setupOpGraphWiring()
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
    rgDr.onValueChange = repaintGraph;
    rgSr.onValueChange = repaintGraph;
    rgSl.onValueChange = repaintGraph;
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
void GuiOpn::updateOpGraph(int opIndex)
{
    auto& apvts = ctx.audioProcessor.apvts;
    auto& graph = cells[(size_t)opIndex].graph();

    const juce::String code = OpnPrKey::prefix + CPK::op + juce::String(opIndex);

    // カーブモードが有効かどうかを判定
    // カーブを使うかどうかは処理側が持っている。画面から引くと、
    // どのタブを開いても Curve タブまで一緒に組み上がってしまう。
    bool isCurveMode = ctx.audioProcessor.prCurve.getEnable();

    int posIdx = opIndex + 1; // Position::Op1 = 1, Op2 = 2 ... (Common=0) に合わせる

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

    // =============================================================
    // Pitch Env
    // =============================================================
    if (currentGraphMode == GuiEnvGraphMode::Pitch) {
        // KEEP のときはカーブを効かせない。音の側も補間そのものを止めてある。
        const bool keepOn = GuiGraphValues::pitchEnvKeep(apvts, code);

        // オペレータの区分は「入れる」札なので、切ってあるときが素通し。
        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::PitchAdsr::enable));
        graph.setKeepLevels(keepOn);
        graph.updatePitchEnv(GuiGraphValues::pitchEnv(apvts, code),
            keepOn ? nullptr : p_curveCore, keepOn ? false : isCurveMode, posIdx);
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSw) {
        const auto v = GuiGraphValues::ssgSwEnv(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwEnv::enable));
        graph.updateSsgSwEnv(v.head, v.rVal, v.rMax, v.lVal, v.lMax, p_curveCore, isCurveMode, posIdx);
    }
    // =============================================================
    // SSG SW Env 11
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSw11) {
        const auto v = GuiGraphValues::ssgSwEnv11(apvts, code);
        const bool keepOn = GuiGraphValues::ssgSwEnv11Keep(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwEnv11::enable));
        graph.setKeepLevels(keepOn);
        graph.updateSsgSwEnv11(v.head, v.rVal, v.rMax, v.lVal, v.lMax,
            keepOn ? nullptr : p_curveCore, keepOn ? false : isCurveMode, posIdx);
    }
    // =============================================================
    // SSG SW PEnv 11
    // =============================================================
    else if (currentGraphMode == GuiEnvGraphMode::SsgSwP11) {
        const auto v = GuiGraphValues::ssgSwPEnv11(apvts, code);
        const bool keepOn = GuiGraphValues::ssgSwPEnv11Keep(apvts, code);

        graph.updateBypass(!GuiGraphValues::flag(apvts, code + CPK::SsgSwPEnv11::enable));
        graph.setKeepLevels(keepOn);
        graph.updateSsgSwPEnv11(v.head, v.rVal, v.rMax, v.lVal, v.lMax,
            keepOn ? nullptr : p_curveCore, keepOn ? false : isCurveMode, posIdx);
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
        float drMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgDr);
        float srMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgSr);
        float slMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgSl);
        float rrMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgRr);
        float tlMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgTl);

        float arVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgAr);
        float drVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgDr);
        float srVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgSr);
        float slVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgSl);
        float rrVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgRr);
        float tlVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgTl);

        float sl = (slMax - slVal) / slMax; // 15=0.0, 0=1.0
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
        float decayWidth = rateToWidth(drVal, drMax);
        phases.push_back({
            .widthPx = decayWidth, .startLevel = 1.0f * tlScale, .endLevel = sl * tlScale, .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Dr),
            .phaseLineColor = juce::Colours::blue
            });
        currentTotalWidth += decayWidth;

        // 3. Sustain (SR) の実線部分 (キーオン中)
        float releaseStartLevel = sl;
        float keyOnWidth = 60.0f;
        float sustainTotalWidth = 0.0f;

        if (srVal > 0.0f && srVal < srMax) {
            sustainTotalWidth = rateToWidth(srVal, srMax, 300.0f);

            // カーブを加味したレベル計算
            auto curveFunce = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Sr);
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
                .color = srVal > 0.0f ? juce::Colours::yellow : color,
                .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Rr),
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX,
                .isMax = (rrVal == rrMax)
                });
        }

        graph.setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
    }

    graph.repaint();
}

void GuiOpn::layoutOpOptionalCat(juce::Rectangle<int>& rect) {
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

void GuiOpn::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiOpn::copyParams(CopyOpn& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    copyObj.n88Lfo.freq = lfoFreqSlider.getValue();
    copyObj.n88Lfo.wave = lfoShapeSelector.getSelectedId();
    copyObj.n88Lfo.amSmRt = lfoAmSmRtSlider.getValue();
    copyObj.n88Lfo.syncDelay = lfoSyncDelaySlider.getValue();
    copyObj.n88Lfo.pmEnable = lfoPmToggle.getToggleState();
    copyObj.n88Lfo.amEnable = lfoAmToggle.getToggleState();
    copyObj.n88Lfo.pmd = lfoPmdSlider.getValue();
    copyObj.n88Lfo.pms = lfoPmsSlider.getValue();
    copyObj.n88Lfo.amd = lfoAmdSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpn::copyOpParams(int p, CopyOpnOp& copyObj) {
    copyObj.detune.mul = mul.getSelectedId();
    copyObj.detune.dt = dt.getSelectedId();
    copyObj.aAdsr.ar = rgAr.getValue();
    copyObj.aAdsr.dr = rgDr.getValue();
    copyObj.aAdsr.sl = rgSl.getValue();
    copyObj.aAdsr.sr = rgSr.getValue();
    copyObj.aAdsr.rr = rgRr.getValue();
    copyObj.aAdsr.tl = rgTl.getValue();
    copyObj.aAdsr.bypass = bypass.getToggleState();
    copyObj.aAdsr.kor = kor.getToggleState();
    copyObj.aAdsr.xof = xof.getToggleState();
    copyObj.aAdsr.ks = ks.getSelectedId();

    copyObj.n88Lfo.ams = n88Ams.getValue();

    copyObj.mask.mask = mask.getToggleState();

    fix.copyParams(copyObj.fix);
    pitchEnv.copyParams(copyObj.pAdsr);
    ssgSwEnv.copyParams(copyObj.aSsgSw);
}

void GuiOpn::pasteParams(CopyOpn& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    lfoFreqSlider.setValue(copyObj.n88Lfo.freq, juce::sendNotification);
    lfoShapeSelector.setSelectedId(copyObj.n88Lfo.wave, juce::sendNotification);
    lfoAmSmRtSlider.setValue(copyObj.n88Lfo.amSmRt, juce::sendNotification);
    lfoSyncDelaySlider.setValue(copyObj.n88Lfo.syncDelay, juce::sendNotification);
    lfoPmToggle.setToggleState(copyObj.n88Lfo.pmEnable, juce::sendNotification);
    lfoAmToggle.setToggleState(copyObj.n88Lfo.amEnable, juce::sendNotification);
    lfoPmdSlider.setValue(copyObj.n88Lfo.pmd, juce::sendNotification);
    lfoPmsSlider.setValue(copyObj.n88Lfo.pms, juce::sendNotification);
    lfoAmdSlider.setValue(copyObj.n88Lfo.amd, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpn::pasteOpParams(int p, CopyOpnOp& copyObj) {
    mul.setSelectedId(copyObj.detune.mul, juce::sendNotification);
    dt.setSelectedId(copyObj.detune.dt, juce::sendNotification);
    rgAr.setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgDr.setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgSl.setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgSr.setValue(copyObj.aAdsr.sr, juce::sendNotification);
    rgRr.setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl.setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass.setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor.setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof.setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ks.setSelectedId(copyObj.aAdsr.ks, juce::sendNotification);

    n88Ams.setValue(copyObj.n88Lfo.ams, juce::sendNotification);

    mask.setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix.pasteParams(copyObj.fix);
    pitchEnv.pasteParams(copyObj.pAdsr);
    ssgSwEnv.pasteParams(copyObj.aSsgSw);
}

void GuiOpn::copyParamsOpm(CopyOpnOpm& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpn::copyOpParamsOpm(int p, CopyOpnOpmOp& copyObj) {
    copyObj.detune.mul = mul.getSelectedId();
    copyObj.detune.dt = dt.getSelectedId();
    copyObj.aAdsr.ar = rgAr.getValue();
    copyObj.aAdsr.dr = rgDr.getValue();
    copyObj.aAdsr.sl = rgSl.getValue();
    copyObj.aAdsr.sr = rgSr.getValue();
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

void GuiOpn::pasteParamsOpm(CopyOpnOpm& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpn::pasteOpParamsOpm(int p, CopyOpnOpmOp& copyObj) {
    mul.setSelectedId(copyObj.detune.mul, juce::sendNotification);
    dt.setSelectedId(copyObj.detune.dt, juce::sendNotification);
    rgAr.setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgDr.setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgSl.setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgSr.setValue(copyObj.aAdsr.sr, juce::sendNotification);
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

void GuiOpn::copyParamsOpnaOpm(CopyOpnaOpnOpm& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpn::copyOpParamsOpnaOpm(int p, CopyOpnaOpnOpmOp& copyObj) {
    copyObj.detune.mul = mul.getSelectedItemIndex();
    copyObj.detune.dt = dt.getSelectedItemIndex();
    copyObj.aAdsr.ar = rgAr.getValue();
    copyObj.aAdsr.dr = rgDr.getValue();
    copyObj.aAdsr.sl = rgSl.getValue();
    copyObj.aAdsr.sr = rgSr.getValue();
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

void GuiOpn::pasteParamsOpnaOpm(CopyOpnaOpnOpm& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpn::pasteOpParamsOpnaOpm(int p, CopyOpnaOpnOpmOp& copyObj) {
    mul.setSelectedItemIndex(copyObj.detune.mul, juce::sendNotification);
    dt.setSelectedItemIndex(copyObj.detune.dt, juce::sendNotification);
    rgAr.setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgDr.setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgSl.setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgSr.setValue(copyObj.aAdsr.sr, juce::sendNotification);
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

void GuiOpn::importPitchEnvParam(int opIndex) {
    pitchEnv.importParams();
}

void GuiOpn::exportPitchEnvParam(int opIndex) {
    pitchEnv.exportParams();
}

void GuiOpn::importSsgSwEnvParam(int opIndex) {
    ssgSwEnv.importParams();
}

void GuiOpn::exportSsgSwEnvParam(int opIndex) {
    ssgSwEnv.exportParams();
}

void GuiOpn::importLfoParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultLfoParamDir,
        { EditorGuiText::ParamBrowser::kindLfoN88 },
        [this](const juce::File& file) { applyLfoParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpn::applyLfoParamFile(const juce::File& file)
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

        Io::ParamWriter writer(n88LfoFormat);

        writeLfoParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, n88LfoFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    lfoFreqSlider.setValue(reader->getInt("lfoFreq", (int)lfoFreqSlider.getValue()), juce::sendNotification);
    lfoShapeSelector.setSelectedItemIndex(reader->getInt("lfoShape", lfoShapeSelector.getSelectedItemIndex()), juce::sendNotification);
    lfoSyncDelaySlider.setValue(reader->getInt("lfoSyncDelay", (int)lfoSyncDelaySlider.getValue()), juce::sendNotification);
    lfoPmToggle.setToggleState(reader->getBool("lfoPm", lfoPmToggle.getToggleState()), juce::sendNotification);
    lfoPmsSlider.setValue(reader->getInt("lfoPms", (int)lfoPmsSlider.getValue()), juce::sendNotification);
    lfoPmdSlider.setValue(reader->getInt("lfoPmd", (int)lfoPmdSlider.getValue()), juce::sendNotification);
    lfoAmToggle.setToggleState(reader->getBool("lfoAm", lfoAmToggle.getToggleState()), juce::sendNotification);
    lfoAmSmRtSlider.setValue(reader->getFloat("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue()), juce::sendNotification);
    lfoAmdSlider.setValue(reader->getInt("lfoAmd", (int)lfoAmdSlider.getValue()), juce::sendNotification);

    auto amsValues = reader->getIntArray("n88Ams");

    for (int i = 0; i < OpnPrValue::ops && i < (int)amsValues.size(); i++) {
        n88Ams.setValue(amsValues[(size_t)i], juce::sendNotification);
    }
}

void GuiOpn::exportLfoParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultLfoParamDir,
        { EditorGuiText::ParamBrowser::kindLfoN88 }, Io::Extension::N88LfoParam,
        [this](const juce::File& file) { writeLfoParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpn::writeLfoParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(n88LfoFormat);
    writeLfoParams(writer);

    writer.writeTo(file);
}

void GuiOpn::importQualityParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindQuality },
        [this](const juce::File& file) { applyQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpn::applyQualityParamFile(const juce::File& file)
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

void GuiOpn::exportQualityParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindQuality }, Io::Extension::QualityParam,
        [this](const juce::File& file) { writeQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpn::writeQualityParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(qualityFormat);
    writeQualityParams(writer);

    writer.writeTo(file);
}

void GuiOpn::importSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11.importParams();
}

void GuiOpn::exportSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11.exportParams();
}

void GuiOpn::importSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11.importParams();
}

void GuiOpn::exportSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11.exportParams();
}

void GuiOpn::importOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp.importParams();
}

void GuiOpn::exportOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp.exportParams();
}

void GuiOpn::importOpWtAmpModParam(int opIndex) {
    wtAmpModOp.importParams();
}

void GuiOpn::exportOpWtAmpModParam(int opIndex) {
    wtAmpModOp.exportParams();
}

void GuiOpn::importOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp.importParams();
}

void GuiOpn::exportOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp.exportParams();
}

void GuiOpn::importOpWtModParam(int opIndex) {
    wtModOp.importParams();
}

void GuiOpn::exportOpWtModParam(int opIndex) {
    wtModOp.exportParams();
}

void GuiOpn::importChParam() {
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser({ "OPN" },
        [this](const juce::File& file) { applyChParamFile(file); });
}

// パラメータファイルのブラウザからも同じ読み込みを使うので、
// ダイアログを出すところと、読んで反映するところを分けてある。
void GuiOpn::applyChParamFile(const juce::File& file) {
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

        Io::ParamWriter writer(opnFormat);

        writeChParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opnFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    readChParams(*reader);
}

void GuiOpn::exportChParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { "OPN" }, Io::Extension::opnParam,
        [this](const juce::File& file) { writeChParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpn::writeChParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(opnFormat);
    writeChParams(writer);

    writer.writeTo(file);
}

void GuiOpn::importOpChParam(int opIndex)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpnOp },
        [this, opIndex](const juce::File& file) { applyOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpn::applyOpChParamFile(int opIndex, const juce::File& file)
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

        Io::ParamWriter writer(opnOpFormat);

        writeOpChFileParams(opIndex, writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opnOpFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    updateAlgorithmDisplay();

    readOpParams(opIndex, *reader);
}

void GuiOpn::exportOpChParam(int opIndex)
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpnOp }, Io::Extension::opnOpParam,
        [this, opIndex](const juce::File& file) { writeOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpn::writeOpChParamFile(int opIndex, const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(opnOpFormat);
    writeOpChFileParams(opIndex, writer);

    writer.writeTo(file);
}

// オペレータ 1 つぶん。並びの中のひとつを渡してもらう。
//
// 名前で引くので、他の音源のファイルを読ませても、こちらに無い項目は
// 勝手に読み飛ばされる。行数を数えて飛ばす細工が要らない。
void GuiOpn::readOpParams(int opIndex, const Io::ParamReader& r) {
    // Mul / Dt
    mul.setSelectedId(r.getInt("mul", mul.getSelectedId()), juce::sendNotification);
    dt.setSelectedId(r.getInt("dt", dt.getSelectedId()), juce::sendNotification);

    // Env
    rgAr.setValue(r.getFloat("ar", (float)rgAr.getValue()), juce::sendNotification);
    rgDr.setValue(r.getFloat("dr", (float)rgDr.getValue()), juce::sendNotification);
    rgSl.setValue(r.getFloat("sl", (float)rgSl.getValue()), juce::sendNotification);
    rgSr.setValue(r.getFloat("sr", (float)rgSr.getValue()), juce::sendNotification);
    rgRr.setValue(r.getFloat("rr", (float)rgRr.getValue()), juce::sendNotification);
    rgTl.setValue(r.getFloat("tl", (float)rgTl.getValue()), juce::sendNotification);

    // Key Scale
    ks.setSelectedId(r.getInt("ks", ks.getSelectedId()), juce::sendNotification);

    // N88 AMS
    n88Ams.setValue(r.getFloat("n88Ams", (float)n88Ams.getValue()), juce::sendNotification);

    // Optional / Mask
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

// チャンネル 1 つぶん。OPNA のファイルを読むときにも使う。
void GuiOpn::readChParams(const Io::ParamReader& reader) {
    // Level
    levelComponent.readParams(reader, "level");

    // Algorithm & Feedback
    algSelector.setSelectedId(reader.getInt("alg", algSelector.getSelectedId()), juce::sendNotification);
    feedbackSlider.setValue(reader.getInt("feedback", (int)feedbackSlider.getValue()), juce::sendNotification);

    updateAlgorithmDisplay();

    // N88 LFO
    lfoFreqSlider.setValue(reader.getFloat("lfoFreq", (float)lfoFreqSlider.getValue()), juce::sendNotification);
    lfoShapeSelector.setSelectedId(reader.getInt("lfoShape", lfoShapeSelector.getSelectedId()), juce::sendNotification);
    lfoAmSmRtSlider.setValue(reader.getFloat("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue()), juce::sendNotification);
    lfoSyncDelaySlider.setValue(reader.getFloat("lfoSyncDelay", (float)lfoSyncDelaySlider.getValue()), juce::sendNotification);
    lfoPmToggle.setToggleState(reader.getBool("lfoPm", lfoPmToggle.getToggleState()), juce::sendNotification);
    lfoPmsSlider.setValue(reader.getFloat("lfoPms", (float)lfoPmsSlider.getValue()), juce::sendNotification);
    lfoPmdSlider.setValue(reader.getFloat("lfoPmd", (float)lfoPmdSlider.getValue()), juce::sendNotification);
    lfoAmToggle.setToggleState(reader.getBool("lfoAm", lfoAmToggle.getToggleState()), juce::sendNotification);
    lfoAmdSlider.setValue(reader.getFloat("lfoAmd", (float)lfoAmdSlider.getValue()), juce::sendNotification);

    // Components (Global)
    ssgHwEnv.readParams(reader, "ssgHwEnv");
    ssgHwPEnv.readParams(reader, "ssgHwPEnv");
    ssgSwEnv11g.readParams(reader, "ssgSwEnv11");
    qualityComponent.readParams(reader, "quality");
    unisonComponent.readParams(reader, "unison");
    ampEnvComponent.readParams(reader, "ampEnv");
    ssgSwPEnv11g.readParams(reader, "ssgSwPEnv11");
    modComponent.readParams(reader, "wtMod");
    ampModComponent.readParams(reader, "wtAmpMod");

    for (int i = 0; i < OpnPrValue::ops; i++) {
        withOp(i, [this, &reader, i] { readOpParams(i, reader.arrayItem(Io::ParamKey::ops, i)); });
    }
}

void GuiOpn::writeOpParams(int opIndex, Io::ParamWriter& w) {
    // Mul / Dt
    w.set("mul", mul.getSelectedId());
    w.set("dt", dt.getSelectedId());

    // Env
    w.set("ar", (float)rgAr.getValue());
    w.set("dr", (float)rgDr.getValue());
    w.set("sl", (float)rgSl.getValue());
    w.set("sr", (float)rgSr.getValue());
    w.set("rr", (float)rgRr.getValue());
    w.set("tl", (float)rgTl.getValue());

    // Key Scale
    w.set("ks", ks.getSelectedId());

    // N88 AMS
    w.set("n88Ams", (float)n88Ams.getValue());

    // Optional / Mask
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

void GuiOpn::importOpnaChParam() {
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser({ "OPNA" },
        [this](const juce::File& file) { applyOpnaChParamFile(file); });
}

// パラメータファイルのブラウザからも同じ読み込みを使うので、
// ダイアログを出すところと、読んで反映するところを分けてある。
void GuiOpn::applyOpnaChParamFile(const juce::File& file) {
    if (!file.existsAsFile()) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    // OPNA のファイル。こちらに無い項目は名前で引けないので
    // そのまま読み飛ばされる。
    auto reader = Io::ParamReader::open(file, opnaFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    readChParams(*reader);
}

void GuiOpn::importOpnaOpChParam(int opIndex)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpnaOp },
        [this, opIndex](const juce::File& file) { applyOpnaOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpn::applyOpnaOpChParamFile(int opIndex, const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    auto reader = Io::ParamReader::open(file, opnaOpFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    updateAlgorithmDisplay();

    readOpParams(opIndex, *reader);
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。並び順を写し直すより確実で、
// 当時の互換の工夫もそのまま残る。
void GuiOpn::setImportingChParams(juce::StringArray& lines, int& index) {
	// Level
	levelComponent.setImportingParams(lines, index);

	// Algorithm & Feedback
	algSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
	feedbackSlider.setValue(lines[index++].getIntValue(), juce::sendNotification);

	updateAlgorithmDisplay();

	// N88 LFO
	lfoFreqSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoShapeSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
	lfoAmSmRtSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoSyncDelaySlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoPmToggle.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	lfoPmsSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoPmdSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	lfoAmToggle.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	lfoAmdSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

	// Components (Global)
	ssgHwEnv.setImportingParams(lines, index);
	ssgSwEnv11g.setImportingParams(lines, index);
	qualityComponent.setImportingParams(lines, index);
	unisonComponent.setImportingParams(lines, index);

	for (int i = 0; i < OpnPrValue::ops; i++) {
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
void GuiOpn::writeChParams(Io::ParamWriter& writer) {
	// Level
	levelComponent.writeParams(writer, "level");

	// Algorithm & Feedback
	writer.set("alg", algSelector.getSelectedId());
	writer.set("feedback", (int)feedbackSlider.getValue());

	// N88 LFO
	writer.set("lfoFreq", (float)lfoFreqSlider.getValue());
	writer.set("lfoShape", lfoShapeSelector.getSelectedId());
	writer.set("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue());
	writer.set("lfoSyncDelay", (float)lfoSyncDelaySlider.getValue());
	writer.set("lfoPm", lfoPmToggle.getToggleState());
	writer.set("lfoPms", (float)lfoPmsSlider.getValue());
	writer.set("lfoPmd", (float)lfoPmdSlider.getValue());
	writer.set("lfoAm", lfoAmToggle.getToggleState());
	writer.set("lfoAmd", (float)lfoAmdSlider.getValue());

	// Components (Global)
	// 名前で持つので、後から足した項目を末尾へ置く必要はない。
	ssgHwEnv.writeParams(writer, "ssgHwEnv");
	ssgHwPEnv.writeParams(writer, "ssgHwPEnv");
	ssgSwEnv11g.writeParams(writer, "ssgSwEnv11");
	qualityComponent.writeParams(writer, "quality");
	unisonComponent.writeParams(writer, "unison");
	ampEnvComponent.writeParams(writer, "ampEnv");
	ssgSwPEnv11g.writeParams(writer, "ssgSwPEnv11");
	modComponent.writeParams(writer, "wtMod");
	ampModComponent.writeParams(writer, "wtAmpMod");

	for (int i = 0; i < OpnPrValue::ops; i++) {
	    auto op = writer.arrayItem(Io::ParamKey::ops, i);

	    writeOpParams(i, op);
	}

	
}

// 3.0.0 より前の形式を読むための補助。履歴から戻したもの。
void GuiOpn::getImportingOpParams(int opIndex, juce::StringArray& lines, int& index) {
    // Mul / Dt
    mul.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    dt.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // Env
    rgAr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgDr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgSl.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgSr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgRr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgTl.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Key Scale
    ks.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // N88 AMS
    n88Ams.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Optional / Mask
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
void GuiOpn::setImportingLfoParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 13) return;

	lfoFreqSlider.setValue(lines[0].getIntValue(), juce::sendNotification);
	lfoShapeSelector.setSelectedItemIndex(lines[1].getIntValue(), juce::sendNotification);
	lfoSyncDelaySlider.setValue(lines[2].getIntValue(), juce::sendNotification);
	lfoPmToggle.setToggleState(lines[3].getIntValue() == 1, juce::sendNotification);
	lfoPmsSlider.setValue(lines[4].getIntValue(), juce::sendNotification);
	lfoPmdSlider.setValue(lines[5].getIntValue(), juce::sendNotification);
	lfoAmToggle.setToggleState(lines[6].getIntValue() == 1, juce::sendNotification);
	lfoAmSmRtSlider.setValue(lines[7].getFloatValue(), juce::sendNotification);
	lfoAmdSlider.setValue(lines[8].getIntValue(), juce::sendNotification);

	for (int i = 0; i < OpnPrValue::ops; i++) {
	    n88Ams.setValue(lines[9+i].getIntValue(), juce::sendNotification);
	}

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpn::writeLfoParams(Io::ParamWriter& writer) {
	writer.set("lfoFreq", (int)lfoFreqSlider.getValue());
	writer.set("lfoShape", lfoShapeSelector.getSelectedItemIndex());
	writer.set("lfoSyncDelay", (int)lfoSyncDelaySlider.getValue());
	writer.set("lfoPm", lfoPmToggle.getToggleState());
	writer.set("lfoPms", (int)lfoPmsSlider.getValue());
	writer.set("lfoPmd", (int)lfoPmdSlider.getValue());
	writer.set("lfoAm", lfoAmToggle.getToggleState());
	writer.set("lfoAmSmRt", (float)lfoAmSmRtSlider.getValue());
	writer.set("lfoAmd", (int)lfoAmdSlider.getValue());

	std::vector<int> amsValues;

	for (int i = 0; i < OpnPrValue::ops; i++) {
	    amsValues.push_back((int)n88Ams.getValue());
	}

	writer.setArray("n88Ams", amsValues);

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpn::setImportingQualityParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	qualityComponent.setBit(lines[0].getIntValue());
	qualityComponent.setRate(lines[1].getIntValue());

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpn::writeQualityParams(Io::ParamWriter& writer) {
	writer.set("bit", qualityComponent.getBit());
	writer.set("rate", qualityComponent.getRate());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpn::setImportingOpChFileParams(int opIndex, juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	getImportingOpParams(opIndex, lines, index);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpn::writeOpChFileParams(int opIndex, Io::ParamWriter& writer) {
	writeOpParams(opIndex, writer);

	
}

void GuiOpn::bypassHiddenCategories()
{
    // いま隠れている区分だけを切る。出したままの区分は触らない。
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv)) ampEnvComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)) ssgHwEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)) ssgSwEnv11g.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod)) ampModComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)) ssgHwPEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)) ssgSwPEnv11g.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod)) modComponent.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::Unison)) unisonComponent.setCategoryBypassed(true);

    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)) ssgHwEnvOp.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv)) ssgSwEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)) ssgSwEnv11.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod)) wtAmpModOp.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::PitchEnv)) pitchEnv.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)) ssgHwPEnvOp.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)) ssgSwPEnv11.setCategoryBypassed(true);
    if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod)) wtModOp.setCategoryBypassed(true);
}

void GuiOpn::openEnabledCategories()
{
    // 効いている区分を開く。札を持たない区分は触らない。
    if (ampEnvComponent.hasBypassSwitch() && !ampEnvComponent.isCategoryBypassed()) ampEnvComponent.setCategoryOpen(true);
    if (ssgHwEnv.hasBypassSwitch() && !ssgHwEnv.isCategoryBypassed()) ssgHwEnv.setCategoryOpen(true);
    if (ssgSwEnv11g.hasBypassSwitch() && !ssgSwEnv11g.isCategoryBypassed()) ssgSwEnv11g.setCategoryOpen(true);
    if (ampModComponent.hasBypassSwitch() && !ampModComponent.isCategoryBypassed()) ampModComponent.setCategoryOpen(true);
    if (ssgHwPEnv.hasBypassSwitch() && !ssgHwPEnv.isCategoryBypassed()) ssgHwPEnv.setCategoryOpen(true);
    if (ssgSwPEnv11g.hasBypassSwitch() && !ssgSwPEnv11g.isCategoryBypassed()) ssgSwPEnv11g.setCategoryOpen(true);
    if (modComponent.hasBypassSwitch() && !modComponent.isCategoryBypassed()) modComponent.setCategoryOpen(true);
    if (unisonComponent.hasBypassSwitch() && !unisonComponent.isCategoryBypassed()) unisonComponent.setCategoryOpen(true);

    if (ssgHwEnvOp.hasBypassSwitch() && !ssgHwEnvOp.isCategoryBypassed()) ssgHwEnvOp.setCategoryOpen(true);
    if (ssgSwEnv.hasBypassSwitch() && !ssgSwEnv.isCategoryBypassed()) ssgSwEnv.setCategoryOpen(true);
    if (ssgSwEnv11.hasBypassSwitch() && !ssgSwEnv11.isCategoryBypassed()) ssgSwEnv11.setCategoryOpen(true);
    if (wtAmpModOp.hasBypassSwitch() && !wtAmpModOp.isCategoryBypassed()) wtAmpModOp.setCategoryOpen(true);
    if (pitchEnv.hasBypassSwitch() && !pitchEnv.isCategoryBypassed()) pitchEnv.setCategoryOpen(true);
    if (ssgHwPEnvOp.hasBypassSwitch() && !ssgHwPEnvOp.isCategoryBypassed()) ssgHwPEnvOp.setCategoryOpen(true);
    if (ssgSwPEnv11.hasBypassSwitch() && !ssgSwPEnv11.isCategoryBypassed()) ssgSwPEnv11.setCategoryOpen(true);
    if (wtModOp.hasBypassSwitch() && !wtModOp.isCategoryBypassed()) wtModOp.setCategoryOpen(true);
}

void GuiOpn::closeBypassedCategories()
{
    // 切ってある区分を閉じる。札を持たない区分は触らない。
    if (ampEnvComponent.hasBypassSwitch() && ampEnvComponent.isCategoryBypassed()) ampEnvComponent.setCategoryOpen(false);
    if (ssgHwEnv.hasBypassSwitch() && ssgHwEnv.isCategoryBypassed()) ssgHwEnv.setCategoryOpen(false);
    if (ssgSwEnv11g.hasBypassSwitch() && ssgSwEnv11g.isCategoryBypassed()) ssgSwEnv11g.setCategoryOpen(false);
    if (ampModComponent.hasBypassSwitch() && ampModComponent.isCategoryBypassed()) ampModComponent.setCategoryOpen(false);
    if (ssgHwPEnv.hasBypassSwitch() && ssgHwPEnv.isCategoryBypassed()) ssgHwPEnv.setCategoryOpen(false);
    if (ssgSwPEnv11g.hasBypassSwitch() && ssgSwPEnv11g.isCategoryBypassed()) ssgSwPEnv11g.setCategoryOpen(false);
    if (modComponent.hasBypassSwitch() && modComponent.isCategoryBypassed()) modComponent.setCategoryOpen(false);
    if (unisonComponent.hasBypassSwitch() && unisonComponent.isCategoryBypassed()) unisonComponent.setCategoryOpen(false);

    if (ssgHwEnvOp.hasBypassSwitch() && ssgHwEnvOp.isCategoryBypassed()) ssgHwEnvOp.setCategoryOpen(false);
    if (ssgSwEnv.hasBypassSwitch() && ssgSwEnv.isCategoryBypassed()) ssgSwEnv.setCategoryOpen(false);
    if (ssgSwEnv11.hasBypassSwitch() && ssgSwEnv11.isCategoryBypassed()) ssgSwEnv11.setCategoryOpen(false);
    if (wtAmpModOp.hasBypassSwitch() && wtAmpModOp.isCategoryBypassed()) wtAmpModOp.setCategoryOpen(false);
    if (pitchEnv.hasBypassSwitch() && pitchEnv.isCategoryBypassed()) pitchEnv.setCategoryOpen(false);
    if (ssgHwPEnvOp.hasBypassSwitch() && ssgHwPEnvOp.isCategoryBypassed()) ssgHwPEnvOp.setCategoryOpen(false);
    if (ssgSwPEnv11.hasBypassSwitch() && ssgSwPEnv11.isCategoryBypassed()) ssgSwPEnv11.setCategoryOpen(false);
    if (wtModOp.hasBypassSwitch() && wtModOp.isCategoryBypassed()) wtModOp.setCategoryOpen(false);
}
