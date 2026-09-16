#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiOpl3.h"

#include "../../Core/Gui/GuiRefresh.h"

#include "../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat oplLfoFormat{ "oplLfo", 1 };
	const Io::ParamFormat qualityFormat{ "quality", 1 };
	const Io::ParamFormat opl3Format{ "opl3", 1 };
	const Io::ParamFormat opl3OpFormat{ "opl3Op", 1 };

	// 相手の音源のファイルを読み込む口があるため、その印も要る
	const Io::ParamFormat oplFormat{ "opl", 1 };
	const Io::ParamFormat oplOpFormat{ "oplOp", 1 };
}

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Opl3/ProcessorOpl3Keys.h"
#include "../../Processor/Opl3/ProcessorOpl3Values.h"
#include "../../Processor/Opl/ProcessorOplValues.h"
#include "../../Core/Const/ConstMmlKeys.h"
#include "../../Core/Const/ConstMmlValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Fm/FmRegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiGraphValues.h"
#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiOpl3Values.h"
#include "./GuiOpl3Text.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> opl3AlgItems = {
    {.name = "00: <OPL3(OP4)-00>", .value = 1 },
    {.name = "01: <OPL3(OP4)-01> / OP2x2", .value = 2 },
    {.name = "02: <OPL3(OP4)-02>", .value = 3 },
    {.name = "03: <OPL3(OP4)-03>", .value = 4 },
    {.name = "04: <EX-00>", .value = 5 },
    {.name = "05: <EX-01>", .value = 6 },
    {.name = "06: <EX-02> / OP2x2", .value = 7 },
    {.name = "07: <EX-03> / OP2x2", .value = 8 },
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

static std::vector<SelectItem> kslItems = {
    {.name = "KSL: 0", .value = 1},
    {.name = "KSL: 1", .value = 2},
    {.name = "KSL: 2", .value = 3},
    {.name = "KSL: 3", .value = 4}
};

static std::vector<SelectItem> opl3EgItems = {
    {.name = "0: Sine",                 .value = 1},
    {.name = "1: Half Sine",            .value = 2},
    {.name = "2: Abs Sine",             .value = 3},
    {.name = "3: Pulse Sine",           .value = 4},
    {.name = "4: Alternative Sine",     .value = 5},
    {.name = "5: Alternative Abs Sine", .value = 6},
    {.name = "6: Square",               .value = 7},
    {.name = "7: Derived Square",       .value = 8},
    {.name = "8: [EX01]Round Square",    .value = 9},
    {.name = "9: [EX02]Log Saw",         .value = 10},
};

