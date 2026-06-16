#include <vector>

#include "./GuiOpm.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Opm/ProcessorOpmKeys.h"
#include "../../Processor/Opm/ProcessorOpmValues.h"
#include "../../Effect/Envelope/Amp/FmRgAdddr/EnvFmRgAdddrParams.h"
#include "../../Core/Const/ConstMmlKeys.h"
#include "../../Core/Const/ConstMmlValues.h"

#include "../../Core/Fm/FmRegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiOpmValues.h"
#include "./GuiOpmText.h"
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

        mulRatio[idx].setEnabled(enableMulRatio);
        mulRatio[idx].label.setEnabled(enableMulRatio);
        };

    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    const juce::String code = OpmPrKey::prefix;
    int tabOrder = 1;

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    mainGroup.setup(*this, OpmGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(juce::FontOptions(18.0f)));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    levelSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::level, .title = OpmGuiText::Fm::level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    qualityCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpmGuiText::Category::visibleQuality, .invisibleTitle = OpmGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::bit, .title = OpmGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::rate, .title = OpmGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpmGuiText::Category::algFb });

    algSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::alg, .title = OpmGuiText::Fm::alg, .items = opmAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    feedbackSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::fb, .title = OpmGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    panCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpmGuiText::Category::visiblePan, .invisibleTitle = OpmGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::pan, .title = OpmGuiText::Fm::pan, .isReset = true });
    panSlider.setWantsKeyboardFocus(true);
    panSlider.setExplicitFocusOrder(++tabOrder);

    setupPanBtn(panToLBtn, OpmGuiText::Fm::Pan::l, tabOrder);
    panToLBtn.onClick = [this] {
        panSlider.setValue(-1, juce::sendNotification);
        };

    setupPanBtn(panToCBtn, OpmGuiText::Fm::Pan::c, tabOrder);
    panToCBtn.onClick = [this] {
        panSlider.setValue(0, juce::sendNotification);
        };

    setupPanBtn(panToRBtn, OpmGuiText::Fm::Pan::r, tabOrder);
    panToRBtn.onClick = [this] {
        panSlider.setValue(1, juce::sendNotification);
        };

    lfoCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = OpmGuiText::Category::visibleHwLfo, .invisibleTitle = OpmGuiText::Category::invisibileHwLfo, .enableChangeDetailVisible = true });

    lfoFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::Lfo::freq, .title = OpmGuiText::Fm::lfoFreq, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::Lfo::amSmoothRatio, .title = OpmGuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::Lfo::syncDelay, .title = OpmGuiText::Fm::lfoSyncDelay, .isReset = true });
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

    lfoPmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::Lfo::pm, .title = OpmGuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPgShapeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::Lfo::pgShape, .title = OpmGuiText::Fm::lfoPgShape, .items = lfoPgShapeItems, .isReset = true });
    lfoPgShapeSelector.setWantsKeyboardFocus(true);
    lfoPgShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmsSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::Lfo::pms, .title = OpmGuiText::Fm::pms, .items = pmsItems, .isReset = true });
    lfoPmsSelector.setWantsKeyboardFocus(true);
    lfoPmsSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::Lfo::pmd, .title = OpmGuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::Lfo::am, .title = OpmGuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoEgShapeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::Lfo::egShape, .title = OpmGuiText::Fm::lfoEgShape, .items = lfoEgShapeItems, .isReset = true });
    lfoEgShapeSelector.setWantsKeyboardFocus(true);
    lfoEgShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmsSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::Lfo::ams, .title = OpmGuiText::Fm::ams, .items = amsItems, .isReset = true });
    lfoAmsSelector.setWantsKeyboardFocus(true);
    lfoAmsSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + OpmPrKey::Lfo::amd, .title = OpmGuiText::Fm::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = OpmGuiText::Category::visibleUtil, .invisibleTitle = OpmGuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = OpmGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelSlider.getValue();

        ctx.editor.breadcastLevel(level);
        };

    auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    for (int i = 0; i < OpmPrValue::algorithms; ++i)
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
    const juce::String opCode = code + OpmPrKey::op;

    for (int i = 0; i < OpmPrValue::ops; ++i)
    {
        opGroups[i].setup(*this, OpmGuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        mul[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::mul, .title = OpmGuiText::Fm::Op::Mul, .items = multems, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);
        mul[i].onChange = [this, i, updateMulRatioEnable] {
            updateMulRatioEnable(i);
            };

        mulRatio[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::mulRatio, .title = OpmGuiText::Fm::Op::MulRatio, .isReset = true });
        mulRatio[i].setWantsKeyboardFocus(true);
        mulRatio[i].setExplicitFocusOrder(++tabOrder);

        dt1[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::dt, .title = OpmGuiText::Fm::Op::Dt1, .isReset = true, .regType = RegisterType::FmDt });
        dt1[i].setWantsKeyboardFocus(true);
        dt1[i].setExplicitFocusOrder(++tabOrder);

        dt2[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::dt2, .title = OpmGuiText::Fm::Op::Dt2, .isReset = true, .regType = RegisterType::FmDt2 });
        dt2[i].setWantsKeyboardFocus(true);
        dt2[i].setExplicitFocusOrder(++tabOrder);

        rgAr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::rgAr, .title = OpmGuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgD1r[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::rgD1r, .title = OpmGuiText::Fm::Op::D1r, .isReset = true });
        rgD1r[i].setWantsKeyboardFocus(true);
        rgD1r[i].setExplicitFocusOrder(++tabOrder);

        rgD1l[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::rgD1l, .title = OpmGuiText::Fm::Op::D1l, .isReset = true });
        rgD1l[i].setWantsKeyboardFocus(true);
        rgD1l[i].setExplicitFocusOrder(++tabOrder);

        rgD2r[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::rgD2r, .title = OpmGuiText::Fm::Op::D2r, .isReset = true });
        rgD2r[i].setWantsKeyboardFocus(true);
        rgD2r[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::rgRr, .title = OpmGuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::rgTl, .title = OpmGuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        ksCat[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpmGuiText::Category::visibleKs, .invisibleTitle = OpmGuiText::Category::invisibleKs, .enableChangeDetailVisible = true });

        ksMode[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::ksMode, .title = "Mode", .items = ksModeItems, .isReset = true });
        ksMode[i].setWantsKeyboardFocus(true);
        ksMode[i].setExplicitFocusOrder(++tabOrder);
        ksMode[i].onChange = [this]() {
            ctx.editor.resized();
            };

        ks[i].setup(GuiComboBox::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::ks, .title = OpmGuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });
        ks[i].setWantsKeyboardFocus(true);
        ks[i].setExplicitFocusOrder(++tabOrder);

        ksrOPP[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::ksrOPP, .title = OpmGuiText::Fm::Op::Ksr, .isReset = true });
        ksrOPP[i].setWantsKeyboardFocus(true);
        ksrOPP[i].setExplicitFocusOrder(++tabOrder);

        kslOPP[i].setup(GuiSlider::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::kslOPP, .title = OpmGuiText::Fm::Op::Ksl, .isReset = true });
        kslOPP[i].setWantsKeyboardFocus(true);
        kslOPP[i].setExplicitFocusOrder(++tabOrder);

        catOptional[i].setupSwCategory({ .parent = opGroups[i].contentCanvas, .title = OpmGuiText::Category::visibleOptional, .invisibleTitle = OpmGuiText::Category::invisibleOptional, .enableChangeDetailVisible = true });

        xof[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::xof, .title = OpmGuiText::Fm::Op::xof, .isReset = true });
        xof[i].setWantsKeyboardFocus(true);
        xof[i].setExplicitFocusOrder(++tabOrder);

        kor[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::kor, .title = OpmGuiText::Fm::Op::kor, .isReset = true });
        kor[i].setWantsKeyboardFocus(true);
        kor[i].setExplicitFocusOrder(++tabOrder);

        bypass[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::ampBypass, .title = OpmGuiText::Fm::Op::bypass, .isReset = true });
        bypass[i].setWantsKeyboardFocus(true);
        bypass[i].setExplicitFocusOrder(++tabOrder);

        pitchEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, OpmPrKey::PitchAdsr::enable, OpmGuiText::PitchAdsr::enable, true);

        ssgSwEnv[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, OpmPrKey::SsgSwEnv::enable, OpmGuiText::SsgSwEnv::enable, true);

        cafLfo[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpmGuiText::Category::visibleHwLfo, .invisibleTitle = OpmGuiText::Category::invisibileHwLfo, .enableChangeDetailVisible = true });

        amsEnable[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::amsEn, .title = OpmGuiText::Fm::Op::AmsEn, .isReset = true });
        amsEnable[i].setWantsKeyboardFocus(true);
        amsEnable[i].setExplicitFocusOrder(++tabOrder);

        fix[i].setupComponent(opGroups[i].contentCanvas, paramPrefix, tabOrder, OpmGuiText::Fm::Op::Opzx7FreqTo440, 440, true);

        catMask[i].setupHwCategory({ .parent = opGroups[i].contentCanvas, .title = OpmGuiText::Category::visibleMask, .invisibleTitle = OpmGuiText::Category::invisibleMask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = opGroups[i].contentCanvas, .id = paramPrefix + OpmPrKey::mask, .title = OpmGuiText::Fm::Op::Mask, .isReset = true });
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

        updateMulRatioEnable(i);

        setupGraph(i);
        updateOpGraph(i);
    }
}

