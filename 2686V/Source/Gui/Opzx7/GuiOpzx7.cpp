#include <vector>

#include "./GuiOpzx7.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Opzx7/ProcessorOpzx7Keys.h"
#include "../../Processor/Opzx7/ProcessorOpzx7Values.h"
#include "../../Core/Const/ConstMmlKeys.h"
#include "../../Core/Const/ConstMmlValues.h"

#include "../../Core/Fm/FmRegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiOpzx7Values.h"
#include "./GuiOpzx7Text.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiOpzx7Helpers.h"

static std::vector<SelectItem> bdItems = {
    {.name = "1: 4-bit (16 steps)",  .value = 1 },
    {.name = "2: 5-bit (32 steps)",  .value = 2 },
    {.name = "3: 6-bit (64 steps)",  .value = 3 },
    {.name = "4: 8-bit (256 steps)", .value = 4 },
    {.name = "5: Raw",               .value = 5 },
    {.name = "6: 7-bit (OPLL/128 steps)", .value = 6 }
};

static std::vector<SelectItem> rateItems = {
    {.name = " 1: 96kHz",    .value = 1 },
    {.name = " 2: 55.5kHz",  .value = 2 },
    {.name = " 3: 49.7kHz",  .value = 3 },
    {.name = " 4: 48kHz",    .value = 4 },
    {.name = " 5: 44.1kHz",  .value = 5 },
    {.name = " 6: 33.08kHz", .value = 6 },
    {.name = " 7: 22.05kHz", .value = 7 },
    {.name = " 8: 16kHz",    .value = 8 },
    {.name = " 9: 12kHz",    .value = 9 },
    {.name = "10: 11kHz",    .value = 10 },
    {.name = "11: 8kHz",     .value = 11 },
    {.name = "12: 5.5kHz",   .value = 12 },
    {.name = "13: 4kHz",     .value = 13 },
    {.name = "14: 2kHz",     .value = 14 },
};

static std::vector<SelectItem> opzx7AlgItems = {
    {.name = "00: <OPX-00>", .value = 1 },
    {.name = "01: <OPX-01>", .value = 2 },
    {.name = "02: <OPX-02>", .value = 3 },
    {.name = "03: <OPX-03>", .value = 4 },
    {.name = "04: <OPX-04>", .value = 5 },
    {.name = "05: <OPX-05>", .value = 6 },
    {.name = "06: <OPX-06>", .value = 7 },
    {.name = "07: <OPX-07>", .value = 8 },
    {.name = "08: <OPX-08>", .value = 9 },
    {.name = "09: <OPX-09>", .value = 10 },
    {.name = "10: <OPX-10>", .value = 11 },
    {.name = "11: <OPX-11>", .value = 12 },
    {.name = "12: <OPX-12>", .value = 13 },
    {.name = "13: <OPX-13>", .value = 14 },
    {.name = "14: <OPX-14>", .value = 15 },
    {.name = "15: <OPX-15>", .value = 16 },
    {.name = "16: <OPX-16>", .value = 17 },
    {.name = "17: <OPX-17>", .value = 18 },
    {.name = "18: <OPX-18>", .value = 19 },
    {.name = "19: <OPX-19>", .value = 20 },
    {.name = "20: <OPX-20>", .value = 21 },
    {.name = "21: <OPX-21>", .value = 22 },
    {.name = "22: <OPX-22>", .value = 23 },
    {.name = "23: <OPX-23>", .value = 24 },
    {.name = "24: <OPX-24>", .value = 25 },
    {.name = "25: <OPX-25>", .value = 26 },
    {.name = "26: <OPX-26>", .value = 27 },
    {.name = "27: <OPX-27>", .value = 28 },
    {.name = "28: <MA7-00>", .value = 29 },
    {.name = "29: <MA7-01>", .value = 30 },
    {.name = "30: <MA7-02>", .value = 31 },
    {.name = "31: <MA7-03>", .value = 32 },
    {.name = "32: <MA7-04>", .value = 33 },
    {.name = "33: <MA7-05>", .value = 34 },
    {.name = "34: <MA7-06>", .value = 35 },
    {.name = "35: <MA7-07>", .value = 36 },
};

static std::vector<SelectItem> multems = {
    { .name = " 0: x  0.5",    .value =  1 },
    { .name = " 1: x  0.891",  .value =  2 },
    { .name = " 2: x  1",      .value =  3 },
    { .name = " 3: x  1.414",  .value =  4 },
    { .name = " 4: x  1.498",  .value =  5 },
    { .name = " 5: x  1.581",  .value =  6 },
    { .name = " 6: x  1.781",  .value =  7 },
    { .name = " 7: x  2",      .value =  8 },
    { .name = " 8: x  3",      .value =  9 },
    { .name = " 9: x  4",      .value = 10 },
    { .name = "10: x  5",      .value = 11 },
    { .name = "11: x  6",      .value = 12 },
    { .name = "12: x  7",      .value = 13 },
    { .name = "13: x  8",      .value = 14 },
    { .name = "14: x  9",      .value = 15 },
    { .name = "15: x 10",      .value = 16 },
    { .name = "16: x 11",      .value = 17 },
    { .name = "17: x 12",      .value = 18 },
    { .name = "18: x 13",      .value = 19 },
    { .name = "19: x 14",      .value = 20 },
    { .name = "20: x 15",      .value = 21 },
    { .name = "21: Use Ratio", .value = 22 }
};

static std::vector<SelectItem> ksItems = {
    {.name = "0 OFF",      .value = 1},
    {.name = "1 (Weak)",   .value = 2},
    {.name = "2 (Mid)",    .value = 3},
    {.name = "3 (Strong)", .value = 4}
};

