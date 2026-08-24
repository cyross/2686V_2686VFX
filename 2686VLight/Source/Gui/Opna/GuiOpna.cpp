#include <vector>

#include "./GuiOpna.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Core/Const/ConstFileValues.h"
#include "../../Core/Processor/ProcessorKeys.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Processor/Opna/ProcessorOpnaKeys.h"
#include "../../Processor/Opna/ProcessorOpnaValues.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"
#include "../../Core/Const/ConstMmlKeys.h"
#include "../../Core/Const/ConstMmlValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Fm/FmRegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiOpnaValues.h"
#include "./GuiOpnaText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> opnaAlgItems = {
    {.name = "00: <OPNA-00>", .value = 1 },
    {.name = "01: <OPNA-01>", .value = 2 },
    {.name = "02: <OPNA-02>", .value = 3 },
    {.name = "03: <OPNA-03>", .value = 4 },
    {.name = "04: <OPNA-04>", .value = 5 },
    {.name = "05: <OPNA-05>", .value = 6 },
    {.name = "06: <OPNA-06>", .value = 7 },
    {.name = "07: <OPNA-07>", .value = 8 },
};

static std::vector<SelectItem> pmsItems = {
    {.name = "1: Pms 0", .value = 1 },
    {.name = "2: Pms 1", .value = 2 },
    {.name = "3: Pms 2", .value = 3 },
    {.name = "4: Pms 3", .value = 4 },
    {.name = "5: Pms 4", .value = 5 },
    {.name = "6: Pms 5", .value = 6 },
    {.name = "7: Pms 6", .value = 7 },
    {.name = "8: Pms 7", .value = 8 },
};

static std::vector<SelectItem> amsItems = {
    {.name = "1: Ams 0", .value = 1 },
    {.name = "2: Ams 1", .value = 2 },
    {.name = "3: Ams 2", .value = 3 },
    {.name = "4: Ams 3", .value = 4 },
};

static std::vector<SelectItem> ksItems = {
    {.name = "0 OFF",      .value = 1},
    {.name = "1 (Weak)",   .value = 2},
    {.name = "2 (Mid)",    .value = 3},
    {.name = "3 (Strong)", .value = 4}
};

static std::vector<SelectItem> opnaLfoShapeItems = {
    {.name = "0: Saw Up",   .value = 1 },
    {.name = "1: Square",   .value = 2 },
    {.name = "2: Triangle", .value = 3 },
    {.name = "3: Noise",    .value = 4 },
};

static std::vector<SelectItem> opnaSeItems = {
    {.name = "0: Normal",                      .value = 1 },
    {.name = "1: Saw Down",                    .value = 2 },
    {.name = "2: Saw Down & Hold",             .value = 3 },
    {.name = "3: Triangle",                    .value = 4 },
    {.name = "4: Alternative Saw Down & Hold", .value = 5 },
    {.name = "5: Saw Up",                      .value = 6 },
    {.name = "6: Saw Up & Hold",               .value = 7 },
    {.name = "7: Triangle Invert",             .value = 8 },
    {.name = "8: Alternative Saw Up & Hold",   .value = 9 },
};

