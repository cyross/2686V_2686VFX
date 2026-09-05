#include <vector>

#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiOpl.h"

#include "../../Core/Gui/GuiRefresh.h"

#include "../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat oplLfoFormat{ "oplLfo", 1 };
	const Io::ParamFormat qualityFormat{ "quality", 1 };
	const Io::ParamFormat oplFormat{ "opl", 1 };
	const Io::ParamFormat oplOpFormat{ "oplOp", 1 };

	// 相手の音源のファイルを読み込む口があるため、その印も要る
	const Io::ParamFormat opl3Format{ "opl3", 1 };
	const Io::ParamFormat opl3OpFormat{ "opl3Op", 1 };
}

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Opl/ProcessorOplKeys.h"
#include "../../Processor/Opl/ProcessorOplValues.h"
#include "../../Processor/Opl3/ProcessorOpl3Values.h"
#include "../../Core/Const/ConstMmlKeys.h"
#include "../../Core/Const/ConstMmlValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Fm/FmRegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiOplValues.h"
#include "./GuiOplText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> oplAlgItems = {
    {.name = "00: <OPL-00>", .value = 1 },
    {.name = "01: <OPL-01>", .value = 2 },
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

static std::vector<SelectItem> oplEgItems = {
    {.name = "0: Sine",       .value = 1},
    {.name = "1: Half Sine",  .value = 2},
    {.name = "2: Abs Sine",   .value = 3},
    {.name = "3: Pulse Sine", .value = 4}
};