static std::vector<SelectItem> lfoShapeItems = {
    {.name = "0: Sine",                .value = 1 },
    {.name = "1: Saw Up",              .value = 2 },
    {.name = "2: Saw Down",            .value = 3 },
    {.name = "3: Square",              .value = 4 },
    {.name = "4: Triangle",            .value = 5 },
    {.name = "5: Sample & Hold",       .value = 6 },
    {.name = "6: Saw Down & One Shot", .value = 7 },
    {.name = "7: Triangle & One Shot", .value = 8 },
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

static std::vector<SelectItem> opzx7WsItems = {
    {.name = "00 Sine",                             .value = 1},
    {.name = "01 Half Sine",                        .value = 2},
    {.name = "02 Abs Sine",                         .value = 3},
    {.name = "03 Quadra Abs Half Sin",              .value = 4},
    {.name = "04 Alt Sine",                         .value = 5},
    {.name = "05 Alt Abs Sine",                     .value = 6},
    {.name = "06 Square",                           .value = 7},
    {.name = "07 Log Saw",                          .value = 8},
    {.name = "08 Pudding Sine",                     .value = 9},
    {.name = "09 Half Pudding Sine",                .value = 10},
    {.name = "10 Abs Pudding Sine",                 .value = 11},
    {.name = "11 Quad Abs Pudding Sine",            .value = 12},
    {.name = "12 Mini Alt Sine",                    .value = 13},
    {.name = "13 Mini Alt Abs Sine",                .value = 14},
    {.name = "14 Half Square",                      .value = 15},
    {.name = u8"15 ★★ Wavetable File",            .value = 16},
    {.name = "16 Triangle",                         .value = 17},
    {.name = "17 Half Triangle",                    .value = 18},
    {.name = "18 Abs Triangle",                     .value = 19},
    {.name = "19 Quad Abs Triangle",                .value = 20},
    {.name = "20 Alt Triangle",                     .value = 21},
    {.name = "21 Alt Abs Triangle",                 .value = 22},
    {.name = "22 Quad Half Square",                 .value = 23},
    {.name = "23 ---",                              .value = 24},
    {.name = "24 Diagram",                          .value = 25},
    {.name = "25 Half Diagram",                     .value = 26},
    {.name = "26 Abs Half Saw Up",                  .value = 27},
    {.name = "27 Quad Abs Half Saw Up",             .value = 28},
    {.name = "28 Alt Diagram",                      .value = 29},
    {.name = "29 Alt Quad Abs Half Saw Up",         .value = 30},
    {.name = "30 Quad Square",                      .value = 31},
    {.name = u8"31 ★★ PCM(Audio) File",           .value = 32},
    {.name = "32 [EX000]Alternating Abs Sine",      .value = 33},
    {.name = "33 [EX001]Derived Square",            .value = 34},
    {.name = "34 [EX002]Saw Down",                  .value = 35},
    {.name = "35 [EX003]Saw Up",                    .value = 36},
    {.name = "36 [EX004]Saw + Sine",                .value = 37},
    {.name = "37 [EX005]Pulse 25%",                 .value = 38},
    {.name = "38 [EX006]Pulse 12.5%",               .value = 39},
    {.name = "39 [EX007]Pulse 6.25%",               .value = 40},
    {.name = "40 [EX008]Round Square",              .value = 41},
    {.name = "41 [EX009]Impulse Train",             .value = 42},
    {.name = "42 [EX010]Comb / Multi-pulse",        .value = 43},
    {.name = "43 [EX011]Resonant Saw (Low)",        .value = 44},
    {.name = "44 [EX012]Resonant Saw (High)",       .value = 45},
    {.name = "45 [EX013]Resonant Triangle",         .value = 46},
    {.name = "46 [EX014]Bulb Sine",                 .value = 47},
    {.name = "47 [EX015]Double Hump",               .value = 48},
    {.name = "48 [EX016]Pseudo Voice Formant 1",    .value = 49},
    {.name = "49 [EX017]Pseudo Voice Formant 2",    .value = 50},
    {.name = "50 [EX018]Metallic 1",                .value = 51},
    {.name = "51 [EX019]Metallic 2",                .value = 52},
    {.name = "52 [EX020]Noise-Like",                .value = 53},
    {.name = "53 [EX021]PD Resonance",              .value = 54},
    {.name = "54 [EX022]PD Resonance High",         .value = 55},
    {.name = "55 [EX023]4-Step Sine",               .value = 56},
    {.name = "56 [EX024]8-Step Sine",               .value = 57},
    {.name = "57 [EX025]Wavefolded Sine (Soft)",    .value = 58},
    {.name = "58 [EX026]Wavefolded Sine (Hard)",    .value = 59},
    {.name = "59 [EX027]Bitwise XOR Fractal",       .value = 60},
    {.name = "60 [EX028]Bitwise AND Texture",       .value = 61},
    {.name = "61 [EX029]Self-Modulated Sine(FB=1)", .value = 62},
    {.name = "62 [EX030]Self-Modulated Sine(FB=2)", .value = 63},
    {.name = "63 [EX031]OPZ(TX81Z) W2",             .value = 64},
    {.name = "64 [EX032]OPZ(TX81Z) W3",             .value = 65},
    {.name = "65 [EX033]OPZ(TX81Z) W4",             .value = 66},
    {.name = "66 [EX034]OPZ(TX81Z) W5",             .value = 67},
    {.name = "67 [EX035]OPZ(TX81Z) W6",             .value = 68},
    {.name = "68 [EX036]OPZ(TX81Z) W7",             .value = 69},
    {.name = "69 [EX037]OPZ(TX81Z) W8",             .value = 70},
    {.name = "70 [EX038]Cubic Triangle",            .value = 71},
    {.name = "71 [EX039]Inverse Circle",            .value = 72},
    {.name = "72 [EX040]Exponential Spike",         .value = 73},
};

void GuiOpzx7::setup()
{
    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    auto setupPanBtn = [this](GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    auto updateMulRatioEnable = [this](int idx) {
        int mulIndex = mul[idx].getSelectedId() - 1;
        bool enableMulRatio = mulIndex == 21; // mul = Ratio

        mulRatio[idx].setEnabled(enableMulRatio);
        mulRatio[idx].label.setEnabled(enableMulRatio);
        };

    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    const juce::String code = Opzx7PrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, Opzx7GuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(juce::FontOptions(18.0f)));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    qualityCat.setupHwCategory({ .parent = *this, .title = Opzx7GuiText::Category::visibleQuality, .invisibleTitle = Opzx7GuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = *this, .id = code + Opzx7PrKey::bit, .title = Opzx7GuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + Opzx7PrKey::rate, .title = Opzx7GuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setupHwCategory({ .parent = *this, .title = Opzx7GuiText::Category::algFb });

    algSelector.setup({ .parent = *this, .id = code + Opzx7PrKey::alg, .title = Opzx7GuiText::Fm::alg, .items = opzx7AlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    feedbackSlider.setup({ .parent = *this, .id = code + Opzx7PrKey::fb, .title = Opzx7GuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    panCat.setupHwCategory({ .parent = *this, .title = Opzx7GuiText::Category::visiblePanpot, .invisibleTitle = Opzx7GuiText::Category::invisiblePanpot , .enableChangeDetailVisible = true });

    panpotEnableToggle.setup({ .parent = *this, .id = code + Opzx7PrKey::panpot_en, .title = Opzx7GuiText::Fm::panpotEnable, .isReset = true });
    panpotEnableToggle.setWantsKeyboardFocus(true);
    panpotEnableToggle.setExplicitFocusOrder(++tabOrder);

    panpotSlider.setup({ .parent = *this, .id = code + Opzx7PrKey::panpot, .title = Opzx7GuiText::Fm::panpot, .isReset = true });
    panpotSlider.setWantsKeyboardFocus(true);
    panpotSlider.setExplicitFocusOrder(++tabOrder);

    setupPanBtn(panToLBtn, Opzx7GuiText::Fm::Pan::l, tabOrder);
    panToLBtn.onClick = [this] {
        panpotEnableToggle.setToggleState(true, juce::sendNotification);
        panpotSlider.setValue(0, juce::sendNotification);
        };

    setupPanBtn(panToCBtn, Opzx7GuiText::Fm::Pan::c, tabOrder);
    panToCBtn.onClick = [this] {
        panpotEnableToggle.setToggleState(false, juce::sendNotification);
        panpotSlider.setValue(15, juce::sendNotification);
        };

    setupPanBtn(panToRBtn, Opzx7GuiText::Fm::Pan::r, tabOrder);
    panToRBtn.onClick = [this] {
        panpotEnableToggle.setToggleState(true, juce::sendNotification);
        panpotSlider.setValue(31, juce::sendNotification);
        };

    glLfo.setupComponent(*this, code, tabOrder);

    unisonComponent.setupComponent(*this, code, tabOrder);

    midiComponent.setupComponent(*this, tabOrder);

    auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    for (int i = 0; i < 36; ++i)
    {
        juce::String fileName = juce::String::formatted(Io::Folder::asset + "/" + Io::Folder::resource + "/ALG_OPZX7_%02d.png", i);
        auto imgFile = docDir.getChildFile(fileName);

        if (imgFile.existsAsFile()) {
            algImages[i] = juce::ImageFileFormat::loadFrom(imgFile);
        }
    }

    // 画像コンポーネントを画面に追加
    addAndMakeVisible(algImageComp);

    // Operators
    const juce::String opCode = code + Opzx7PrKey::op;

    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, Opzx7GuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        mul[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::mul, .title = Opzx7GuiText::Fm::Op::Mul, .items = multems, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);
        mul[i].onChange = [this, i, updateMulRatioEnable] {
            updateMulRatioEnable(i);
            };

        mulRatio[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::mulRatio, .title = Opzx7GuiText::Fm::Op::MulRatio, .isReset = true });
        mulRatio[i].setWantsKeyboardFocus(true);
        mulRatio[i].setExplicitFocusOrder(++tabOrder);

        dt1[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::dt, .title = Opzx7GuiText::Fm::Op::Dt1, .isReset = true, .regType = RegisterType::FmDt });
        dt1[i].setWantsKeyboardFocus(true);
        dt1[i].setExplicitFocusOrder(++tabOrder);

        dt2[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::dt2, .title = Opzx7GuiText::Fm::Op::Dt2, .isReset = true, .regType = RegisterType::FmDt2 });
        dt2[i].setWantsKeyboardFocus(true);
        dt2[i].setExplicitFocusOrder(++tabOrder);

        dt3[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::dt3, .title = Opzx7GuiText::Fm::Op::Dt3, .isReset = true });
        dt3[i].setWantsKeyboardFocus(true);
        dt3[i].setExplicitFocusOrder(++tabOrder);

        dt3Btns[i].setupComponent(*this, dt3[i], tabOrder);

        rgEn[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::rgEn, .title = Opzx7GuiText::Fm::Op::RgEn, .isReset = true });
        rgEn[i].setWantsKeyboardFocus(true);
        rgEn[i].setExplicitFocusOrder(++tabOrder);
        rgEn[i].onStateChange = [this, i] {
            updateOpGraph(i); // RG_ENを切り替えるときにアンプエンベロープの内容を切り替える
            ctx.editor.resized();
            };

        rgAr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::rgAr, .title = Opzx7GuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgD1r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::rgD1r, .title = Opzx7GuiText::Fm::Op::D1r, .isReset = true });
        rgD1r[i].setWantsKeyboardFocus(true);
        rgD1r[i].setExplicitFocusOrder(++tabOrder);

        rgD1l[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::rgD1l, .title = Opzx7GuiText::Fm::Op::D1l, .isReset = true });
        rgD1l[i].setWantsKeyboardFocus(true);
        rgD1l[i].setExplicitFocusOrder(++tabOrder);

        rgD2r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::rgD2r, .title = Opzx7GuiText::Fm::Op::D2r, .isReset = true });
        rgD2r[i].setWantsKeyboardFocus(true);
        rgD2r[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::rgRr, .title = Opzx7GuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::rgTl, .title = Opzx7GuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::ar, .title = Opzx7GuiText::Fm::Op::Ar, .isReset = true, .regType = RegisterType::FmAr });
        ar[i].setWantsKeyboardFocus(true);
        ar[i].setExplicitFocusOrder(++tabOrder);

        d1r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::d1r, .title = Opzx7GuiText::Fm::Op::D1r, .isReset = true, .regType = RegisterType::FmDr });
        d1r[i].setWantsKeyboardFocus(true);
        d1r[i].setExplicitFocusOrder(++tabOrder);

        d1l[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::d1l, .title = Opzx7GuiText::Fm::Op::D1l, .isReset = true, .regType = RegisterType::FmSl });
        d1l[i].setWantsKeyboardFocus(true);
        d1l[i].setExplicitFocusOrder(++tabOrder);

        d2r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::d2r, .title = Opzx7GuiText::Fm::Op::D2r, .isReset = true, .regType = RegisterType::FmSr });
        d2r[i].setWantsKeyboardFocus(true);
        d2r[i].setExplicitFocusOrder(++tabOrder);

        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::rr, .title = Opzx7GuiText::Fm::Op::Rr, .isReset = true, .regType = RegisterType::FmRr });
        rr[i].setWantsKeyboardFocus(true);
        rr[i].setExplicitFocusOrder(++tabOrder);

        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::tl, .title = Opzx7GuiText::Fm::Op::Tl, .isReset = true, .regType = RegisterType::FmTl });
        tl[i].setWantsKeyboardFocus(true);
        tl[i].setExplicitFocusOrder(++tabOrder);

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::ks, .title = Opzx7GuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });
        ks[i].setWantsKeyboardFocus(true);
        ks[i].setExplicitFocusOrder(++tabOrder);

        sus[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::sus, .title = Opzx7GuiText::Fm::Op::sus, .isReset = true });
        sus[i].setWantsKeyboardFocus(true);
        sus[i].setExplicitFocusOrder(++tabOrder);

        xof[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::xof, .title = Opzx7GuiText::Fm::Op::xof, .isReset = true });
        xof[i].setWantsKeyboardFocus(true);
        xof[i].setExplicitFocusOrder(++tabOrder);

        kor[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::kor, .title = Opzx7GuiText::Fm::Op::kor, .isReset = true });
        kor[i].setWantsKeyboardFocus(true);
        kor[i].setExplicitFocusOrder(++tabOrder);

        catWaveShape[i].setupHwCategory({ .parent = *this, .title = Opzx7GuiText::Category::waveShape });

        ws[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::ws, .title = Opzx7GuiText::Fm::Op::Ws, .items = opzx7WsItems, .isReset = true });
        ws[i].setWantsKeyboardFocus(true);
        ws[i].setExplicitFocusOrder(++tabOrder);
        ws[i].onChange = [this, i] {
            updateOnWsChange(i);

            ctx.editor.resized();
            };

        loadPcmBtn[i].setup({ .parent = *this, .title = Opzx7GuiText::File::Pcm, .isReset = false, .isResized = false });
        loadPcmBtn[i].setWantsKeyboardFocus(true);
        loadPcmBtn[i].setExplicitFocusOrder(++tabOrder);
        loadPcmBtn[i].onClick = [this, i] {
            auto fileFilter = ctx.audioProcessor.formatManager.getWildcardForAllFormats();
            ctx.editor.openFileChooser(
                "Load PCM for OP" + juce::String(i + 1),
                ctx.audioProcessor.lastSampleDirectory,
                fileFilter,
                [this, i](const juce::FileChooser& fc) {
                    auto file = fc.getResult();
                    if (file.existsAsFile()) {
                        ctx.audioProcessor.loadOpzx7PcmFile(i, file);
                        updatePcmFileName(i, file.getFileName());
                        ctx.audioProcessor.lastSampleDirectory = file.getParentDirectory();
                    }
                }
            );
            };

        clearPcmBtn[i].setup({ .parent = *this, .title = Opzx7GuiText::File::clear, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = false });
        clearPcmBtn[i].setWantsKeyboardFocus(true);
        clearPcmBtn[i].setExplicitFocusOrder(++tabOrder);
        clearPcmBtn[i].onClick = [this, i] {
            ctx.audioProcessor.unloadOpzx7PcmFile(i);
            updatePcmFileName(i, Io::empty);
            };

        pcmFileNameLabel[i].setup({ .parent = *this, .title = Io::empty });
        if (ctx.audioProcessor.opzx7PcmFilePaths[i].isNotEmpty()) {
            updatePcmFileName(i, juce::File(ctx.audioProcessor.opzx7PcmFilePaths[i]).getFileName());
        }

        pcmOffset[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::pcmOffset, .title = Opzx7GuiText::Fm::Op::PcmOffset, .isReset = true });
        pcmOffset[i].setWantsKeyboardFocus(true);
        pcmOffset[i].setExplicitFocusOrder(++tabOrder);

        pcmRatio[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::pcmRatio, .title = Opzx7GuiText::Fm::Op::PcmRatio, .isReset = true });
        pcmRatio[i].setWantsKeyboardFocus(true);
        pcmRatio[i].setExplicitFocusOrder(++tabOrder);

        loadWtBtn[i].setup({ .parent = *this, .title = "WS", .isReset = false, .isResized = false });
        loadWtBtn[i].setWantsKeyboardFocus(true);
        loadWtBtn[i].setExplicitFocusOrder(++tabOrder);
        loadWtBtn[i].onClick = [this, i] {
            ctx.editor.openFileChooser(
                "Load Wavetable for OP" + juce::String(i + 1),
                ctx.audioProcessor.defaultWavetableDir,
                "*.wt",
                [this, i](const juce::FileChooser& fc) {
                    auto file = fc.getResult();
                    if (file.existsAsFile()) {
                        ctx.audioProcessor.loadOpzx7WtFile(i, file);
                        updateWtFileName(i, file.getFileName());
                        ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();
                    }
                }
            );
            };

        clearWtBtn[i].setup({ .parent = *this, .title = Opzx7GuiText::File::clear, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = false });
        clearWtBtn[i].setWantsKeyboardFocus(true);
        clearWtBtn[i].setExplicitFocusOrder(++tabOrder);
        clearWtBtn[i].onClick = [this, i] {
            ctx.audioProcessor.unloadOpzx7WtFile(i);
            updateWtFileName(i, Io::empty);
            };

        wtFileNameLabel[i].setup({ .parent = *this, .title = Io::empty });
        if (ctx.audioProcessor.opzx7WtFilePaths[i].isNotEmpty()) {
            updateWtFileName(i, juce::File(ctx.audioProcessor.opzx7WtFilePaths[i]).getFileName());
        }

        catOptional[i].setupSwCategory({ .parent = *this, .title = Opzx7GuiText::Category::visibleOptional, .invisibleTitle = Opzx7GuiText::Category::invisibleOptional, .enableChangeDetailVisible = true });

        bypass[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::ampBypass, .title = Opzx7GuiText::Fm::Op::bypass, .isReset = true });
        bypass[i].setWantsKeyboardFocus(true);
        bypass[i].setExplicitFocusOrder(++tabOrder);

        catSsgEnv[i].setupSwCategory({ .parent = *this, .title = Opzx7GuiText::Category::visibleSsgEnv, .invisibleTitle = Opzx7GuiText::Category::invisibleSsgEnv, .enableChangeDetailVisible = true });

        se[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::se, .title = Opzx7GuiText::Fm::Op::SEnv, .items = opnaSeItems, .isReset = true });
        se[i].setWantsKeyboardFocus(true);
        se[i].setExplicitFocusOrder(++tabOrder);

        seFreq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::seFreq, .title = Opzx7GuiText::Fm::Op::SFreq, .isReset = true });
        seFreq[i].setWantsKeyboardFocus(true);
        seFreq[i].setExplicitFocusOrder(++tabOrder);

        pitchEnv[i].setupComponent(*this, paramPrefix, tabOrder, Opzx7PrKey::PitchAdsr::enable, Opzx7GuiText::PitchAdsr::enable, true);

        ssgSwEnv[i].setupComponent(*this, paramPrefix, tabOrder, Opzx7PrKey::SsgSwEnv::enable, Opzx7GuiText::SsgSwEnv::enable, true);

        lfo[i].setupComponent(*this, paramPrefix, tabOrder);

        fix[i].setupComponent(*this, paramPrefix, tabOrder, Opzx7GuiText::Fm::Op::Opzx7FreqTo440, 440);

        catMask[i].setupHwCategory({ .parent = *this, .title = Opzx7GuiText::Category::visibleMask, .invisibleTitle = Opzx7GuiText::Category::invisibleMask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::mask, .title = Opzx7GuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        addAndMakeVisible(mmlSeparator[i]);
        mmlSeparator[i].setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

        mml[i].setup({ .parent = *this, .title = juce::String("") + "MML風入力", .isReset = false, .isResized = false });
        mml[i].setWantsKeyboardFocus(true);
        mml[i].setExplicitFocusOrder(++tabOrder);
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = juce::String("") + "MML風にパラメータを入力してください。 例: AR:31 AR31 DT-1 等",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

        setupGraph(i);
        updateOpGraph(i);

        updateMulRatioEnable(i);
    }
}

