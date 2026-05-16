#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Opzx7/Keys.h"
#include "../../Processor/Opzx7/Values.h"
#include "../../Core/Const/MmlKeys.h"
#include "../../Core/Const/MmlValues.h"

#include "../../Core/Fm/RegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiValues.h"
#include "./GuiText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "./GuiHelpers.h"

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
    {.name = " 6: 22.05kHz", .value = 6 },
    {.name = " 7: 16kHz",    .value = 7 },
    {.name = " 8: 12kHz",    .value = 8 },
    {.name = " 9: 11kHz",    .value = 9 },
    {.name = "10: 8kHz",     .value = 10 },
    {.name = "11: 5.5kHz",   .value = 11 },
    {.name = "12: 4kHz",     .value = 12 },
    {.name = "13: 2kHz",     .value = 13 },
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

    presetNameCat.setup({ .parent = *this, .title = Opzx7GuiText::Category::preset });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    qualityCat.setup({ .parent = *this, .title = Opzx7GuiText::Category::visibleQuality, .invisibleTitle = Opzx7GuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = *this, .id = code + Opzx7PrKey::bit, .title = Opzx7GuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + Opzx7PrKey::rate, .title = Opzx7GuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setup({ .parent = *this, .title = Opzx7GuiText::Category::algFb });

    algSelector.setup({ .parent = *this, .id = code + Opzx7PrKey::alg, .title = Opzx7GuiText::Fm::alg, .items = opzx7AlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    feedbackSlider.setup({ .parent = *this, .id = code + Opzx7PrKey::fb0, .title = Opzx7GuiText::Fm::fb01, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    feedback2Slider.setup({ .parent = *this, .id = code + Opzx7PrKey::fb2, .title = Opzx7GuiText::Fm::fb2, .isReset = true });
    feedback2Slider.setWantsKeyboardFocus(true);
    feedback2Slider.setExplicitFocusOrder(++tabOrder);

    panCat.setup({ .parent = *this, .title = Opzx7GuiText::Category::visiblePanpot, .invisibleTitle = Opzx7GuiText::Category::invisiblePanpot , .enableChangeDetailVisible = true });

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

    lfoCat.setup({ .parent = *this, .title = Opzx7GuiText::Category::visibleLfo, .invisibleTitle = Opzx7GuiText::Category::invisibleLfo, .enableChangeDetailVisible = true });

    lfoFreqSlider.setup({ .parent = *this, .id = code + Opzx7PrKey::GlLfo::freq, .title = Opzx7GuiText::Fm::lfoFreq, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + Opzx7PrKey::GlLfo::amSmoothRatio, .title = Opzx7GuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = *this, .id = code + Opzx7PrKey::GlLfo::syncDelay, .title = Opzx7GuiText::Fm::lfoSyncDelay, .isReset = true });
    lfoSyncDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoSyncDelaySlider.setWantsKeyboardFocus(true);
    lfoSyncDelaySlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelayToZeroBtn.setup({ .parent = *this, .title = "Async", .isReset = false, .isResized = false});
    lfoSyncDelayToZeroBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToZeroBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToZeroBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(0.0f);
        };

    lfoSyncDelayToOneBtn.setup({ .parent = *this, .title = "Sync", .isReset = false, .isResized = false });
    lfoSyncDelayToOneBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToOneBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToOneBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(1.0f);
        };

    lfoPmToggle.setup({ .parent = *this, .id = code + Opzx7PrKey::GlLfo::pm, .title = Opzx7GuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPgShapeSelector.setup({ .parent = *this, .id = code + Opzx7PrKey::GlLfo::pgShape, .title = Opzx7GuiText::Fm::lfoPgShape, .items = lfoShapeItems, .isReset = true });
    lfoPgShapeSelector.setWantsKeyboardFocus(true);
    lfoPgShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = *this, .id = code + Opzx7PrKey::GlLfo::pms, .title = Opzx7GuiText::Fm::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = *this, .id = code + Opzx7PrKey::GlLfo::pmd, .title = Opzx7GuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = *this, .id = code + Opzx7PrKey::GlLfo::am, .title = Opzx7GuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoEgShapeSelector.setup({ .parent = *this, .id = code + Opzx7PrKey::GlLfo::egShape, .title = Opzx7GuiText::Fm::lfoEgShape, .items = lfoShapeItems, .isReset = true });
    lfoEgShapeSelector.setWantsKeyboardFocus(true);
    lfoEgShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmsSlider.setup({ .parent = *this, .id = code + Opzx7PrKey::GlLfo::ams, .title = Opzx7GuiText::Fm::ams, .isReset = true });
    lfoAmsSlider.setWantsKeyboardFocus(true);
    lfoAmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = *this, .id = code + Opzx7PrKey::GlLfo::amd, .title = Opzx7GuiText::Fm::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setup({ .parent = *this, .title = Opzx7GuiText::Category::visibleMonoMode, .invisibleTitle = Opzx7GuiText::Category::invisibleMonoMode, .enableChangeDetailVisible = true });

    monoModeToggle.setup({ .parent = *this, .id = Opzx7PrKey::monoMode, .title = Opzx7GuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = Opzx7GuiText::Category::visibleMvol, .invisibleTitle = Opzx7GuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

    masterVolSlider.setup({ .parent = *this, .id = Opzx7PrKey::masterVol, .title = Opzx7GuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

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

        catMain[i].setup({ .parent = *this, .title = Opzx7GuiText::Category::m });

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

        rgEn[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::rgEn, .title = Opzx7GuiText::Fm::Op::RgEn, .isReset = true });
        rgEn[i].setWantsKeyboardFocus(true);
        rgEn[i].setExplicitFocusOrder(++tabOrder);
        rgEn[i].onStateChange = [this, i] {
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

        phaseOffset[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::phaseOffset, .title = Opzx7GuiText::Fm::Op::phaseOffset, .isReset = true });
        phaseOffset[i].setWantsKeyboardFocus(true);
        phaseOffset[i].setExplicitFocusOrder(++tabOrder);

        sus[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::sus, .title = Opzx7GuiText::Fm::Op::sus, .isReset = true });
        sus[i].setWantsKeyboardFocus(true);
        sus[i].setExplicitFocusOrder(++tabOrder);

        xof[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::xof, .title = Opzx7GuiText::Fm::Op::xof, .isReset = true });
        xof[i].setWantsKeyboardFocus(true);
        xof[i].setExplicitFocusOrder(++tabOrder);

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

        catSsgEnv[i].setup({ .parent = *this, .title = Opzx7GuiText::Category::visibleSsgEnv, .invisibleTitle = Opzx7GuiText::Category::invisibleSsgEnv, .enableChangeDetailVisible = true });

        se[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::se, .title = Opzx7GuiText::Fm::Op::SEnv, .items = opnaSeItems, .isReset = true });
        se[i].setWantsKeyboardFocus(true);
        se[i].setExplicitFocusOrder(++tabOrder);

        seFreq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::seFreq, .title = Opzx7GuiText::Fm::Op::SFreq, .isReset = true });
        seFreq[i].setWantsKeyboardFocus(true);
        seFreq[i].setExplicitFocusOrder(++tabOrder);

        catPitchEnv[i].setup({ .parent = *this, .title = Opzx7GuiText::Category::visiblePitchAdsr, .invisibleTitle = Opzx7GuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

        pitchEnvEnable[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::PitchAdsr::enable, .title = Opzx7GuiText::PitchAdsr::enable, .isReset = true });
        pitchEnvEnable[i].setWantsKeyboardFocus(true);
        pitchEnvEnable[i].setExplicitFocusOrder(++tabOrder);

        pitchAttack[i].setup({ .parent = *this, .id = paramPrefix + Opzx7PrKey::PitchAdsr::ar, .title = Opzx7GuiText::PitchAdsr::ar, .isReset = true });
        pitchAttack[i].setWantsKeyboardFocus(true);
        pitchAttack[i].setExplicitFocusOrder(++tabOrder);

        pitchDecay[i].setup({ .parent = *this, .id = paramPrefix + Opzx7PrKey::PitchAdsr::dr , .title = Opzx7GuiText::PitchAdsr::dr, .isReset = true });
        pitchDecay[i].setWantsKeyboardFocus(true);
        pitchDecay[i].setExplicitFocusOrder(++tabOrder);

        pitchRelease[i].setup({ .parent = *this, .id = paramPrefix + Opzx7PrKey::PitchAdsr::rr, .title = Opzx7GuiText::PitchAdsr::rr, .isReset = true });
        pitchRelease[i].setWantsKeyboardFocus(true);
        pitchRelease[i].setExplicitFocusOrder(++tabOrder);

        pitchStartLevel[i].setup({ .parent = *this, .id = paramPrefix + Opzx7PrKey::PitchAdsr::stl, .title = Opzx7GuiText::PitchAdsr::stl, .isReset = true });
        pitchStartLevel[i].setWantsKeyboardFocus(true);
        pitchStartLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchAttackLevel[i].setup({ .parent = *this, .id = paramPrefix + Opzx7PrKey::PitchAdsr::atl, .title = Opzx7GuiText::PitchAdsr::atl, .isReset = true });
        pitchAttackLevel[i].setWantsKeyboardFocus(true);
        pitchAttackLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchSustainLevel[i].setup({ .parent = *this, .id = paramPrefix + Opzx7PrKey::PitchAdsr::ssl, .title = Opzx7GuiText::PitchAdsr::ssl, .isReset = true });
        pitchSustainLevel[i].setWantsKeyboardFocus(true);
        pitchSustainLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchReleaseLevel[i].setup({ .parent = *this, .id = paramPrefix + Opzx7PrKey::PitchAdsr::rll, .title = Opzx7GuiText::PitchAdsr::rll, .isReset = true });
        pitchReleaseLevel[i].setWantsKeyboardFocus(true);
        pitchReleaseLevel[i].setExplicitFocusOrder(++tabOrder);

        catLfo[i].setup({ .parent = *this, .title = Opzx7GuiText::Category::visibleLfo, .invisibleTitle = Opzx7GuiText::Category::invisibleLfo, .enableChangeDetailVisible = true });

        lFreq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::lfoFreq, .title = Opzx7GuiText::Fm::Op::Freqs, .isReset = true });
        lFreq[i].setWantsKeyboardFocus(true);
        lFreq[i].setExplicitFocusOrder(++tabOrder);

        syncDelay[i].setup({ .parent = *this, .id = paramPrefix + Opzx7PrKey::syncDelay, .title = Opzx7GuiText::Fm::Op::SyncDelay, .isReset = true });
        syncDelay[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        syncDelay[i].setWantsKeyboardFocus(true);
        syncDelay[i].setExplicitFocusOrder(++tabOrder);

        syncDelayToZero[i].setup({.parent = *this, .title = "Async", .isReset = false, .isResized = false});
        syncDelayToZero[i].setWantsKeyboardFocus(true);
        syncDelayToZero[i].setExplicitFocusOrder(++tabOrder);
        syncDelayToZero[i].onClick = [this, i] {
            syncDelay[i].setValue(0.0f);
            };

        syncDelayToOne[i].setup({ .parent = *this, .title = "Sync", .isReset = false, .isResized = false });
        syncDelayToOne[i].setWantsKeyboardFocus(true);
        syncDelayToOne[i].setExplicitFocusOrder(++tabOrder);
        syncDelayToOne[i].onClick = [this, i] {
            syncDelay[i].setValue(1.0f);
            };

        pm[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::vib, .title = Opzx7GuiText::Fm::Op::PmEn, .isReset = true });
        pm[i].setWantsKeyboardFocus(true);
        pm[i].setExplicitFocusOrder(++tabOrder);

        pgShape[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::pgShape, .title = Opzx7GuiText::Fm::Op::PgShape, .items = lfoShapeItems, .isReset = true });
        pgShape[i].setWantsKeyboardFocus(true);
        pgShape[i].setExplicitFocusOrder(++tabOrder);

        pms[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::pms, .title = Opzx7GuiText::Fm::Op::Pms, .isReset = true });
        pms[i].setWantsKeyboardFocus(true);
        pms[i].setExplicitFocusOrder(++tabOrder);

        pmd[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::pmd, .title = Opzx7GuiText::Fm::Op::Pmd, .isReset = true });
        pmd[i].setWantsKeyboardFocus(true);
        pmd[i].setExplicitFocusOrder(++tabOrder);

        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::am, .title = Opzx7GuiText::Fm::Op::AmEn, .isReset = true });
        am[i].setWantsKeyboardFocus(true);
        am[i].setExplicitFocusOrder(++tabOrder);

        egShape[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::egShape, .title = Opzx7GuiText::Fm::Op::EgShape, .items = lfoShapeItems, .isReset = true });
        egShape[i].setWantsKeyboardFocus(true);
        egShape[i].setExplicitFocusOrder(++tabOrder);

        ams[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::ams, .title = Opzx7GuiText::Fm::Op::Ams, .isReset = true });
        ams[i].setWantsKeyboardFocus(true);
        ams[i].setExplicitFocusOrder(++tabOrder);

        amd[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::amd, .title = Opzx7GuiText::Fm::Op::Amd, .isReset = true });
        amd[i].setWantsKeyboardFocus(true);
        amd[i].setExplicitFocusOrder(++tabOrder);

        cafFix[i].setup({ .parent = *this, .title = Opzx7GuiText::Category::visibleFix, .invisibleTitle = Opzx7GuiText::Category::invisibleFix, .enableChangeDetailVisible = true });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::fix, .title = Opzx7GuiText::Fm::Op::Fix, .isReset = true });
        fix[i].setWantsKeyboardFocus(true);
        fix[i].setExplicitFocusOrder(++tabOrder);

        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::fixFreq, .title = Opzx7GuiText::Fm::Op::FFreq, .isReset = true });
        freq[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        freq[i].setWantsKeyboardFocus(true);
        freq[i].setExplicitFocusOrder(++tabOrder);

        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = Opzx7GuiText::Fm::Op::Opzx7FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].setWantsKeyboardFocus(true);
        freqToZero[i].setExplicitFocusOrder(++tabOrder);
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };

        freqTo05[i].setup(GuiTextButton::Config{ .parent = *this, .title = Opzx7GuiText::Fm::Op::Opzx7FreqTo05, .isReset = false, .isResized = false });
        freqTo05[i].setWantsKeyboardFocus(true);
        freqTo05[i].setExplicitFocusOrder(++tabOrder);
        freqTo05[i].onClick = [this, index = i] { freq[index].setValue(0.5, juce::sendNotification); };

        freqTo1[i].setup(GuiTextButton::Config{ .parent = *this, .title = Opzx7GuiText::Fm::Op::Opzx7FreqTo1, .isReset = false, .isResized = false });
        freqTo1[i].setWantsKeyboardFocus(true);
        freqTo1[i].setExplicitFocusOrder(++tabOrder);
        freqTo1[i].onClick = [this, index = i] { freq[index].setValue(1, juce::sendNotification); };

        freqTo2[i].setup(GuiTextButton::Config{ .parent = *this, .title = Opzx7GuiText::Fm::Op::Opzx7FreqTo2, .isReset = false, .isResized = false });
        freqTo2[i].setWantsKeyboardFocus(true);
        freqTo2[i].setExplicitFocusOrder(++tabOrder);
        freqTo2[i].onClick = [this, index = i] { freq[index].setValue(2, juce::sendNotification); };

        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = Opzx7GuiText::Fm::Op::Opzx7FreqTo440, .isReset = false, .isResized = false });
        freqTo440[i].setWantsKeyboardFocus(true);
        freqTo440[i].setExplicitFocusOrder(++tabOrder);
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        catMask[i].setup({ .parent = *this, .title = Opzx7GuiText::Category::visibleMask, .invisibleTitle = Opzx7GuiText::Category::invisibleMask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + Opzx7PrKey::mask, .title = Opzx7GuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        catMml[i].setup({ .parent = *this, .title = Opzx7GuiText::Category::mml });

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setWantsKeyboardFocus(true);
        mml[i].setExplicitFocusOrder(++tabOrder);
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 D1R:0 D1L:0 D2R:0 RR:15 TL:0 MUL:1 DT1:0 DT2:0",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

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

    layoutMainCategory({ .mainRect = mRect, .label = &presetNameCat });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = Opzx7GuiValue::PresetName::paddingBottom });

    layoutQualityCat(mRect);

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });
    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });
    layoutMain({ .mainRect = mRect, .label = &feedback2Slider.label, .component = &feedback2Slider, });

    layoutPanpotCat(mRect);

    layoutLfoCat(mRect);

    layoutMonoModeCat(mRect);

    layoutMvolCat(mRect);

    auto imgArea = mRect.removeFromBottom(100);
    algImageComp.setBounds(imgArea);
    mRect.removeFromTop(Opzx7GuiValue::Category::paddingTop);

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(Opzx7GuiValue::Fm::Op::width);

        // 枠線
        opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(Opzx7GuiValue::Fm::Op::Padding::width, Opzx7GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(Opzx7GuiValue::Group::TitlePaddingTop);

        bool rgMode = rgEn[i].getToggleState();
		int selectedWs = ws[i].getSelectedId();

        layoutRowCategory({ .rowRect = innerRect, .component = &catMain[i] });
        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        layoutRow({ .rowRect = innerRect, .label = &mulRatio[i].label, .component = &mulRatio[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt1[i].label, .component = &dt1[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt2[i].label, .component = &dt2[i] });
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
        layoutRow({ .rowRect = innerRect, .label = &phaseOffset[i].label, .component = &phaseOffset[i], });

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

        layoutRow({ .rowRect = innerRect, .component = &sus[i] });
        layoutRow({ .rowRect = innerRect, .component = &xof[i] });

        layoutOpSsgEnvCat(i, innerRect);

        layoutOpPitchEnvCat(i, innerRect);

        layoutOpLfoCat(i, innerRect);

        layoutOpFixCat(i, innerRect);

        layoutOpMaskCat(i, innerRect);

        layoutRowCategory({ .rowRect = innerRect, .component = &catMml[i] });
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
    catMain[idx].setEnabled(enable);
    mul[idx].setEnabledWithLabel(enable);

    int mulIndex = mul[idx].getSelectedId() - 1;
    bool enableMulRatio = mulIndex == 21; // mul = Ratio

    mulRatio[idx].setEnabledWithLabel(enable && enableMulRatio);

    dt1[idx].setEnabledWithLabel(enable);
    dt2[idx].setEnabledWithLabel(enable);
    ar[idx].setEnabledWithLabel(enable);
    d1r[idx].setEnabledWithLabel(enable);
    d1l[idx].setEnabledWithLabel(enable);
    rr[idx].setEnabledWithLabel(enable);
    d2r[idx].setEnabledWithLabel(enable);
    tl[idx].setEnabledWithLabel(enable);
    ks[idx].setEnabledWithLabel(enable);
    phaseOffset[idx].setEnabledWithLabel(enable);
    se[idx].setEnabledWithLabel(enable);
    seFreq[idx].setEnabledWithLabel(enable);
    cafFix[idx].setEnabled(enable);
    fix[idx].setEnabled(enable);
    freq[idx].setEnabledWithLabel(enable);
    freqToZero[idx].setEnabled(enable);
    freqTo05[idx].setEnabled(enable);
    freqTo1[idx].setEnabled(enable);
    freqTo2[idx].setEnabled(enable);
    freqTo440[idx].setEnabled(enable);
    catLfo[idx].setEnabled(enable);
    lFreq[idx].setEnabledWithLabel(enable);
    syncDelay[idx].setEnabledWithLabel(enable);
    syncDelayToZero[idx].setEnabled(enable);
    syncDelayToOne[idx].setEnabled(enable);
    pm[idx].setEnabled(enable);
    pgShape[idx].setEnabledWithLabel(enable);
    pms[idx].setEnabledWithLabel(enable);
    pmd[idx].setEnabledWithLabel(enable);
    am[idx].setEnabled(enable);
    egShape[idx].setEnabledWithLabel(enable);
    ams[idx].setEnabledWithLabel(enable);
    amd[idx].setEnabledWithLabel(enable);
    ws[idx].setEnabledWithLabel(enable);
    loadPcmBtn[idx].setEnabled(enable);
    clearPcmBtn[idx].setEnabled(enable);
    pcmFileNameLabel[idx].setEnabled(enable);
    pcmOffset[idx].setEnabledWithLabel(enable);
    pcmRatio[idx].setEnabledWithLabel(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    catMml[idx].setEnabled(enable);
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
    catPitchEnv[idx].setEnabled(enable);
    pitchEnvEnable[idx].setEnabled(enable);
    pitchAttack[idx].setEnabledWithLabel(enable);
    pitchDecay[idx].setEnabledWithLabel(enable);
    pitchRelease[idx].setEnabledWithLabel(enable);
    pitchStartLevel[idx].setEnabledWithLabel(enable);
    pitchAttackLevel[idx].setEnabledWithLabel(enable);
    pitchSustainLevel[idx].setEnabledWithLabel(enable);
    pitchReleaseLevel[idx].setEnabledWithLabel(enable);
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

void GuiOpzx7::layoutOpFixCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &cafFix[opIndex] });

    bool visibleFix = cafFix[opIndex].isDetailVisible();

    fix[opIndex].setVisible(visibleFix);
    freq[opIndex].setVisibleWithLabel(visibleFix);
    freqToZero[opIndex].setVisible(visibleFix);
    freqTo05[opIndex].setVisible(visibleFix);
    freqTo1[opIndex].setVisible(visibleFix);
    freqTo2[opIndex].setVisible(visibleFix);
    freqTo440[opIndex].setVisible(visibleFix);

    if (visibleFix)
    {
        layoutRow({ .rowRect = rect, .component = &fix[opIndex] });
        layoutRow({ .rowRect = rect, .label = &freq[opIndex].label, .component = &freq[opIndex] });
        layoutRowFiveComps({ .rect = rect, .comp1 = &freqToZero[opIndex], .comp2 = &freqTo05[opIndex], .comp3 = &freqTo1[opIndex], .comp4 = &freqTo2[opIndex], .comp5 = &freqTo440[opIndex] });
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

void GuiOpzx7::layoutMonoModeCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &monoPolyCat });

    bool visible = monoPolyCat.isDetailVisible();

    monoModeToggle.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoModeToggle });
    }
}

