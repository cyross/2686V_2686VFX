#include <vector>

#include "./GuiOpl.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Opl/ProcessorOplKeys.h"
#include "../../Processor/Opl/ProcessorOplValues.h"
#include "../../Core/Const/ConstMmlKeys.h"
#include "../../Core/Const/ConstMmlValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Fm/FmRegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiOplValues.h"
#include "./GuiOplText.h"
#include "../../Core/Gui/GuiStructs.h"

// 1:4bit, 2:5bit, 3:6bit, 4:7bit, 5:8bit, 6:9bit, 7:10bit, 8:12bit, 9:16bit, 10:20bit, 11:24bit, 12:raw(32bit)
static std::vector<SelectItem> bdItems = {
    { .name = " 1:  4-bit (16 steps)",       .value = 1 },
    { .name = " 2:  5-bit (32 steps)",       .value = 2 },
    { .name = " 3:  6-bit (64 steps)",       .value = 3 },
    { .name = " 4:  7-bit (128 steps)",      .value = 4 },
    { .name = " 5:  8-bit (256 steps)",      .value = 5 },
    { .name = " 6:  9-bit (512 steps)",      .value = 6 },
    { .name = " 7: 10-bit (1024 steps)",     .value = 7 },
    { .name = " 8: 12-bit (4096 steps)",     .value = 8 },
    { .name = " 9: 16-bit (32768 steps)",    .value = 9 },
    { .name = "10: 20-bit (1048576 steps)",  .value = 10 },
    { .name = "11: 24-bit (16777216 steps)", .value = 11 },
    { .name = "12: Raw",                     .value = 12 }
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
    p_guiCurve = ctx.editor.getCurveGui();

    const juce::String code = OplPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, OplGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(juce::FontOptions(18.0f)));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    levelSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OplPrKey::level, .title = OplGuiText::Fm::level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    qualityCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Category::visibleQuality, .invisibleTitle = OplGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });
    bitSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OplPrKey::bit, .title = OplGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);
    rateSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OplPrKey::rate, .title = OplGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Category::algFb });
    algSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OplPrKey::alg, .title = OplGuiText::Fm::alg, .items = oplAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };
    feedbackSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OplPrKey::fb, .title = OplGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Category::visibleUtil, .invisibleTitle = OplGuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelSlider.getValue();

        ctx.editor.breadcastLevel(level);
        };

    mainGroup.contentCanvas.addAndMakeVisible(uSep001);
    uSep001.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

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

    mainGroup.contentCanvas.addAndMakeVisible(uSep002);
    uSep002.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

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

    mainGroup.contentCanvas.addAndMakeVisible(uSep003);
    uSep003.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

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

    mainGroup.contentCanvas.addAndMakeVisible(uSep004);
    uSep004.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

    importLfoParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::lfoFileImport, .bgColor = juce::Colours::darkmagenta.darker(0.5f), .isReset = false, .isResized = false});
    importLfoParamButton.setWantsKeyboardFocus(true);
    importLfoParamButton.setExplicitFocusOrder(++tabOrder);
    importLfoParamButton.onClick = [this] {
        int opIndex = (int)targerOpSlider.getValue() - 1;

        importLfoParam(opIndex);
        };

    exportLfoParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::lfoFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportLfoParamButton.setWantsKeyboardFocus(true);
    exportLfoParamButton.setExplicitFocusOrder(++tabOrder);
    exportLfoParamButton.onClick = [this] {
        int opIndex = (int)targerOpSlider.getValue() - 1;

        exportLfoParam(opIndex);
        };

    importPitchEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::pitchEnvFileImport, .bgColor = juce::Colours::darkmagenta.darker(0.5f), .isReset = false, .isResized = false });
    importPitchEnvParamButton.setWantsKeyboardFocus(true);
    importPitchEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importPitchEnvParamButton.onClick = [this] {
        int opIndex = (int)targerOpSlider.getValue() - 1;

        importPitchEnvParam(opIndex);
        };

    exportPitchEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::pitchEnvFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportPitchEnvParamButton.setWantsKeyboardFocus(true);
    exportPitchEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportPitchEnvParamButton.onClick = [this] {
        int opIndex = (int)targerOpSlider.getValue() - 1;

        exportPitchEnvParam(opIndex);
        };

    importSsgSwEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::ssgSwEnvFileImport, .bgColor = juce::Colours::darkmagenta.darker(0.5f), .isReset = false, .isResized = false });
    importSsgSwEnvParamButton.setWantsKeyboardFocus(true);
    importSsgSwEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importSsgSwEnvParamButton.onClick = [this] {
        int opIndex = (int)targerOpSlider.getValue() - 1;

        importSsgSwEnvParam(opIndex);
        };

    exportSsgSwEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::ssgSwEnvFileExport, .bgColor = juce::Colours::darkblue, .isReset = false, .isResized = false });
    exportSsgSwEnvParamButton.setWantsKeyboardFocus(true);
    exportSsgSwEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportSsgSwEnvParamButton.onClick = [this] {
        int opIndex = (int)targerOpSlider.getValue() - 1;

        exportSsgSwEnvParam(opIndex);
        };

    targerOpSlider.setup({ .parent = mainGroup.contentCanvas, .title = "Op", .isReset = false });
    targerOpSlider.setRange(1.0, 2.0, 1.0);
    targerOpSlider.setNumDecimalPlacesToDisplay(0);
    targerOpSlider.setValue(1, juce::sendNotification);
    targerOpSlider.setWantsKeyboardFocus(true);
    targerOpSlider.setExplicitFocusOrder(++tabOrder);

    mainGroup.contentCanvas.addAndMakeVisible(uSep005);
    uSep005.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

    importUnisonParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::unisonFileImport, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false });
    importUnisonParamButton.setWantsKeyboardFocus(true);
    importUnisonParamButton.setExplicitFocusOrder(++tabOrder);
    importUnisonParamButton.onClick = [this] {
        importUnisonParam();
        };

    exportUnisonParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::unisonFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportUnisonParamButton.setWantsKeyboardFocus(true);
    exportUnisonParamButton.setExplicitFocusOrder(++tabOrder);
    exportUnisonParamButton.onClick = [this] {
        exportUnisonParam();
        };

    importQualityParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::qualityFileImport, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false });
    importQualityParamButton.setWantsKeyboardFocus(true);
    importQualityParamButton.setExplicitFocusOrder(++tabOrder);
    importQualityParamButton.onClick = [this] {
        importQualityParam();
        };

    exportQualityParamButton.setup({ .parent = mainGroup.contentCanvas, .title = OplGuiText::Utility::qualityFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportQualityParamButton.setWantsKeyboardFocus(true);
    exportQualityParamButton.setExplicitFocusOrder(++tabOrder);
    exportQualityParamButton.onClick = [this] {
        exportQualityParam();
        };

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    for (int i = 0; i < OplPrValue::algorithms; ++i)
    {
        juce::String fileName = juce::String::formatted(Io::Folder::asset + "/" + Io::Folder::resource + "/ALG_OPL_%02d.png", i);
        auto imgFile = docDir.getChildFile(fileName);

        if (imgFile.existsAsFile()) {
            algImages[i] = juce::ImageFileFormat::loadFrom(imgFile);
        }
    }

    // 画像コンポーネントを画面に追加
    mainGroup.contentCanvas.addAndMakeVisible(algImageComp);

    const juce::String opCode = code + OplPrKey::op;

    for (int i = 0; i < OplPrValue::ops; ++i)
    {
        opGroups[i].setup(*this, OplGuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        catDet[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::visibleDetune, .invisibleTitle = OplGuiText::Category::invisibleDetune, .enableChangeDetailVisible = true });
        catAmp[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::visibleAmpEnv, .invisibleTitle = OplGuiText::Category::invisibleAmpEnv, .enableChangeDetailVisible = true });

        mul[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::mul, .title = OplGuiText::Fm::Op::Mul, .items = multems, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);

        rgAr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::rgAr, .title = OplGuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgDr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::rgDr, .title = OplGuiText::Fm::Op::Dr, .isReset = true });
        rgDr[i].setWantsKeyboardFocus(true);
        rgDr[i].setExplicitFocusOrder(++tabOrder);

        rgSl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::rgSl, .title = OplGuiText::Fm::Op::Sl, .isReset = true });
        rgSl[i].setWantsKeyboardFocus(true);
        rgSl[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::rgRr, .title = OplGuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::rgTl, .title = OplGuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        egType[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::egType, .title = OplGuiText::Fm::Op::EgType, .isReset = true });
        egType[i].setWantsKeyboardFocus(true);
        egType[i].setExplicitFocusOrder(++tabOrder);

        sus[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::sus, .title = OplGuiText::Fm::Op::sus, .isReset = true });

        ksCat[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::visibleKs, .invisibleTitle = OplGuiText::Category::invisibleKs, .enableChangeDetailVisible = true });

        ksr[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::ksr, .title = OplGuiText::Fm::Op::Ksr, .isReset = true });
        ksr[i].setWantsKeyboardFocus(true);
        ksr[i].setExplicitFocusOrder(++tabOrder);

        ksl[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::ksl, .title = OplGuiText::Fm::Op::Ksl, .items = kslItems, .isReset = true });
        ksl[i].setWantsKeyboardFocus(true);
        ksl[i].setExplicitFocusOrder(++tabOrder);

        catOptional[i].setupSwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::visibleOptional, .invisibleTitle = OplGuiText::Category::invisibleOptional, .enableChangeDetailVisible = true });

        xof[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::xof, .title = OplGuiText::Fm::Op::xof, .isReset = true });
        xof[i].setWantsKeyboardFocus(true);
        xof[i].setExplicitFocusOrder(++tabOrder);

        kor[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::kor, .title = OplGuiText::Fm::Op::kor, .isReset = true });
        kor[i].setWantsKeyboardFocus(true);
        kor[i].setExplicitFocusOrder(++tabOrder);

        bypass[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::ampBypass, .title = OplGuiText::Fm::Op::bypass, .isReset = true });
        bypass[i].setWantsKeyboardFocus(true);
        bypass[i].setExplicitFocusOrder(++tabOrder);

        pitchEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, OplPrKey::PitchAdsr::enable, OplGuiText::PitchAdsr::enable, true);

        ssgSwEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, OplPrKey::SsgSwEnv::enable, OplGuiText::SsgSwEnv::enable, true);

        catShape[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::visibleEg, .invisibleTitle = OplGuiText::Category::invisibleEg, .enableChangeDetailVisible = true });

        eg[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::eg, .title = OplGuiText::Fm::Op::Eg, .items = oplEgItems, .isReset = true });
        eg[i].setWantsKeyboardFocus(true);
        eg[i].setExplicitFocusOrder(++tabOrder);

        catLfo[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::visibleLfo, .invisibleTitle = OplGuiText::Category::invisibleLfo, .enableChangeDetailVisible = true });

        am[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::am, .title = OplGuiText::Fm::Op::Am, .isReset = true });
        am[i].setWantsKeyboardFocus(true);
        am[i].setExplicitFocusOrder(++tabOrder);

        ams[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::ams, .title = OplGuiText::Fm::Op::Ams, .isReset = true });

        amsTo37[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->3.7Hz", .isReset = false, .isResized = false });
        amsTo37[i].setWantsKeyboardFocus(true);
        amsTo37[i].setExplicitFocusOrder(++tabOrder);
        amsTo37[i].onClick = [this, index = i] { ams[index].setValue(3.7, juce::sendNotification); };

        amsTo606[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->6.06Hz", .isReset = false, .isResized = false });
        amsTo606[i].setWantsKeyboardFocus(true);
        amsTo606[i].setExplicitFocusOrder(++tabOrder);
        amsTo606[i].onClick = [this, index = i] { ams[index].setValue(6.06, juce::sendNotification); };

        amd[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::amd, .title = OplGuiText::Fm::Op::Amd, .isReset = true });
        amd[i].setWantsKeyboardFocus(true);
        amd[i].setExplicitFocusOrder(++tabOrder);

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

        vib[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::vib, .title = OplGuiText::Fm::Op::Vib, .isReset = true });
        vib[i].setWantsKeyboardFocus(true);
        vib[i].setExplicitFocusOrder(++tabOrder);

        pms[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::pms, .title = OplGuiText::Fm::Op::Pms, .isReset = true });
        pms[i].setWantsKeyboardFocus(true);
        pms[i].setExplicitFocusOrder(++tabOrder);

        pmsTo606[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->6.06Hz", .isReset = false, .isResized = false });
        pmsTo606[i].setWantsKeyboardFocus(true);
        pmsTo606[i].setExplicitFocusOrder(++tabOrder);
        pmsTo606[i].onClick = [this, index = i] { pms[index].setValue(6.06, juce::sendNotification); };

        pmsTo64[i].setup(GuiTextButton::Config{ .parent = opGroups[i].contentCanvas, .title = "->6.4Hz", .isReset = false, .isResized = false });
        pmsTo64[i].setWantsKeyboardFocus(true);
        pmsTo64[i].setExplicitFocusOrder(++tabOrder);
        pmsTo64[i].onClick = [this, index = i] { pms[index].setValue(6.4, juce::sendNotification); };

        pmd[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::pmd, .title = OplGuiText::Fm::Op::Pmd, .isReset = true });
        pmd[i].setWantsKeyboardFocus(true);
        pmd[i].setExplicitFocusOrder(++tabOrder);

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

        catMask[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OplGuiText::Category::visibleMask, .invisibleTitle = OplGuiText::Category::invisibleMask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OplPrKey::mask, .title = OplGuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        opGroups[i].contentCanvas.addAndMakeVisible(mmlSeparator[i]);
        mmlSeparator[i].setup({.lineThick = 2.0f, .lineColour = juce::Colours::grey});

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

    auto mainArea = pageArea.removeFromLeft(OplGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(OplGuiValue::Group::Padding::width, OplGuiValue::Group::Padding::height);
    mmRect.removeFromTop(OplGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mmRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mmRect.removeFromTop(OplGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.viewport.getMaximumVisibleWidth(), 2000);

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });

    mRect.removeFromTop(OplGuiValue::Category::paddingTop);

    auto imgArea = mRect.removeFromTop(120);
    algImageComp.setBounds(imgArea);

    mRect.removeFromTop(OplGuiValue::Category::paddingTop);

    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);
    // --- B. Operators Section ---
    for (int i = 0; i < OplPrValue::ops; ++i)
    {
        auto opArea = pageArea.removeFromLeft(OplGuiValue::Fm::Op::width);
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

        layoutOpDetCat(i, innerRect);

        updateRgDisplayAsOp(i, true);

        layoutOpAmpCat(i, innerRect);

        layoutOpEgCat(i, innerRect);

        layoutOpKsCat(i, innerRect);

        layoutOpOptionalCat(i, innerRect);

        pitchEnv[i].layoutComponentRow(innerRect);

        ssgSwEnv[i].layoutComponentRow(innerRect);

        layoutOpLfoCat(i, innerRect);

        layoutOpMaskCat(i, innerRect);

        // 区切り線エリアを確保
        auto mmlSeparatorArea = innerRect.removeFromTop(OplGuiValue::ParamGroup::Separator::height);
        mmlSeparator[i].setBounds(mmlSeparatorArea);

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

    for (int i = 0; i < OplPrValue::ops; ++i)
    {
        juce::String newTitle = OplGuiText::Group::opPrefix + juce::String(i + 1) + algOpPrefix[algIndex][i];

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

void GuiOpl::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].setVisibleWithLabel(rgMode);
    rgDr[idx].setVisibleWithLabel(rgMode);
    rgSl[idx].setVisibleWithLabel(rgMode);
    rgRr[idx].setVisibleWithLabel(rgMode);
    rgTl[idx].setVisibleWithLabel(rgMode);
}