void GuiOpl3::setup()
{
    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    p_curveCore = ctx.audioProcessor.getCurveCore();

    const juce::String code = Opl3PrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, Opl3GuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    qualityComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    algFbCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = Opl3GuiText::Category::algFb });
    algSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::alg, .title = Opl3GuiText::Fm::alg, .items = opl3AlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    mainGroup.contentCanvas.addAndMakeVisible(&algStaticGraphComp);

    algFbSep.setupComponent(mainGroup.contentCanvas);

    feedbackSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::fb, .title = Opl3GuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

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

    // 大区分。音量と音程にかかわる区分を、それぞれまとめて畳めるようにする。
    // 最初は閉じておく。
    ampMajorCat.setupMajorCategory({ .parent = mainGroup.contentCanvas, .title = CoreGuiText::MajorCategory::ampEnv, .enableChangeDetailVisible = true });
    pitchMajorCat.setupMajorCategory({ .parent = mainGroup.contentCanvas, .title = CoreGuiText::MajorCategory::pitchEnv, .enableChangeDetailVisible = true });

    // チャンネルの設定を OPZX7S のパラメータファイルとして書き出す
    exportOpzx7Btn.setup({ .parent = mainGroup.contentCanvas, .title = "[EX]OPZX7S Params", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    exportOpzx7Btn.setWantsKeyboardFocus(true);
    exportOpzx7Btn.setExplicitFocusOrder(++tabOrder);
    exportOpzx7Btn.onClick = [this] { exportOpzx7Params(); };

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = Opl3GuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = Opl3GuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    initLfoToOplBtn.setup({ .parent = mainGroup.contentCanvas, .title = Opl3GuiText::Fm::initLfoToOpl });
    initLfoToOplBtn.setWantsKeyboardFocus(true);
    initLfoToOplBtn.setExplicitFocusOrder(++tabOrder);
    initLfoToOplBtn.onClick = [this] {
        for (int i = 0; i < Opl3PrValue::ops; i++)
        {
            ams.setValue(3.7, juce::sendNotification);
            amd.setValue(4.8, juce::sendNotification);

            pms.setValue(6.4, juce::sendNotification);
            pmd.setValue(14.0, juce::sendNotification);
        }
        };
    initLfoToOpllBtn.setup({ .parent = mainGroup.contentCanvas, .title = Opl3GuiText::Fm::initLfoToOpll });
    initLfoToOpllBtn.setWantsKeyboardFocus(true);
    initLfoToOpllBtn.setExplicitFocusOrder(++tabOrder);
    initLfoToOpllBtn.onClick = [this] {
        for (int i = 0; i < Opl3PrValue::ops; i++)
        {
            ams.setValue(6.06, juce::sendNotification);
            amd.setValue(1.2, juce::sendNotification);

            pms.setValue(6.06, juce::sendNotification);
            pmd.setValue(13.7, juce::sendNotification);
        }
        };

    uSep002.setupComponent(mainGroup.contentCanvas);

    copyOpParamToOplBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Params (OP 1/2) -> OPL", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamToOplBtn.setWantsKeyboardFocus(true);
    copyOpParamToOplBtn.setExplicitFocusOrder(++tabOrder);
    copyOpParamToOplBtn.onClick = [this] {
        ctx.editor.copyOpl3ParamsToOpl();
        };

    copyOpParamToOpl12Btn.setup({ .parent = mainGroup.contentCanvas, .title = "1/2 OP Params -> OPL", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamToOpl12Btn.setWantsKeyboardFocus(true);
    copyOpParamToOpl12Btn.setExplicitFocusOrder(++tabOrder);
    copyOpParamToOpl12Btn.onClick = [this] {
        ctx.editor.copyOpl312ParamsToOpl();
        };

    copyOpParamToOpl34Btn.setup({ .parent = mainGroup.contentCanvas, .title = "3/4 OP Params -> OPL", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamToOpl34Btn.setWantsKeyboardFocus(true);
    copyOpParamToOpl34Btn.setExplicitFocusOrder(++tabOrder);
    copyOpParamToOpl34Btn.onClick = [this] {
        ctx.editor.copyOpl334ParamsToOpl();
        };

    uSep003.setupComponent(mainGroup.contentCanvas);

    copyOpParamBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Copy Op Params", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamBtn.setWantsKeyboardFocus(true);
    copyOpParamBtn.setExplicitFocusOrder(++tabOrder);
    copyOpParamBtn.onClick = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        ctx.editor.copyOpl3OpParams(from, to);
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

    uSep004.setupComponent(mainGroup.contentCanvas);

    ieOpLfo.setupComponentOp(mainGroup.contentCanvas, tabOrder, "OP LFO");
    ieOpLfo.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importLfoParam(opIndex); };
    ieOpLfo.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportLfoParam(opIndex); };

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

    imOplOpChParam.setupComponentOp(mainGroup.contentCanvas, tabOrder, "OPL OP Params");
    imOplOpChParam.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOplOpChParam(opIndex); };

    uSep005.setupComponent(mainGroup.contentCanvas);

    // TARGET。設定ひとそろいの帯へ置く。値が動いたら束縛と枠線を付け替える。
    targerOpSlider.setup({ .parent = *this, .title = "TARGET", .isReset = false });
    targerOpSlider.setRange(1.0, (double)Opl3PrValue::ops, 1.0);
    targerOpSlider.setNumDecimalPlacesToDisplay(0);
    targerOpSlider.setWantsKeyboardFocus(true);
    targerOpSlider.setExplicitFocusOrder(++tabOrder);
    targerOpSlider.onValueChange = [this] { applyOpTarget(); };

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

    imOplChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "OPL CH Params");
    imOplChParam.onClickImport = [this] { importOplChParam(); };

    imOplChAllOpParam.setupComponent(mainGroup.contentCanvas, tabOrder, "OPL CH Params (All OP)");
    imOplChAllOpParam.onClickImport = [this] { importOplChAllOpParam(); };

    const juce::String opCode = code + CPK::op;

    // ---- 上の絵と、下の設定 ----
    //
    // 絵はオペレータの数だけ。設定はひとそろいだけ置き、TARGET で
    // 指し先を切り替える。
    for (int i = 0; i < Opl3PrValue::ops; ++i)
    {
        cells[(size_t)i].setup(*this, i, Opl3GuiText::Group::opPrefix + juce::String(i + 1), false);
        cells[(size_t)i].onSelect = [this](int index) {
            targerOpSlider.setValue(index + 1, juce::sendNotification);
            };
    }

    // 区分は縦に積まず、横へ並べる。1 列 1 区分が基本。
    stripViewport.setViewedComponent(&stripCanvas, false);
    stripViewport.setScrollBarsShown(false, true);
    stripViewport.setOpaque(false);

    addAndMakeVisible(stripViewport);

    colAmp.setup(stripCanvas, juce::String("") + "AMP ENV / OPTIONAL / EG");
    colSsgHwEnv.setup(stripCanvas, juce::String("") + "SSG HW AMP ENV");
    colSsgSwEnv.setup(stripCanvas, juce::String("") + "SSG SW AMP ENV");
    colSsgSwEnv11.setup(stripCanvas, juce::String("") + "SSG SW AMP ENV[11]");
    colAmpMod.setup(stripCanvas, juce::String("") + "WT AMP MOD");
    colPitchEnv.setup(stripCanvas, juce::String("") + "PITCH ENV");
    colSsgHwPEnv.setup(stripCanvas, juce::String("") + "SSG HW PITCH ENV");
    colSsgSwPEnv11.setup(stripCanvas, juce::String("") + "SSG SW PITCH ENV[11]");
    colMod.setup(stripCanvas, juce::String("") + "WT PITCH MOD");
    colKs.setup(stripCanvas, juce::String("") + "KEY SCALE / MUL / LFO");
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

    catDet.setupHwCategory({ .parent = colKs.contentCanvas, .title = Opl3GuiText::Category::detune, .enableChangeDetailVisible = true });
    catAmp.setupHwCategory({ .parent = colAmp.contentCanvas, .title = Opl3GuiText::Category::ampEnv, .detailVisible = true, .enableChangeDetailVisible = true });

    mul.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::mul, .title = Opl3GuiText::Fm::Op::Mul, .items = multems, .isReset = true });
    mul.setWantsKeyboardFocus(true);
    mul.setExplicitFocusOrder(++tabOrder);

    rgAr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgAr, .title = Opl3GuiText::Fm::Op::Ar, .isReset = true });
    rgAr.setWantsKeyboardFocus(true);
    rgAr.setExplicitFocusOrder(++tabOrder);

    rgDr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgDr, .title = Opl3GuiText::Fm::Op::Dr, .isReset = true });
    rgDr.setWantsKeyboardFocus(true);
    rgDr.setExplicitFocusOrder(++tabOrder);

    rgSl.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgSl, .title = Opl3GuiText::Fm::Op::Sl, .isReset = true });
    rgSl.setWantsKeyboardFocus(true);
    rgSl.setExplicitFocusOrder(++tabOrder);

    rgRr.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgRr, .title = Opl3GuiText::Fm::Op::Rr, .isReset = true });
    rgRr.setWantsKeyboardFocus(true);
    rgRr.setExplicitFocusOrder(++tabOrder);

    rgTl.setup(GuiSlider::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::rgTl, .title = Opl3GuiText::Fm::Op::Tl, .isReset = true });
    rgTl.setWantsKeyboardFocus(true);
    rgTl.setExplicitFocusOrder(++tabOrder);

    egType.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::egType, .title = Opl3GuiText::Fm::Op::EgType, .isReset = true });
    egType.setWantsKeyboardFocus(true);
    egType.setExplicitFocusOrder(++tabOrder);

    ksCat.setupHwCategory({ .parent = colKs.contentCanvas, .title = Opl3GuiText::Category::ks, .enableChangeDetailVisible = true });

    ksr.setup(GuiToggleButton::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksr, .title = Opl3GuiText::Fm::Op::Ksr, .isReset = true });
    ksr.setWantsKeyboardFocus(true);
    ksr.setExplicitFocusOrder(++tabOrder);

    ksl.setup(GuiComboBox::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::ksl, .title = Opl3GuiText::Fm::Op::Ksl, .items = kslItems, .isReset = true });
    ksl.setWantsKeyboardFocus(true);
    ksl.setExplicitFocusOrder(++tabOrder);

    catOptional.setupSwCategory({ .parent = colAmp.contentCanvas, .title = Opl3GuiText::Category::optional, .enableChangeDetailVisible = true });

    opDelay.setupComponent(colAmp.contentCanvas, paramPrefix + CPK::delay, "DELAY", tabOrder, std::nullopt);

    opDelayNudge.setupComponent(colAmp.contentCanvas, opDelay.getSlider(), tabOrder);

    opDelaySeparator.setupComponent(colAmp.contentCanvas);

    xof.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::xof, .title = Opl3GuiText::Fm::Op::xof, .isReset = true });
    xof.setWantsKeyboardFocus(true);
    xof.setExplicitFocusOrder(++tabOrder);

    kor.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::kor, .title = Opl3GuiText::Fm::Op::kor, .isReset = true });
    kor.setWantsKeyboardFocus(true);
    kor.setExplicitFocusOrder(++tabOrder);

    bypass.setup(GuiToggleButton::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::bypass, .title = Opl3GuiText::Fm::Op::bypass, .isReset = true });
    bypass.setWantsKeyboardFocus(true);
    bypass.setExplicitFocusOrder(++tabOrder);

    pitchEnv.setupComponent(colPitchEnv.contentCanvas, paramPrefix, tabOrder, CPK::PitchAdsr::enable, Opl3GuiText::PitchAdsr::enable, true);

    ssgSwEnv.setupComponent(colSsgSwEnv.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv::enable, Opl3GuiText::SsgSwEnv::enable, true);

    ssgSwEnv11.setupComponent(colSsgSwEnv11.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv11::enable, Opl3GuiText::SsgSwEnv11::enable, true);

    ssgSwPEnv11.setupComponent(colSsgSwPEnv11.contentCanvas, paramPrefix, tabOrder, CPK::SsgSwPEnv11::enable, Opl3GuiText::SsgSwPEnv11::enable, true);
    ssgHwPEnvOp.setupComponent(colSsgHwPEnv.contentCanvas, paramPrefix, tabOrder);
    wtAmpModOp.setupComponent(colAmpMod.contentCanvas, paramPrefix, tabOrder);
    ssgHwEnvOp.setupComponent(colSsgHwEnv.contentCanvas, paramPrefix, tabOrder);
    wtModOp.setupComponent(colMod.contentCanvas, paramPrefix, tabOrder);

    catShape.setupHwCategory({ .parent = colAmp.contentCanvas, .title = Opl3GuiText::Category::eg, .enableChangeDetailVisible = true });

    eg.setup(GuiComboBox::Config{ .parent = colAmp.contentCanvas, .id = paramPrefix + CPK::Fm::eg, .title = Opl3GuiText::Fm::Op::Eg, .items = opl3EgItems, .isReset = true });
    eg.setWantsKeyboardFocus(true);
    eg.setExplicitFocusOrder(++tabOrder);

    catLfo.setupHwCategory({ .parent = colKs.contentCanvas, .title = Opl3GuiText::Category::lfo, .enableChangeDetailVisible = true });

    am.setup(GuiToggleButton::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::am, .title = Opl3GuiText::Fm::Op::Am, .isReset = true });
    am.setWantsKeyboardFocus(true);
    am.setExplicitFocusOrder(++tabOrder);

    ams.setupComponent(colKs.contentCanvas, paramPrefix + CPK::Fm::ams, Opl3GuiText::Fm::Op::Ams, tabOrder, std::nullopt);

    amsTo37.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .title = "->3.7Hz", .isReset = false, .isResized = false });
    amsTo37.setWantsKeyboardFocus(true);
    amsTo37.setExplicitFocusOrder(++tabOrder);
    amsTo37.onClick = [this] { ams.setValue(3.7, juce::sendNotification); };

    amd.setupComponent(colKs.contentCanvas, paramPrefix + CPK::Fm::amd, Opl3GuiText::Fm::Op::Amd, tabOrder, std::nullopt);

    amdTo1.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .title = "->1dB", .isReset = false, .isResized = false });
    amdTo1.setWantsKeyboardFocus(true);
    amdTo1.setExplicitFocusOrder(++tabOrder);
    amdTo1.onClick = [this] { amd.setValue(1.0, juce::sendNotification); };

    amdTo48.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .title = "->4.8dB", .isReset = false, .isResized = false });
    amdTo48.setWantsKeyboardFocus(true);
    amdTo48.setExplicitFocusOrder(++tabOrder);
    amdTo48.onClick = [this] { amd.setValue(4.8, juce::sendNotification); };

    vib.setup(GuiToggleButton::Config{ .parent = colKs.contentCanvas, .id = paramPrefix + CPK::Fm::vib, .title = Opl3GuiText::Fm::Op::Vib, .isReset = true });
    vib.setWantsKeyboardFocus(true);
    vib.setExplicitFocusOrder(++tabOrder);

    pms.setupComponent(colKs.contentCanvas, paramPrefix + CPK::Fm::pms, Opl3GuiText::Fm::Op::Pms, tabOrder, std::nullopt);

    pmsTo64.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .title = "->6.4Hz", .isReset = false, .isResized = false });
    pmsTo64.setWantsKeyboardFocus(true);
    pmsTo64.setExplicitFocusOrder(++tabOrder);
    pmsTo64.onClick = [this] { pms.setValue(6.4, juce::sendNotification); };

    pmd.setupComponent(colKs.contentCanvas, paramPrefix + CPK::Fm::pmd, Opl3GuiText::Fm::Op::Pmd, tabOrder, std::nullopt);

    pmdTo7.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .title = "->7cent", .isReset = false, .isResized = false });
    pmdTo7.setWantsKeyboardFocus(true);
    pmdTo7.setExplicitFocusOrder(++tabOrder);
    pmdTo7.onClick = [this] { pmd.setValue(7.0, juce::sendNotification); };

    pmdTo14.setup(GuiTextButton::Config{ .parent = colKs.contentCanvas, .title = "->14cent", .isReset = false, .isResized = false });
    pmdTo14.setWantsKeyboardFocus(true);
    pmdTo14.setExplicitFocusOrder(++tabOrder);
    pmdTo14.onClick = [this] { pmd.setValue(14.0, juce::sendNotification); };

    lfoSep.setupComponent(colKs.contentCanvas);

    catMask.setupHwCategory({ .parent = colMask.contentCanvas, .title = Opl3GuiText::Category::mask, .enableChangeDetailVisible = true });

    mask.setup(GuiToggleButton::Config{ .parent = colMask.contentCanvas, .id = paramPrefix + CPK::Fm::mask, .title = Opl3GuiText::Fm::Op::Mask, .isReset = true });
    mask.setWantsKeyboardFocus(true);
    mask.setExplicitFocusOrder(++tabOrder);

    mmlSeparator.setupComponent(colMask.contentCanvas);

    mml.setup({ .parent = colMask.contentCanvas, .title = I18n::pick(u8"MML風入力", u8"MML-style input"), .isReset = false, .isResized = false });
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
        &colAmp, &colSsgHwEnv, &colSsgSwEnv,
        &colSsgSwEnv11, &colAmpMod, &colPitchEnv, &colSsgHwPEnv,
        &colSsgSwPEnv11, &colMod, &colKs,
        &colMask,
        })
    {
        for (auto* child : group->contentCanvas.getChildren())
        {
            if (auto* cat = dynamic_cast<GuiCategoryLabel*>(child)) cat->setDetailVisible(true);
        }
    }

    // 前に開いていたときの指し先から始める。
    const int saved = (int)ctx.audioProcessor.apvts.state.getProperty(ProcessorStateKey::oplTarget, 0);

    targerOpSlider.setValue(juce::jlimit(0, Opl3PrValue::ops - 1, saved) + 1, juce::dontSendNotification);

    applyOpTarget();
}

