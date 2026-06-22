#include <vector>

#include "./GuiOpn.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Opn/ProcessorOpnKeys.h"
#include "../../Processor/Opn/ProcessorOpnValues.h"
#include "../../Core/Const/ConstMmlKeys.h"
#include "../../Core/Const/ConstMmlValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Fm/FmRegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiOpnValues.h"
#include "./GuiOpnText.h"
#include "../../Core/Gui/GuiStructs.h"

// 1:4bit, 2:5bit, 3:6bit, 4:7bit, 5:8bit, 6:9bit, 7:10bit, 8:12bit, 9:16bit, 10:20bit, 11:24bit, 12:raw(32bit)
static std::vector<SelectItem> bdItems = {
    {.name = " 1:  4-bit (16 steps)",       .value = 1 },
    {.name = " 2:  5-bit (32 steps)",       .value = 2 },
    {.name = " 3:  6-bit (64 steps)",       .value = 3 },
    {.name = " 4:  7-bit (128 steps)",      .value = 4 },
    {.name = " 5:  8-bit (256 steps)",      .value = 5 },
    {.name = " 6:  9-bit (512 steps)",      .value = 6 },
    {.name = " 7: 10-bit (1024 steps)",     .value = 7 },
    {.name = " 8: 12-bit (4096 steps)",     .value = 8 },
    {.name = " 9: 16-bit (32768 steps)",    .value = 9 },
    {.name = "10: 20-bit (1048576 steps)",  .value = 10 },
    {.name = "11: 24-bit (16777216 steps)", .value = 11 },
    {.name = "12: Raw",                     .value = 12 }
};

// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
static std::vector<SelectItem> rateItems = {
    {.name = " 1: 96kHz",    .value = 1 },
    {.name = " 2: 55.5kHz",  .value = 2 },
    {.name = " 3: 49.7kHz",  .value = 3 },
    {.name = " 4: 48kHz",    .value = 4 },
    {.name = " 5: 44.1kHz",  .value = 5 },
    {.name = " 6: 33.08kHz", .value = 6 },
    {.name = " 7: 32kHz",    .value = 7 },
    {.name = " 8: 22.05kHz", .value = 8 },
    {.name = " 9: 16kHz",    .value = 9 },
    {.name = "10: 12kHz",    .value = 10 },
    {.name = "11: 11kHz",    .value = 11 },
    {.name = "12: 8kHz",     .value = 12 },
    {.name = "12: 5.5kHz",   .value = 13 },
    {.name = "13: 4kHz",     .value = 14 },
    {.name = "15: 2kHz",     .value = 15 },
};

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
    p_guiCurve = ctx.editor.getCurveGui();

    mainGroup.setup(*this, OpnGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(juce::FontOptions(18.0f)));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    levelSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::level, .title = OpnGuiText::Fm::level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    qualityCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Category::visibleQuality, .invisibleTitle = OpnGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::bit, .title = OpnGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::rate, .title = OpnGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Category::algFb });

    algSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::alg, .title = OpnGuiText::Fm::alg, .items = opnAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    feedbackSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::fb, .title = OpnGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    lfoCat.setupSwCategory({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Category::visibleN88Lfo, .invisibleTitle = OpnGuiText::Category::invisibleN88Lfo, .enableChangeDetailVisible = true });

    lfoFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::N88Lfo::freq, .title = OpnGuiText::Fm::lfoSpeed, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoShapeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::N88Lfo::shape, .title = OpnGuiText::Fm::lfoShape, .items = lfoShapeItems, .isReset = true });
    lfoShapeSelector.setWantsKeyboardFocus(true);
    lfoShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::N88Lfo::amSmoothRatio, .title = OpnGuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::N88Lfo::syncDelay, .title = OpnGuiText::Fm::lfoSyncDelay, .isReset = true });
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

    lfoPmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::N88Lfo::pm, .title = OpnGuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::N88Lfo::pmd, .title = OpnGuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::N88Lfo::pms, .title = OpnGuiText::Fm::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::N88Lfo::am, .title = OpnGuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpnPrKey::N88Lfo::amd, .title = OpnGuiText::Fm::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Category::visibleUtil, .invisibleTitle = OpnGuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelSlider.getValue();

        ctx.editor.breadcastLevel(level);
        };

    mainGroup.contentCanvas.addAndMakeVisible(uSep001);
    uSep001.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

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

    mainGroup.contentCanvas.addAndMakeVisible(uSep002);
    uSep002.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

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

    mainGroup.contentCanvas.addAndMakeVisible(uSep003);
    uSep003.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

    importPitchEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Utility::pitchEnvFileImport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    importPitchEnvParamButton.setWantsKeyboardFocus(true);
    importPitchEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importPitchEnvParamButton.onClick = [this] {
        int opIndex = (int)targerOpSlider.getValue() - 1;

        importPitchEnvParam(opIndex);
        };

    exportPitchEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Utility::pitchEnvFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportPitchEnvParamButton.setWantsKeyboardFocus(true);
    exportPitchEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportPitchEnvParamButton.onClick = [this] {
        int opIndex = (int)targerOpSlider.getValue() - 1;

        exportPitchEnvParam(opIndex);
        };

    importSsgSwEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Utility::ssgSwEnvFileImport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    importSsgSwEnvParamButton.setWantsKeyboardFocus(true);
    importSsgSwEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importSsgSwEnvParamButton.onClick = [this] {
        int opIndex = (int)targerOpSlider.getValue() - 1;

        importSsgSwEnvParam(opIndex);
        };

    exportSsgSwEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Utility::ssgSwEnvFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportSsgSwEnvParamButton.setWantsKeyboardFocus(true);
    exportSsgSwEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportSsgSwEnvParamButton.onClick = [this] {
        int opIndex = (int)targerOpSlider.getValue() - 1;

        exportSsgSwEnvParam(opIndex);
        };

    targerOpSlider.setup({ .parent = mainGroup.contentCanvas, .title = "Op", .isReset = false });
    targerOpSlider.setRange(1.0, 4.0, 1.0);
    targerOpSlider.setNumDecimalPlacesToDisplay(0);
    targerOpSlider.setValue(1, juce::sendNotification);
    targerOpSlider.setWantsKeyboardFocus(true);
    targerOpSlider.setExplicitFocusOrder(++tabOrder);

    mainGroup.contentCanvas.addAndMakeVisible(uSep004);
    uSep004.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

    importLfoParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Utility::lfoFileImport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    importLfoParamButton.setWantsKeyboardFocus(true);
    importLfoParamButton.setExplicitFocusOrder(++tabOrder);
    importLfoParamButton.onClick = [this] {
        importLfoParam();
        };

    exportLfoParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Utility::lfoFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportLfoParamButton.setWantsKeyboardFocus(true);
    exportLfoParamButton.setExplicitFocusOrder(++tabOrder);
    exportLfoParamButton.onClick = [this] {
        exportLfoParam();
        };

    importUnisonParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Utility::unisonFileImport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    importUnisonParamButton.setWantsKeyboardFocus(true);
    importUnisonParamButton.setExplicitFocusOrder(++tabOrder);
    importUnisonParamButton.onClick = [this] {
        importUnisonParam();
        };

    exportUnisonParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OpnGuiText::Utility::unisonFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportUnisonParamButton.setWantsKeyboardFocus(true);
    exportUnisonParamButton.setExplicitFocusOrder(++tabOrder);
    exportUnisonParamButton.onClick = [this] {
        exportUnisonParam();
        };

    auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    for (int i = 0; i < OpnPrValue::algorithms; ++i)
    {
        juce::String fileName = juce::String::formatted(Io::Folder::asset + "/" + Io::Folder::resource + "/ALG_OPNA_OPN_OPM_%02d.png", i);
        auto imgFile = docDir.getChildFile(fileName);

        if (imgFile.existsAsFile()) {
            algImages[i] = juce::ImageFileFormat::loadFrom(imgFile);
        }
    }

    // 画像コンポーネントを画面に追加
    mainGroup.contentCanvas.addAndMakeVisible(algImageComp);

    // Operators
    const juce::String opCode = code + OpnPrKey::op;

    for (int i = 0; i < OpnPrValue::ops; ++i)
    {
        opGroups[i].setup(*this, OpnGuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        mul[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::mul, .title = OpnGuiText::Fm::Op::Mul, .items = multems, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);

        dt[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::dt, .title = OpnGuiText::Fm::Op::Dt, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        dt[i].setWantsKeyboardFocus(true);
        dt[i].setExplicitFocusOrder(++tabOrder);

        rgAr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::rgAr, .title = OpnGuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgDr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::rgDr, .title = OpnGuiText::Fm::Op::Dr, .isReset = true });
        rgDr[i].setWantsKeyboardFocus(true);
        rgDr[i].setExplicitFocusOrder(++tabOrder);

        rgSl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::rgSl, .title = OpnGuiText::Fm::Op::Sl, .isReset = true });
        rgSl[i].setWantsKeyboardFocus(true);
        rgSl[i].setExplicitFocusOrder(++tabOrder);

        rgSr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::rgSr, .title = OpnGuiText::Fm::Op::Sr, .isReset = true });
        rgSr[i].setWantsKeyboardFocus(true);
        rgSr[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::rgRr, .title = OpnGuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::rgTl, .title = OpnGuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        ksCat[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnGuiText::Category::visibleKs, .invisibleTitle = OpnGuiText::Category::invisibleKs, .enableChangeDetailVisible = true });

        ks[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::ks, .title = OpnGuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });
        ks[i].setWantsKeyboardFocus(true);
        ks[i].setExplicitFocusOrder(++tabOrder);

        catOptional[i].setupSwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnGuiText::Category::visibleOptional, .invisibleTitle = OpnGuiText::Category::invisibleOptional, .enableChangeDetailVisible = true });

        xof[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::xof, .title = OpnGuiText::Fm::Op::xof, .isReset = true });
        xof[i].setWantsKeyboardFocus(true);
        xof[i].setExplicitFocusOrder(++tabOrder);

        kor[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::kor, .title = OpnGuiText::Fm::Op::kor, .isReset = true });
        kor[i].setWantsKeyboardFocus(true);
        kor[i].setExplicitFocusOrder(++tabOrder);

        bypass[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::ampBypass, .title = OpnGuiText::Fm::Op::bypass, .isReset = true });
        bypass[i].setWantsKeyboardFocus(true);
        bypass[i].setExplicitFocusOrder(++tabOrder);

        pitchEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, OpnPrKey::PitchAdsr::enable, OpnGuiText::PitchAdsr::enable, true);

        ssgSwEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, OpnPrKey::SsgSwEnv::enable, OpnGuiText::SsgSwEnv::enable, true);

        catN88Lfo[i].setupSwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnGuiText::Category::visibleN88Lfo, .invisibleTitle = OpnGuiText::Category::invisibleN88Lfo, .enableChangeDetailVisible = true });

        n88Ams[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::n88Ams, .title = OpnGuiText::Fm::Op::Ams, .isReset = true });
        n88Ams[i].setWantsKeyboardFocus(true);
        n88Ams[i].setExplicitFocusOrder(++tabOrder);

        fix[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, OpnGuiText::Fm::Op::Opzx7FreqTo440, 440, true);

        catMask[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpnGuiText::Category::visibleMask, .invisibleTitle = OpnGuiText::Category::invisibleMask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpnPrKey::mask, .title = OpnGuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        opGroups[i].contentCanvas.addAndMakeVisible(mmlSeparator[i]);
        mmlSeparator[i].setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

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

void GuiOpn::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(OpnGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(OpnGuiValue::Group::Padding::width, OpnGuiValue::Group::Padding::height);

    mmRect.removeFromTop(OpnGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mmRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mmRect.removeFromTop(OpnGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.viewport.getMaximumVisibleWidth(), 2000);

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });

    mRect.removeFromTop(OpnGuiValue::Category::paddingTop);

    auto imgArea = mRect.removeFromTop(120);
    algImageComp.setBounds(imgArea);

    mRect.removeFromTop(OpnGuiValue::Category::paddingTop);

    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    layoutN88LfoCat(mRect);

    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < OpnPrValue::ops; ++i)
    {
        auto opArea = pageArea.removeFromLeft(OpnGuiValue::Fm::Op::width);

        // 枠線
        opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto iinnerRect = opArea.reduced(OpnGuiValue::Fm::Op::Padding::width, OpnGuiValue::Fm::Op::Padding::height);
        iinnerRect.removeFromTop(OpnGuiValue::Group::TitlePaddingTop);

        // グラフ用の区画を確保
        layoutOpGraph(i, iinnerRect);
        updateOpGraph(i);

        // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
        // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
        opGroups[i].setViewportCustomBounds(iinnerRect.translated(-opArea.getX(), -opArea.getY()));

        // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
        juce::Rectangle<int> innerRect(0, 0, opGroups[i].viewport.getMaximumVisibleWidth(), 2000);

        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt[i].label, .component = &dt[i] });
        updateRgDisplayAsOp(i, true);
        layoutRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgDr[i].label, .component = &rgDr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgSr[i].label, .component = &rgSr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgSl[i].label, .component = &rgSl[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i] });

        layoutOpKsCat(i, innerRect);

        layoutOpOptionalCat(i, innerRect);

        pitchEnv[i].layoutComponentRow(innerRect);

        ssgSwEnv[i].layoutComponentRow(innerRect);

        layoutOpN88LfoCat(i, innerRect);

        fix[i].layoutComponentRow(innerRect);

        layoutOpMaskCat(i, innerRect);

        // 区切り線エリアを確保
        auto mmlSeparatorArea = innerRect.removeFromTop(OpnGuiValue::ParamGroup::Separator::height);
        mmlSeparator[i].setBounds(mmlSeparatorArea);

        layoutRow({ .rowRect = innerRect, .component = &mml[i] });

        int usedHeight = 2000 - innerRect.getHeight();

        // 下部の余白を足して、キャンバスの最終的な高さをセット
        opGroups[i].setContentHeight(usedHeight + 20);
    }

    updateAlgorithmDisplay();
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpn::applyMmlString(const juce::String& mml, int opIndex)
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