void GuiOpl::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
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
    }
}

void GuiOpl::layoutQualityCat(juce::Rectangle<int>& rect) {
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
    importLfoParamButton.setVisible(visible);
    exportLfoParamButton.setVisible(visible);
    importPitchEnvParamButton.setVisible(visible);
    exportPitchEnvParamButton.setVisible(visible);
    importSsgSwEnvParamButton.setVisible(visible);
    exportSsgSwEnvParamButton.setVisible(visible);
    targerOpSlider.setVisibleWithLabel(visible);
    uSep005.setVisible(visible);
    importUnisonParamButton.setVisible(visible);
    exportUnisonParamButton.setVisible(visible);
    importQualityParamButton.setVisible(visible);
    exportQualityParamButton.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });
        
        auto uSep001Area = rect.removeFromTop(4);
        uSep001.setBounds(uSep001Area);

        layoutMain({ .mainRect = rect, .component = &initLfoToOplBtn });
        layoutMain({ .mainRect = rect, .component = &initLfoToOpllBtn });

        auto uSep002Area = rect.removeFromTop(4);
        uSep002.setBounds(uSep002Area);

        layoutMain({ .mainRect = rect, .component = &copyOpParamToOpl3Btn });
        layoutMain({ .mainRect = rect, .component = &copyOpParamToOpl312Btn });
        layoutMain({ .mainRect = rect, .component = &copyOpParamToOpl334Btn });

        auto uSep003Area = rect.removeFromTop(4);
        uSep003.setBounds(uSep003Area);

        layoutMain({ .mainRect = rect, .component = &copyOpParamBtn });
        layoutMain({ .mainRect = rect, .label = &copyOpFromSlider.label, .component = &copyOpFromSlider });
        layoutMain({ .mainRect = rect, .label = &copyOpToSlider.label, .component = &copyOpToSlider });

        auto uSep004Area = rect.removeFromTop(4);
        uSep004.setBounds(uSep004Area);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importLfoParamButton, .comp2 = &exportLfoParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importPitchEnvParamButton, .comp2 = &exportPitchEnvParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importSsgSwEnvParamButton, .comp2 = &exportSsgSwEnvParamButton });

        rect.removeFromTop(4);

        layoutMain({ .mainRect = rect, .label = &targerOpSlider.label, .component = &targerOpSlider });

        auto uSep005Area = rect.removeFromTop(4);
        uSep005.setBounds(uSep005Area);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importUnisonParamButton, .comp2 = &exportUnisonParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importQualityParamButton, .comp2 = &exportQualityParamButton });
    }
}