static std::vector<SelectItem> ssgEnvItems = {
    {.name = "0: Saw Down",                    .value = 1 },
    {.name = "1: Saw Down & Hold",             .value = 2 },
    {.name = "2: Triangle",                    .value = 3 },
    {.name = "3: Alternative Saw Down & Hold", .value = 4 },
    {.name = "4: Saw Up",                      .value = 5 },
    {.name = "5: Saw Up & Hold",               .value = 6 },
    {.name = "6: Triangle Invert",             .value = 7 },
    {.name = "7: Alternative Saw Up & Hold",   .value = 8 },
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

static std::vector<SelectItem> lfoShapeItems = {
    {.name = "0: Saw Up",              .value = 1 },
    {.name = "1: Square",              .value = 2 },
    {.name = "2: Triangle",            .value = 3 },
    {.name = "3: Sample & Hold",       .value = 4 },
    {.name = "4: Saw Down & One Shot", .value = 5 },
    {.name = "5: Triangle & One Shot", .value = 6 },
};

static std::vector<SelectItem> lfoFreqsItems = {
    {.name = "0: 3.98Hz", .value = 1 },
    {.name = "1: 5.56Hz", .value = 2 },
    {.name = "2: 6.02Hz", .value = 3 },
    {.name = "3: 6.37Hz", .value = 4 },
    {.name = "4: 6.88Hz", .value = 5 },
    {.name = "5: 9.63Hz", .value = 6 },
    {.name = "6: 48.1Hz", .value = 7 },
    {.name = "7: 72.2Hz", .value = 8 }
};

static std::vector<SelectItem> hwPmsItems = {
    {.name = "0: 0cent",   .value = 1 },
    {.name = "1: +-3.4cent", .value = 2 },
    {.name = "2: +-6.7cent", .value = 3 },
    {.name = "3: +-10cent",  .value = 4 },
    {.name = "4: +-14cent",  .value = 5 },
    {.name = "5: +-20cent",  .value = 6 },
    {.name = "6: +-40cent",  .value = 7 },
    {.name = "7: +-80cent",  .value = 8 },
};

static std::vector<SelectItem> hwAmsItems = {
    {.name = "0: 0dB", .value = 1 },
    {.name = "1: 1.4dB", .value = 2 },
    {.name = "2: 5.9dB", .value = 3 },
    {.name = "3: 11.8dB", .value = 4 },
};

void GuiOpna::setup()
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

    const juce::String code = OpnaPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, OpnaGuiText::Group::mainGroup);

    presetName.setupComponent(*this, tabOrder, ctx.audioProcessor.presetName);

    levelComponent.setupComponent(mainGroup.contentCanvas, tabOrder, code);

    qualityComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    algFbCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpnaGuiText::Category::algFb });

    algSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::alg, .title = OpnaGuiText::Fm::alg, .items = opnaAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
    };

    mainGroup.contentCanvas.addAndMakeVisible(&algStaticGraphComp);

    algFbSep.setupComponent(mainGroup.contentCanvas);

    feedbackSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::Fm::fb, .title = OpnaGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    panCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpnaGuiText::Category::visiblePan, .invisibleTitle = OpnaGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::pan, .title = OpnaGuiText::Fm::pan, .isReset = true });
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    panToLBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = OpnaGuiText::Fm::Pan::l, .isReset = false });
    panToLBtn.setWantsKeyboardFocus(true);
    panToLBtn.setExplicitFocusOrder(++tabOrder);
    panToLBtn.onClick = [this]() {
        panSlider.setValue(-1, juce::sendNotification);
        };

    panToCBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = OpnaGuiText::Fm::Pan::c, .isReset = false });
    panToCBtn.setWantsKeyboardFocus(true);
    panToCBtn.setExplicitFocusOrder(++tabOrder);
    panToCBtn.onClick = [this]() {
        panSlider.setValue(0, juce::sendNotification);
        };

    panToRBtn.setup(GuiTextButton::Config{ .parent = mainGroup.contentCanvas, .id = "", .title = OpnaGuiText::Fm::Pan::r, .isReset = false });
    panToRBtn.setWantsKeyboardFocus(true);
    panToRBtn.setExplicitFocusOrder(++tabOrder);
    panToRBtn.onClick = [this]() {
        panSlider.setValue(1, juce::sendNotification);
        };

    lfoCat.setupSwCategory({ .parent = mainGroup.contentCanvas, .title = OpnaGuiText::Category::visibleN88Lfo, .invisibleTitle = OpnaGuiText::Category::invisibleN88Lfo, .enableChangeDetailVisible = true });

    lfoFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::freq, .title = OpnaGuiText::Fm::lfoSpeed, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoShapeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::shape, .title = OpnaGuiText::Fm::lfoShape, .items = lfoShapeItems, .isReset = true });
    lfoShapeSelector.setWantsKeyboardFocus(true);
    lfoShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::amSmoothRatio, .title = OpnaGuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::syncDelay, .title = OpnaGuiText::Fm::lfoSyncDelay, .isReset = true });
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

    lfoPmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::pm, .title = OpnaGuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::pmd, .title = OpnaGuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::pms, .title = OpnaGuiText::Fm::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::am, .title = OpnaGuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + CPK::N88Lfo::amd, .title = OpnaGuiText::Fm::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoSep1.setupComponent(mainGroup.contentCanvas);
    lfoSep2.setupComponent(mainGroup.contentCanvas);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = OpnaGuiText::Category::visibleUtil, .invisibleTitle = OpnaGuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnaGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelComponent.getLevel();

        ctx.editor.breadcastLevel(level);
        };

    uSep001.setupComponent(mainGroup.contentCanvas);

    copyHwLfoParamsBtn.setup({ .parent = mainGroup.contentCanvas, .title = "HW LFO Params -> OPs", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyHwLfoParamsBtn.setWantsKeyboardFocus(true);
    copyHwLfoParamsBtn.setExplicitFocusOrder(++tabOrder);
    copyHwLfoParamsBtn.onClick = [this] {
        int fromOpIndex = (int)copyHwLfoFromSlider.getValue() - 1;

        int freqsVal = freqs[fromOpIndex].getSelectedId();
        int syncDelayVal = (int)syncDelay[fromOpIndex].getValue();
        bool pmValue = pm[fromOpIndex].getToggleState();
        int pmsVal = pms[fromOpIndex].getSelectedId();
        bool amValue = am[fromOpIndex].getToggleState();
        int amsVal = ams[fromOpIndex].getSelectedId();

        for (int i = 0; i < OpnaPrValue::ops; i++) {
            if (i == fromOpIndex) {
                continue;
            }

            freqs[i].setSelectedId(freqsVal);
            syncDelay[i].setValue(syncDelayVal);
            pm[i].setToggleState(pmValue, juce::NotificationType::sendNotification);
            pms[i].setSelectedId(pmsVal);
            am[i].setToggleState(amValue, juce::NotificationType::sendNotification);
            ams[i].setSelectedId(amsVal);
        }
        };

    copyHwLfoFromSlider.setup({ .parent = mainGroup.contentCanvas, .title = "FROM", .isReset = false });
    copyHwLfoFromSlider.setRange(1.0, 4.0, 1.0);
    copyHwLfoFromSlider.setNumDecimalPlacesToDisplay(0);
    copyHwLfoFromSlider.setWantsKeyboardFocus(true);
    copyHwLfoFromSlider.setExplicitFocusOrder(++tabOrder);

    uSep002.setupComponent(mainGroup.contentCanvas);

    copyParamsToOpnBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Params -> OPN", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyParamsToOpnBtn.setWantsKeyboardFocus(true);
    copyParamsToOpnBtn.setExplicitFocusOrder(++tabOrder);
    copyParamsToOpnBtn.onClick = [this] {
        ctx.editor.copyOpnaParamsToOpn();
        };

    copyParamsToOpmBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Params -> OPM", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyParamsToOpmBtn.setWantsKeyboardFocus(true);
    copyParamsToOpmBtn.setExplicitFocusOrder(++tabOrder);
    copyParamsToOpmBtn.onClick = [this] {
        ctx.editor.copyOpnaParamsToOpm();
        };

    uSep003.setupComponent(mainGroup.contentCanvas);

    copyOpParamBtn.setup({ .parent = mainGroup.contentCanvas, .title = "Copy Op Params", .bgColor = juce::Colours::turquoise.darker(0.5f) });
    copyOpParamBtn.setWantsKeyboardFocus(true);
    copyOpParamBtn.setExplicitFocusOrder(++tabOrder);
    copyOpParamBtn.onClick = [this] {
        int from = copyOpFromSlider.getValue() - 1;
        int to = copyOpToSlider.getValue() - 1;

        ctx.editor.copyOpnaOpParams(from, to);
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

    ieHwLfo.setupComponentOp(mainGroup.contentCanvas, tabOrder, "HW LFO");
    ieHwLfo.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importHwLfoParam(opIndex); };
    ieHwLfo.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportHwLfoParam(opIndex); };

    ieOpPitchEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "HW LFO");
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

    targerOpSlider.setup({ .parent = mainGroup.contentCanvas, .title = "Op", .isReset = false });
    targerOpSlider.setRange(1.0, 4.0, 1.0);
    targerOpSlider.setNumDecimalPlacesToDisplay(0);
    targerOpSlider.setValue(1, juce::sendNotification);
    targerOpSlider.setWantsKeyboardFocus(true);
    targerOpSlider.setExplicitFocusOrder(++tabOrder);

    uSep005.setupComponent(mainGroup.contentCanvas);

    ieOpPitchEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "Pitch Env");
    ieOpPitchEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importPitchEnvParam(opIndex); };
    ieOpPitchEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportPitchEnvParam(opIndex); };

    ieOpSsgSwEnv.setupComponentOp(mainGroup.contentCanvas, tabOrder, "SSG SW Env");
    ieOpSsgSwEnv.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importSsgSwEnvParam(opIndex); };
    ieOpSsgSwEnv.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportSsgSwEnvParam(opIndex); };

    ieOpChParam.setupComponentOp(mainGroup.contentCanvas, tabOrder, "OP Params");
    ieOpChParam.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpChParam(opIndex); };
    ieOpChParam.onClickExport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; exportOpChParam(opIndex); };

    imOpnOpChParam.setupComponentOp(mainGroup.contentCanvas, tabOrder, "OPN OP Params");
    imOpnOpChParam.onClickImport = [this] { int opIndex = (int)targerOpSlider.getValue() - 1; importOpnOpChParam(opIndex); };

    ieLfo.setupComponent(mainGroup.contentCanvas, tabOrder, "N88 LFO");
    ieLfo.onClickImport = [this] { importLfoParam(); };
    ieLfo.onClickExport = [this] { exportLfoParam(); };

    ieUnison.setupComponent(mainGroup.contentCanvas, tabOrder, "Unison");
    ieUnison.onClickImport = [this] { importUnisonParam(); };
    ieUnison.onClickExport = [this] { exportUnisonParam(); };

    ieQuality.setupComponent(mainGroup.contentCanvas, tabOrder, "Quality");
    ieQuality.onClickImport = [this] { importQualityParam(); };
    ieQuality.onClickExport = [this] { exportQualityParam(); };

    ieChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "CH Params");
    ieChParam.onClickImport = [this] { importChParam(); };
    ieChParam.onClickExport = [this] { exportChParam(); };

    imOpnChParam.setupComponent(mainGroup.contentCanvas, tabOrder, "OPN CH Params");
    imOpnChParam.onClickImport = [this] { importOpnChParam(); };

    const juce::String opCode = code + CPK::op;

    for (int i = 0; i < OpnaPrValue::ops; ++i)
    {
        opGroups[i].setup(*this, OpnaGuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        catDet[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnaGuiText::Category::visibleDetune, .invisibleTitle = OpnaGuiText::Category::invisibleDetune, .enableChangeDetailVisible = true });
        catAmp[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnaGuiText::Category::visibleAmpEnv, .invisibleTitle = OpnaGuiText::Category::invisibleAmpEnv, .detailVisible = true, .enableChangeDetailVisible = true });

        mul[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::mul, .title = OpnaGuiText::Fm::Op::Mul, .items = multems, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);

        dt[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::dt, .title = OpnaGuiText::Fm::Op::Dt, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        dt[i].setWantsKeyboardFocus(true);
        dt[i].setExplicitFocusOrder(++tabOrder);

        rgAr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgAr, .title = OpnaGuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgDr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgDr, .title = OpnaGuiText::Fm::Op::Dr, .isReset = true });
        rgDr[i].setWantsKeyboardFocus(true);
        rgDr[i].setExplicitFocusOrder(++tabOrder);

        rgSl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgSl, .title = OpnaGuiText::Fm::Op::Sl, .isReset = true });
        rgSl[i].setWantsKeyboardFocus(true);
        rgSl[i].setExplicitFocusOrder(++tabOrder);

        rgSr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgSr, .title = OpnaGuiText::Fm::Op::Sr, .isReset = true });
        rgSr[i].setWantsKeyboardFocus(true);
        rgSr[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgRr, .title = OpnaGuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::rgTl, .title = OpnaGuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        ksCat[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnaGuiText::Category::visibleKs, .invisibleTitle = OpnaGuiText::Category::invisibleKs, .enableChangeDetailVisible = true });

        ks[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ks, .title = OpnaGuiText::Fm::Op::Ks, .items = ksItems, .isReset = true, .regType = RegisterType::FmKs });
        ks[i].setWantsKeyboardFocus(true);
        ks[i].setExplicitFocusOrder(++tabOrder);

        catOptional[i].setupSwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnaGuiText::Category::visibleOptional, .invisibleTitle = OpnaGuiText::Category::invisibleOptional, .enableChangeDetailVisible = true });

        xof[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::xof, .title = OpnaGuiText::Fm::Op::xof, .isReset = true });
        xof[i].setWantsKeyboardFocus(true);
        xof[i].setExplicitFocusOrder(++tabOrder);

        kor[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::kor, .title = OpnaGuiText::Fm::Op::kor, .isReset = true });
        kor[i].setWantsKeyboardFocus(true);
        kor[i].setExplicitFocusOrder(++tabOrder);

        bypass[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::bypass, .title = OpnaGuiText::Fm::Op::bypass, .isReset = true });
        bypass[i].setWantsKeyboardFocus(true);
        bypass[i].setExplicitFocusOrder(++tabOrder);

        pitchEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::PitchAdsr::enable, OpnaGuiText::PitchAdsr::enable, true);

        ssgSwEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv::enable, OpnaGuiText::SsgSwEnv::enable, true);

        ssgSwEnv11[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwEnv11::enable, OpnaGuiText::SsgSwEnv11::enable, true);

        ssgSwPEnv11[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, CPK::SsgSwPEnv11::enable, OpnaGuiText::SsgSwPEnv11::enable, true);

        catSsgEnv[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnaGuiText::Category::visibleSsgEnv, .invisibleTitle = OpnaGuiText::Category::invisibleSsgEnv, .enableChangeDetailVisible = true });

        se[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::se, .title = OpnaGuiText::Fm::Op::SEnv, .items = opnaSeItems, .isReset = true });
        se[i].setWantsKeyboardFocus(true);
        se[i].setExplicitFocusOrder(++tabOrder);

        seFreq[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::seFreq, .title = OpnaGuiText::Fm::Op::SFreq, .isReset = true });
        seFreq[i].setWantsKeyboardFocus(true);
        seFreq[i].setExplicitFocusOrder(++tabOrder);

        catLfo[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnaGuiText::Category::visibleHwLfo, .invisibleTitle = OpnaGuiText::Category::invisibleHwLfo, .enableChangeDetailVisible = true });

        freqs[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::freqs, .title = OpnaGuiText::Fm::Op::Freqs, .items = lfoFreqsItems, .isReset = true });
        freqs[i].setWantsKeyboardFocus(true);
        freqs[i].setExplicitFocusOrder(++tabOrder);

        syncDelay[i].setup({.parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::syncDelay, .title = OpnaGuiText::Fm::Op::SyncDelay, .isReset = true});
        syncDelay[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        syncDelay[i].setWantsKeyboardFocus(true);
        syncDelay[i].setExplicitFocusOrder(++tabOrder);

        syncDelayToZero[i].setup({ .parent = opGroups[i].contentCanvas, .title = "Async", .isReset = false, .isResized = false });
        syncDelayToZero[i].setWantsKeyboardFocus(true);
        syncDelayToZero[i].setExplicitFocusOrder(++tabOrder);
        syncDelayToZero[i].onClick = [this, i] {
            syncDelay[i].setValue(0.0f);
            };

        syncDelayToOne[i].setup({ .parent = opGroups[i].contentCanvas, .title = "Sync", .isReset = false, .isResized = false });
        syncDelayToOne[i].setWantsKeyboardFocus(true);
        syncDelayToOne[i].setExplicitFocusOrder(++tabOrder);
        syncDelayToOne[i].onClick = [this, i] {
            syncDelay[i].setValue(1.0f);
            };

        pm[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::vib, .title = OpnaGuiText::Fm::Op::PmEn, .isReset = true });
        pm[i].setWantsKeyboardFocus(true);
        pm[i].setExplicitFocusOrder(++tabOrder);

        pms[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::pms, .title = OpnaGuiText::Fm::Op::Pms, .items = hwPmsItems, .isReset = true });
        pms[i].setWantsKeyboardFocus(true);
        pms[i].setExplicitFocusOrder(++tabOrder);

        am[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::am, .title = OpnaGuiText::Fm::Op::AmEn, .isReset = true });
        am[i].setWantsKeyboardFocus(true);
        am[i].setExplicitFocusOrder(++tabOrder);

        ams[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::ams, .title = OpnaGuiText::Fm::Op::Ams, .items = hwAmsItems, .isReset = true });
        ams[i].setWantsKeyboardFocus(true);
        ams[i].setExplicitFocusOrder(++tabOrder);

        catN88Lfo[i].setupSwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnaGuiText::Category::visibleN88Lfo, .invisibleTitle = OpnaGuiText::Category::invisibleN88Lfo, .enableChangeDetailVisible = true });

        n88Ams[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::N88Lfo::ams, .title = OpnaGuiText::Fm::Op::Ams, .isReset = true });
        n88Ams[i].setWantsKeyboardFocus(true);
        n88Ams[i].setExplicitFocusOrder(++tabOrder);

        fix[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, OpnaGuiText::Fm::Op::Opzx7FreqTo440, 440, true);

        catMask[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnaGuiText::Category::visibleMask, .invisibleTitle = OpnaGuiText::Category::invisibleMask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + CPK::Fm::mask, .title = OpnaGuiText::Fm::Op::Mask, .isReset = true });
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