void GuiOpl::setup()
{
    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    p_curveCore = ctx.audioProcessor.getCurveCore();

    const juce::String code = OplPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, OplGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    qualityComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    algFbCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Category::algFb });
    algSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::alg, .title = OplGuiText::Fm::alg, .items = oplAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    mainGroup.contentCanvas.addAndMakeVisible(&algStaticGraphComp);

	algFbSep.setupComponent(mainGroup.contentCanvas);

    feedbackSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::fb, .title = OplGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Category::util, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    initLfoToOplBtn.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Fm::initLfoToOpl });
    initLfoToOplBtn.setWantsKeyboardFocus(true);
    initLfoToOplBtn.setExplicitFocusOrder(++tabOrder);
    initLfoToOplBtn.onClick = [this] {
        for (int i = 0; i < OplPrValue::ops; i++)
        {
            ams[i].setValue(3.7, juce::sendNotification);
            amd[i].setValue(4.8, juce::sendNotification);

            pms[i].setValue(6.4, juce::sendNotification);
            pmd[i].setValue(14.0, juce::sendNotification);
        }
        };
    initLfoToOpllBtn.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Fm::initLfoToOpll });
    initLfoToOpllBtn.setWantsKeyboardFocus(true);
    initLfoToOpllBtn.setExplicitFocusOrder(++tabOrder);
    initLfoToOpllBtn.onClick = [this] {
        for (int i = 0; i < OplPrValue::ops; i++)
        {
            ams[i].setValue(6.06, juce::sendNotification);
            amd[i].setValue(1.2, juce::sendNotification);

            pms[i].setValue(6.06, juce::sendNotification);
            pmd[i].setValue(13.7, juce::sendNotification);
        }
        };

    uSep002.setupComponent(mainGroup.contentCanvas);

    copyOpParamToOpl3Btn.setup({ .parent = mainGroup.contentCanvas, .title = "Params -> OPL3", .bgColor = juce::Colours::turquoise.darker(0.5f)});
    copyOpParamToOpl3Btn.setWantsKeyboardFocus(true);
    copyOpParamToOpl3Btn.setExplicitFocusOrder(++tabOrder);
    copyOpParamToOpl3Btn.onClick = [this] {
        ctx.editor.copyOplParamsToOpl3();
        };

    copyOpParamToOpl312Btn.setup({ .parent = mainGroup.contentCanvas, .title = "OP Params -> OPL3 OP1/2", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamToOpl312Btn.setWantsKeyboardFocus(true);
    copyOpParamToOpl312Btn.setExplicitFocusOrder(++tabOrder);
    copyOpParamToOpl312Btn.onClick = [this] {
        ctx.editor.copyOplParamsToOpl312();
        };

    copyOpParamToOpl334Btn.setup({ .parent = mainGroup.contentCanvas, .title = "OP Params -> OPL3 OP3/4", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamToOpl334Btn.setWantsKeyboardFocus(true);
    copyOpParamToOpl334Btn.setExplicitFocusOrder(++tabOrder);
    copyOpParamToOpl334Btn.onClick = [this] {
        ctx.editor.copyOplParamsToOpl334();
        };

    uSep003.setupComponent(mainGroup.contentCanvas);

    copyOpParamBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Copy Op Params", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamBtn.setWantsKeyboardFocus(true);
    copyOpParamBtn.setExplicitFocusOrder(++tabOrder);
    copyOpParamBtn.onClick = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        ctx.editor.copyOplOpParams(from, to);
        };

    copyOpFromSlider.setup({ .parent = mainGroup.contentCanvas, .title = "FROM", .isReset = false });
    copyOpFromSlider.setRange(1.0, 2.0, 1.0);
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
    copyOpToSlider.setRange(1.0, 2.0, 1.0);
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

    imOpl3OpChParam.setupComponentOp(mainGroup.contentCanvas, tabOrder, "OPL3 OP Params");
    imOpl3OpChParam.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpl3OpChParam(opIndex); };

    targerOpSlider.setup({ .parent = mainGroup.contentCanvas, .title = "Op", .isReset = false });
    targerOpSlider.setRange(1.0, 2.0, 1.0);
    targerOpSlider.setNumDecimalPlacesToDisplay(0);
    targerOpSlider.setValue(1, juce::sendNotification);
    targerOpSlider.setWantsKeyboardFocus(true);
    targerOpSlider.setExplicitFocusOrder(++tabOrder);

    uSep005.setupComponent(mainGroup.contentCanvas);

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

    imOpl3ChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "OPL3 CH Params");
    imOpl3ChParam.onClickImport = [this] { importOpl3ChParam(); };

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

    const juce::String opCode = code + CPK::op;

    for (int i = 0; i < OplPrValue::ops; ++i)
    {
        opGroups[i].setup(*this, OplGuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        catDet[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::detune, .enableChangeDetailVisible = true });
        catAmp[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::ampEnv, .detailVisible = true, .enableChangeDetailVisible = true });

        mul[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::mul, .title = OplGuiText::Fm::Op::Mul, .items = multems, .isReset = true });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);

        rgAr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgAr, .title = OplGuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgDr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgDr, .title = OplGuiText::Fm::Op::Dr, .isReset = true });
        rgDr[i].setWantsKeyboardFocus(true);
        rgDr[i].setExplicitFocusOrder(++tabOrder);

        rgSl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgSl, .title = OplGuiText::Fm::Op::Sl, .isReset = true });
        rgSl[i].setWantsKeyboardFocus(true);
        rgSl[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgRr, .title = OplGuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgTl, .title = OplGuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        egType[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::egType, .title = OplGuiText::Fm::Op::EgType, .isReset = true });
        egType[i].setWantsKeyboardFocus(true);
        egType[i].setExplicitFocusOrder(++tabOrder);

        sus[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::sus, .title = OplGuiText::Fm::Op::sus, .isReset = true });

        ksCat[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::ks, .enableChangeDetailVisible = true });

        ksr[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksr, .title = OplGuiText::Fm::Op::Ksr, .isReset = true });
        ksr[i].setWantsKeyboardFocus(true);
        ksr[i].setExplicitFocusOrder(++tabOrder);

        ksl[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ksl, .title = OplGuiText::Fm::Op::Ksl, .items = kslItems, .isReset = true });
        ksl[i].setWantsKeyboardFocus(true);
        ksl[i].setExplicitFocusOrder(++tabOrder);

        catOptional[i].setupSwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::optional, .enableChangeDetailVisible = true });

        xof[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::xof, .title = OplGuiText::Fm::Op::xof, .isReset = true });
        xof[i].setWantsKeyboardFocus(true);
        xof[i].setExplicitFocusOrder(++tabOrder);

        kor[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::kor, .title = OplGuiText::Fm::Op::kor, .isReset = true });
        kor[i].setWantsKeyboardFocus(true);
        kor[i].setExplicitFocusOrder(++tabOrder);

        bypass[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::bypass, .title = OplGuiText::Fm::Op::bypass, .isReset = true });
        bypass[i].setWantsKeyboardFocus(true);
        bypass[i].setExplicitFocusOrder(++tabOrder);

        pitchEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::PitchAdsr::enable, OplGuiText::PitchAdsr::enable, true);

        ssgSwEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv::enable, OplGuiText::SsgSwEnv::enable, true);

        ssgSwEnv11[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv11::enable, OplGuiText::SsgSwEnv11::enable, true);

        ssgSwPEnv11[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwPEnv11::enable, OplGuiText::SsgSwPEnv11::enable, true);
        ssgHwPEnvOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);
        wtAmpModOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);
        ssgHwEnvOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);
        wtModOp[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder);

        catShape[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::eg, .enableChangeDetailVisible = true });

        eg[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::eg, .title = OplGuiText::Fm::Op::Eg, .items = oplEgItems, .isReset = true });
        eg[i].setWantsKeyboardFocus(true);
        eg[i].setExplicitFocusOrder(++tabOrder);

        catLfo[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::lfo, .enableChangeDetailVisible = true });

        am[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::am, .title = OplGuiText::Fm::Op::Am, .isReset = true });
        am[i].setWantsKeyboardFocus(true);
        am[i].setExplicitFocusOrder(++tabOrder);

        ams[i].setupComponent(opGroups[i].contentCanvas, paramPrefix + CPK::Fm::ams, OplGuiText::Fm::Op::Ams, tabOrder, std::nullopt);

        amsTo37[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->3.7Hz", .isReset = false, .isResized = false });
        amsTo37[i].setWantsKeyboardFocus(true);
        amsTo37[i].setExplicitFocusOrder(++tabOrder);
        amsTo37[i].onClick = [this, index = i] { ams[index].setValue(3.7, juce::sendNotification); };

        amsTo606[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->6.06Hz", .isReset = false, .isResized = false });
        amsTo606[i].setWantsKeyboardFocus(true);
        amsTo606[i].setExplicitFocusOrder(++tabOrder);
        amsTo606[i].onClick = [this, index = i] { ams[index].setValue(6.06, juce::sendNotification); };

        amd[i].setupComponent(opGroups[i].contentCanvas, paramPrefix + CPK::Fm::amd, OplGuiText::Fm::Op::Amd, tabOrder, std::nullopt);

        amdTo1[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->1dB", .isReset = false, .isResized = false });
        amdTo1[i].setWantsKeyboardFocus(true);
        amdTo1[i].setExplicitFocusOrder(++tabOrder);
        amdTo1[i].onClick = [this, index = i] { amd[index].setValue(1.0, juce::sendNotification); };

        amdTo12[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->1.2dB", .isReset = false, .isResized = false });
        amdTo12[i].setWantsKeyboardFocus(true);
        amdTo12[i].setExplicitFocusOrder(++tabOrder);
        amdTo12[i].onClick = [this, index = i] { amd[index].setValue(1.2, juce::sendNotification); };

        amdTo48[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->4.8dB", .isReset = false, .isResized = false });
        amdTo48[i].setWantsKeyboardFocus(true);
        amdTo48[i].setExplicitFocusOrder(++tabOrder);
        amdTo48[i].onClick = [this, index = i] { amd[index].setValue(4.8, juce::sendNotification); };

        vib[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::vib, .title = OplGuiText::Fm::Op::Vib, .isReset = true });
        vib[i].setWantsKeyboardFocus(true);
        vib[i].setExplicitFocusOrder(++tabOrder);

        pms[i].setupComponent(opGroups[i].contentCanvas, paramPrefix + CPK::Fm::pms, OplGuiText::Fm::Op::Pms, tabOrder, std::nullopt);

        pmsTo606[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->6.06Hz", .isReset = false, .isResized = false });
        pmsTo606[i].setWantsKeyboardFocus(true);
        pmsTo606[i].setExplicitFocusOrder(++tabOrder);
        pmsTo606[i].onClick = [this, index = i] { pms[index].setValue(6.06, juce::sendNotification); };

        pmsTo64[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->6.4Hz", .isReset = false, .isResized = false });
        pmsTo64[i].setWantsKeyboardFocus(true);
        pmsTo64[i].setExplicitFocusOrder(++tabOrder);
        pmsTo64[i].onClick = [this, index = i] { pms[index].setValue(6.4, juce::sendNotification); };

        pmd[i].setupComponent(opGroups[i].contentCanvas, paramPrefix + CPK::Fm::pmd, OplGuiText::Fm::Op::Pmd, tabOrder, std::nullopt);

        pmdTo7[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->7cent", .isReset = false, .isResized = false});
        pmdTo7[i].setWantsKeyboardFocus(true);
        pmdTo7[i].setExplicitFocusOrder(++tabOrder);
        pmdTo7[i].onClick = [this, index = i] { pmd[index].setValue(7.0, juce::sendNotification); };

        pmdTo137[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->13.7cent", .isReset = false, .isResized = false });
        pmdTo137[i].setWantsKeyboardFocus(true);
        pmdTo137[i].setExplicitFocusOrder(++tabOrder);
        pmdTo137[i].onClick = [this, index = i] { pmd[index].setValue(13.7, juce::sendNotification); };

        pmdTo14[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->14cent", .isReset = false, .isResized = false });
        pmdTo14[i].setWantsKeyboardFocus(true);
        pmdTo14[i].setExplicitFocusOrder(++tabOrder);
        pmdTo14[i].onClick = [this, index = i] { pmd[index].setValue(14.0, juce::sendNotification); };

		lfoSep[i].setupComponent(opGroups[i].contentCanvas);

        catMask[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::mask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::mask, .title = OplGuiText::Fm::Op::Mask, .isReset = true });
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

        setupGraph(i);
        updateOpGraph(i);
    }
}

void GuiOpl::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    auto mainArea = pageArea.removeFromLeft(OplGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(OplGuiValue::Group::Padding::width, OplGuiValue::Group::Padding::height);
    mmRect.removeFromTop(OplGuiValue::Group::TitlePaddingTop);

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

    mRect.removeFromTop(OplGuiValue::Category::paddingTop);

    auto graphArea = mRect.removeFromTop(90); // 描画領域確保
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

    unisonComponent.setCategoryVisible(ctx.audioProcessor.isSimpleShown(SimpleView::Unison));
    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    int opWidth = pageArea.getWidth() / 4;

    // --- B. Operators Section ---
    for (int i = 0; i < OplPrValue::ops; ++i)
    {
        auto opArea = pageArea.removeFromLeft(opWidth);
        opGroups[i].setBounds(opArea);

        auto iinnerRect = opArea.reduced(OplGuiValue::Fm::Op::Padding::width, OplGuiValue::Fm::Op::Padding::height);
        iinnerRect.removeFromTop(OplGuiValue::Group::TitlePaddingTop);

        // グラフ用の区画を確保
        layoutOpGraph(i, iinnerRect);
        updateOpGraph(i);

        // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
        // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
        opGroups[i].setViewportCustomBounds(iinnerRect.translated(-opArea.getX(), -opArea.getY()));

        // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
        juce::Rectangle<int> innerRect(0, 0, opGroups[i].viewport.getMaximumVisibleWidth(), 2000);

        updateRgDisplayAsOp(i, true);

        layoutOpAmpCat(i, innerRect);

        layoutOpOptionalCat(i, innerRect);

        layoutOpEgCat(i, innerRect);

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

        layoutOpLfoCat(i, innerRect);

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
void GuiOpl::applyMmlString(const juce::String& mml, int opIndex)
{
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMl,   [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMask, [&](int v) { mask[opIndex].setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },
        { mmlPrefixAm,   [&](int v) { am[opIndex].setToggleState(RegisterConverter::convertOplAm(v), juce::sendNotification); } },
        { mmlPrefixVib,   [&](int v) { vib[opIndex].setToggleState(RegisterConverter::convertOplVib(v), juce::sendNotification); } },
        { mmlPrefixVb,   [&](int v) { vib[opIndex].setToggleState(RegisterConverter::convertOplVib(v), juce::sendNotification); } },
        { mmlPrefixPm,   [&](int v) { vib[opIndex].setToggleState(RegisterConverter::convertOplVib(v), juce::sendNotification); } },
        { mmlPrefixEgType, [&](int v) { egType[opIndex].setToggleState(RegisterConverter::convertOplEgType(v), juce::sendNotification); } },
        { mmlPrefixEt,   [&](int v) { egType[opIndex].setToggleState(RegisterConverter::convertOplEgType(v), juce::sendNotification); } },
        { mmlPrefixKsr,   [&](int v) { ksr[opIndex].setToggleState(RegisterConverter::convertOplKsr(v), juce::sendNotification); } },
        { mmlPrefixKr,   [&](int v) { ksr[opIndex].setToggleState(RegisterConverter::convertOplKsr(v), juce::sendNotification); } },
        { mmlPrefixKsl,   [&](int v) { ksl[opIndex].setSelectedItemIndex(RegisterConverter::convertOplKsl(v), juce::sendNotification); } },
        { mmlPrefixKl,   [&](int v) { ksl[opIndex].setSelectedItemIndex(RegisterConverter::convertOplKsl(v), juce::sendNotification); } },

        // --- TL系 ---
        { mmlPrefixTl,   [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg63(v), juce::sendNotification); }},
        { mmlPrefixO,    [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg63(v), juce::sendNotification); }},
        { mmlPrefixOl,   [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg63(v), juce::sendNotification); }},

        // --- エンベロープ系 ---
        { mmlPrefixAr,   [&](int v) { rgAr[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
        { mmlPrefixDr,   [&](int v) { rgDr[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
        { mmlPrefixSl,   [&](int v) { rgSl[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification); }},
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

void GuiOpl::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    mul[idx].setEnabledWithLabel(enable);
    ksr[idx].setEnabled(enable);
    ksl[idx].setEnabledWithLabel(enable);
    egType[idx].setEnabled(enable);
    catOptional[idx].setEnabled(enable);
    xof[idx].setEnabled(enable);
    kor[idx].setEnabled(enable);
    bypass[idx].setEnabled(enable);
    eg[idx].setEnabledWithLabel(enable);
    catShape[idx].setEnabled(enable);
    catLfo[idx].setEnabled(enable);
    vib[idx].setEnabled(enable);
    pms[idx].setEnabledWithLabel(enable);
    pmsTo64[idx].setEnabled(enable);
    pmd[idx].setEnabledWithLabel(enable);
    pmdTo7[idx].setEnabled(enable);
    pmdTo14[idx].setEnabled(enable);
    am[idx].setEnabled(enable);
    ams[idx].setEnabledWithLabel(enable);
    amsTo37[idx].setEnabled(enable);
    amd[idx].setEnabledWithLabel(enable);
    amdTo1[idx].setEnabled(enable);
    amdTo48[idx].setEnabled(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    mmlSeparator[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
    pitchEnv[idx].setEnabled(enable);
    ssgSwEnv[idx].setEnabled(enable);
}

void GuiOpl::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > OplPrValue::Alg::max) return;

    // Coreのルーティング情報から FmAlgState を生成
    const auto& routing = OplCore::routings[algIndex];
    FmAlgState s;
    s.numOps = OplPrValue::ops;

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

    for (int i = 0; i < OplPrValue::ops; ++i)
    {
        // 配列を使わず、到達可能性から判定したフラグをセット
        updateOpEnable(i, activeOps[i]);
    }
}

void GuiOpl::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].setVisibleWithLabel(rgMode);
    rgDr[idx].setVisibleWithLabel(rgMode);
    rgSl[idx].setVisibleWithLabel(rgMode);
    rgRr[idx].setVisibleWithLabel(rgMode);
    rgTl[idx].setVisibleWithLabel(rgMode);
}

void GuiOpl::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

// ==============================================================================
// Keyboard Shortcut Logic
// ==============================================================================
bool GuiOpl::keyPressed(const juce::KeyPress& key)
{
    int opIndex = -1;
    int code = key.getKeyCode();
    juce::ModifierKeys metaKeys = key.getModifiers();

    // 通常の 1〜4キー、または テンキーの 1〜4 を判定
    if (code == '1' || code == juce::KeyPress::numberPad1) opIndex = 0;
    else if (code == '2' || code == juce::KeyPress::numberPad2) opIndex = 1;

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

void GuiOpl::copyFmParamsToString()
{
    int mask1 = FmMml::bool2Int(!mask[0].getToggleState());
    int mask2 = FmMml::bool2Int(!mask[1].getToggleState());
    int maskVal = FmMml::genMask2(mask1, mask2);

    auto formatCoreBasic = [this, maskVal]() {
        return juce::String::formatted(
            u8"    %1d, %1d,  %2d\n",
            algSelector.getSelectedId() - 1, // ALG
            (int)feedbackSlider.getValue(),  // FB
            maskVal);                        // MASK
        };
    auto formatOpBasic = [this](int index) {
        // ' MUL AR DR SL RR  TL KSR KSL AM VIB WS EGTYPE PMS PMD AMS AMD
        return juce::String::formatted(
            u8"  %3d, %2d, %2d, %2d, %2d, %3d,  %1d,  %1d, %1d,  %1d, %1d,     %1d, %5.2f, %5.2f, %5.2f, %5.2f\n",
            (int)this->mul[index].getSelectedId() - 1,             // MUL
            (int)this->rgAr[index].getValue(),                     // AR
            (int)this->rgDr[index].getValue(),                     // DR
            (int)this->rgSl[index].getValue(),                     // SL
            (int)this->rgRr[index].getValue(),                     // RR
            (int)this->rgTl[index].getValue(),                     // TL
            FmMml::bool2Int(this->ksr[index].getToggleState()),    // KSR
            this->ksl[0].getSelectedId() - 1,                      // KSL
            FmMml::bool2Int(this->am[index].getToggleState()),     // AM
            FmMml::bool2Int(this->vib[index].getToggleState()),    // VIB
            this->eg[index].getSelectedId() - 1,                   // WS
            FmMml::bool2Int(this->egType[index].getToggleState()), // EGTYPE
            this->pms[index].getValue(),                           // PMS
            this->pmd[index].getValue(),                           // PMD
            this->ams[index].getValue(),                           // AMS
            this->amd[index].getValue()                            // AMD
        );
        };
    auto formatOpsBasic = [this, formatOpBasic]() {
        return formatOpBasic(0) + formatOpBasic(1);
        };
    auto formatOpExt = [this](int index) {
        // ' MUL AR DR SL RR TL KSR KSL
        return juce::String::formatted(
            u8"mul%d ar%d dr%d rr%d sl%d tl%d ksr%d ksl%d\n",
            (int)this->mul[index].getSelectedId() - 1,
            (int)this->rgAr[index].getValue(),
            (int)this->rgDr[index].getValue(),
            (int)this->rgSl[index].getValue(),
            (int)this->rgRr[index].getValue(),
            (int)this->rgTl[index].getValue(),
            FmMml::bool2Int(this->ksr[index].getToggleState()),
            this->ksl[index].getSelectedId() - 1
        );
        };
    auto formatOpsExt = [this, formatOpExt]() {
        return formatOpExt(0) + formatOpExt(1);
        };

    juce::String mml = juce::String("[OPL]\n")
        + FmMml::basicMmlHeader
        + juce::String(u8"' ALG FB MASK\n")
        + formatCoreBasic()
        + juce::String(u8"' MUL  AR  DR  SL  RR   TL KSR KSL AM VIB WS EGTYPE    PMS    PMD    AMS    AMD\n")
        + formatOpsBasic()
        + juce::String(u8"\n")
        + FmMml::extMmlHeader
        + formatOpsExt();

    juce::SystemClipboard::copyTextToClipboard(mml);
}

void GuiOpl::copyFmParamsToObject()
{

}

void GuiOpl::pasteFmParamsFromObject()
{

}

void GuiOpl::initParams()
{
    this->ctx.audioProcessor.initParams("OPL_");
}

void GuiOpl::layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect) {
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

void GuiOpl::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityComponent.layoutComponent(rect);
}

void GuiOpl::layoutUtilityCat(Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    initLfoToOplBtn.setVisible(visible);
    initLfoToOpllBtn.setVisible(visible);
    uSep002.setVisible(visible);
    copyOpParamToOpl3Btn.setVisible(visible);
    copyOpParamToOpl312Btn.setVisible(visible);
    copyOpParamToOpl334Btn.setVisible(visible);
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
    imOpl3OpChParam.setVisible(visible);
    targerOpSlider.setVisibleWithLabel(visible);
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
    imOpl3ChParam.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        uSep001.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &initLfoToOplBtn });
        layoutMain({ .mainRect = rect, .component = &initLfoToOpllBtn });

        uSep002.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyOpParamToOpl3Btn });
        layoutMain({ .mainRect = rect, .component = &copyOpParamToOpl312Btn });
        layoutMain({ .mainRect = rect, .component = &copyOpParamToOpl334Btn });

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
        imOpl3OpChParam.layoutComponent(rect);
        rect.removeFromTop(4);
        layoutMain({ .mainRect = rect, .label = &targerOpSlider.label, .component = &targerOpSlider });

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
        imOpl3ChParam.layoutComponent(rect);
        rect.removeFromTop(4);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpl::layoutOpLfoCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catLfo[opIndex] });

    bool visible = catLfo[opIndex].isDetailVisible();

    am[opIndex].setVisible(visible);
    ams[opIndex].setVisibleWithLabel(visible);
    amsTo37[opIndex].setVisible(visible && ams[opIndex].isVisibleNudge());
    amsTo606[opIndex].setVisible(visible && ams[opIndex].isVisibleNudge());
    amd[opIndex].setVisibleWithLabel(visible);
    amdTo1[opIndex].setVisible(visible && amd[opIndex].isVisibleNudge());
    amdTo12[opIndex].setVisible(visible && amd[opIndex].isVisibleNudge());
    amdTo48[opIndex].setVisible(visible && amd[opIndex].isVisibleNudge());
	lfoSep[opIndex].setVisible(visible);
    vib[opIndex].setVisible(visible);
    pms[opIndex].setVisibleWithLabel(visible);
    pmsTo606[opIndex].setVisible(visible && pms[opIndex].isVisibleNudge());
    pmsTo64[opIndex].setVisible(visible && pms[opIndex].isVisibleNudge());
    pmd[opIndex].setVisibleWithLabel(visible);
    pmdTo137[opIndex].setVisible(visible && pmd[opIndex].isVisibleNudge());
    pmdTo14[opIndex].setVisible(visible && pmd[opIndex].isVisibleNudge());
    pmdTo7[opIndex].setVisible(visible && pmd[opIndex].isVisibleNudge());

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &am[opIndex] });
        ams[opIndex].layoutComponentRow(rect);
        if (ams[opIndex].isVisibleNudge()) layoutRowTwoComps({ .rect = rect, .comp1 = &amsTo37[opIndex], .comp2 = &amsTo606[opIndex] });
        amd[opIndex].layoutComponentRow(rect);
        if (amd[opIndex].isVisibleNudge()) layoutRowThreeComps({ .rect = rect, .comp1 = &amdTo1[opIndex], .comp2 = &amdTo12[opIndex], .comp3 = &amdTo48[opIndex] });
		lfoSep[opIndex].layoutComponent(rect);
        layoutRow({ .rowRect = rect, .component = &vib[opIndex] });
        pms[opIndex].layoutComponentRow(rect);
        if (pms[opIndex].isVisibleNudge()) layoutRowTwoComps({ .rect = rect, .comp1 = &pmsTo606[opIndex], .comp2 = &pmsTo64[opIndex] });
        pmd[opIndex].layoutComponentRow(rect);
        if (pmd[opIndex].isVisibleNudge()) layoutRowThreeComps({ .rect = rect, .comp1 = &pmdTo7[opIndex], .comp2 = &pmdTo137[opIndex], .comp3 = &pmdTo14[opIndex] });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpl::layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &ksCat[opIndex] });

    bool visible = ksCat[opIndex].isDetailVisible();

    ksr[opIndex].setVisible(visible);
    ksl[opIndex].setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .component = &ksr[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ksl[opIndex].label, .component = &ksl[opIndex] });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpl::layoutOpDetCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catDet[opIndex] });

    bool visible = catDet[opIndex].isDetailVisible();

    mul[opIndex].setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &mul[opIndex].label, .component = &mul[opIndex] });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpl::layoutOpAmpCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catAmp[opIndex] });

    bool visible = catAmp[opIndex].isDetailVisible();

    rgAr[opIndex].setVisibleWithLabel(visible);
    rgDr[opIndex].setVisibleWithLabel(visible);
    rgSl[opIndex].setVisibleWithLabel(visible);
    rgRr[opIndex].setVisibleWithLabel(visible);
    rgTl[opIndex].setVisibleWithLabel(visible);
    egType[opIndex].setVisible(visible);
    sus[opIndex].setVisible(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &rgAr[opIndex].label, .component = &rgAr[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgDr[opIndex].label, .component = &rgDr[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgSl[opIndex].label, .component = &rgSl[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgRr[opIndex].label, .component = &rgRr[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgTl[opIndex].label, .component = &rgTl[opIndex] });
        layoutRow({ .rowRect = rect, .component = &egType[opIndex] });
        layoutRow({ .rowRect = rect, .component = &sus[opIndex] });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiOpl::layoutOpEgCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catShape[opIndex] });

    bool visible = catShape[opIndex].isDetailVisible();

    eg[opIndex].setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &eg[opIndex].label, .component = &eg[opIndex] });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