void GuiOpl::layoutOpLfoCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catLfo[opIndex] });

    bool visible = catLfo[opIndex].isDetailVisible();

    am[opIndex].setVisible(visible);
    ams[opIndex].setVisibleWithLabel(visible);
    amsTo37[opIndex].setVisible(visible);
    amsTo606[opIndex].setVisible(visible);
    amd[opIndex].setVisibleWithLabel(visible);
    amdTo1[opIndex].setVisible(visible);
    amdTo12[opIndex].setVisible(visible);
    amdTo48[opIndex].setVisible(visible);
    vib[opIndex].setVisible(visible);
    pms[opIndex].setVisibleWithLabel(visible);
    pmsTo606[opIndex].setVisible(visible);
    pmsTo64[opIndex].setVisible(visible);
    pmd[opIndex].setVisibleWithLabel(visible);
    pmdTo137[opIndex].setVisible(visible);
    pmdTo14[opIndex].setVisible(visible);
    pmdTo7[opIndex].setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &am[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ams[opIndex].label, .component = &ams[opIndex] });
        layoutRowTwoComps({ .rect = rect, .comp1 = &amsTo37[opIndex], .comp2 = &amsTo606[opIndex] });
        layoutRow({ .rowRect = rect, .label = &amd[opIndex].label, .component = &amd[opIndex] });
        layoutRowThreeComps({ .rect = rect, .comp1 = &amdTo1[opIndex], .comp2 = &amdTo12[opIndex], .comp3 = &amdTo48[opIndex] });
        layoutRow({ .rowRect = rect, .component = &vib[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pms[opIndex].label, .component = &pms[opIndex] });
        layoutRowTwoComps({ .rect = rect, .comp1 = &pmsTo606[opIndex], .comp2 = &pmsTo64[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pmd[opIndex].label, .component = &pmd[opIndex] });
        layoutRowThreeComps({ .rect = rect, .comp1 = &pmdTo7[opIndex], .comp2 = &pmdTo137[opIndex], .comp3 = &pmdTo14[opIndex] });
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
    }
}