void GuiOpzx7::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(Opzx7GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(Opzx7GuiValue::Group::Padding::width, Opzx7GuiValue::Group::Padding::height);
    mRect.removeFromTop(Opzx7GuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mRect.removeFromTop(Opzx7GuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });

    mRect.removeFromTop(Opzx7GuiValue::Category::paddingTop);

    auto imgArea = mRect.removeFromTop(120);
    algImageComp.setBounds(imgArea);

    mRect.removeFromTop(Opzx7GuiValue::Category::paddingTop);

    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    layoutPanpotCat(mRect);

    glLfo.layoutComponent(mRect);

    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(Opzx7GuiValue::Fm::Op::width);

        // 枠線
        opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(Opzx7GuiValue::Fm::Op::Padding::width, Opzx7GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(Opzx7GuiValue::Group::TitlePaddingTop);

        // グラフ用の区画を確保
        layoutOpGraph(i, innerRect);
        updateOpGraph(i);

        bool rgMode = rgEn[i].getToggleState();
		int selectedWs = ws[i].getSelectedId();

        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        layoutRow({ .rowRect = innerRect, .label = &mulRatio[i].label, .component = &mulRatio[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt1[i].label, .component = &dt1[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt2[i].label, .component = &dt2[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt3[i].label, .component = &dt3[i] });

        dt3Btns[i].layoutComponentRow(innerRect);

        layoutRow({ .rowRect = innerRect, .component = &rgEn[i] });
        updateRgDisplayAsOp(i, rgMode);
        if (rgMode)
        {
            layoutRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i] });
            layoutRow({ .rowRect = innerRect, .label = &rgD1r[i].label, .component = &rgD1r[i] });
            layoutRow({ .rowRect = innerRect, .label = &rgD1l[i].label, .component = &rgD1l[i] });
            layoutRow({ .rowRect = innerRect, .label = &rgD2r[i].label, .component = &rgD2r[i] });
            layoutRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i] });
            layoutRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i] });
        }
        else
        {
            layoutRow({ .rowRect = innerRect, .label = &ar[i].label, .component = &ar[i] });
            layoutRow({ .rowRect = innerRect, .label = &d1r[i].label, .component = &d1r[i] });
            layoutRow({ .rowRect = innerRect, .label = &d1l[i].label, .component = &d1l[i] });
            layoutRow({ .rowRect = innerRect, .label = &d2r[i].label, .component = &d2r[i] });
            layoutRow({ .rowRect = innerRect, .label = &rr[i].label, .component = &rr[i] });
            layoutRow({ .rowRect = innerRect, .label = &tl[i].label, .component = &tl[i] });
        }
        layoutRow({ .rowRect = innerRect, .label = &ks[i].label, .component = &ks[i] });
        layoutRow({ .rowRect = innerRect, .component = &sus[i] });
        layoutRow({ .rowRect = innerRect, .component = &xof[i] });
        layoutRow({ .rowRect = innerRect, .component = &kor[i] });

		layoutRowCategory({ .rowRect = innerRect, .component = &catWaveShape[i] });

        layoutRow({ .rowRect = innerRect, .label = &ws[i].label, .component = &ws[i] });
        if (selectedWs == 32)
        {
            layoutRowOpzx7File({ .rect = innerRect, .loadPcmBtn = &loadPcmBtn[i], .pcmFileNameLabel = &pcmFileNameLabel[i], .clearPcmBtn = &clearPcmBtn[i] });
            layoutRow({ .rowRect = innerRect, .label = &pcmOffset[i].label, .component = &pcmOffset[i] });
            layoutRow({ .rowRect = innerRect, .label = &pcmRatio[i].label, .component = &pcmRatio[i] });
        }
        if (selectedWs == 16)
        {
            layoutRowOpzx7File({ .rect = innerRect, .loadPcmBtn = &loadWtBtn[i], .pcmFileNameLabel = &wtFileNameLabel[i], .clearPcmBtn = &clearWtBtn[i] });
        }

        layoutOpOptionalCat(i, innerRect);

        layoutOpSsgEnvCat(i, innerRect);

        pitchEnv[i].layoutComponentRow(innerRect);

        ssgSwEnv[i].layoutComponentRow(innerRect);

        lfo[i].layoutComponentRow(innerRect);

        fix[i].layoutComponentRow(innerRect);

        layoutOpMaskCat(i, innerRect);

        // 区切り線エリアを確保
        auto mmlSeparatorArea = innerRect.removeFromTop(Opzx7GuiValue::ParamGroup::Separator::height);
        mmlSeparator[i].setBounds(mmlSeparatorArea);

        layoutRow({ .rowRect = innerRect, .component = &mml[i], .paddingBottom = 0 });

        updateOnWsChange(i);
    }

    updateAlgorithmDisplay();
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpzx7::applyMmlString(const juce::String& mml, int opIndex)
{
    std::vector<RegisterUnit> units = RegisterConverter::convertToRegisterUnit(mml);
    bool rgMode = rgEn[opIndex].getToggleState();

    // 文字列キーと、実行する処理(ラムダ式)とのマップ
    std::map<juce::String, std::function<void(int)>> actionMap = {
        // --- 基本パラメータ ---
        { mmlPrefixMul,  [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertFmMulOpzx7(v), juce::sendNotification); } },
        { mmlPrefixMl,   [&](int v) { mul[opIndex].setSelectedItemIndex(RegisterConverter::convertFmMulOpzx7(v), juce::sendNotification); } },
        { mmlPrefixDt,   [&](int v) { dt1[opIndex].setValue(RegisterConverter::convertFmDtOpzx7(v), juce::sendNotification); } },
        { mmlPrefixDt1,   [&](int v) { dt1[opIndex].setValue(RegisterConverter::convertFmDtOpzx7(v), juce::sendNotification); } },
        { mmlPrefixDto,   [&](int v) { dt1[opIndex].setValue(RegisterConverter::convertFmDtOpzx7(v), juce::sendNotification); } },
        { mmlPrefixDt2,  [&](int v) { dt2[opIndex].setValue(RegisterConverter::convertMmlDt2ToReg(v), juce::sendNotification); } },
        { mmlPrefixDtt,  [&](int v) { dt2[opIndex].setValue(RegisterConverter::convertMmlDt2ToReg(v), juce::sendNotification); } },
        { mmlPrefixKs,   [&](int v) { ks[opIndex].setSelectedItemIndex(RegisterConverter::convertFmKs(v), juce::sendNotification); } },
        { mmlPrefixMask, [&](int v) { mask[opIndex].setToggleState(RegisterConverter::convertFmMask(v), juce::sendNotification); } },

        // --- TL系 (RGモードで分岐) ---
        { mmlPrefixTl,   [&](int v) {
            rgMode ? rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification)
                   : tl[opIndex].setValue(RegisterConverter::convertFmTl(v), juce::sendNotification);
        }},
        { mmlPrefixO,    [&](int v) {
            rgMode ? rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification)
                   : tl[opIndex].setValue(RegisterConverter::convertFmTl(v), juce::sendNotification);
        }},
        { mmlPrefixOl,   [&](int v) {
            rgMode ? rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(v), juce::sendNotification)
                   : tl[opIndex].setValue(RegisterConverter::convertFmTl(v), juce::sendNotification);
        }},

        // --- エンベロープ系 (RGモードで分岐) ---
        { mmlPrefixAr,   [&](int v) {
            rgMode ? rgAr[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification)
                   : ar[opIndex].setValue(RegisterConverter::convertFmAr(v), juce::sendNotification);
        }},
        { mmlPrefixD1r,   [&](int v) {
            rgMode ? rgD1r[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification)
                   : d1r[opIndex].setValue(RegisterConverter::convertFmDr(v), juce::sendNotification);
        }},
        { mmlPrefixD2r,   [&](int v) {
            rgMode ? rgD2r[opIndex].setValue(RegisterConverter::convertFmRg31(v), juce::sendNotification)
                   : d2r[opIndex].setValue(RegisterConverter::convertFmSr(v), juce::sendNotification);
        }},
        { mmlPrefixD1l,   [&](int v) {
            rgMode ? rgD1l[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification)
                   : d1l[opIndex].setValue(RegisterConverter::convertFmSl(v), juce::sendNotification);
        }},
        { mmlPrefixRr,   [&](int v) {
            rgMode ? rgRr[opIndex].setValue(RegisterConverter::convertFmRg15(v), juce::sendNotification)
                   : rr[opIndex].setValue(RegisterConverter::convertFmRr(v), juce::sendNotification);
        }}
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