// ============================================================================
// チップ全体のエンベロープを見るグラフ
// ============================================================================
// オペレータごとのグラフと同じ作りで、対象がチップ全体の 3 つになる。
// 値が変わったときだけ描き直すので、常時の負荷は無い。
void GuiOpl::setupGlobalGraph()
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

void GuiOpl::layoutGlobalGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(OplGuiValue::MainGroup::Graph::height + NormalSeparator::getHeight());

    gGraphSeparator.layoutComponentBottom(mainArea);

    auto btnArea = mainArea.removeFromBottom(OplGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    gGraphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsg11.setBounds(btnArea.removeFromLeft(btnWidth));
    gGraphBtnSsgP11.setBounds(btnArea);

    gGraph.setBounds(mainArea);
}

void GuiOpl::updateGlobalGraph()
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

void GuiOpl::setGlobalGraphMode(GlobalGraphMode mode)
{
    currentGlobalGraphMode = mode;

    // ラジオボタン的な排他制御
    gGraphBtnAmp.setToggleState(mode == GlobalGraphMode::Amp, juce::dontSendNotification);
    gGraphBtnSsg11.setToggleState(mode == GlobalGraphMode::SsgSw11, juce::dontSendNotification);
    gGraphBtnSsgP11.setToggleState(mode == GlobalGraphMode::SsgSwP11, juce::dontSendNotification);

    updateGlobalGraph();
}