void GuiOpn::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    mul[idx].setEnabledWithLabel(enable);
    dt[idx].setEnabledWithLabel(enable);
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

void GuiOpn::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > OpnPrValue::Alg::max) return;

    for (int i = 0; i < OpnPrValue::ops; ++i)
    {
        juce::String newTitle = OpnGuiText::Group::opPrefix + juce::String(i + 1) + algOpPrefix[algIndex][i];

        opGroups[i].setText(newTitle);
    }

    // ==========================================================
    // 画像の切り替え
    // ==========================================================
    if (algImages[algIndex].isValid())
    {
        // 読み込めている場合はその画像をセット
        // centred | onlyReduceInSize を指定すると、アスペクト比を保ったまま綺麗に収まります
        algImageComp.setImage(algImages[algIndex], juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
    }
    else
    {
        // 画像がない場合（ファイルが見つからなかった時など）はクリア
        algImageComp.setImage(juce::Image());
    }
}

void GuiOpn::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].setVisibleWithLabel(rgMode);
    rgDr[idx].setVisibleWithLabel(rgMode);
    rgSl[idx].setVisibleWithLabel(rgMode);
    rgSr[idx].setVisibleWithLabel(rgMode);
    rgRr[idx].setVisibleWithLabel(rgMode);
    rgTl[idx].setVisibleWithLabel(rgMode);
}