void GuiOpzx7::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    mul[idx].setEnabledWithLabel(enable);

    int mulIndex = mul[idx].getSelectedId() - 1;
    bool enableMulRatio = mulIndex == 21; // mul = Ratio

    mulRatio[idx].setEnabledWithLabel(enable && enableMulRatio);

    dt1[idx].setEnabledWithLabel(enable);
    dt2[idx].setEnabledWithLabel(enable);
    dt3[idx].setEnabledWithLabel(enable);
    dt3Btns[idx].setEnabled(enable);
    ar[idx].setEnabledWithLabel(enable);
    d1r[idx].setEnabledWithLabel(enable);
    d1l[idx].setEnabledWithLabel(enable);
    rr[idx].setEnabledWithLabel(enable);
    d2r[idx].setEnabledWithLabel(enable);
    tl[idx].setEnabledWithLabel(enable);
    ks[idx].setEnabledWithLabel(enable);
    se[idx].setEnabledWithLabel(enable);
    seFreq[idx].setEnabledWithLabel(enable);
    catOptional[idx].setEnabled(enable);
    bypass[idx].setEnabled(enable);
    fix[idx].setEnables(enable);
    lfo[idx].setEnabled(enable);
    ws[idx].setEnabledWithLabel(enable);
    loadPcmBtn[idx].setEnabled(enable);
    clearPcmBtn[idx].setEnabled(enable);
    pcmFileNameLabel[idx].setEnabled(enable);
    pcmOffset[idx].setEnabledWithLabel(enable);
    pcmRatio[idx].setEnabledWithLabel(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    mmlSeparator[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
    rgEn[idx].setEnabled(enable);
    rgAr[idx].setEnabledWithLabel(enable);
    rgD1r[idx].setEnabledWithLabel(enable);
    rgD1r[idx].setEnabledWithLabel(enable);
    rgD1l[idx].setEnabledWithLabel(enable);
    rgD1l[idx].setEnabledWithLabel(enable);
    rgD2r[idx].setEnabledWithLabel(enable);
    rgD2r[idx].setEnabledWithLabel(enable);
    rgRr[idx].setEnabledWithLabel(enable);
    rgRr[idx].setEnabled(enable);
    rgTl[idx].setEnabledWithLabel(enable);
    rgTl[idx].setEnabled(enable);
    sus[idx].setEnabled(enable);
    xof[idx].setEnabled(enable);
    kor[idx].setEnabled(enable);
    pitchEnv[idx].setEnabled(enable);
    ssgSwEnv[idx].setEnabled(enable);
}

void GuiOpzx7::updateOnWsChange(int idx)
{
    int selectedWs = ws[idx].getSelectedId();
    if (selectedWs == 16)
    {
        loadPcmBtn[idx].setVisible(false);
        clearPcmBtn[idx].setVisible(false);
        pcmFileNameLabel[idx].setVisible(false);
        pcmOffset[idx].setVisibleWithLabel(false);
        pcmRatio[idx].setVisibleWithLabel(false);
        loadWtBtn[idx].setVisible(true);
        clearWtBtn[idx].setVisible(true);
        wtFileNameLabel[idx].setVisible(true);
    }
    else if (selectedWs == 32)
    {
        loadPcmBtn[idx].setVisible(true);
        clearPcmBtn[idx].setVisible(true);
        pcmFileNameLabel[idx].setVisible(true);
        pcmOffset[idx].setVisibleWithLabel(true);
        pcmRatio[idx].setVisibleWithLabel(true);
        loadWtBtn[idx].setVisible(false);
        clearWtBtn[idx].setVisible(false);
        wtFileNameLabel[idx].setVisible(false);
    }
    else {
        loadPcmBtn[idx].setVisible(false);
        clearPcmBtn[idx].setVisible(false);
        pcmFileNameLabel[idx].setVisible(false);
        pcmOffset[idx].setVisibleWithLabel(false);
        pcmRatio[idx].setVisibleWithLabel(false);
        loadWtBtn[idx].setVisible(false);
        clearWtBtn[idx].setVisible(false);
        wtFileNameLabel[idx].setVisible(false);
    }
}

void GuiOpzx7::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > 35) return;

    for (int i = 0; i < 4; ++i)
    {
        juce::String newTitle = Opzx7GuiText::Group::opPrefix + juce::String(i + 1) + algOpPrefix[algIndex][i];

        opGroups[i].setText(newTitle);

        bool enable = opEnableOnAlg[algIndex][i];

        updateOpEnable(i, enable);
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

void GuiOpzx7::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].setVisibleWithLabel(rgMode);
    rgD1r[idx].setVisibleWithLabel(rgMode);
    rgD1l[idx].setVisibleWithLabel(rgMode);
    rgD2r[idx].setVisibleWithLabel(rgMode);
    rgRr[idx].setVisibleWithLabel(rgMode);
    rgTl[idx].setVisibleWithLabel(rgMode);

    ar[idx].setVisibleWithLabel(!rgMode);
    d1r[idx].setVisibleWithLabel(!rgMode);
    d1l[idx].setVisibleWithLabel(!rgMode);
    d2r[idx].setVisibleWithLabel(!rgMode);
    rr[idx].setVisibleWithLabel(!rgMode);
    tl[idx].setVisibleWithLabel(!rgMode);
}