void GuiOpna::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(OpnaGuiValue::MainGroup::width);
    mainArea.removeFromBottom(40);
    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(OpnaGuiValue::Group::Padding::width, OpnaGuiValue::Group::Padding::height);

    mmRect.removeFromTop(OpnaGuiValue::Group::TitlePaddingTop);

    presetName.layoutComponent(mmRect);

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.viewport.getMaximumVisibleWidth(), 2000);

    levelComponent.layoutComponent(mRect);

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });

    mRect.removeFromTop(OpnaGuiValue::Category::paddingTop);

    auto graphArea = mRect.removeFromTop(150); // 描画領域確保
    algStaticGraphComp.setBounds(graphArea.reduced(10));

    algFbSep.layoutComponent(mRect);

    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    layoutPanCat(mRect);

    layoutN88LfoCat(mRect);

    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    int opWidth = pageArea.getWidth() / 4;

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < OpnaPrValue::ops; ++i)
    {
        auto opArea = pageArea.removeFromLeft(opWidth);

        opGroups[i].setBounds(opArea);

        auto iinnerRect = opArea.reduced(OpnaGuiValue::Fm::Op::Padding::width, OpnaGuiValue::Fm::Op::Padding::height);
        iinnerRect.removeFromTop(OpnaGuiValue::Group::TitlePaddingTop);

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

        layoutOpSsgEnvelopeCat(i, innerRect);

        ssgSwEnv[i].layoutComponentRow(innerRect);

        ssgSwEnv11[i].layoutComponentRow(innerRect);

        pitchEnv[i].layoutComponentRow(innerRect);

        ssgSwPEnv11[i].layoutComponentRow(innerRect);

        layoutOpHwLfoCat(i, innerRect);

        layoutOpN88LfoCat(i, innerRect);

        layoutOpDetCat(i, innerRect);

        layoutOpKsCat(i, innerRect);

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
void GuiOpna::applyMmlString(const juce::String& mml, int opIndex)
{
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixMl,   [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertOplMul(v), juce::sendNotification); } },
        { mmlPrefixDt,   [&](int v) { dt[opIndex].setSelectedItemIndex(RegisterConverter::convertMmlDtToReg(v), juce::sendNotification); } },
        { mmlPrefixKs,   [&](int v) { ks[opIndex].setSelectedItemIndex(RegisterConverter::convertFmKs(v), juce::sendNotification); } },
        { mmlPrefixMask, [&](int v) { mask[opIndex].setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },

        // --- TL系 (RGモードで分岐) ---
        { mmlPrefixTl,   [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixO,    [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},
        { mmlPrefixOl,   [&](int v) { rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification); }},

        // --- エンベロープ系 (RGモードで分岐) ---
        { mmlPrefixAr,   [&](int v) { rgAr[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixDr,   [&](int v) { rgDr[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
        { mmlPrefixSr,   [&](int v) { rgSr[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification); }},
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

void GuiOpna::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    mul[idx].setEnabledWithLabel(enable);
    dt[idx].setEnabledWithLabel(enable);
    ks[idx].setEnabledWithLabel(enable);
    catOptional[idx].setEnabled(enable);
    xof[idx].setEnabled(enable);
    kor[idx].setEnabled(enable);
    bypass[idx].setEnabled(enable);
    catSsgEnv[idx].setEnabled(enable);
    se[idx].setEnabledWithLabel(enable);
    seFreq[idx].setEnabledWithLabel(enable);
    fix[idx].setEnables(enable);
    catLfo[idx].setEnabled(enable);
    pm[idx].setEnabled(enable);
    pms[idx].setEnabledWithLabel(enable);
    am[idx].setEnabled(enable);
    ams[idx].setEnabledWithLabel(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    mmlSeparator[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
    pitchEnv[idx].setEnabled(enable);
    ssgSwEnv[idx].setEnabled(enable);
}

void GuiOpna::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].setVisibleWithLabel(rgMode);
    rgDr[idx].setVisibleWithLabel(rgMode);
    rgSl[idx].setVisibleWithLabel(rgMode);
    rgSr[idx].setVisibleWithLabel(rgMode);
    rgRr[idx].setVisibleWithLabel(rgMode);
    rgTl[idx].setVisibleWithLabel(rgMode);
}

void GuiOpna::updatePresetName(const juce::String& name)
{
    presetName.updatePresetName(name);
}

void GuiOpna::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > OpnaPrValue::Alg::max) return;

    // Coreのルーティング情報から FmAlgState を生成
    const auto& routing = OpnaCore::routings[algIndex];
    FmAlgState s;
    s.numOps = OpnaPrValue::ops;

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

    for (int i = 0; i < OpnaPrValue::ops; ++i)
    {
        // 配列を使わず、到達可能性から判定したフラグをセット
        updateOpEnable(i, activeOps[i]);
    }
}

// ==============================================================================
// Keyboard Shortcut Logic
// ==============================================================================
bool GuiOpna::keyPressed(const juce::KeyPress& key)
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

void GuiOpna::copyFmParamsToString()
{
    int mask1 = FmMml::bool2Int(!mask[0].getToggleState());
    int mask2 = FmMml::bool2Int(!mask[1].getToggleState());
    int mask3 = FmMml::bool2Int(!mask[2].getToggleState());
    int mask4 = FmMml::bool2Int(!mask[3].getToggleState());
    int maskVal = FmMml::genMask4(mask1, mask2, mask2, mask3);

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
    auto formatOpBasic = [this](int index) {
        return juce::String::formatted(
            // ' MUL    DT   AR   DR   SL   RR   SR   TL   KS  AMS
            u8"  %2d, %+1d, %2d, %2d, %2d, %2d, %2d, %3d, %1d, %+d\n",
            (int)this->mul[index].getSelectedId() - 1,          // MUL
            FmMml::int2dt(this->dt[index].getSelectedId() - 1), // DT
            (int)this->rgAr[index].getValue(),                  // AR
            (int)this->rgDr[index].getValue(),                  // DR
            (int)this->rgSl[index].getValue(),                  // SL
            (int)this->rgSr[index].getValue(),                  // SR
            (int)this->rgRr[index].getValue(),                  // RR
            (int)this->rgTl[index].getValue(),                  // TL
            this->ks[index].getSelectedId() - 1,                // KS
            (int)this->n88Ams[index].getValue()                 // AMS
        );
        };
    auto formatOpsBasic = [this, formatOpBasic]() {
        return formatOpBasic(0) + formatOpBasic(1) + formatOpBasic(2) + formatOpBasic(3);
        };
    auto formatOpExt = [this](int index) {
        return juce::String::formatted(
            // ' MUL    DT   AR   DR   SL   SR   RR   TL   KS
            u8"MUL%d DT%+d AR%d DR%d SL%d SR%d RR%d TL%d KS%d\n",
            (int)this->mul[index].getSelectedId() - 1,
            FmMml::int2dt(this->dt[index].getSelectedId() - 1),
            (int)this->rgAr[index].getValue(),
            (int)this->rgDr[index].getValue(),
            (int)this->rgSl[index].getValue(),
            (int)this->rgSr[index].getValue(),
            (int)this->rgRr[index].getValue(),
            (int)this->rgTl[index].getValue(),
            this->ks[index].getSelectedId() - 1
        );
        };
    auto formatOpsExt = [this, formatOpExt]() {
        return formatOpExt(0) + formatOpExt(1) + formatOpExt(2) + formatOpExt(3);
        };

    juce::String mml = juce::String("[OPNA]\n")
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

void GuiOpna::copyFmParamsToObject()
{

}

void GuiOpna::pasteFmParamsFromObject()
{

}

void GuiOpna::initParams()
{
    this->ctx.audioProcessor.initParams("OPNA_");
}

void GuiOpna::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    copyParamsToOpnBtn.setVisible(visible);
    copyParamsToOpmBtn.setVisible(visible);
    uSep002.setVisible(visible);
    copyHwLfoParamsBtn.setVisible(visible);
    copyHwLfoFromSlider.setVisibleWithLabel(visible);
    uSep003.setVisible(visible);
    copyOpParamBtn.setVisible(visible);
    copyOpFromSlider.setVisibleWithLabel(visible);
    copyOpToSlider.setVisibleWithLabel(visible);
    uSep004.setVisible(visible);
    ieHwLfo.setVisible(visible);
    ieOpPitchEnv.setVisible(visible);
    ieOpSsgSwEnv.setVisible(visible);
    ieOpSsgSwEnv11.setVisible(visible);
    ieOpSsgSwPEnv11.setVisible(visible);
    ieOpChParam.setVisible(visible);
    imOpnOpChParam.setVisible(visible);
    targerOpSlider.setVisibleWithLabel(visible);
    uSep005.setVisible(visible);
    ieLfo.setVisible(visible);
    ieUnison.setVisible(visible);
    ieQuality.setVisible(visible);
    ieChParam.setVisible(visible);
    imOpnChParam.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        uSep001.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyHwLfoParamsBtn });
        layoutMain({ .mainRect = rect, .label = &copyHwLfoFromSlider.label, .component = &copyHwLfoFromSlider });

        uSep002.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyParamsToOpnBtn });
        layoutMain({ .mainRect = rect, .component = &copyParamsToOpmBtn });

        uSep003.layoutComponent(rect);

        layoutMain({ .mainRect = rect, .component = &copyOpParamBtn });
        layoutMain({ .mainRect = rect, .label = &copyOpFromSlider.label, .component = &copyOpFromSlider });
        layoutMain({ .mainRect = rect, .label = &copyOpToSlider.label, .component = &copyOpToSlider });

        uSep004.layoutComponent(rect);

        ieHwLfo.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpPitchEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwEnv.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwEnv11.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpSsgSwPEnv11.layoutComponent(rect);
        rect.removeFromTop(4);
        ieOpChParam.layoutComponent(rect);
        rect.removeFromTop(4);
        imOpnOpChParam.layoutComponent(rect);
        rect.removeFromTop(4);
        layoutMain({ .mainRect = rect, .label = &targerOpSlider.label, .component = &targerOpSlider });

        uSep005.layoutComponent(rect);

        ieLfo.layoutComponent(rect);
        rect.removeFromTop(4);
        ieUnison.layoutComponent(rect);
        rect.removeFromTop(4);
        ieQuality.layoutComponent(rect);
        rect.removeFromTop(4);
        ieChParam.layoutComponent(rect);
        rect.removeFromTop(4);
        imOpnChParam.layoutComponent(rect);
    }
}