void GuiOpm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(OpmGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mmRect = mainArea.reduced(OpmGuiValue::Group::Padding::width, OpmGuiValue::Group::Padding::height);
    mmRect.removeFromTop(OpmGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mmRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mmRect.removeFromTop(OpmGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.viewport.getMaximumVisibleWidth(), 2000);

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });

    mRect.removeFromTop(OpmGuiValue::Category::paddingTop);

    auto imgArea = mRect.removeFromTop(120);
    algImageComp.setBounds(imgArea);

    mRect.removeFromTop(OpmGuiValue::Category::paddingTop);

    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    layoutPanCat(mRect);

    layoutHwLfoCat(mRect);

    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < OpmPrValue::ops; ++i)
    {
        auto opArea = pageArea.removeFromLeft(OpmGuiValue::Fm::Op::width);

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
        juce::Rectangle<int> innerRect(0, 0, opGroups[i].viewport.getMaximumVisibleWidth(), 2000);

        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        layoutRow({ .rowRect = innerRect, .label = &mulRatio[i].label, .component = &mulRatio[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt1[i].label, .component = &dt1[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt2[i].label, .component = &dt2[i] });
        updateRgDisplayAsOp(i, true);
        layoutRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgD1r[i].label, .component = &rgD1r[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgD1l[i].label, .component = &rgD1l[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgD2r[i].label, .component = &rgD2r[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i] });

        layoutOpKsCat(i, innerRect);

        layoutOpOptionalCat(i, innerRect);

        pitchEnv[i].layoutComponentRow(innerRect);

        ssgSwEnv[i].layoutComponentRow(innerRect);

        layoutOpHwLfoCat(i, innerRect);

        fix[i].layoutComponentRow(innerRect);

        layoutOpMaskCat(i, innerRect);

        // 区切り線エリアを確保
        auto mmlSeparatorArea = innerRect.removeFromTop(OpmGuiValue::ParamGroup::Separator::height);
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

    for (int i = 0; i < OpmPrValue::ops; ++i)
    {
        juce::String newTitle = OpmGuiText::Group::opPrefix + juce::String(i + 1) + algOpPrefix[algIndex][i];

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

void GuiOpm::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].setVisibleWithLabel(rgMode);
    rgD1r[idx].setVisibleWithLabel(rgMode);
    rgD1l[idx].setVisibleWithLabel(rgMode);
    rgD2r[idx].setVisibleWithLabel(rgMode);
    rgRr[idx].setVisibleWithLabel(rgMode);
    rgTl[idx].setVisibleWithLabel(rgMode);
}

void GuiOpm::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
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

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });
    }
}