void GuiOpzx7::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

// ==============================================================================
// Keyboard Shortcut Logic
// ==============================================================================
bool GuiOpzx7::keyPressed(const juce::KeyPress& key)
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

void GuiOpzx7::copyFmParamsToString()
{
    auto formatOpExt = [this](int index) {
        // ' MUL AR DR SL RR TL KSR KSL
        return juce::String::formatted(
            u8"MUL%d DT1%+d DT2+%d AR%d D1R%d D1L%d D2R%d RR%d TL%d KS%d\n",
            (int)this->mul[index].getSelectedId() - 1,
            this->dt1[index].getValue(),
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

    juce::String mml = juce::String("[OPZX7]\n")
        + FmMml::extMmlHeader
        + formatOpsExt();
    juce::SystemClipboard::copyTextToClipboard(mml);
}

void GuiOpzx7::copyFmParamsToObject()
{

}

void GuiOpzx7::pasteFmParamsFromObject()
{

}

void GuiOpzx7::initParams()
{
    this->ctx.audioProcessor.initParams("OPZX7_");
    for (int i = 0; i < 4; i++)
    {
        this->ctx.audioProcessor.unloadOpzx7PcmFile(i);
        updatePcmFileName(i, Io::empty);

        this->ctx.audioProcessor.unloadOpzx7WtFile(i);
        updateWtFileName(i, Io::empty);
    }
}

void GuiOpzx7::layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask[opIndex] });

    bool visibleMask = catMask[opIndex].isDetailVisible();

    mask[opIndex].setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask[opIndex] });
    }
}