void GuiOpzx7::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
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

void GuiOpzx7::layoutLfoCat(juce::Rectangle<int>& rect)
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
    lfoPmsSlider.setVisibleWithLabel(visible);
    lfoPmdSlider.setVisibleWithLabel(visible);
    lfoAmToggle.setVisible(visible);
    lfoEgShapeSelector.setVisibleWithLabel(visible);
    lfoAmsSlider.setVisibleWithLabel(visible);
    lfoAmdSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &lfoFreqSlider.label, .component = &lfoFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
        layoutMain({ .mainRect = rect, .label = &lfoSyncDelaySlider.label, .component = &lfoSyncDelaySlider });
        layoutMainTwoComps({ .rect = rect, .comp1 = &lfoSyncDelayToZeroBtn, .comp2 = &lfoSyncDelayToOneBtn });
        layoutMain({ .mainRect = rect, .component = &lfoPmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoPgShapeSelector.label, .component = &lfoPgShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoPmsSlider.label, .component = &lfoPmsSlider });
        layoutMain({ .mainRect = rect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
        layoutMain({ .mainRect = rect, .component = &lfoAmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoEgShapeSelector.label, .component = &lfoEgShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoAmsSlider.label, .component = &lfoAmsSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });
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

void GuiOpzx7::layoutOpPitchEnvCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catPitchEnv[opIndex] });

    bool visible = catPitchEnv[opIndex].isDetailVisible();

    pitchEnvEnable[opIndex].setVisible(visible);
    pitchAttack[opIndex].setVisibleWithLabel(visible);
    pitchDecay[opIndex].setVisibleWithLabel(visible);
    pitchRelease[opIndex].setVisibleWithLabel(visible);
    pitchStartLevel[opIndex].setVisibleWithLabel(visible);
    pitchAttackLevel[opIndex].setVisibleWithLabel(visible);
    pitchSustainLevel[opIndex].setVisibleWithLabel(visible);
    pitchReleaseLevel[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &pitchEnvEnable[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchAttack[opIndex].label, .component = &pitchAttack[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchDecay[opIndex].label, .component = &pitchDecay[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchRelease[opIndex].label, .component = &pitchRelease[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchStartLevel[opIndex].label, .component = &pitchStartLevel[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchAttackLevel[opIndex].label, .component = &pitchAttackLevel[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchSustainLevel[opIndex].label, .component = &pitchSustainLevel[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pitchReleaseLevel[opIndex].label, .component = &pitchReleaseLevel[opIndex] });
    }
}

void GuiOpzx7::layoutOpLfoCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catLfo[opIndex] });

    bool visible = catLfo[opIndex].isDetailVisible();

    lFreq[opIndex].setVisibleWithLabel(visible);
    syncDelay[opIndex].setVisibleWithLabel(visible);
    syncDelayToZero[opIndex].setVisible(visible);
    syncDelayToOne[opIndex].setVisible(visible);
    pm[opIndex].setVisible(visible);
    pgShape[opIndex].setVisibleWithLabel(visible);
    pms[opIndex].setVisibleWithLabel(visible);
    pmd[opIndex].setVisibleWithLabel(visible);
    am[opIndex].setVisible(visible);
    egShape[opIndex].setVisibleWithLabel(visible);
    ams[opIndex].setVisibleWithLabel(visible);
    amd[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &lFreq[opIndex].label, .component = &lFreq[opIndex] });
        layoutRow({ .rowRect = rect, .label = &syncDelay[opIndex].label, .component = &syncDelay[opIndex] });
        layoutRowTwoComps({ .rect = rect, .comp1 = &syncDelayToZero[opIndex], .comp2 = &syncDelayToOne[opIndex] });
        layoutRow({ .rowRect = rect, .component = &pm[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pgShape[opIndex].label, .component = &pgShape[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pms[opIndex].label, .component = &pms[opIndex] });
        layoutRow({ .rowRect = rect, .label = &pmd[opIndex].label, .component = &pmd[opIndex] });
        layoutRow({ .rowRect = rect, .component = &am[opIndex] });
        layoutRow({ .rowRect = rect, .label = &egShape[opIndex].label, .component = &egShape[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ams[opIndex].label, .component = &ams[opIndex] });
        layoutRow({ .rowRect = rect, .label = &amd[opIndex].label, .component = &amd[opIndex] });
    }
}