void GuiOpl3::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    auto mainArea = pageArea.removeFromLeft(Opl3GuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(Opl3GuiValue::Group::Padding::width, Opl3GuiValue::Group::Padding::height);
    mmRect.removeFromTop(Opl3GuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    // グラフはスクロールさせない固定ヘッダに置く。
    // 他のチャンネルやオペレータと同じく、プリセット名の下に来る。
    layoutGlobalGraph(mmRect);
    updateGlobalGraph();

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.viewport.getMaximumVisibleWidth(), 2000);

    levelComponent.layoutComponent(mRect);

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });

    mRect.removeFromTop(Opl3GuiValue::Category::paddingTop);

    auto graphArea = mRect.removeFromTop(150); // 描画領域確保
    algStaticGraphComp.setBounds(graphArea.reduced(10));

    algFbSep.layoutComponent(mRect);

    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    mRect.removeFromTop(CoreGuiValue::Category::gapBelow);

    // [[AMP ENV]] の大区分。閉じているあいだは、中の区分を見出しごと出さない。
    const bool ampOpen = layoutMajorCategory(ampMajorCat, mRect,
        ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)
        || ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));

    ampEnvComponent.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::AmpEnv));
    ampEnvComponent.layoutComponent(mRect);
    ssgHwEnv.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv));
    ssgHwEnv.layoutComponent(mRect);
    ssgSwEnv11g.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11));
    ssgSwEnv11g.layoutComponent(mRect);
    ampModComponent.setCategoryVisible(ampOpen && ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod));
    ampModComponent.layoutComponent(mRect);

    ampMajorCat.endMajor(mRect);

    // [[PITCH ENV]] の大区分。閉じているあいだは、中の区分を見出しごと出さない。
    const bool pitchOpen = layoutMajorCategory(pitchMajorCat, mRect,
        ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)
        || ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)
        || ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));

    ssgHwPEnv.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv));
    ssgHwPEnv.layoutComponent(mRect);
    ssgSwPEnv11g.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11));
    ssgSwPEnv11g.layoutComponent(mRect);
    modComponent.setCategoryVisible(pitchOpen && ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod));
    modComponent.layoutComponent(mRect);

    pitchMajorCat.endMajor(mRect);

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
    constexpr int rows = (Opl3PrValue::ops + cols - 1) / cols;

    const int cellW = pageArea.getWidth() / cols;
    const int cellH = GuiTargetCell::naturalHeight(false);

    auto cellsArea = pageArea.removeFromTop(cellH * rows);

    for (int r = 0; r < rows; ++r) {
        auto rowArea = cellsArea.removeFromTop(cellH);

        for (int col = 0; col < cols; ++col) {
            const int i = r * cols + col;

            auto cellArea = rowArea.removeFromLeft(cellW);

            if (i >= Opl3PrValue::ops) continue;

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
void GuiOpl3::applyMmlString(const juce::String& mml)
{
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mul.setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMl,   [&](int v) { mul.setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMask, [&](int v) { mask.setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },
        { mmlPrefixAm,   [&](int v) { am.setToggleState(RegisterConverter::convertOplAm(v), juce::sendNotification); } },
        { mmlPrefixVib,   [&](int v) { vib.setToggleState(RegisterConverter::convertOplVib(v), juce::sendNotification); } },
        { mmlPrefixVb,   [&](int v) { vib.setToggleState(RegisterConverter::convertOplVib(v), juce::sendNotification); } },
        { mmlPrefixPm,   [&](int v) { vib.setToggleState(RegisterConverter::convertOplVib(v), juce::sendNotification); } },
        { mmlPrefixEgType,   [&](int v) { egType.setToggleState(RegisterConverter::convertOplEgType(v), juce::sendNotification); } },
        { mmlPrefixEt,   [&](int v) { egType.setToggleState(RegisterConverter::convertOplEgType(v), juce::sendNotification); } },
        { mmlPrefixKsr,   [&](int v) { ksr.setToggleState(RegisterConverter::convertOplKsr(v), juce::sendNotification); } },
        { mmlPrefixKr,   [&](int v) { ksr.setToggleState(RegisterConverter::convertOplKsr(v), juce::sendNotification); } },
        { mmlPrefixKsl,   [&](int v) { ksl.setSelectedItemIndex(RegisterConverter::convertOplKsl(v), juce::sendNotification); } },
        { mmlPrefixKl,   [&](int v) { ksl.setSelectedItemIndex(RegisterConverter::convertOplKsl(v), juce::sendNotification); } },

        // --- TL系 (RGモードで分岐) ---
        { mmlPrefixTl,   [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg63(v), juce::sendNotification); }},
        { mmlPrefixO,    [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg63(v), juce::sendNotification); }},
        { mmlPrefixOl,   [&](int v) { rgTl.setValue(RegisterConverter::convertFmRg63(v), juce::sendNotification); }},

        // --- エンベロープ系 (RGモードで分岐) ---
        { mmlPrefixAr,   [&](int v) { rgAr.setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
        { mmlPrefixDr,   [&](int v) { rgDr.setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
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
void GuiOpl3::updateOpEnable(int idx, bool enable)
{
    if (idx != currentOp()) return;

    mul.setEnabledWithLabel(enable);
    ksr.setEnabled(enable);
    ksl.setEnabledWithLabel(enable);
    egType.setEnabled(enable);
    catOptional.setEnabled(enable);
    opDelay.setEnabled(enable);
    opDelayNudge.setEnables(enable);
    xof.setEnabled(enable);
    kor.setEnabled(enable);
    bypass.setEnabled(enable);
    eg.setEnabledWithLabel(enable);
    catShape.setEnabled(enable);
    catLfo.setEnabled(enable);
    vib.setEnabled(enable);
    pms.setEnabledWithLabel(enable);
    pmsTo64.setEnabled(enable);
    pmd.setEnabledWithLabel(enable);
    pmdTo7.setEnabled(enable);
    pmdTo14.setEnabled(enable);
    am.setEnabled(enable);
    ams.setEnabledWithLabel(enable);
    amsTo37.setEnabled(enable);
    amd.setEnabledWithLabel(enable);
    amdTo1.setEnabled(enable);
    amdTo48.setEnabled(enable);
    catMask.setEnabled(enable);
    mask.setEnabled(enable);
    mmlSeparator.setEnabled(enable);
    mml.setEnabled(enable);
    pitchEnv.setEnabled(enable);
    ssgSwEnv.setEnabled(enable);
}

void GuiOpl3::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > Opl3PrValue::Alg::max) return;

    // Coreのルーティング情報から FmAlgState を生成
    const auto& routing = Opl3Core::routings[algIndex];
    FmAlgState s;
    s.numOps = Opl3PrValue::ops;

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

    for (int i = 0; i < Opl3PrValue::ops; ++i)
    {
        // 配列を使わず、到達可能性から判定したフラグをセット
        opActive[(size_t)i] = activeOps[i];

        updateOpEnable(i, activeOps[i]);
    }
}

void GuiOpl3::updateRgDisplayAsOp(bool rgMode)
{
    rgAr.setVisibleWithLabel(rgMode);
    rgDr.setVisibleWithLabel(rgMode);
    rgSl.setVisibleWithLabel(rgMode);
    rgRr.setVisibleWithLabel(rgMode);
    rgTl.setVisibleWithLabel(rgMode);
}

// TARGET が今どのオペレータを指しているか (0 から数える)。
int GuiOpl3::currentOp() const
{
    return juce::jlimit(0, Opl3PrValue::ops - 1, (int)targerOpSlider.getValue() - 1);
}

// TARGET が指すオペレータへ、設定の束縛を丸ごと移す。
//
// オペレータごとに部品を並べるのをやめたので、指し先はここで差し替える。
// 見た目は組み直さない。
void GuiOpl3::rebind(int opIndex)
{
    const juce::String code = Opl3PrKey::prefix + CPK::op + juce::String(opIndex);

    opNameLabel.setText(Opl3GuiText::Group::opPrefix + juce::String(opIndex + 1), juce::dontSendNotification);

    mul.rebind(code + CPK::mul);
    ksr.rebind(code + CPK::Fm::ksr);
    ksl.rebind(code + CPK::Fm::ksl);

    rgAr.rebind(code + CPK::Fm::rgAr);
    rgDr.rebind(code + CPK::Fm::rgDr);
    rgSl.rebind(code + CPK::Fm::rgSl);
    rgRr.rebind(code + CPK::Fm::rgRr);
    rgTl.rebind(code + CPK::Fm::rgTl);

    egType.rebind(code + CPK::Fm::egType);

    opDelay.getSlider().rebind(code + CPK::delay);

    xof.rebind(code + CPK::Fm::xof);
    kor.rebind(code + CPK::Fm::kor);
    bypass.rebind(code + CPK::Fm::bypass);

    eg.rebind(code + CPK::Fm::eg);

    am.rebind(code + CPK::Fm::am);
    ams.getSlider().rebind(code + CPK::Fm::ams);
    amd.getSlider().rebind(code + CPK::Fm::amd);
    vib.rebind(code + CPK::Fm::vib);
    pms.getSlider().rebind(code + CPK::Fm::pms);
    pmd.getSlider().rebind(code + CPK::Fm::pmd);

    mask.rebind(code + CPK::Fm::mask);

    pitchEnv.rebind(code);
    ssgSwEnv.rebind(code);
    ssgSwEnv11.rebind(code);
    ssgSwPEnv11.rebind(code);
    ssgHwPEnvOp.rebind(code);
    wtAmpModOp.rebind(code);
    ssgHwEnvOp.rebind(code);
    wtModOp.rebind(code);
}

// 指し先を切り替える。
//
// 設定の束縛を移し、枠線を付け替える。開き直したときに続きから
// 触れるよう、指し先は覚えておく。
void GuiOpl3::applyOpTarget()
{
    const int op = currentOp();

    ctx.audioProcessor.apvts.state.setProperty(ProcessorStateKey::opl3Target, op, nullptr);

    rebind(op);

    for (int i = 0; i < Opl3PrValue::ops; ++i) cells[(size_t)i].setActive(i == op);

    // 鳴っていないオペレータは触れないようにしてある。指し先が変わったので
    // 入り切りを引き直す。
    updateAlgorithmDisplay();
}

// 指し先を一時的に動かして何かをする。
//
// 設定はひとそろいしか無いので、TARGET が指していないオペレータを
// 読み書きするには、いったんそこへ繋ぎ替えるしかない。
// 終わったら必ず元へ戻す。
void GuiOpl3::withOp(int opIndex, const std::function<void()>& fn)
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
void GuiOpl3::setGraphMode(GuiEnvGraphMode mode)
{
    currentGraphMode = mode;

    // 札は排他。押したものだけを入れる。
    graphBtnAmp.setToggleState(mode == GuiEnvGraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GuiEnvGraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GuiEnvGraphMode::SsgSw, juce::dontSendNotification);
    graphBtnSsg11.setToggleState(mode == GuiEnvGraphMode::SsgSw11, juce::dontSendNotification);
    graphBtnSsgP11.setToggleState(mode == GuiEnvGraphMode::SsgSwP11, juce::dontSendNotification);

    for (int i = 0; i < Opl3PrValue::ops; ++i) updateOpGraph(i);
}

void GuiOpl3::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

bool GuiOpl3::keyPressed(const juce::KeyPress& key)
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

    const juce::String opPrefix = Opl3PrKey::prefix + CPK::op + juce::String(opIndex);

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

void GuiOpl3::copyFmParamsToString()
{
    // オペレータごとに部品を並べるのをやめたので、値はつまみからではなく
    // パラメータから直に採る。画面へ出ているのは TARGET の 1 本だけで、
    // MML には 4 本ぶんが要る。
    //
    // 選ぶ形のパラメータは、つまみの選択番号から 1 を引いたものと同じ値を
    // 持っている。以前 getSelectedId() - 1 と書いていたところがこれに当たる。
    auto& apvts = ctx.audioProcessor.apvts;

    auto opRaw = [&apvts](int index, const juce::String& key) {
        return GuiGraphValues::value(apvts, Opl3PrKey::prefix + CPK::op + juce::String(index) + key);
        };

    auto opVal = [&opRaw](int index, const juce::String& key) {
        return (int)opRaw(index, key);
        };

    auto opFlag = [&apvts](int index, const juce::String& key) {
        return FmMml::bool2Int(GuiGraphValues::flag(apvts,
            Opl3PrKey::prefix + CPK::op + juce::String(index) + key));
        };

    auto opMask = [&apvts](int index) {
        return FmMml::bool2Int(!GuiGraphValues::flag(apvts,
            Opl3PrKey::prefix + CPK::op + juce::String(index) + CPK::Fm::mask));
        };

    int maskVal = FmMml::genMask4(opMask(0), opMask(1), opMask(2), opMask(3));

    auto formatCoreBasic = [this, maskVal]() {
        return juce::String::formatted(
            u8"    %1d,  %1d,  %2d\n",
            algSelector.getSelectedId() - 1, // ALG
            (int)feedbackSlider.getValue(),  // FB
            maskVal);                        // MASK
        };
    auto formatOpBasic = [&opRaw, &opVal, &opFlag](int index) {
        // ' MUL AR DR SL RR  TL KSR KSL AM VIB WS EGTYPE PMS PMD AMS AMD
        return juce::String::formatted(
            u8"  %3d, %2d, %2d, %2d, %2d, %3d,  %1d,  %1d, %1d,  %1d, %1d,     %1d, %5.2f, %5.2f, %5.2f, %5.2f\n",
            opVal(index, CPK::mul),            // MUL
            opVal(index, CPK::Fm::rgAr),       // AR
            opVal(index, CPK::Fm::rgDr),       // DR
            opVal(index, CPK::Fm::rgSl),       // SL
            opVal(index, CPK::Fm::rgRr),       // RR
            opVal(index, CPK::Fm::rgTl),       // TL
            opFlag(index, CPK::Fm::ksr),       // KSR
            opVal(index, CPK::Fm::ksl),        // KSL
            opFlag(index, CPK::Fm::am),        // AM
            opFlag(index, CPK::Fm::vib),       // VIB
            opVal(index, CPK::Fm::eg),         // WS
            opFlag(index, CPK::Fm::egType),    // EGTYPE
            opRaw(index, CPK::Fm::pms),        // PMS
            opRaw(index, CPK::Fm::pmd),        // PMD
            opRaw(index, CPK::Fm::ams),        // AMS
            opRaw(index, CPK::Fm::amd)         // AMD
        );
        };
    auto formatOpsBasic = [&formatOpBasic]() {
        return formatOpBasic(0) + formatOpBasic(1) + formatOpBasic(2) + formatOpBasic(3);
        };
    auto formatOpExt = [&opVal, &opFlag](int index) {
        // ' MUL AR DR SL RR TL KSR KSL
        return juce::String::formatted(
            u8"mul%d ar%d dr%d rr%d sl%d tl%d ksr%d ksl%d\n",
            opVal(index, CPK::mul),
            opVal(index, CPK::Fm::rgAr),
            opVal(index, CPK::Fm::rgDr),
            opVal(index, CPK::Fm::rgSl),
            opVal(index, CPK::Fm::rgRr),
            opVal(index, CPK::Fm::rgTl),
            opFlag(index, CPK::Fm::ksr),
            opVal(index, CPK::Fm::ksl)
        );
        };
    auto formatOpsExt = [&formatOpExt]() {
        return formatOpExt(0) + formatOpExt(1) + formatOpExt(2) + formatOpExt(3);
        };

    juce::String mml = juce::String("[OPL3]\n")
        + FmMml::basicMmlHeader
        + juce::String(u8"' ALG FB0 FB2 MASK\n")
        + formatCoreBasic()
        + juce::String(u8"' MUL  AR  DR  SL  RR   TL KSR KSL AM VIB WS EGTYPE    PMS    PMD    AMS    AMD\n")
        + formatOpsBasic()
        + juce::String(u8"\n")
        + FmMml::extMmlHeader
        + formatOpsExt();

    juce::SystemClipboard::copyTextToClipboard(mml);
}

void GuiOpl3::copyFmParamsToObject()
{

}

void GuiOpl3::pasteFmParamsFromObject()
{

}

void GuiOpl3::initParams()
{
    this->ctx.audioProcessor.initParams("OPL3_");
}

// 設定ひとそろいの置き方。
//
// 一番上が TARGET の帯、その下に区切り線、そこから下が区分。
// 区分は縦に積まず横へ並べ、あふれたぶんは横の送り棒で送る。
void GuiOpl3::layoutOpPanel(juce::Rectangle<int> area)
{
    {
        auto row = area.removeFromTop(20);
        auto slot = row.removeFromLeft(Opl3GuiValue::Fm::Op::Col::width);

        layoutMain({ .mainRect = slot, .label = &targerOpSlider.label, .component = &targerOpSlider });

        row.removeFromLeft(Opl3GuiValue::Fm::Op::Col::gap);

        // TARGET のすぐ右が、いま指しているものの名前。
        opNameLabel.setBounds(row.removeFromLeft(Opl3GuiValue::Fm::Op::Col::nameWidth).withHeight(18));

        // 名前と切り替えの間だけを空ける。
        row.removeFromLeft(Opl3GuiValue::Fm::Op::Col::nameGap);

        // その右が、どのエンベロープを映すかの切り替え。
        {
            const int w = Opl3GuiValue::ParamGroup::Graph::ModeButtonWidth;

            auto btnRow = row.removeFromLeft(w * 5).withHeight(Opl3GuiValue::ParamGroup::Graph::ButtonHeight);

            graphBtnAmp.setBounds(btnRow.removeFromLeft(w));
            graphBtnPitch.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsg11.setBounds(btnRow.removeFromLeft(w));
            graphBtnSsgP11.setBounds(btnRow);
        }
    }

    opTargetSeparator.layoutComponent(area);

    stripViewport.setBounds(area);

    const int colW = Opl3GuiValue::Fm::Op::Col::width;
    const int colGap = Opl3GuiValue::Fm::Op::Col::gap;

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

            auto inner = colArea.reduced(Opl3GuiValue::Group::Padding::width, Opl3GuiValue::Group::Padding::height);

            inner.removeFromTop(Opl3GuiValue::Group::TitlePaddingTop);

            group.setViewportCustomBounds(inner.translated(-colArea.getX(), -colArea.getY()));

            juce::Rectangle<int> rect(0, 0, group.getContentWidth(), 20000);

            body(rect);

            group.setContentHeight(rect.getY() + 20);
        };

    const auto shown = [this](SimpleView::Cat cat) { return ctx.audioProcessor.isSimpleShown(cat); };

    layoutCol(colAmp, true, [&](juce::Rectangle<int>& rect) {
        updateRgDisplayAsOp(true);
        layoutOpAmpCat(rect);
        layoutOpOptionalCat(rect);

        layoutOpEgCat(rect);
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

    // OPL3 に DT は無いので、この列は KEY SCALE と MUL のふたつ。
    layoutCol(colKs, true, [&](juce::Rectangle<int>& rect) {
        layoutOpKsCat(rect);
        layoutOpDetCat(rect);

        layoutOpLfoCat(rect);
        });

    // MASK と MML の札も 1 列へまとめてある。
    layoutCol(colMask, true, [&](juce::Rectangle<int>& rect) {
        layoutOpMaskCat(rect);

        mmlSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .component = &mml, .paddingBottom = 0 });
        });

    stripCanvas.setSize(juce::jmax(1, x - colGap), colH);
}

void GuiOpl3::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    exportOpzx7Btn.setVisible(visible);

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    initLfoToOplBtn.setVisible(visible);
    initLfoToOpllBtn.setVisible(visible);
    uSep002.setVisible(visible);
    copyOpParamToOplBtn.setVisible(visible);
    copyOpParamToOpl12Btn.setVisible(visible);
    copyOpParamToOpl34Btn.setVisible(visible);
    uSep003.setVisible(visible);
    copyOpParamBtn.setVisible(visible);
    copyOpFromSlider.setVisibleWithLabel(visible);
    copyOpToSlider.setVisibleWithLabel(visible);
    uSep004.setVisible(visible);
    ieOpLfo.setVisible(visible);
    ieOpPitchEnv.setVisible(visible);
    ieOpSsgSwEnv.setVisible(visible);
    ieOpSsgSwEnv11.setVisible(visible);
    ieOpSsgSwPEnv11.setVisible(visible);
    ieOpSsgHwPEnv.setVisible(visible);
    ieOpWtAmpMod.setVisible(visible);
    ieOpSsgHwEnv.setVisible(visible);
    ieOpWtMod.setVisible(visible);
    ieOpChParam.setVisible(visible);
    imOplOpChParam.setVisible(visible);
    uSep005.setVisible(visible);
    ieAmpEnvG.setVisible(visible);
    ieSsgHwEnv.setVisible(visible);
    ieSsgHwPEnv.setVisible(visible);
    ieWtMod.setVisible(visible);
    ieWtAmpMod.setVisible(visible);
    ieSsgSwEnv11.setVisible(visible);
    ieSsgSwPEnv11g.setVisible(visible);
    ieUnison.setVisible(visible);
    ieQuality.setVisible(visible);
    ieChParam.setVisible(visible);
    imOplChParam.setVisible(visible);
    imOplChAllOpParam.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        uSep001.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &initLfoToOplBtn });
        layoutMain({ .mainRect = rect, .component = &initLfoToOpllBtn });

        uSep002.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyOpParamToOplBtn });
        layoutMain({ .mainRect = rect, .component = &copyOpParamToOpl12Btn });
        layoutMain({ .mainRect = rect, .component = &copyOpParamToOpl34Btn });

        uSep003.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyOpParamBtn });
        layoutMain({ .mainRect = rect, .label = &copyOpFromSlider.label, .component = &copyOpFromSlider });
        layoutMain({ .mainRect = rect, .label = &copyOpToSlider.label, .component = &copyOpToSlider });

        uSep004.layoutComponent(rect);

        ieOpLfo.layoutComponent(rect);
        rect.removeFromTop(4);
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
        imOplOpChParam.layoutComponent(rect);
        rect.removeFromTop(4);

        uSep005.layoutComponent(rect);

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
        ieUnison.layoutComponent(rect);
        rect.removeFromTop(4);
        ieQuality.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);
        rect.removeFromTop(4);
        imOplChParam.layoutComponent(rect);
        rect.removeFromTop(4);
        imOplChAllOpParam.layoutComponent(rect);

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .component = &exportOpzx7Btn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpl3::layoutOpMaskCat(juce::Rectangle<int>& rect) {
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

void GuiOpl3::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityComponent.layoutComponent(rect);
}