void GuiOpn::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

// ==============================================================================
// Keyboard Shortcut Logic
// ==============================================================================
bool GuiOpn::keyPressed(const juce::KeyPress& key)
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

void GuiOpn::copyFmParamsToString()
{
    int mask1 = FmMml::bool2Int(!mask[0].getToggleState());
    int mask2 = FmMml::bool2Int(!mask[1].getToggleState());
    int mask3 = FmMml::bool2Int(!mask[2].getToggleState());
    int mask4 = FmMml::bool2Int(!mask[3].getToggleState());
    int maskVal = FmMml::genMask4(mask1, mask2, mask2, mask3);

    auto formatCoreBasic = [this, maskVal]() {
        return juce::String::formatted(
            // '   ALG   FB0   MSK    SHP  SPD  SYC  PMD  PMS  AMD 
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

void GuiOpn::layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask[opIndex] });

    bool visibleMask = catMask[opIndex].isDetailVisible();

    mask[opIndex].setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask[opIndex] });
    }
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
    importLfoParamButton.setVisible(visible);
    exportLfoParamButton.setVisible(visible);
    importPitchEnvParamButton.setVisible(visible);
    exportPitchEnvParamButton.setVisible(visible);
    importSsgSwEnvParamButton.setVisible(visible);
    exportSsgSwEnvParamButton.setVisible(visible);
    targerOpSlider.setVisibleWithLabel(visible);
    uSep004.setVisible(visible);
    importUnisonParamButton.setVisible(visible);
    exportUnisonParamButton.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        auto uSep001Area = rect.removeFromTop(4);
        uSep001.setBounds(uSep001Area);

        layoutMain({ .mainRect = rect, .component = &copyParamsToOpnaBtn });
        layoutMain({ .mainRect = rect, .component = &copyParamsToOpmBtn });

        auto uSep002Area = rect.removeFromTop(4);
        uSep002.setBounds(uSep002Area);

        layoutMain({ .mainRect = rect, .component = &copyOpParamBtn });
        layoutMain({ .mainRect = rect, .label = &copyOpFromSlider.label, .component = &copyOpFromSlider });
        layoutMain({ .mainRect = rect, .label = &copyOpToSlider.label, .component = &copyOpToSlider });

        auto uSep003Area = rect.removeFromTop(4);
        uSep003.setBounds(uSep003Area);

        layoutMain({ .mainRect = rect, .component = &importPitchEnvParamButton });
        layoutMain({ .mainRect = rect, .component = &exportPitchEnvParamButton });

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .component = &importSsgSwEnvParamButton });
        layoutMain({ .mainRect = rect, .component = &exportSsgSwEnvParamButton });

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .label = &targerOpSlider.label, .component = &targerOpSlider });

        auto uSep004Area = rect.removeFromTop(4);
        uSep004.setBounds(uSep004Area);

        layoutMain({ .mainRect = rect, .component = &importLfoParamButton });
        layoutMain({ .mainRect = rect, .component = &exportLfoParamButton });

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .component = &importUnisonParamButton });
        layoutMain({ .mainRect = rect, .component = &exportUnisonParamButton });
    }
}