void GuiOpl::setupGraph(int opIndex)
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
    rgDr[opIndex].onValueChange = repaintGraph;
    rgSl[opIndex].onValueChange = repaintGraph;
    rgRr[opIndex].onValueChange = repaintGraph;
    rgTl[opIndex].onValueChange = repaintGraph;
    sus[opIndex].onStateChange = repaintGraph;

    pitchEnv[opIndex].setupGraph(repaintGraph);
    ssgSwEnv[opIndex].setupGraph(repaintGraph);
    ssgSwEnv11[opIndex].setupGraph(repaintGraph);
    ssgSwPEnv11[opIndex].setupGraph(repaintGraph);

    graphSeparator[opIndex].setupComponent(*this);
}

void GuiOpl::setGraphMode(int opIndex, GraphMode mode)
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

void GuiOpl::layoutOpGraph(int opIndex, juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(OplGuiValue::ParamGroup::Graph::height + NormalSeparator::getHeight());

    graphSeparator[opIndex].layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(OplGuiValue::ParamGroup::Graph::ButtonHeight);
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
void GuiOpl::updateOpGraph(int opIndex)
{
    GraphMode mode = currentGraphMode[opIndex];

    // カーブモードが有効かどうかを判定
    // カーブを使うかどうかは処理側が持っている。画面から引くと、
    // どのタブを開いても Curve タブまで一緒に組み上がってしまう。
    bool isCurveMode = ctx.audioProcessor.prCurve.getEnable();

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

        bool isSus = sus[opIndex].getToggleState();
        bool isXof = xof[opIndex].getToggleState();
        bool isKor = kor[opIndex].getToggleState();

        float arMax = (float)rgAr[opIndex].getMaximum();
        float drMax = (float)rgDr[opIndex].getMaximum();
        float slMax = (float)rgSl[opIndex].getMaximum();
        float rrMax = (float)rgRr[opIndex].getMaximum();
        float tlMax = (float)rgTl[opIndex].getMaximum();

        float arVal = (float)rgAr[opIndex].getValue();
        float drVal = (float)rgDr[opIndex].getValue();
        float slVal = (float)rgSl[opIndex].getValue();
        float rrVal = (float)rgRr[opIndex].getValue();
        float tlVal = (float)rgTl[opIndex].getValue();

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

        // 3. Sustain
        float releaseStartLevel = sl;
        float keyOnWidth = 60.0f;
        float sustainTotalWidth = 0.0f;

        phases.push_back({ .widthPx = keyOnWidth, .startLevel = sl * tlScale, .endLevel = sl * tlScale, .color = color, .phaseLineColor = juce::Colours::green });

        currentTotalWidth += keyOnWidth;
        float noteOffPositionX = currentTotalWidth;

        // 4. Release (通常時のみカーブを適用)
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
        else if (isSus) {
            phases.push_back({
                .widthPx = rateToWidth(5.0f, rrMax, 200.0f),
                .startLevel = releaseStartLevel * tlScale,
                .endLevel = 0.0f,
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

        opGraphs[opIndex].setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
    }
}

void GuiOpl::layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect) {
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

void GuiOpl::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiOpl::copyParams(CopyOpl& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpl::copyOpParams(int p, CopyOplOp& copyObj) {
    copyObj.detune.mul = mul[p].getSelectedId();
    copyObj.aAdsr.ar = rgAr[p].getValue();
    copyObj.aAdsr.dr = rgDr[p].getValue();
    copyObj.aAdsr.sl = rgSl[p].getValue();
    copyObj.aAdsr.rr = rgRr[p].getValue();
    copyObj.aAdsr.tl = rgTl[p].getValue();
    copyObj.aAdsr.ksr = ksr[p].getToggleState();
    copyObj.aAdsr.ksl = ksl[p].getSelectedId();
    copyObj.aAdsr.egType = egType[p].getToggleState();
    copyObj.aAdsr.bypass = bypass[p].getToggleState();
    copyObj.aAdsr.kor = kor[p].getToggleState();
    copyObj.aAdsr.xof = xof[p].getToggleState();
    copyObj.aAdsr.sus = sus[p].getToggleState();
    copyObj.waveSelect = eg[p].getSelectedId();

    copyObj.lfo.am = am[p].getToggleState();
    copyObj.lfo.amd = amd[p].getValue();
    copyObj.lfo.ams = ams[p].getValue();
    copyObj.lfo.pm = vib[p].getToggleState();
    copyObj.lfo.pmd = pmd[p].getValue();
    copyObj.lfo.pms = pms[p].getValue();

    copyObj.mask.mask = mask[p].getToggleState();

    pitchEnv[p].copyParams(copyObj.pAdsr);
    ssgSwEnv[p].copyParams(copyObj.aSsgSw);
}

void GuiOpl::pasteParams(CopyOpl& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpl::pasteOpParams(int p, CopyOplOp& copyObj) {
    mul[p].setSelectedId(copyObj.detune.mul, juce::sendNotification);
    rgAr[p].setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgDr[p].setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgSl[p].setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgRr[p].setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl[p].setValue(copyObj.aAdsr.tl, juce::sendNotification);
    ksr[p].setToggleState(copyObj.aAdsr.ksr, juce::sendNotification);
    ksl[p].setSelectedId(copyObj.aAdsr.ksl, juce::sendNotification);
    egType[p].setToggleState(copyObj.aAdsr.egType, juce::sendNotification);
    bypass[p].setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor[p].setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof[p].setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    sus[p].setToggleState(copyObj.aAdsr.sus, juce::sendNotification);

    am[p].setToggleState(copyObj.lfo.am, juce::sendNotification);
    amd[p].setValue(copyObj.lfo.amd, juce::sendNotification);
    ams[p].setValue(copyObj.lfo.ams, juce::sendNotification);
    vib[p].setToggleState(copyObj.lfo.pm, juce::sendNotification);
    pmd[p].setValue(copyObj.lfo.pmd, juce::sendNotification);
    pms[p].setValue(copyObj.lfo.pms, juce::sendNotification);

    mask[p].setToggleState(copyObj.mask.mask, juce::sendNotification);

    pitchEnv[p].pasteParams(copyObj.pAdsr);
    ssgSwEnv[p].pasteParams(copyObj.aSsgSw);
}

void GuiOpl::importLfoParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultLfoParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importLfoParamFile, defaultDir, Io::ExtensionGlob::OplLfoParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, opIndex](const juce::FileChooser& fc) {
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

                vib[opIndex].setToggleState(reader->getBool("vib", vib[opIndex].getToggleState()), juce::sendNotification);
                pms[opIndex].setValue(reader->getFloat("pms", (float)pms[opIndex].getValue()), juce::sendNotification);
                pmd[opIndex].setValue(reader->getFloat("pmd", (float)pmd[opIndex].getValue()), juce::sendNotification);
                am[opIndex].setToggleState(reader->getBool("am", am[opIndex].getToggleState()), juce::sendNotification);
                ams[opIndex].setValue(reader->getFloat("ams", (float)ams[opIndex].getValue()), juce::sendNotification);
                amd[opIndex].setValue(reader->getFloat("amd", (float)amd[opIndex].getValue()), juce::sendNotification);
            }
        });
}

void GuiOpl::exportLfoParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultLfoParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportLfoParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::OplLfoParam)), Io::saveGlob(Io::Extension::OplLfoParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(oplLfoFormat);
                writeLfoParams(opIndex, writer);

                writer.writeTo(file);
            }
        });
}