void GuiOpl3::layoutOpLfoCat(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catLfo });

    bool visible = catLfo.isDetailVisible();

    am.setVisible(visible);
    ams.setVisibleWithLabel(visible);
    amsTo37.setVisible(visible && ams.isVisibleNudge());
    amd.setVisibleWithLabel(visible);
    amdTo1.setVisible(visible && amd.isVisibleNudge());
    amdTo48.setVisible(visible && amd.isVisibleNudge());
    lfoSep.setVisible(visible);
    vib.setVisible(visible);
    pms.setVisibleWithLabel(visible);
    pmsTo64.setVisible(visible && pms.isVisibleNudge());
    pmd.setVisibleWithLabel(visible);
    pmdTo14.setVisible(visible && pmd.isVisibleNudge());
    pmdTo7.setVisible(visible && pmd.isVisibleNudge());

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &am });
        ams.layoutComponentRow(rect);
        if (ams.isVisibleNudge()) layoutRow({ .rowRect = rect, .component = &amsTo37 });
        amd.layoutComponentRow(rect);
        if (amd.isVisibleNudge()) layoutRowTwoComps({ .rect = rect, .comp1 = &amdTo1, .comp2 = &amdTo48 });
        lfoSep.layoutComponent(rect);
        layoutRow({ .rowRect = rect, .component = &vib });
        pms.layoutComponentRow(rect);
        if (pms.isVisibleNudge()) layoutRow({ .rowRect = rect, .component = &pmsTo64 });
        pmd.layoutComponentRow(rect);
        if (pmd.isVisibleNudge()) layoutRowTwoComps({ .rect = rect, .comp1 = &pmdTo7, .comp2 = &pmdTo14 });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpl3::layoutOpKsCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &ksCat });

    bool visible = ksCat.isDetailVisible();

    ksr.setVisible(visible);
    ksl.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .component = &ksr });
        layoutRow({ .rowRect = rect, .label = &ksl.label, .component = &ksl });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpl3::layoutOpDetCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catDet });

    bool visible = catDet.isDetailVisible();

    mul.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &mul.label, .component = &mul });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpl3::layoutOpAmpCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catAmp });

    bool visible = catAmp.isDetailVisible();

    rgAr.setVisibleWithLabel(visible);
    rgDr.setVisibleWithLabel(visible);
    rgSl.setVisibleWithLabel(visible);
    rgRr.setVisibleWithLabel(visible);
    rgTl.setVisibleWithLabel(visible);
    egType.setVisible(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &rgAr.label, .component = &rgAr });
        layoutRow({ .rowRect = rect, .label = &rgDr.label, .component = &rgDr });
        layoutRow({ .rowRect = rect, .label = &rgSl.label, .component = &rgSl });
        layoutRow({ .rowRect = rect, .label = &rgRr.label, .component = &rgRr });
        layoutRow({ .rowRect = rect, .label = &rgTl.label, .component = &rgTl });
        layoutRow({ .rowRect = rect, .component = &egType });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpl3::layoutOpEgCat(juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catShape });

    bool visible = catShape.isDetailVisible();

    eg.setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &eg.label, .component = &eg });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// ============================================================================