void GuiOpn::layoutQualityCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &qualityCat });

    bool visibleQuality = qualityCat.isDetailVisible();

    bitSelector.setVisibleWithLabel(visibleQuality);
    rateSelector.setVisibleWithLabel(visibleQuality);

    if (visibleQuality)
    {
        layoutMain({ .mainRect = rect, .label = &bitSelector.label, .component = &bitSelector });
        layoutMain({ .mainRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
    }
}

void GuiOpn::layoutN88LfoCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &lfoCat });

    bool visible = lfoCat.isDetailVisible();

    lfoFreqSlider.setVisibleWithLabel(visible);
    lfoShapeSelector.setVisibleWithLabel(visible);
    lfoAmSmRtSlider.setVisibleWithLabel(visible);
    lfoSyncDelaySlider.setVisibleWithLabel(visible);
    lfoSyncDelayToZeroBtn.setVisible(visible);
    lfoSyncDelayToOneBtn.setVisible(visible);
    lfoPmToggle.setVisible(visible);
    lfoPmsSlider.setVisibleWithLabel(visible);
    lfoPmdSlider.setVisibleWithLabel(visible);
    lfoAmToggle.setVisible(visible);
    lfoAmdSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &lfoFreqSlider.label, .component = &lfoFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoShapeSelector.label, .component = &lfoShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
        layoutMain({ .mainRect = rect, .label = &lfoSyncDelaySlider.label, .component = &lfoSyncDelaySlider });
        layoutMainTwoComps({ .rect = rect, .comp1 = &lfoSyncDelayToZeroBtn, .comp2 = &lfoSyncDelayToOneBtn });
        layoutMain({ .mainRect = rect, .component = &lfoPmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
        layoutMain({ .mainRect = rect, .label = &lfoPmsSlider.label, .component = &lfoPmsSlider });
        layoutMain({ .mainRect = rect, .component = &lfoAmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });
    }
}