void GuiOpzx7::layoutQualityCat(juce::Rectangle<int>& rect) {
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

void GuiOpzx7::layoutPanpotCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &panCat });

    bool visible = panCat.isDetailVisible();

    panpotEnableToggle.setVisible(visible);
    panpotSlider.setVisibleWithLabel(visible);
    panToLBtn.setVisible(visible);
    panToCBtn.setVisible(visible);
    panToRBtn.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &panpotEnableToggle });
        layoutMain({ .mainRect = rect, .label = &panpotSlider.label, .component = &panpotSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &panToLBtn, .comp2 = &panToCBtn, .comp3 = &panToRBtn });
    }
}

void GuiOpzx7::layoutOpSsgEnvCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catSsgEnv[opIndex] });

    bool visible = catSsgEnv[opIndex].isDetailVisible();

    se[opIndex].setVisibleWithLabel(visible);
    seFreq[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &se[opIndex].label, .component = &se[opIndex] });
        layoutRow({ .rowRect = rect, .label = &seFreq[opIndex].label, .component = &seFreq[opIndex], });
    }
}

void GuiOpzx7::setupGraph(int opIndex)
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

    ar[opIndex].onValueChange = repaintGraph;
    d1r[opIndex].onValueChange = repaintGraph;
    d2r[opIndex].onValueChange = repaintGraph;
    d1l[opIndex].onValueChange = repaintGraph;
    rr[opIndex].onValueChange = repaintGraph;
    tl[opIndex].onValueChange = repaintGraph;
    rgAr[opIndex].onValueChange = repaintGraph;
    rgD1r[opIndex].onValueChange = repaintGraph;
    rgD2r[opIndex].onValueChange = repaintGraph;
    rgD1l[opIndex].onValueChange = repaintGraph;
    rgRr[opIndex].onValueChange = repaintGraph;
    rgTl[opIndex].onValueChange = repaintGraph;
    sus[opIndex].onStateChange = repaintGraph;
    xof[opIndex].onStateChange = repaintGraph;
    kor[opIndex].onStateChange = repaintGraph;

    pitchEnv[opIndex].setupGraph(repaintGraph);
    ssgSwEnv[opIndex].setupGraph(repaintGraph);

    addAndMakeVisible(graphSeparator[opIndex]);
    graphSeparator[opIndex].setup({.lineThick = 2.0f, .lineColour = juce::Colours::grey});
}