// チップ全体のエンベロープを見るグラフ
// ============================================================================
// オペレータごとのグラフと同じ作りで、対象がチップ全体の 3 つになる。
// 値が変わったときだけ描き直すので、常時の負荷は無い。
void GuiOpl3::setupGlobalGraph()
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

void GuiOpl3::layoutGlobalGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(Opl3GuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    gGraphSeparator.layoutComponentBottom(mainArea);

    auto btnArea = mainArea.removeFromBottom(Opl3GuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    gGraphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsg11.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsgP11.setBounds(btnArea);

    gGraph.setBounds(mainArea);
}

void GuiOpl3::updateGlobalGraph()
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

void GuiOpl3::setGlobalGraphMode(GlobalGraphMode mode)
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
void GuiOpl3::setupOpGraphWiring()
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
void GuiOpl3::updateOpGraph(int opIndex)
{
    auto& apvts = ctx.audioProcessor.apvts;
    auto& graph = cells[(size_t)opIndex].graph();

    const juce::String code = Opl3PrKey::prefix + CPK::op + juce::String(opIndex);

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
        float slMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgSl);
        float rrMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgRr);
        float tlMax = GuiGraphValues::maxOf(apvts, code + CPK::Fm::rgTl);

        float arVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgAr);
        float drVal = GuiGraphValues::value(apvts, code + CPK::Fm::rgDr);
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

        // 2. Decay
        float decayWidth = rateToWidth(drVal, drMax);
        phases.push_back({
            .widthPx = decayWidth, .startLevel = 1.0f * tlScale, .endLevel = sl * tlScale, .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Dr),
            .phaseLineColor = juce::Colours::blue
            });
        currentTotalWidth += decayWidth;

        // 3. Sustain の実線部分 (キーオン中)
        float releaseStartLevel = sl;
        float keyOnWidth = 60.0f;

        phases.push_back({ .widthPx = keyOnWidth, .startLevel = sl * tlScale, .endLevel = sl * tlScale, .color = color, .phaseLineColor = juce::Colours::green });

        currentTotalWidth += keyOnWidth;
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
                .color = color,
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