void GuiOpm::layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask[opIndex] });

    bool visibleMask = catMask[opIndex].isDetailVisible();

    mask[opIndex].setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask[opIndex] });
    }
}

void GuiOpm::layoutQualityCat(juce::Rectangle<int>& rect) {
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
    }
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
    lfoPmToggle.setVisible(visible);
    lfoPgShapeSelector.setVisibleWithLabel(visible);
    lfoPmsSelector.setVisibleWithLabel(visible);
    lfoPmdSlider.setVisibleWithLabel(visible);
    lfoAmToggle.setVisible(visible);
    lfoEgShapeSelector.setVisibleWithLabel(visible);
    lfoAmsSelector.setVisibleWithLabel(visible);
    lfoAmdSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &lfoFreqSlider.label, .component = &lfoFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
        layoutMain({ .mainRect = rect, .label = &lfoSyncDelaySlider.label, .component = &lfoSyncDelaySlider });
        layoutMainTwoComps({ .rect = rect, .comp1 = &lfoSyncDelayToZeroBtn, .comp2 = &lfoSyncDelayToOneBtn });
        layoutMain({ .mainRect = rect, .component = &lfoPmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoPgShapeSelector.label, .component = &lfoPgShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoPmsSelector.label, .component = &lfoPmsSelector });
        layoutMain({ .mainRect = rect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
        layoutMain({ .mainRect = rect, .component = &lfoAmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoEgShapeSelector.label, .component = &lfoEgShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoAmsSelector.label, .component = &lfoAmsSelector });
        layoutMain({ .mainRect = rect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });
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
    }
}