void GuiOpzx7::setGraphMode(int opIndex, GraphMode mode)
{
    currentGraphMode[opIndex] = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp[opIndex].setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch[opIndex].setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg[opIndex].setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateOpGraph(opIndex);
}

void GuiOpzx7::layoutOpGraph(int opIndex, juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(Opzx7GuiValue::ParamGroup::Graph::height + Opzx7GuiValue::ParamGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(Opzx7GuiValue::ParamGroup::Separator::height);

    graphSeparator[opIndex].setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(Opzx7GuiValue::ParamGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch[opIndex].setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg[opIndex].setBounds(btnArea);

    // 残りをグラフエリアにする
    opGraphs[opIndex].setBounds(mainArea);
}

// グラフを再計算して描画
void GuiOpzx7::updateOpGraph(int opIndex)
{
    GraphMode mode = currentGraphMode[opIndex];

    // カーブモードが有効かどうかを判定
    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();

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

        bool isRg = rgEn[opIndex].getToggleState();
        bool isSus = sus[opIndex].getToggleState();
        bool isXof = xof[opIndex].getToggleState();
        bool isKor = kor[opIndex].getToggleState();

        auto getValue = [isRg](GuiSlider& rgSlider, GuiSlider& realSlider) -> float {
            return isRg ? (float)rgSlider.getValue() : (float)realSlider.getValue();
            };

        auto getMax = [isRg](GuiSlider& rgSlider, GuiSlider& realSlider) -> float {
            return isRg ? (float)rgSlider.getMaximum() : (float)realSlider.getMaximum();
            };

        float arMax = getMax(rgAr[opIndex], ar[opIndex]);
        float d1rMax = getMax(rgD1r[opIndex], d1r[opIndex]);
        float d2rMax = getMax(rgD2r[opIndex], d2r[opIndex]);
        float d1lMax = getMax(rgD1l[opIndex], d1l[opIndex]);
        float rrMax = getMax(rgRr[opIndex], rr[opIndex]);
        float tlMax = getMax(rgTl[opIndex], tl[opIndex]);

        float arVal = getValue(rgAr[opIndex], ar[opIndex]);
        float d1rVal = getValue(rgD1r[opIndex], d1r[opIndex]);
        float d2rVal = getValue(rgD2r[opIndex], d2r[opIndex]);
        float d1lVal = getValue(rgD1l[opIndex], d1l[opIndex]);
        float rrVal = getValue(rgRr[opIndex], rr[opIndex]);
        float tlVal = getValue(rgTl[opIndex], tl[opIndex]);

        float sl = isRg ? (d1lMax - d1lVal) / d1lMax : d1lVal / d1lMax; // 15=0.0, 0=1.0
        float tlScale = isRg ? 1.0f - (tlVal / tlMax) : tlVal / tlMax; // TL=127で無音

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
            phases.push_back({
                .widthPx = keyOnWidth,
                .startLevel = sl * tlScale,
                .endLevel = sl * tlScale,
                .color = color,
                .phaseLineColor = juce::Colours::green
                });

            currentTotalWidth += keyOnWidth;
        }

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

void GuiOpzx7::layoutOpOptionalCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catOptional[opIndex] });

    bool visible = catOptional[opIndex].isDetailVisible();

    bypass[opIndex].setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &bypass[opIndex] });
    }
}