void GuiOpna::layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask[opIndex] });

    bool visibleMask = catMask[opIndex].isDetailVisible();

    mask[opIndex].setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask[opIndex] });
    }
}

void GuiOpna::layoutQualityCat(juce::Rectangle<int>& rect) {
    qualityComponent.layoutComponent(rect);
}

void GuiOpna::layoutPanCat(juce::Rectangle<int>& rect)
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
    }
}

void GuiOpna::layoutN88LfoCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &lfoCat });

    bool visible = lfoCat.isDetailVisible();

    lfoFreqSlider.setVisibleWithLabel(visible);
    lfoShapeSelector.setVisibleWithLabel(visible);
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
        layoutMain({ .mainRect = rect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
        layoutMain({ .mainRect = rect, .label = &lfoPmsSlider.label, .component = &lfoPmsSlider });
        lfoSep2.layoutComponent(rect);
        layoutMain({ .mainRect = rect, .component = &lfoAmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });
    }
}

void GuiOpna::layoutOpSsgEnvelopeCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catSsgEnv[opIndex] });

    bool visible = catSsgEnv[opIndex].isDetailVisible();

    se[opIndex].setVisibleWithLabel(visible);
    seFreq[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &se[opIndex].label, .component = &se[opIndex] });
        layoutRow({ .rowRect = rect, .label = &seFreq[opIndex].label, .component = &seFreq[opIndex] });
    }
}