void GuiOpl::layoutOpDetCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catDet[opIndex] });

    bool visible = catDet[opIndex].isDetailVisible();

    mul[opIndex].setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &mul[opIndex].label, .component = &mul[opIndex] });
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
    }
}

void GuiOpl::layoutOpEgCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catShape[opIndex] });

    bool visible = catShape[opIndex].isDetailVisible();

    eg[opIndex].setVisibleWithLabel(visible);

    if (visible) {
        layoutRow({ .rowRect = rect, .label = &eg[opIndex].label, .component = &eg[opIndex] });
    }
}

void GuiOpl::setupGraph(int opIndex)
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
    rgSl[opIndex].onValueChange = repaintGraph;
    rgRr[opIndex].onValueChange = repaintGraph;
    rgTl[opIndex].onValueChange = repaintGraph;
    sus[opIndex].onStateChange = repaintGraph;

    pitchEnv[opIndex].setupGraph(repaintGraph);
    ssgSwEnv[opIndex].setupGraph(repaintGraph);

    addAndMakeVisible(graphSeparator[opIndex]);
    graphSeparator[opIndex].setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });
}

void GuiOpl::setGraphMode(int opIndex, GraphMode mode)
{
    currentGraphMode[opIndex] = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp[opIndex].setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch[opIndex].setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg[opIndex].setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateOpGraph(opIndex);
}