void GuiOpm::layoutOpKsCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &ksCat[opIndex] });

    bool visible = ksCat[opIndex].isDetailVisible();
    FmRgAdddrKeyScaleMode mode = (FmRgAdddrKeyScaleMode)(ksMode[opIndex].getSelectedItemIndex());

    ksMode[opIndex].setVisibleWithLabel(visible);
    ks[opIndex].setVisibleWithLabel(visible && mode == FmRgAdddrKeyScaleMode::OPM);
    ksrOPP[opIndex].setVisible(visible && mode == FmRgAdddrKeyScaleMode::OPP);
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
    }
}

void GuiOpm::setupGraph(int opIndex)
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
    rgD1r[opIndex].onValueChange = repaintGraph;
    rgD2r[opIndex].onValueChange = repaintGraph;
    rgD1l[opIndex].onValueChange = repaintGraph;
    rgRr[opIndex].onValueChange = repaintGraph;
    rgTl[opIndex].onValueChange = repaintGraph;

    pitchEnv[opIndex].setupGraph(repaintGraph);
    ssgSwEnv[opIndex].setupGraph(repaintGraph);

    addAndMakeVisible(graphSeparator[opIndex]);
    graphSeparator[opIndex].setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });
}

void GuiOpm::setGraphMode(int opIndex, GraphMode mode)
{
    currentGraphMode[opIndex] = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp[opIndex].setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch[opIndex].setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg[opIndex].setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateOpGraph(opIndex);
}

void GuiOpm::layoutOpGraph(int opIndex, juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(OpmGuiValue::ParamGroup::Graph::height + OpmGuiValue::ParamGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(OpmGuiValue::ParamGroup::Separator::height);

    graphSeparator[opIndex].setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(OpmGuiValue::ParamGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg[opIndex].setBounds(btnArea);

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
    }
}

void GuiOpm::setLevel(float level) {
    levelSlider.setValue(level, juce::NotificationType::sendNotification);
}

void GuiOpm::copyParams(CopyOpm& copyObj) {
    copyObj.quality.depth = bitSelector.getSelectedId();
    copyObj.quality.rate = rateSelector.getSelectedId();
    copyObj.fmBase.level = levelSlider.getValue();
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
    bitSelector.setSelectedId(copyObj.quality.depth, juce::sendNotification);
    rateSelector.setSelectedId(copyObj.quality.rate, juce::sendNotification);
    levelSlider.setValue(copyObj.fmBase.level, juce::sendNotification);
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