void GuiOpna::layoutOpHwLfoCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catLfo[opIndex] });

    bool visible = catLfo[opIndex].isDetailVisible();

    freqs[opIndex].setVisibleWithLabel(visible);
    syncDelay[opIndex].setVisibleWithLabel(visible);
    syncDelayToZero[opIndex].setVisible(visible);
    syncDelayToOne[opIndex].setVisible(visible);
    pm[opIndex].setVisible(visible);
    pms[opIndex].setVisibleWithLabel(visible);
    am[opIndex].setVisible(visible);
    ams[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &freqs[opIndex].label, .component = &freqs[opIndex] });
        layoutRow({ .rowRect = rect, .label = &syncDelay[opIndex].label, .component = &syncDelay[opIndex] });
        layoutRowTwoComps({ .rect = rect, .comp1 = &syncDelayToZero[opIndex], .comp2 = &syncDelayToOne[opIndex] });
        layoutRow({ .rowRect = rect, .component = &pm[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pms[opIndex].label, .component = &pms[opIndex] });
        layoutRow({ .rowRect = rect, .component = &am[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ams[opIndex].label, .component = &ams[opIndex] });
    }
}

void GuiOpna::layoutOpN88LfoCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catN88Lfo[opIndex] });

    bool visible = catN88Lfo[opIndex].isDetailVisible();

    n88Ams[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &n88Ams[opIndex].label, .component = &n88Ams[opIndex] });
    }
}

void GuiOpna::layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &ksCat[opIndex] });

    bool visible = ksCat[opIndex].isDetailVisible();

    ks[opIndex].setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &ks[opIndex].label, .component = &ks[opIndex] });
    }
}

void GuiOpna::layoutOpDetCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catDet[opIndex] });

    bool visible = catDet[opIndex].isDetailVisible();

    mul[opIndex].setVisibleWithLabel(visible);
    dt[opIndex].setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &mul[opIndex].label, .component = &mul[opIndex] });
        layoutRow({ .rowRect = rect, .label = &dt[opIndex].label, .component = &dt[opIndex] });
    }
}

void GuiOpna::layoutOpAmpCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catAmp[opIndex] });

    bool visible = catAmp[opIndex].isDetailVisible();

    rgAr[opIndex].setVisibleWithLabel(visible);
    rgDr[opIndex].setVisibleWithLabel(visible);
    rgSr[opIndex].setVisibleWithLabel(visible);
    rgSl[opIndex].setVisibleWithLabel(visible);
    rgRr[opIndex].setVisibleWithLabel(visible);
    rgTl[opIndex].setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &rgAr[opIndex].label, .component = &rgAr[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgDr[opIndex].label, .component = &rgDr[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgSr[opIndex].label, .component = &rgSr[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgSl[opIndex].label, .component = &rgSl[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgRr[opIndex].label, .component = &rgRr[opIndex] });
        layoutRow({ .rowRect = rect, .label = &rgTl[opIndex].label, .component = &rgTl[opIndex] });
    }
}