void GuiOpl::importPitchEnvParam(int opIndex) {
    pitchEnv[opIndex].importParams();
}

void GuiOpl::exportPitchEnvParam(int opIndex) {
    pitchEnv[opIndex].exportParams();
}

void GuiOpl::importSsgSwEnvParam(int opIndex) {
    ssgSwEnv[opIndex].importParams();
}

void GuiOpl::exportSsgSwEnvParam(int opIndex) {
    ssgSwEnv[opIndex].exportParams();
}

void GuiOpl::importQualityParam() {
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

void GuiOpl::exportQualityParam() {
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

void GuiOpl::importSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11[opIndex].importParams();
}

void GuiOpl::exportSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11[opIndex].exportParams();
}

void GuiOpl::importSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11[opIndex].importParams();
}

void GuiOpl::exportSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11[opIndex].exportParams();
}

void GuiOpl::importOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp[opIndex].importParams();
}

void GuiOpl::exportOpSsgHwPEnvParam(int opIndex) {
    ssgHwPEnvOp[opIndex].exportParams();
}

void GuiOpl::importOpWtAmpModParam(int opIndex) {
    wtAmpModOp[opIndex].importParams();
}

void GuiOpl::exportOpWtAmpModParam(int opIndex) {
    wtAmpModOp[opIndex].exportParams();
}