void GuiOpn::layoutOpN88LfoCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catN88Lfo[opIndex] });

    bool visible = catN88Lfo[opIndex].isDetailVisible();

    n88Ams[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &n88Ams[opIndex].label, .component = &n88Ams[opIndex] });
    }
}

void GuiOpn::layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &ksCat[opIndex] });

    bool visible = ksCat[opIndex].isDetailVisible();

    ks[opIndex].setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &ks[opIndex].label, .component = &ks[opIndex] });
    }
}

void GuiOpn::setupGraph(int opIndex)
{
    addAndMakeVisible(&opGraphs[opIndex]); // グラフを追加

    graphBtnAmp[opIndex].setup({ .parent = *this, .title = "Amp", .isReset = false, .isResized = false });
    graphBtnAmp[opIndex].setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::Amp); };

    graphBtnPitch[opIndex].setup({ .parent = *this, .title = "Pitch", .isReset = false, .isResized = false });
    graphBtnPitch[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::Pitch); };

    graphBtnSsg[opIndex].setup({ .parent = *this, .title = "SSG SW", .isReset = false, .isResized = false });
    graphBtnSsg[opIndex].onClick = [this, opIndex] { setGraphMode(opIndex, GraphMode::SsgSw); };

    auto repaintGraph = [this, opIndex]() { updateOpGraph(opIndex); };

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

    addAndMakeVisible(graphSeparator[opIndex]);
    graphSeparator[opIndex].setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });
}

void GuiOpn::setGraphMode(int opIndex, GraphMode mode)
{
    currentGraphMode[opIndex] = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp[opIndex].setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch[opIndex].setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg[opIndex].setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateOpGraph(opIndex);
}

void GuiOpn::layoutOpGraph(int opIndex, juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(OpnGuiValue::ParamGroup::Graph::height + OpnGuiValue::ParamGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(OpnGuiValue::ParamGroup::Separator::height);

    graphSeparator[opIndex].setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(OpnGuiValue::ParamGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg[opIndex].setBounds(btnArea);

    // 残りをグラフエリアにする
    opGraphs[opIndex].setBounds(mainArea);
}

// グラフを再計算して描画
void GuiOpn::updateOpGraph(int opIndex)
{
    GraphMode mode = currentGraphMode[opIndex];

    // カーブモードが有効かどうかを判定
    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();

    // -------------------------------------------------------------
    // Helper: 幅の計算 (Amp 用)
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
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
                .color = srVal > 0.0f ? juce::Colours::yellow : color,
                .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Rr),
                .moveToStart = true,
                .startXOffsetPx = noteOffPositionX,
                .isMax = (rrVal == rrMax)
                });
        }

        opGraphs[opIndex].setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
    }
}