void GuiOpna::setupGraph(int opIndex)
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
        if (this->isUpdatingGraph) return;

        this->isUpdatingGraph = true;
        this->updateOpGraph(opIndex);
        this->isUpdatingGraph = false;
        };


    bypass[opIndex].onStateChange = repaintGraph;
    xof[opIndex].onStateChange = repaintGraph;
    kor[opIndex].onStateChange = repaintGraph;

    rgAr[opIndex].onValueChange = repaintGraph;
    rgDr[opIndex].onValueChange = repaintGraph;
    rgSr[opIndex].onValueChange = repaintGraph;
    rgSl[opIndex].onValueChange = repaintGraph;
    rgRr[opIndex].onValueChange = repaintGraph;
    rgTl[opIndex].onValueChange = repaintGraph;

    pitchEnv[opIndex].setupGraph(repaintGraph);
    ssgSwEnv[opIndex].setupGraph(repaintGraph);
    ssgSwEnv11[opIndex].setupGraph(repaintGraph);
    ssgSwPEnv11[opIndex].setupGraph(repaintGraph);

    graphSeparator[opIndex].setupComponent(*this);
}

void GuiOpna::setGraphMode(int opIndex, GraphMode mode)
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

void GuiOpna::layoutOpGraph(int opIndex, juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(OpnaGuiValue::ParamGroup::Graph::height + NormalSeparator::getHeight());

    graphSeparator[opIndex].layoutComponentBottom(mainArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(OpnaGuiValue::ParamGroup::Graph::ButtonHeight);
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
void GuiOpna::updateOpGraph(int opIndex)
{
    GraphMode mode = currentGraphMode[opIndex];

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
    if (mode == GraphMode::Pitch) {
        pitchEnv[opIndex].updateGraph(opGraphs[opIndex]);
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        ssgSwEnv[opIndex].updateGraph(opGraphs[opIndex]);
    }
    // =============================================================
    // SSG SW Env 11
    // =============================================================
    else if (mode == GraphMode::SsgSw11) {
        ssgSwEnv11[opIndex].updateGraph(opGraphs[opIndex]);
    }
    // =============================================================
    // SSG SW PEnv 11
    // =============================================================
    else if (mode == GraphMode::SsgSwP11) {
        ssgSwPEnv11[opIndex].updateGraph(opGraphs[opIndex]);
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
        float drMax = (float)rgDr[opIndex].getMaximum();
        float srMax = (float)rgSr[opIndex].getMaximum();
        float slMax = (float)rgSl[opIndex].getMaximum();
        float rrMax = (float)rgRr[opIndex].getMaximum();
        float tlMax = (float)rgTl[opIndex].getMaximum();

        float arVal = (float)rgAr[opIndex].getValue();
        float drVal = (float)rgDr[opIndex].getValue();
        float srVal = (float)rgSr[opIndex].getValue();
        float slVal = (float)rgSl[opIndex].getValue();
        float rrVal = (float)rgRr[opIndex].getValue();
        float tlVal = (float)rgTl[opIndex].getValue();

        float sl = (slMax - slVal) / slMax; // 15=0.0, 0=1.0
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
        float decayWidth = rateToWidth(drVal, drMax);
        phases.push_back({
            .widthPx = decayWidth, .startLevel = 1.0f * tlScale, .endLevel = sl * tlScale, .color = color,
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
            float decayRatio = sustainTotalWidth / 300.0f;
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
                .color = srVal > 0.0f ? juce::Colours::yellow : color,
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX,
                .isMax = (rrVal == rrMax)
                });
        }

        opGraphs[opIndex].setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
    }
}

void GuiOpna::layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect) {
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
    }
}

void GuiOpna::setLevel(float level) {
    levelComponent.setLevel(level);
}