void GuiOpl::importOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp[opIndex].importParams();
}

void GuiOpl::exportOpSsgHwEnvParam(int opIndex) {
    ssgHwEnvOp[opIndex].exportParams();
}

void GuiOpl::importOpWtModParam(int opIndex) {
    wtModOp[opIndex].importParams();
}

void GuiOpl::exportOpWtModParam(int opIndex) {
    wtModOp[opIndex].exportParams();
}

void GuiOpl::importChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::oplParam);
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

                    Io::ParamWriter writer(oplFormat);

                    writeChParams(writer);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, oplFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
                // 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
                GuiRefresh::Batch batch;

                readChParams(*reader);
            }
        });
}

void GuiOpl::exportChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::oplParam)), Io::saveGlob(Io::Extension::oplParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(oplFormat);
                writeChParams(writer);

                writer.writeTo(file);
            }
        });
}

void GuiOpl::importOpChParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::oplOpParam);
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

                    Io::ParamWriter writer(oplOpFormat);

                    writeOpChFileParams(opIndex, writer);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, oplOpFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す
                GuiRefresh::Batch batch;

                readOpParams(opIndex, *reader);
            }
        });
}

void GuiOpl::exportOpChParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::oplOpParam)), Io::saveGlob(Io::Extension::oplOpParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                                Io::ParamWriter writer(oplOpFormat);
                writeOpChFileParams(opIndex, writer);

                writer.writeTo(file);
            }
        });
}