void GuiOpl3::layoutOpOptionalCat(juce::Rectangle<int>& rect) {
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

void GuiOpl3::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiOpl3::copyParams(CopyOpl3& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpl3::copyOpParams(int p, CopyOpl3Op& copyObj) {
    copyObj.detune.mul = mul.getSelectedId();
    copyObj.aAdsr.ar = rgAr.getValue();
    copyObj.aAdsr.dr = rgDr.getValue();
    copyObj.aAdsr.sl = rgSl.getValue();
    copyObj.aAdsr.rr = rgRr.getValue();
    copyObj.aAdsr.tl = rgTl.getValue();
    copyObj.aAdsr.ksr = ksr.getToggleState();
    copyObj.aAdsr.ksl = ksl.getSelectedId();
    copyObj.aAdsr.egType = egType.getToggleState();
    copyObj.aAdsr.bypass = bypass.getToggleState();
    copyObj.aAdsr.kor = kor.getToggleState();
    copyObj.aAdsr.xof = xof.getToggleState();
    copyObj.waveSelect = eg.getSelectedId();

    copyObj.lfo.am = am.getToggleState();
    copyObj.lfo.amd = amd.getValue();
    copyObj.lfo.ams = ams.getValue();
    copyObj.lfo.pm = vib.getToggleState();
    copyObj.lfo.pmd = pmd.getValue();
    copyObj.lfo.pms = pms.getValue();

    copyObj.mask.mask = mask.getToggleState();

    pitchEnv.copyParams(copyObj.pAdsr);
    ssgSwEnv.copyParams(copyObj.aSsgSw);
}

void GuiOpl3::pasteParams(CopyOpl3& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpl3::pasteOpParams(int p, CopyOpl3Op& copyObj) {
    mul.setSelectedId(copyObj.detune.mul, juce::sendNotification);
    rgAr.setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgDr.setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgSl.setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgRr.setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl.setValue(copyObj.aAdsr.tl, juce::sendNotification);
    ksr.setToggleState(copyObj.aAdsr.ksr, juce::sendNotification);
    ksl.setSelectedId(copyObj.aAdsr.ksl, juce::sendNotification);
    egType.setToggleState(copyObj.aAdsr.egType, juce::sendNotification);
    bypass.setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor.setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof.setToggleState(copyObj.aAdsr.xof, juce::sendNotification);

    am.setToggleState(copyObj.lfo.am, juce::sendNotification);
    amd.setValue(copyObj.lfo.amd, juce::sendNotification);
    ams.setValue(copyObj.lfo.ams, juce::sendNotification);
    vib.setToggleState(copyObj.lfo.pm, juce::sendNotification);
    pmd.setValue(copyObj.lfo.pmd, juce::sendNotification);
    pms.setValue(copyObj.lfo.pms, juce::sendNotification);

    mask.setToggleState(copyObj.mask.mask, juce::sendNotification);

    pitchEnv.pasteParams(copyObj.pAdsr);
    ssgSwEnv.pasteParams(copyObj.aSsgSw);
}

void GuiOpl3::importLfoParam(int opIndex)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultLfoParamDir,
        { EditorGuiText::ParamBrowser::kindLfoOpl },
        [this, opIndex](const juce::File& file) { applyLfoParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpl3::applyLfoParamFile(int opIndex, const juce::File& file)
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

            setImportingLfoParams(opIndex, lines, index);
        }

        Io::ParamWriter writer(oplLfoFormat);

        writeLfoParams(opIndex, writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, oplLfoFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    vib.setToggleState(reader->getBool("vib", vib.getToggleState()), juce::sendNotification);
    pms.setValue(reader->getFloat("pms", (float)pms.getValue()), juce::sendNotification);
    pmd.setValue(reader->getFloat("pmd", (float)pmd.getValue()), juce::sendNotification);
    am.setToggleState(reader->getBool("am", am.getToggleState()), juce::sendNotification);
    ams.setValue(reader->getFloat("ams", (float)ams.getValue()), juce::sendNotification);
    amd.setValue(reader->getFloat("amd", (float)amd.getValue()), juce::sendNotification);
}

void GuiOpl3::exportLfoParam(int opIndex)
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultLfoParamDir,
        { EditorGuiText::ParamBrowser::kindLfoOpl }, Io::Extension::OplLfoParam,
        [this, opIndex](const juce::File& file) { writeLfoParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpl3::writeLfoParamFile(int opIndex, const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(oplLfoFormat);
    writeLfoParams(opIndex, writer);

    writer.writeTo(file);
}

void GuiOpl3::importPitchEnvParam(int opIndex) {
    pitchEnv.importParams();
}

void GuiOpl3::exportPitchEnvParam(int opIndex) {
    pitchEnv.exportParams();
}

void GuiOpl3::importSsgSwEnvParam(int opIndex) {
    ssgSwEnv.importParams();
}

void GuiOpl3::exportSsgSwEnvParam(int opIndex) {
    ssgSwEnv.exportParams();
}

void GuiOpl3::importQualityParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindQuality },
        [this](const juce::File& file) { applyQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpl3::applyQualityParamFile(const juce::File& file)
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

void GuiOpl3::exportQualityParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultQualityParamDir,
        { EditorGuiText::ParamBrowser::kindQuality }, Io::Extension::QualityParam,
        [this](const juce::File& file) { writeQualityParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpl3::writeQualityParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(qualityFormat);
    writeQualityParams(writer);

    writer.writeTo(file);
}

void GuiOpl3::importSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11.importParams();
}

void GuiOpl3::exportSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11.exportParams();
}

void GuiOpl3::importSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11.importParams();
}

void GuiOpl3::exportSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11.exportParams();
}