void GuiOpn::layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect) {
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

void GuiOpn::setLevel(float level) {
    levelSlider.setValue(level, juce::NotificationType::sendNotification);
}

void GuiOpn::copyParams(CopyOpn& copyObj) {
    copyObj.quality.depth = bitSelector.getSelectedId();
    copyObj.quality.rate = rateSelector.getSelectedId();
    copyObj.fmBase.level = levelSlider.getValue();
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

    copyObj.n88Lfo.ams = n88Ams[p].getValue();

    copyObj.mask.mask = mask[p].getToggleState();

    fix[p].copyParams(copyObj.fix);
    pitchEnv[p].copyParams(copyObj.pAdsr);
    ssgSwEnv[p].copyParams(copyObj.aSsgSw);
}

void GuiOpn::pasteParams(CopyOpn& copyObj) {
    bitSelector.setSelectedId(copyObj.quality.depth, juce::sendNotification);
    rateSelector.setSelectedId(copyObj.quality.rate, juce::sendNotification);
    levelSlider.setValue(copyObj.fmBase.level, juce::sendNotification);
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

    n88Ams[p].setValue(copyObj.n88Lfo.ams, juce::sendNotification);

    mask[p].setToggleState(copyObj.mask.mask, juce::sendNotification);

    fix[p].pasteParams(copyObj.fix);
    pitchEnv[p].pasteParams(copyObj.pAdsr);
    ssgSwEnv[p].pasteParams(copyObj.aSsgSw);
}

void GuiOpn::copyParamsOpm(CopyOpnOpm& copyObj) {
    copyObj.quality.depth = bitSelector.getSelectedId();
    copyObj.quality.rate = rateSelector.getSelectedId();
    copyObj.fmBase.level = levelSlider.getValue();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpn::copyOpParamsOpm(int p, CopyOpnOpmOp& copyObj) {
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

void GuiOpn::pasteParamsOpm(CopyOpnOpm& copyObj) {
    bitSelector.setSelectedId(copyObj.quality.depth, juce::sendNotification);
    rateSelector.setSelectedId(copyObj.quality.rate, juce::sendNotification);
    levelSlider.setValue(copyObj.fmBase.level, juce::sendNotification);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpn::pasteOpParamsOpm(int p, CopyOpnOpmOp& copyObj) {
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

void GuiOpn::copyParamsOpnaOpm(CopyOpnaOpnOpm& copyObj) {
    copyObj.quality.depth = bitSelector.getSelectedId();
    copyObj.quality.rate = rateSelector.getSelectedId();
    copyObj.fmBase.level = levelSlider.getValue();
    copyObj.fmBase.algorithm = algSelector.getSelectedId();
    copyObj.fmBase.feedback = feedbackSlider.getValue();

    unisonComponent.copyParams(copyObj.unison);
}

void GuiOpn::copyOpParamsOpnaOpm(int p, CopyOpnaOpnOpmOp& copyObj) {
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

void GuiOpn::pasteParamsOpnaOpm(CopyOpnaOpnOpm& copyObj) {
    bitSelector.setSelectedId(copyObj.quality.depth, juce::sendNotification);
    rateSelector.setSelectedId(copyObj.quality.rate, juce::sendNotification);
    levelSlider.setValue(copyObj.fmBase.level, juce::sendNotification);
    algSelector.setSelectedId(copyObj.fmBase.algorithm, juce::sendNotification);
    feedbackSlider.setValue(copyObj.fmBase.feedback, juce::sendNotification);

    unisonComponent.pasteParams(copyObj.unison);
}

void GuiOpn::pasteOpParamsOpnaOpm(int p, CopyOpnaOpnOpmOp& copyObj) {
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

void GuiOpn::importPitchEnvParam(int opIndex) {
    pitchEnv[opIndex].importParams();
}

void GuiOpn::exportPitchEnvParam(int opIndex) {
    pitchEnv[opIndex].exportParams();
}

void GuiOpn::importSsgSwEnvParam(int opIndex) {
    ssgSwEnv[opIndex].importParams();
}

void GuiOpn::exportSsgSwEnvParam(int opIndex) {
    ssgSwEnv[opIndex].exportParams();
}

void GuiOpn::importLfoParam() {
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

                if (lines.size() == 0) return;
            }
        });
}

void GuiOpn::exportLfoParam() {
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

                file.replaceWithText(content);
            }
        });
}

void GuiOpn::importUnisonParam() {
    unisonComponent.importParams();
}

void GuiOpn::exportUnisonParam() {
    unisonComponent.exportParams();
}