void GuiOpna::copyParams(CopyOpna& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();
    copyObj.pan.pan = panSlider.getValue();

    copyObj.n88Lfo.freq = lfoFreqSlider.getValue();
    copyObj.n88Lfo.wave = lfoShapeSelector.getSelectedId();
    copyObj.n88Lfo.amSmRt =lfoAmSmRtSlider.getValue();
    copyObj.n88Lfo.syncDelay = lfoSyncDelaySlider.getValue();
    copyObj.n88Lfo.pmEnable = lfoPmToggle.getToggleState();
    copyObj.n88Lfo.amEnable = lfoAmToggle.getToggleState();
    copyObj.n88Lfo.pmd = lfoPmdSlider.getValue();
    copyObj.n88Lfo.pms = lfoPmsSlider.getValue();
    copyObj.n88Lfo.amd = lfoAmdSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpna::copyOpParams(int p, CopyOpnaOp& copyObj) {
    copyObj.detune.mul = mul[p].getSelectedId();
    copyObj.detune.dt = dt[p].getSelectedId();
    copyObj.aAdsr.ar = rgAr[p].getValue();
    copyObj.aAdsr.dr = rgDr[p].getValue();
    copyObj.aAdsr.sl = rgSl[p].getValue();
    copyObj.aAdsr.sr = rgSr[p].getValue();
    copyObj.aAdsr.rr = rgRr[p].getValue();
    copyObj.aAdsr.tl = rgTl[p].getValue();
    copyObj.ssgEg.ssgEg = se[p].getSelectedId();
    copyObj.ssgEg.fmSsgEgFreq = seFreq[p].getValue();
    copyObj.aAdsr.bypass = bypass[p].getToggleState();
    copyObj.aAdsr.kor = kor[p].getToggleState();
    copyObj.aAdsr.xof = xof[p].getToggleState();
    copyObj.aAdsr.ks = ks[p].getSelectedId();

    copyObj.opnaLfo.freqsIndex = freqs[p].getSelectedId();
    copyObj.opnaLfo.syncDelay = syncDelay[p].getValue();
    copyObj.opnaLfo.pm = pm[p].getToggleState();
    copyObj.opnaLfo.pms = pms[p].getSelectedId();
    copyObj.opnaLfo.am = am[p].getToggleState();
    copyObj.opnaLfo.ams = ams[p].getSelectedId();

    copyObj.n88Lfo.ams = n88Ams[p].getValue();

    copyObj.mask.mask = mask[p].getToggleState();
        
    fix[p].copyParams(copyObj.fix);
    pitchEnv[p].copyParams(copyObj.pAdsr);
    ssgSwEnv[p].copyParams(copyObj.aSsgSw);
}

void GuiOpna::pasteParams(CopyOpna& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);
    panSlider.setValue(copyObj.pan.pan, juce::sendNotification);

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

void GuiOpna::pasteOpParams(int p, CopyOpnaOp& copyObj) {
    mul[p].setSelectedId(copyObj.detune.mul, juce::sendNotification);
    dt[p].setSelectedId(copyObj.detune.dt, juce::sendNotification);
    rgAr[p].setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgDr[p].setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgSl[p].setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgSr[p].setValue(copyObj.aAdsr.sr, juce::sendNotification);
    rgRr[p].setValue(copyObj.aAdsr.rr, juce::sendNotification);
    rgTl[p].setValue(copyObj.aAdsr.tl, juce::sendNotification);
    bypass[p].setToggleState(copyObj.aAdsr.bypass, juce::sendNotification);
    kor[p].setToggleState(copyObj.aAdsr.kor, juce::sendNotification);
    xof[p].setToggleState(copyObj.aAdsr.xof, juce::sendNotification);
    ks[p].setSelectedId(copyObj.aAdsr.ks, juce::sendNotification);
    se[p].setSelectedId(copyObj.ssgEg.ssgEg, juce::sendNotification);
    seFreq[p].setValue(copyObj.ssgEg.fmSsgEgFreq, juce::sendNotification);

    freqs[p].setSelectedId(copyObj.opnaLfo.freqsIndex, juce::sendNotification);
    syncDelay[p].setValue(copyObj.opnaLfo.syncDelay, juce::sendNotification);
    pm[p].setToggleState(copyObj.opnaLfo.pm, juce::sendNotification);
    pms[p].setSelectedId(copyObj.opnaLfo.pms, juce::sendNotification);
    am[p].setToggleState(copyObj.opnaLfo.am, juce::sendNotification);
    ams[p].setSelectedId(copyObj.opnaLfo.ams, juce::sendNotification);

    n88Ams[p].setValue(copyObj.n88Lfo.ams, juce::sendNotification);

    mask[p].setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix[p].pasteParams(copyObj.fix);
    pitchEnv[p].pasteParams(copyObj.pAdsr);
    ssgSwEnv[p].pasteParams(copyObj.aSsgSw);
}

void GuiOpna::copyParamsOpm(CopyOpnOpm& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpna::copyOpParamsOpm(int p, CopyOpnOpmOp& copyObj) {
    copyObj.detune.mul = mul[p].getSelectedId();
    copyObj.detune.dt = dt[p].getSelectedId();
    copyObj.aAdsr.ar = rgAr[p].getValue();
    copyObj.aAdsr.dr = rgDr[p].getValue();
    copyObj.aAdsr.sl = rgSl[p].getValue();
    copyObj.aAdsr.sr = rgSr[p].getValue();
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

void GuiOpna::pasteParamsOpm(CopyOpnOpm& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpna::pasteOpParamsOpm(int p, CopyOpnOpmOp& copyObj) {
    mul[p].setSelectedId(copyObj.detune.mul, juce::sendNotification);
    dt[p].setSelectedId(copyObj.detune.dt, juce::sendNotification);
    rgAr[p].setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgDr[p].setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgSl[p].setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgSr[p].setValue(copyObj.aAdsr.sr, juce::sendNotification);
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

void GuiOpna::copyParamsOpnOpm(CopyOpnaOpnOpm& copyObj) {
    copyObj.quality.depth = qualityComponent.getBit();
    copyObj.quality.rate = qualityComponent.getRate();
    copyObj.fmBase.level = levelComponent.getLevel();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpna::copyOpParamsOpnOpm(int p, CopyOpnaOpnOpmOp& copyObj) {
    copyObj.detune.mul = mul[p].getSelectedItemIndex();
    copyObj.detune.dt = dt[p].getSelectedItemIndex();
    copyObj.aAdsr.ar = rgAr[p].getValue();
    copyObj.aAdsr.dr = rgDr[p].getValue();
    copyObj.aAdsr.sl = rgSl[p].getValue();
    copyObj.aAdsr.sr = rgSr[p].getValue();
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

void GuiOpna::pasteParamsOpnOpm(CopyOpnaOpnOpm& copyObj) {
    qualityComponent.setBit(copyObj.quality.depth);
    qualityComponent.setRate(copyObj.quality.rate);
    levelComponent.setLevel(copyObj.fmBase.level);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpna::pasteOpParamsOpnOpm(int p, CopyOpnaOpnOpmOp& copyObj) {
    mul[p].setSelectedItemIndex(copyObj.detune.mul, juce::sendNotification);
    dt[p].setSelectedItemIndex(copyObj.detune.dt, juce::sendNotification);
    rgAr[p].setValue(copyObj.aAdsr.ar, juce::sendNotification);
    rgDr[p].setValue(copyObj.aAdsr.dr, juce::sendNotification);
    rgSl[p].setValue(copyObj.aAdsr.sl, juce::sendNotification);
    rgSr[p].setValue(copyObj.aAdsr.sr, juce::sendNotification);
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

void GuiOpna::importHwLfoParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultLfoParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importLfoParamFile, defaultDir, Io::ExtensionGlob::OpnaLfoParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 6) return;

                freqs[opIndex].setSelectedItemIndex(lines[0].getIntValue(), juce::sendNotification);
                syncDelay[opIndex].setValue(lines[1].getIntValue(), juce::sendNotification);
                pm[opIndex].setToggleState(lines[2].getIntValue() == 1, juce::sendNotification);
                pms[opIndex].setSelectedItemIndex(lines[3].getIntValue(), juce::sendNotification);
                am[opIndex].setToggleState(lines[4].getIntValue() == 1, juce::sendNotification);
                ams[opIndex].setSelectedItemIndex(lines[5].getIntValue(), juce::sendNotification);
            }
        });
}

void GuiOpna::exportHwLfoParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultLfoParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportLfoParamFile, defaultDir.getChildFile("default.lfoOpna"), Io::ExtensionGlob::OpnaLfoParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "\n";

                content += juce::String(freqs[opIndex].getSelectedItemIndex()) + "\n";
                content += juce::String(syncDelay[opIndex].getValue()) + "\n";
                content += juce::String(pm[opIndex].getToggleState() ? 1 : 0) + "\n";
                content += juce::String(pms[opIndex].getSelectedItemIndex()) + "\n";
                content += juce::String(am[opIndex].getToggleState() ? 1 : 0) + "\n";
                content += juce::String(ams[opIndex].getSelectedItemIndex()) + "\n";

                file.replaceWithText(content);
            }
        });
}

void GuiOpna::importPitchEnvParam(int opIndex) {
    pitchEnv[opIndex].importParams();
}

void GuiOpna::exportPitchEnvParam(int opIndex) {
    pitchEnv[opIndex].exportParams();
}

void GuiOpna::importSsgSwEnvParam(int opIndex) {
    ssgSwEnv[opIndex].importParams();
}

void GuiOpna::exportSsgSwEnvParam(int opIndex) {
    ssgSwEnv[opIndex].exportParams();
}

void GuiOpna::importLfoParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultLfoParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importLfoParamFile, defaultDir, Io::ExtensionGlob::N88LfoParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

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

                for (int i = 0; i < OpmPrValue::ops; i++) {
                    n88Ams[i].setValue(lines[9 + i].getIntValue(), juce::sendNotification);
                }
            }
        });
}