// チャンネル 1 つぶん。
void GuiOpl::readChParams(const Io::ParamReader& reader) {
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

				for (int i = 0; i < OplPrValue::ops; i++) {
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
void GuiOpl::readOpParams(int opIndex, const Io::ParamReader& r) {
    // Mul
    mul[opIndex].setSelectedId(r.getInt("mul", mul[opIndex].getSelectedId()), juce::sendNotification);

    // Env
    rgAr[opIndex].setValue(r.getFloat("ar", (float)rgAr[opIndex].getValue()), juce::sendNotification);
    rgDr[opIndex].setValue(r.getFloat("dr", (float)rgDr[opIndex].getValue()), juce::sendNotification);
    rgSl[opIndex].setValue(r.getFloat("sl", (float)rgSl[opIndex].getValue()), juce::sendNotification);
    rgRr[opIndex].setValue(r.getFloat("rr", (float)rgRr[opIndex].getValue()), juce::sendNotification);
    rgTl[opIndex].setValue(r.getFloat("tl", (float)rgTl[opIndex].getValue()), juce::sendNotification);

    // Key Scale & EG Type
    ksr[opIndex].setToggleState(r.getBool("ksr", ksr[opIndex].getToggleState()), juce::sendNotification);
    ksl[opIndex].setSelectedId(r.getInt("ksl", ksl[opIndex].getSelectedId()), juce::sendNotification);
    egType[opIndex].setToggleState(r.getBool("egType", egType[opIndex].getToggleState()), juce::sendNotification);

    // Optional / Mask
    bypass[opIndex].setToggleState(r.getBool("bypass", bypass[opIndex].getToggleState()), juce::sendNotification);
    sus[opIndex].setToggleState(r.getBool("sus", sus[opIndex].getToggleState()), juce::sendNotification);
    kor[opIndex].setToggleState(r.getBool("kor", kor[opIndex].getToggleState()), juce::sendNotification);
    xof[opIndex].setToggleState(r.getBool("xof", xof[opIndex].getToggleState()), juce::sendNotification);
    mask[opIndex].setToggleState(r.getBool("mask", mask[opIndex].getToggleState()), juce::sendNotification);

    // Wave Shape
    eg[opIndex].setSelectedId(r.getInt("eg", eg[opIndex].getSelectedId()), juce::sendNotification);

    // LFO (AM / VIB)
    am[opIndex].setToggleState(r.getBool("am", am[opIndex].getToggleState()), juce::sendNotification);
    amd[opIndex].setValue(r.getFloat("amd", (float)amd[opIndex].getValue()), juce::sendNotification);
    ams[opIndex].setValue(r.getFloat("ams", (float)ams[opIndex].getValue()), juce::sendNotification);
    vib[opIndex].setToggleState(r.getBool("vib", vib[opIndex].getToggleState()), juce::sendNotification);
    pmd[opIndex].setValue(r.getFloat("pmd", (float)pmd[opIndex].getValue()), juce::sendNotification);
    pms[opIndex].setValue(r.getFloat("pms", (float)pms[opIndex].getValue()), juce::sendNotification);

    // Components
    pitchEnv[opIndex].readParams(r, "pitchEnv");
    ssgSwEnv[opIndex].readParams(r, "ssgSwEnv");
    ssgSwEnv11[opIndex].readParams(r, "ssgSwEnv11");
    ssgSwPEnv11[opIndex].readParams(r, "ssgSwPEnv11");
    ssgHwPEnvOp[opIndex].readParams(r, "ssgHwPEnv");
    wtAmpModOp[opIndex].readParams(r, "wtAmpMod");
    ssgHwEnvOp[opIndex].readParams(r, "ssgHwEnv");
    wtModOp[opIndex].readParams(r, "wtMod");
}

void GuiOpl::writeOpParams(int opIndex, Io::ParamWriter& w) {
    // Mul
    w.set("mul", mul[opIndex].getSelectedId());

    // Env
    w.set("ar", (float)rgAr[opIndex].getValue());
    w.set("dr", (float)rgDr[opIndex].getValue());
    w.set("sl", (float)rgSl[opIndex].getValue());
    w.set("rr", (float)rgRr[opIndex].getValue());
    w.set("tl", (float)rgTl[opIndex].getValue());

    // Key Scale & EG Type
    w.set("ksr", ksr[opIndex].getToggleState());
    w.set("ksl", ksl[opIndex].getSelectedId());
    w.set("egType", egType[opIndex].getToggleState());

    // Optional / Mask
    w.set("bypass", bypass[opIndex].getToggleState());
    w.set("sus", sus[opIndex].getToggleState());
    w.set("kor", kor[opIndex].getToggleState());
    w.set("xof", xof[opIndex].getToggleState());
    w.set("mask", mask[opIndex].getToggleState());

    // Wave Shape
    w.set("eg", eg[opIndex].getSelectedId());

    // LFO (AM / VIB)
    w.set("am", am[opIndex].getToggleState());
    w.set("amd", (float)amd[opIndex].getValue());
    w.set("ams", (float)ams[opIndex].getValue());
    w.set("vib", vib[opIndex].getToggleState());
    w.set("pmd", (float)pmd[opIndex].getValue());
    w.set("pms", (float)pms[opIndex].getValue());

    // Components
    pitchEnv[opIndex].writeParams(w, "pitchEnv");
    ssgSwEnv[opIndex].writeParams(w, "ssgSwEnv");
    ssgSwEnv11[opIndex].writeParams(w, "ssgSwEnv11");
    ssgSwPEnv11[opIndex].writeParams(w, "ssgSwPEnv11");
    ssgHwPEnvOp[opIndex].writeParams(w, "ssgHwPEnv");
    wtAmpModOp[opIndex].writeParams(w, "wtAmpMod");
    ssgHwEnvOp[opIndex].writeParams(w, "ssgHwEnv");
    wtModOp[opIndex].writeParams(w, "wtMod");
}

void GuiOpl::importOpl3ChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::opl3Param);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                // 相手の音源のファイル。名前で引くので、こちらに無い項目は
                // 読み飛ばされ、こちらにしか無い項目は今の値のまま残る。
                auto reader = Io::ParamReader::open(file, opl3Format);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す
                GuiRefresh::Batch batch;

                readChParams(*reader);
            }
        });
}