void GuiOpl3::importOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp.importParams();
}

void GuiOpl3::exportOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp.exportParams();
}

void GuiOpl3::importOpWtAmpModParam(int opIndex) {
    wtAmpModOp.importParams();
}

void GuiOpl3::exportOpWtAmpModParam(int opIndex) {
    wtAmpModOp.exportParams();
}

void GuiOpl3::importOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp.importParams();
}

void GuiOpl3::exportOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp.exportParams();
}

void GuiOpl3::importOpWtModParam(int opIndex) {
    wtModOp.importParams();
}

void GuiOpl3::exportOpWtModParam(int opIndex) {
    wtModOp.exportParams();
}

void GuiOpl3::importChParam() {
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser({ "OPL3" },
        [this](const juce::File& file) { applyChParamFile(file); });
}

// パラメータファイルのブラウザからも同じ読み込みを使うので、
// ダイアログを出すところと、読んで反映するところを分けてある。
void GuiOpl3::applyChParamFile(const juce::File& file) {
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

        Io::ParamWriter writer(opl3Format);

        writeChParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opl3Format);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
    // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
    GuiRefresh::Batch batch;

    readChParams(*reader);
}

void GuiOpl3::exportChParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { "OPL3" }, Io::Extension::opl3Param,
        [this](const juce::File& file) { writeChParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpl3::writeChParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(opl3Format);
    writeChParams(writer);

    writer.writeTo(file);
}