void GuiOpl::layoutOpGraph(int opIndex, juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(OplGuiValue::ParamGroup::Graph::height + OplGuiValue::ParamGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(OplGuiValue::ParamGroup::Separator::height);

    graphSeparator[opIndex].setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(OplGuiValue::ParamGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg[opIndex].setBounds(btnArea);

    // 残りをグラフエリアにする
    opGraphs[opIndex].setBounds(mainArea);
}

// グラフを再計算して描画
void GuiOpl::updateOpGraph(int opIndex)
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
    }
}

void GuiOpl::setLevel(float level) {
    levelSlider.setValue(level, juce::NotificationType::sendNotification);
}

void GuiOpl::copyParams(CopyOpl& copyObj) {
    copyObj.quality.depth = bitSelector.getSelectedId();
    copyObj.quality.rate = rateSelector.getSelectedId();
    copyObj.fmBase.level = levelSlider.getValue();
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
    bitSelector.setSelectedId(copyObj.quality.depth, juce::sendNotification);
    rateSelector.setSelectedId(copyObj.quality.rate, juce::sendNotification);
    levelSlider.setValue(copyObj.fmBase.level, juce::sendNotification);
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
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importLfoParamFile, defaultDir, Io::ExtensionGlob::OplLfoParam);
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

                vib[opIndex].setToggleState(lines[0].getIntValue() == 1, juce::sendNotification);
                pms[opIndex].setValue(lines[1].getFloatValue(), juce::sendNotification);
                pmd[opIndex].setValue(lines[2].getFloatValue(), juce::sendNotification);
                am[opIndex].setToggleState(lines[3].getIntValue() == 1, juce::sendNotification);
                ams[opIndex].setValue(lines[4].getFloatValue(), juce::sendNotification);
                amd[opIndex].setValue(lines[5].getFloatValue(), juce::sendNotification);
            }
        });
}