void GuiOpna::exportLfoParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultLfoParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportLfoParamFile, defaultDir.getChildFile("default.lfoN88"), Io::ExtensionGlob::N88LfoParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "\n";

                content += juce::String(lfoFreqSlider.getValue()) + "\n";
                content += juce::String(lfoShapeSelector.getSelectedItemIndex()) + "\n";
                content += juce::String(lfoSyncDelaySlider.getValue()) + "\n";
                content += juce::String(lfoPmToggle.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(lfoPmsSlider.getValue()) + "\n";
                content += juce::String(lfoPmdSlider.getValue()) + "\n";
                content += juce::String(lfoAmToggle.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(lfoAmSmRtSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(lfoAmdSlider.getValue()) + "\n";

                for (int i = 0; i < OpmPrValue::ops; i++) {
                    content += juce::String(n88Ams[i].getValue()) + "\n";
                }

                file.replaceWithText(content);
            }
        });
}

void GuiOpna::importUnisonParam() {
    unisonComponent.importParams();
}

void GuiOpna::exportUnisonParam() {
    unisonComponent.exportParams();
}

void GuiOpna::importQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importQualityParamFile, defaultDir, Io::ExtensionGlob::QualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 2) return;

                qualityComponent.setBit(lines[0].getIntValue());
                qualityComponent.setRate(lines[1].getIntValue());
            }
        });
}

void GuiOpna::exportQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportQualityParamFile, defaultDir.getChildFile("default.quality"), Io::ExtensionGlob::QualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += juce::String(qualityComponent.getBit()) + "\n";
                content += juce::String(qualityComponent.getRate()) + "\n";

                file.replaceWithText(content);
            }
        });
}

void GuiOpna::importSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11[opIndex].importParams();
}

void GuiOpna::exportSsgSwEnv11Param(int opIndex) {
    ssgSwEnv11[opIndex].exportParams();
}

void GuiOpna::importSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11[opIndex].importParams();
}

void GuiOpna::exportSsgSwPEnv11Param(int opIndex) {
    ssgSwPEnv11[opIndex].exportParams();
}

void GuiOpna::importChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::opnaParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();
                int index = 0;

                // Level
                levelComponent.setImportingParams(lines, index);

                // Algorithm & Feedback
                algSelector.setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
                feedbackSlider.setValue(lines[index++].getIntValue(), juce::sendNotification);

                updateAlgorithmDisplay();

                // Pan
                panSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);

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
                qualityComponent.setImportingParams(lines, index);
                unisonComponent.setImportingParams(lines, index);

                for (int i = 0; i < OpnaPrValue::ops; i++) {
                    getImportingOpParams(i, lines, index);
                }
            }
        });
}

void GuiOpna::exportChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile("default." + Io::Extension::opnaParam), Io::ExtensionGlob::opnaParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                // Level
                content += levelComponent.getExportedParams();

                // Algorithm & Feedback
                content += juce::String(algSelector.getSelectedId()) + "\n";
                content += juce::String(feedbackSlider.getValue(), 0) + "\n";

                // Pan
                content += juce::String(panSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                // N88 LFO
                content += juce::String(lfoFreqSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(lfoShapeSelector.getSelectedId()) + "\n";
                content += juce::String(lfoAmSmRtSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(lfoSyncDelaySlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(lfoPmToggle.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(lfoPmsSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(lfoPmdSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(lfoAmToggle.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(lfoAmdSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                // Components (Global)
                content += qualityComponent.getExportedParams();
                content += unisonComponent.getExportedParams();

                for (int i = 0; i < OpnaPrValue::ops; i++) {
                    content += setExportedOpParams(i);
                }

                file.replaceWithText(content);
            }
        });
}

void GuiOpna::importOpChParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::opnaOpParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();
                int index = 0;

                updateAlgorithmDisplay();

                getImportingOpParams(opIndex, lines, index);
            }
        });
}

void GuiOpna::exportOpChParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportChannelParamFile, defaultDir.getChildFile("default." + Io::Extension::opnaOpParam), Io::ExtensionGlob::opnaOpParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += setExportedOpParams(opIndex);

                file.replaceWithText(content);
            }
        });

}

void GuiOpna::getImportingOpParams(int opIndex, juce::StringArray& lines, int& index) {
    // Mul / Dt
    mul[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    dt[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // Env
    rgAr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgDr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgSl[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgSr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgRr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgTl[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Key Scale
    ks[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // HW LFO
    freqs[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    syncDelay[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    pm[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    pms[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    am[opIndex].setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    ams[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // N88 AMS
    n88Ams[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // SSG Env
    se[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    seFreq[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Optional / Mask
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

juce::String GuiOpna::setExportedOpParams(int opIndex) {
    juce::String content = "";

    // Mul / Dt
    content += juce::String(mul[opIndex].getSelectedId()) + "\n";
    content += juce::String(dt[opIndex].getSelectedId()) + "\n";

    // Env
    content += juce::String(rgAr[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(rgDr[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(rgSl[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(rgSr[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(rgRr[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(rgTl[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";

    // Key Scale
    content += juce::String(ks[opIndex].getSelectedId()) + "\n";

    // HW LFO
    content += juce::String(freqs[opIndex].getSelectedId()) + "\n";
    content += juce::String(syncDelay[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(pm[opIndex].getToggleState() ? 1 : 0) + "\n";
    content += juce::String(pms[opIndex].getSelectedId()) + "\n";
    content += juce::String(am[opIndex].getToggleState() ? 1 : 0) + "\n";
    content += juce::String(ams[opIndex].getSelectedId()) + "\n";

    // N88 AMS
    content += juce::String(n88Ams[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";

    // SSG Env
    content += juce::String(se[opIndex].getSelectedId()) + "\n";
    content += juce::String(seFreq[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";

    // Optional / Mask
    content += juce::String(bypass[opIndex].getToggleState() ? 1 : 0) + "\n";
    content += juce::String(xof[opIndex].getToggleState() ? 1 : 0) + "\n";
    content += juce::String(kor[opIndex].getToggleState() ? 1 : 0) + "\n";
    content += juce::String(mask[opIndex].getToggleState() ? 1 : 0) + "\n";

    // Components
    content += fix[opIndex].getExportedParams();
    content += pitchEnv[opIndex].getExportedParams();
    content += ssgSwEnv[opIndex].getExportedParams();
    content += ssgSwEnv11[opIndex].getExportedParams();
    content += ssgSwPEnv11[opIndex].getExportedParams();

    return content;
}

void GuiOpna::importOpnChParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::opnParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();
                int index = 0;

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
                qualityComponent.setImportingParams(lines, index);
                unisonComponent.setImportingParams(lines, index);

                for (int i = 0; i < OpnPrValue::ops; i++) {
                    getImportingOpnOpParams(i, lines, index);
                }
            }
        });
}

void GuiOpna::importOpnOpChParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultChannelParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importChannelParamFile, defaultDir, Io::ExtensionGlob::opnOpParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();
                int index = 0;

                updateAlgorithmDisplay();

                getImportingOpnOpParams(opIndex, lines, index);
            }
        });
}

void GuiOpna::getImportingOpnOpParams(int opIndex, juce::StringArray& lines, int& index) {
    // Mul / Dt
    mul[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);
    dt[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // Env
    rgAr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgDr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgSl[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgSr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgRr[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);
    rgTl[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // Key Scale
    ks[opIndex].setSelectedId(lines[index++].getIntValue(), juce::sendNotification);

    // HW LFO はスキップ

    // N88 AMS
    n88Ams[opIndex].setValue(lines[index++].getFloatValue(), juce::sendNotification);

    // SSG Env はスキップ

    // Optional / Mask
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