void GuiOpl3::importOpChParam(int opIndex)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpl3Op },
        [this, opIndex](const juce::File& file) { applyOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpl3::applyOpChParamFile(int opIndex, const juce::File& file)
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

        Io::ParamWriter writer(opl3OpFormat);

        writeOpChFileParams(opIndex, writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, opl3OpFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    readOpParams(opIndex, *reader);
}

void GuiOpl3::exportOpChParam(int opIndex)
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOpl3Op }, Io::Extension::opl3OpParam,
        [this, opIndex](const juce::File& file) { writeOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiOpl3::writeOpChParamFile(int opIndex, const juce::File& file)
{
    if (file == juce::File{}) return;

    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                    Io::ParamWriter writer(opl3OpFormat);
    writeOpChFileParams(opIndex, writer);

    writer.writeTo(file);
}


// チャンネル 1 つぶん。
void GuiOpl3::readChParams(const Io::ParamReader& reader) {
    // Level
    levelComponent.readParams(reader, "level");

    // Algorithm & Feedback
    algSelector.setSelectedId(reader.getInt("alg", algSelector.getSelectedId()), juce::sendNotification);
    feedbackSlider.setValue(reader.getInt("feedback", (int)feedbackSlider.getValue()), juce::sendNotification);

    updateAlgorithmDisplay();

    // Components
    ssgHwEnv.readParams(reader, "ssgHwEnv");
    ssgHwPEnv.readParams(reader, "ssgHwPEnv");
    ssgSwEnv11g.readParams(reader, "ssgSwEnv11");
    qualityComponent.readParams(reader, "quality");
    unisonComponent.readParams(reader, "unison");

    for (int i = 0; i < Opl3PrValue::ops; i++) {
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
void GuiOpl3::readOpParams(int opIndex, const Io::ParamReader& r) {
    // Mul
    mul.setSelectedId(r.getInt("mul", mul.getSelectedId()), juce::sendNotification);

    // Env
    rgAr.setValue(r.getFloat("ar", (float)rgAr.getValue()), juce::sendNotification);
    rgDr.setValue(r.getFloat("dr", (float)rgDr.getValue()), juce::sendNotification);
    rgSl.setValue(r.getFloat("sl", (float)rgSl.getValue()), juce::sendNotification);
    rgRr.setValue(r.getFloat("rr", (float)rgRr.getValue()), juce::sendNotification);
    rgTl.setValue(r.getFloat("tl", (float)rgTl.getValue()), juce::sendNotification);

    // Key Scale & EG Type
    ksr.setToggleState(r.getBool("ksr", ksr.getToggleState()), juce::sendNotification);
    ksl.setSelectedId(r.getInt("ksl", ksl.getSelectedId()), juce::sendNotification);
    egType.setToggleState(r.getBool("egType", egType.getToggleState()), juce::sendNotification);

    // Optional / Mask
    bypass.setToggleState(r.getBool("bypass", bypass.getToggleState()), juce::sendNotification);
    kor.setToggleState(r.getBool("kor", kor.getToggleState()), juce::sendNotification);
    xof.setToggleState(r.getBool("xof", xof.getToggleState()), juce::sendNotification);
    mask.setToggleState(r.getBool("mask", mask.getToggleState()), juce::sendNotification);

    // Wave Shape
    eg.setSelectedId(r.getInt("eg", eg.getSelectedId()), juce::sendNotification);

    // LFO (AM / VIB)
    am.setToggleState(r.getBool("am", am.getToggleState()), juce::sendNotification);
    amd.setValue(r.getFloat("amd", (float)amd.getValue()), juce::sendNotification);
    ams.setValue(r.getFloat("ams", (float)ams.getValue()), juce::sendNotification);
    vib.setToggleState(r.getBool("vib", vib.getToggleState()), juce::sendNotification);
    pmd.setValue(r.getFloat("pmd", (float)pmd.getValue()), juce::sendNotification);
    pms.setValue(r.getFloat("pms", (float)pms.getValue()), juce::sendNotification);

    // Components
    pitchEnv.readParams(r, "pitchEnv");
    ssgSwEnv.readParams(r, "ssgSwEnv");
    ssgSwEnv11.readParams(r, "ssgSwEnv11");
    ssgSwPEnv11.readParams(r, "ssgSwPEnv11");
    ssgHwPEnvOp.readParams(r, "ssgHwPEnv");
    wtAmpModOp.readParams(r, "wtAmpMod");
    ssgHwEnvOp.readParams(r, "ssgHwEnv");
    wtModOp.readParams(r, "wtMod");
}

void GuiOpl3::writeOpParams(int opIndex, Io::ParamWriter& w) {
    // Mul
    w.set("mul", mul.getSelectedId());

    // Env
    w.set("ar", (float)rgAr.getValue());
    w.set("dr", (float)rgDr.getValue());
    w.set("sl", (float)rgSl.getValue());
    w.set("rr", (float)rgRr.getValue());
    w.set("tl", (float)rgTl.getValue());

    // Key Scale & EG Type
    w.set("ksr", ksr.getToggleState());
    w.set("ksl", ksl.getSelectedId());
    w.set("egType", egType.getToggleState());

    // Optional / Mask
    w.set("bypass", bypass.getToggleState());
    w.set("kor", kor.getToggleState());
    w.set("xof", xof.getToggleState());
    w.set("mask", mask.getToggleState());

    // Wave Shape
    w.set("eg", eg.getSelectedId());

    // LFO (AM / VIB)
    w.set("am", am.getToggleState());
    w.set("amd", (float)amd.getValue());
    w.set("ams", (float)ams.getValue());
    w.set("vib", vib.getToggleState());
    w.set("pmd", (float)pmd.getValue());
    w.set("pms", (float)pms.getValue());

    // Components
    pitchEnv.writeParams(w, "pitchEnv");
    ssgSwEnv.writeParams(w, "ssgSwEnv");
    ssgSwEnv11.writeParams(w, "ssgSwEnv11");
    ssgSwPEnv11.writeParams(w, "ssgSwPEnv11");
    ssgHwPEnvOp.writeParams(w, "ssgHwPEnv");
    wtAmpModOp.writeParams(w, "wtAmpMod");
    ssgHwEnvOp.writeParams(w, "ssgHwEnv");
    wtModOp.writeParams(w, "wtMod");
}

void GuiOpl3::importOplChParam() {
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser({ "OPL" },
        [this](const juce::File& file) { applyOplChParamFile(file); });
}

// パラメータファイルのブラウザからも同じ読み込みを使うので、
// ダイアログを出すところと、読んで反映するところを分けてある。
void GuiOpl3::applyOplChParamFile(const juce::File& file) {
    if (!file.existsAsFile()) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    // 相手の音源のファイル。名前で引くので、こちらに無い項目は
    // 読み飛ばされ、こちらにしか無い項目は今の値のまま残る。
    auto reader = Io::ParamReader::open(file, oplFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    readChParams(*reader);
}

void GuiOpl3::importOplChAllOpParam() {
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser({ "OPL" },
        [this](const juce::File& file) { applyOplChAllOpParamFile(file); });
}

// パラメータファイルのブラウザからも同じ読み込みを使うので、
// ダイアログを出すところと、読んで反映するところを分けてある。
void GuiOpl3::applyOplChAllOpParamFile(const juce::File& file) {
    if (!file.existsAsFile()) return;

    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    // OPL のファイル。名前で引くので、こちらに無い項目は読み飛ばされる。
    auto reader = Io::ParamReader::open(file, oplFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    // Level
    levelComponent.readParams(*reader, "level");

    // Algorithm & Feedback
    algSelector.setSelectedId(reader->getInt("alg", algSelector.getSelectedId()), juce::sendNotification);
    feedbackSlider.setValue(reader->getInt("feedback", (int)feedbackSlider.getValue()), juce::sendNotification);

    updateAlgorithmDisplay();

    // Components
    qualityComponent.readParams(*reader, "quality");
    unisonComponent.readParams(*reader, "unison");

    for (int i = 0; i < OplPrValue::ops; i++) {
        auto op = reader->arrayItem(Io::ParamKey::ops, i);

        // OPL は 2 つしか無いので、同じものを OPL3 の 3・4 番へも入れる
        withOp(i, [this, &op, i] { readOpParams(i, op); });
        withOp(i + OplPrValue::ops, [this, &op, i] { readOpParams(i + OplPrValue::ops, op); });
    }
}

void GuiOpl3::importOplOpChParam(int opIndex)
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    ctx.editor.openParamBrowser(ctx.audioProcessor.defaultChannelParamDir,
        { EditorGuiText::ParamBrowser::kindOplOp },
        [this, opIndex](const juce::File& file) { applyOplOpChParamFile(opIndex, file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiOpl3::applyOplOpChParamFile(int opIndex, const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

    auto reader = Io::ParamReader::open(file, oplOpFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    readOpParams(opIndex, *reader);
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。並び順を写し直すより確実で、
// 当時の互換の工夫もそのまま残る。
void GuiOpl3::setImportingChParams(juce::StringArray& lines, int& index) {
	// Level
	levelComponent.setImportingParams(lines, index);

	// Algorithm & Feedback
	algSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
	feedbackSlider.setValue(lines[index++].getIntValue(), juce::sendNotification);

	updateAlgorithmDisplay();

	// Components
	ssgHwEnv.setImportingParams(lines, index);
	ssgSwEnv11g.setImportingParams(lines, index);
	qualityComponent.setImportingParams(lines, index);
	unisonComponent.setImportingParams(lines, index);

	for (int i = 0; i < Opl3PrValue::ops; i++) {
	    withOp(i, [this, &lines, &index, i] { getImportingOpParams(i, lines, index); });
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
void GuiOpl3::writeChParams(Io::ParamWriter& writer) {
	// Level
	levelComponent.writeParams(writer, "level");

	// Algorithm & Feedback
	writer.set("alg", algSelector.getSelectedId());
	writer.set("feedback", (float)feedbackSlider.getValue());

	// Components
	ssgHwEnv.writeParams(writer, "ssgHwEnv");
	ssgHwPEnv.writeParams(writer, "ssgHwPEnv");
	ssgSwEnv11g.writeParams(writer, "ssgSwEnv11");
	qualityComponent.writeParams(writer, "quality");
	unisonComponent.writeParams(writer, "unison");

	for (int i = 0; i < Opl3PrValue::ops; i++) {
	    auto op = writer.arrayItem(Io::ParamKey::ops, i);

	    withOp(i, [this, &op, i] { writeOpParams(i, op); });
	}

	// 名前で持つので、置き場所に意味は無い
	ampEnvComponent.writeParams(writer, "ampEnv");
	ssgSwPEnv11g.writeParams(writer, "ssgSwPEnv11");
	modComponent.writeParams(writer, "wtMod");
	ampModComponent.writeParams(writer, "wtAmpMod");

	
}

// 3.0.0 より前の形式を読むための補助。履歴から戻したもの。
void GuiOpl3::getImportingOpParams(int opIndex, juce::StringArray& lines, int& index) {
    // Mul
    mul.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // Env
    rgAr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgDr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgSl.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgRr.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgTl.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Key Scale & EG Type
    ksr.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    ksl.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    egType.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

    // Optional / Mask
    bypass.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    kor.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    xof.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    mask.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

    // Wave Shape
    eg.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // LFO (AM / VIB)
    am.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    amd.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    ams.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    vib.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    pmd.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    pms.setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Components
    pitchEnv.setImportingParams(lines, index);
    ssgSwEnv.setImportingParams(lines, index);
    ssgSwEnv11.setImportingParams(lines, index);
    ssgSwPEnv11.setImportingParams(lines, index);
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpl3::setImportingQualityParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 2) return;

	qualityComponent.setBit(lines[0].getIntValue());
	qualityComponent.setRate(lines[1].getIntValue());

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpl3::writeQualityParams(Io::ParamWriter& writer) {
	writer.set("bit", qualityComponent.getBit());
	writer.set("rate", qualityComponent.getRate());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpl3::setImportingOpChFileParams(int opIndex, juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	updateAlgorithmDisplay();

	getImportingOpParams(opIndex, lines, index);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpl3::writeOpChFileParams(int opIndex, Io::ParamWriter& writer) {
	writeOpParams(opIndex, writer);

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpl3::setImportingLfoParams(int opIndex, juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 6) return;

	vib.setToggleState(lines[0].getIntValue() == 1, juce::sendNotification);
	pms.setValue(lines[1].getFloatValue(), juce::sendNotification);
	pmd.setValue(lines[2].getFloatValue(), juce::sendNotification);
	am.setToggleState(lines[3].getIntValue() == 1, juce::sendNotification);
	ams.setValue(lines[4].getFloatValue(), juce::sendNotification);
	amd.setValue(lines[5].getFloatValue(), juce::sendNotification);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpl3::writeLfoParams(int opIndex, Io::ParamWriter& writer) {
	writer.set("vib", vib.getToggleState());
	writer.set("pms", (float)pms.getValue());
	writer.set("pmd", (float)pmd.getValue());
	writer.set("am", am.getToggleState());
	writer.set("ams", (float)ams.getValue());
	writer.set("amd", (float)amd.getValue());

	
}

void GuiOpl3::bypassHiddenCategories()
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

void GuiOpl3::openEnabledCategories()
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

    // 大区分は中の区分に合わせる。効いている区分があれば開く。
    if (anyCategoryEnabled(ampEnvComponent, ssgHwEnv, ssgSwEnv11g, ampModComponent)) ampMajorCat.setDetailVisible(true);
    if (anyCategoryEnabled(ssgHwPEnv, ssgSwPEnv11g, modComponent)) pitchMajorCat.setDetailVisible(true);
}

void GuiOpl3::closeBypassedCategories()
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

    // 大区分は中の区分に合わせる。どれも切ってあれば閉じる。
    if (allCategoriesBypassed(ampEnvComponent, ssgHwEnv, ssgSwEnv11g, ampModComponent)) ampMajorCat.setDetailVisible(false);
    if (allCategoriesBypassed(ssgHwPEnv, ssgSwPEnv11g, modComponent)) pitchMajorCat.setDetailVisible(false);
}

// ----------------------------------------------------------------------------
// OPZX7S のパラメータファイルへ書き出す
// ----------------------------------------------------------------------------
// 書き出すのは CH Params と同じ中身。直し方は OPZX7S の [IM]FM Params と同じ
void GuiOpl3::exportOpzx7Params()
{
    FmToOpzx7::exportFile(ctx, FmToOpzx7::Source::opl3, [this](Io::ParamWriter& w) { writeChParams(w); });
}