void GuiOpl::exportLfoParam(int opIndex) {
    juce::File defaultDir(ctx.audioProcessor.defaultLfoParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportLfoParamFile, defaultDir.getChildFile("default.lfoOpl"), Io::ExtensionGlob::OplLfoParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, opIndex](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultLfoParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += juce::String(vib[opIndex].getToggleState() ? 1 : 0) + "\n";
                content += juce::String(pms[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(pmd[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(am[opIndex].getToggleState() ? 1 : 0) + "\n";
                content += juce::String(ams[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(amd[opIndex].getValue(), Global::floatDecimalPlaces) + "\n";

                file.replaceWithText(content);
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

void GuiOpl::importUnisonParam() {
    unisonComponent.importParams();
}

void GuiOpl::exportUnisonParam() {
    unisonComponent.exportParams();
}

void GuiOpl::importQualityParam() {
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

                bitSelector.setSelectedItemIndex(lines[0].getIntValue(), juce::sendNotification);
                rateSelector.setSelectedItemIndex(lines[1].getIntValue(), juce::sendNotification);
            }
        });
}

void GuiOpl::exportQualityParam() {
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

                content += juce::String(bitSelector.getSelectedItemIndex()) + "\n";
                content += juce::String(rateSelector.getSelectedItemIndex()) + "\n";

                file.replaceWithText(content);
            }
        });
}