void GuiOpl::importOpl3OpChParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::opl3OpParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                auto reader = Io::ParamReader::open(file, opl3OpFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す
                GuiRefresh::Batch batch;

                readOpParams(opIndex, *reader);
            }
        });
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。並び順を写し直すより確実で、
// 当時の互換の工夫もそのまま残る。
void GuiOpl::setImportingChParams(juce::StringArray& lines, int& index) {
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

	for (int i = 0; i < OplPrValue::ops; i++) {
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
void GuiOpl::writeChParams(Io::ParamWriter& writer) {
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

	            for (int i = 0; i < OplPrValue::ops; i++) {
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
void GuiOpl::getImportingOpParams(int opIndex, juce::StringArray& lines, int& index) {
    // Mul
    mul[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // Env
    rgAr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgDr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgSl[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgRr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgTl[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Key Scale & EG Type
    ksr[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    ksl[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    egType[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

    // Optional / Mask
    bypass[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    sus[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    kor[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    xof[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    mask[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

    // Wave Shape
    eg[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // LFO (AM / VIB)
    am[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    amd[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    ams[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    vib[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    pmd[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    pms[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Components
    pitchEnv[opIndex].setImportingParams(lines, index);
    ssgSwEnv[opIndex].setImportingParams(lines, index);
    ssgSwEnv11[opIndex].setImportingParams(lines, index);
    ssgSwPEnv11[opIndex].setImportingParams(lines, index);
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpl::setImportingQualityParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	            if (size < 2) return;

	qualityComponent.setBit(lines[0].getIntValue());
	qualityComponent.setRate(lines[1].getIntValue());
	        
}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpl::writeQualityParams(Io::ParamWriter& writer) {
	writer.set("bit", qualityComponent.getBit());
	writer.set("rate", qualityComponent.getRate());

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpl::setImportingOpChFileParams(int opIndex, juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	getImportingOpParams(opIndex, lines, index);
	        
}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpl::writeOpChFileParams(int opIndex, Io::ParamWriter& writer) {
	writeOpParams(opIndex, writer);

	
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiOpl::setImportingLfoParams(int opIndex, juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていることがある
    int size = lines.size();

    juce::ignoreUnused(index, size);

	if (size < 6) return;

	vib[opIndex].setToggleState(lines[0].getIntValue() == 1, juce::sendNotification);
	pms[opIndex].setValue(lines[1].getFloatValue(), juce::sendNotification);
	pmd[opIndex].setValue(lines[2].getFloatValue(), juce::sendNotification);
	am[opIndex].setToggleState(lines[3].getIntValue() == 1, juce::sendNotification);
	ams[opIndex].setValue(lines[4].getFloatValue(), juce::sendNotification);
	amd[opIndex].setValue(lines[5].getFloatValue(), juce::sendNotification);

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiOpl::writeLfoParams(int opIndex, Io::ParamWriter& writer) {
	writer.set("vib", vib[opIndex].getToggleState());
	writer.set("pms", (float)pms[opIndex].getValue());
	writer.set("pmd", (float)pmd[opIndex].getValue());
	writer.set("am", am[opIndex].getToggleState());
	writer.set("ams", (float)ams[opIndex].getValue());
	writer.set("amd", (float)amd[opIndex].getValue());

	
}

void GuiOpl::bypassHiddenCategories()
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

    for (int i = 0; i < OplPrValue::ops; ++i)
    {
        if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwAmpEnv)) ssgHwEnvOp[i].setCategoryBypassed(true);
        if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv)) ssgSwEnv[i].setCategoryBypassed(true);
        if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwAmpEnv11)) ssgSwEnv11[i].setCategoryBypassed(true);
        if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtAmpMod)) wtAmpModOp[i].setCategoryBypassed(true);
        if (!ctx.audioProcessor.isSimpleShown(SimpleView::PitchEnv)) pitchEnv[i].setCategoryBypassed(true);
        if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgHwPitchEnv)) ssgHwPEnvOp[i].setCategoryBypassed(true);
        if (!ctx.audioProcessor.isSimpleShown(SimpleView::SsgSwPitchEnv11)) ssgSwPEnv11[i].setCategoryBypassed(true);
        if (!ctx.audioProcessor.isSimpleShown(SimpleView::WtPitchMod)) wtModOp[i].setCategoryBypassed(true);
    }
}

void GuiOpl::openEnabledCategories()
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

    for (int i = 0; i < OplPrValue::ops; ++i)
    {
        if (ssgHwEnvOp[i].hasBypassSwitch() && !ssgHwEnvOp[i].isCategoryBypassed()) ssgHwEnvOp[i].setCategoryOpen(true);
        if (ssgSwEnv[i].hasBypassSwitch() && !ssgSwEnv[i].isCategoryBypassed()) ssgSwEnv[i].setCategoryOpen(true);
        if (ssgSwEnv11[i].hasBypassSwitch() && !ssgSwEnv11[i].isCategoryBypassed()) ssgSwEnv11[i].setCategoryOpen(true);
        if (wtAmpModOp[i].hasBypassSwitch() && !wtAmpModOp[i].isCategoryBypassed()) wtAmpModOp[i].setCategoryOpen(true);
        if (pitchEnv[i].hasBypassSwitch() && !pitchEnv[i].isCategoryBypassed()) pitchEnv[i].setCategoryOpen(true);
        if (ssgHwPEnvOp[i].hasBypassSwitch() && !ssgHwPEnvOp[i].isCategoryBypassed()) ssgHwPEnvOp[i].setCategoryOpen(true);
        if (ssgSwPEnv11[i].hasBypassSwitch() && !ssgSwPEnv11[i].isCategoryBypassed()) ssgSwPEnv11[i].setCategoryOpen(true);
        if (wtModOp[i].hasBypassSwitch() && !wtModOp[i].isCategoryBypassed()) wtModOp[i].setCategoryOpen(true);
    }
}

void GuiOpl::closeBypassedCategories()
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

    for (int i = 0; i < OplPrValue::ops; ++i)
    {
        if (ssgHwEnvOp[i].hasBypassSwitch() && ssgHwEnvOp[i].isCategoryBypassed()) ssgHwEnvOp[i].setCategoryOpen(false);
        if (ssgSwEnv[i].hasBypassSwitch() && ssgSwEnv[i].isCategoryBypassed()) ssgSwEnv[i].setCategoryOpen(false);
        if (ssgSwEnv11[i].hasBypassSwitch() && ssgSwEnv11[i].isCategoryBypassed()) ssgSwEnv11[i].setCategoryOpen(false);
        if (wtAmpModOp[i].hasBypassSwitch() && wtAmpModOp[i].isCategoryBypassed()) wtAmpModOp[i].setCategoryOpen(false);
        if (pitchEnv[i].hasBypassSwitch() && pitchEnv[i].isCategoryBypassed()) pitchEnv[i].setCategoryOpen(false);
        if (ssgHwPEnvOp[i].hasBypassSwitch() && ssgHwPEnvOp[i].isCategoryBypassed()) ssgHwPEnvOp[i].setCategoryOpen(false);
        if (ssgSwPEnv11[i].hasBypassSwitch() && ssgSwPEnv11[i].isCategoryBypassed()) ssgSwPEnv11[i].setCategoryOpen(false);
        if (wtModOp[i].hasBypassSwitch() && wtModOp[i].isCategoryBypassed()) wtModOp[i].setCategoryOpen(false);
    }
}
