#include "GuiOpzx3.h"

#include <vector>

#include "../processor/PluginProcessor.h"

#include "../core/PrKeys.h"
#include "../core/PrValues.h"
#include "../core/MmlKeys.h"
#include "../core/MmlValues.h"

#include "../fm/RegisterConverter.h"

#include "../gui/GuiHelpers.h"
#include "../gui/GuiValues.h"
#include "../gui/GuiText.h"
#include "../gui/GuiStructs.h"

static std::vector<SelectItem> bdItems = {
    {.name = "1: 4-bit (16 steps)",  .value = 1 },
    {.name = "2: 5-bit (32 steps)",  .value = 2 },
    {.name = "3: 6-bit (64 steps)",  .value = 3 },
    {.name = "4: 8-bit (256 steps)", .value = 4 },
    {.name = "5: Raw",               .value = 5 },
    {.name = "6: 7-bit (OPLL/128 steps)", .value = 6 }
};

static std::vector<SelectItem> rateItems = {
    {.name = "1: 96kHz",    .value = 1 },
    {.name = "2: 55.5kHz",  .value = 2 },
    {.name = "3: 48kHz",    .value = 3 },
    {.name = "4: 44.1kHz",  .value = 4 },
    {.name = "5: 22.05kHz", .value = 5 },
    {.name = "6: 16kHz",    .value = 6 },
    {.name = "7: 8kHz",     .value = 7 },
};

static std::vector<SelectItem> opzx3AlgItems = {
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
    {.name = "28: <MA3-00>", .value = 29 },
    {.name = "29: <MA3-01>", .value = 30 },
    {.name = "30: <MA3-02>", .value = 31 },
    {.name = "31: <MA3-03>", .value = 32 },
    {.name = "32: <MA3-04>", .value = 33 },
    {.name = "33: <MA3-05>", .value = 34 },
    {.name = "34: <MA3-06>", .value = 35 },
    {.name = "35: <MA3-07>", .value = 36 },
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
    {.name = "0: Sine",     .value = 1 },
    {.name = "1: Saw Down", .value = 2 },
    {.name = "2: Square",   .value = 3 },
    {.name = "3: Triangle", .value = 4 },
    {.name = "4: Noise",    .value = 5 },
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

static std::vector<SelectItem> opzx3WsItems = {
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
    {.name = "15 ---",                              .value = 16},
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
    {.name = "31 PCM(Audio) File",                  .value = 32},
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
};

void GuiOpzx3::setup()
{
    const juce::String code = PrKey::Prefix::opzx3;
    int tabOrder = 1;

    mainGroup.setup(*this, GuiText::Group::mainGroup);

    presetNameCat.setup({ .parent = *this, .title = GuiText::Category::preset });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });

    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::bit, .title = GuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::rate, .title = GuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setup({ .parent = *this, .title = GuiText::Category::algFb });

    algSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::alg, .title = GuiText::Fm::alg, .items = opzx3AlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    feedbackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb0, .title = GuiText::Fm::fb01, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    feedback2Slider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb2, .title = GuiText::Fm::fb2, .isReset = true });
    feedback2Slider.setWantsKeyboardFocus(true);
    feedback2Slider.setExplicitFocusOrder(++tabOrder);

    lfoCat.setup({ .parent = *this, .title = GuiText::Category::lfo });

    lfoFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::freq, .title = GuiText::Fm::lfoFreq, .isReset = true });
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::amSmoothRatio, .title = GuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoShapeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::shape, .title = GuiText::Fm::lfoShape, .items = lfoShapeItems, .isReset = true });
    lfoShapeSelector.setWantsKeyboardFocus(true);
    lfoShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmToggle.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pm, .title = GuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmsSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pms, .title = GuiText::Fm::pms, .items = pmsItems, .isReset = true });
    lfoPmsSelector.setWantsKeyboardFocus(true);
    lfoPmsSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pmd, .title = GuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::am, .title = GuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmsSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::ams, .title = GuiText::Fm::ams, .items = amsItems, .isReset = true });
    lfoAmsSelector.setWantsKeyboardFocus(true);
    lfoAmsSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::amd, .title = GuiText::Fm::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setup({ .parent = *this, .title = GuiText::Category::monoMode });

    monoModeToggle.setup({ .parent = *this, .id = PrKey::monoMode, .title = GuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    for (int i = 0; i < 36; ++i)
    {
        juce::String fileName = juce::String::formatted(Io::Folder::asset + "/" + Io::Folder::resource + "/ALG_OPZX3_%02d.png", i);
        auto imgFile = docDir.getChildFile(fileName);

        if (imgFile.existsAsFile()) {
            algImages[i] = juce::ImageFileFormat::loadFrom(imgFile);
        }
    }

    // 画像コンポーネントを画面に追加
    addAndMakeVisible(algImageComp);

    // Operators
    const juce::String opCode = code + PrKey::Innder::op;

    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, GuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        catMain[i].setup({ .parent = *this, .title = GuiText::Category::m });

        mul[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mul, .title = GuiText::Fm::Op::Mul, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);

        dt1[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dt, .title = GuiText::Fm::Op::Dt1, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        dt1[i].setWantsKeyboardFocus(true);
        dt1[i].setExplicitFocusOrder(++tabOrder);

        dt2[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::dt2, .title = GuiText::Fm::Op::Dt2, .isReset = true, .regType = RegisterType::FmDt2 });
        dt2[i].setWantsKeyboardFocus(true);
        dt2[i].setExplicitFocusOrder(++tabOrder);

        rgEn[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgEn, .title = GuiText::Fm::Op::RgEn, .isReset = true });
        rgEn[i].setWantsKeyboardFocus(true);
        rgEn[i].setExplicitFocusOrder(++tabOrder);
        rgEn[i].onStateChange = [this, i] {
            ctx.editor.resized();
            };

        rgAr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgAr, .title = GuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgD1r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgD1r, .title = GuiText::Fm::Op::D1r, .isReset = true });
        rgD1r[i].setWantsKeyboardFocus(true);
        rgD1r[i].setExplicitFocusOrder(++tabOrder);

        rgD1l[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgD1l, .title = GuiText::Fm::Op::D1l, .isReset = true });
        rgD1l[i].setWantsKeyboardFocus(true);
        rgD1l[i].setExplicitFocusOrder(++tabOrder);

        rgD2r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgD2r, .title = GuiText::Fm::Op::D2r, .isReset = true });
        rgD2r[i].setWantsKeyboardFocus(true);
        rgD2r[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgRr, .title = GuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rgTl, .title = GuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        ar[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ar, .title = GuiText::Fm::Op::Ar, .isReset = true, .regType = RegisterType::FmAr });
        ar[i].setWantsKeyboardFocus(true);
        ar[i].setExplicitFocusOrder(++tabOrder);

        arTo000[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::ArTo000, .isReset = false, .isResized = false });
        arTo000[i].setWantsKeyboardFocus(true);
        arTo000[i].setExplicitFocusOrder(++tabOrder);
        arTo000[i].onClick = [this, index = i] { ar[index].setValue(0.00, juce::sendNotification); };

        arTo003[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::ArTo003, .isReset = false, .isResized = false });
        arTo003[i].setWantsKeyboardFocus(true);
        arTo003[i].setExplicitFocusOrder(++tabOrder);
        arTo003[i].onClick = [this, index = i] { ar[index].setValue(0.03, juce::sendNotification); };

        d1r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::d1r, .title = GuiText::Fm::Op::D1r, .isReset = true, .regType = RegisterType::FmDr });
        d1r[i].setWantsKeyboardFocus(true);
        d1r[i].setExplicitFocusOrder(++tabOrder);

        d1l[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::d1l, .title = GuiText::Fm::Op::D1l, .isReset = true, .regType = RegisterType::FmSl });
        d1l[i].setWantsKeyboardFocus(true);
        d1l[i].setExplicitFocusOrder(++tabOrder);

        d2r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::d2r, .title = GuiText::Fm::Op::D2r, .isReset = true, .regType = RegisterType::FmSr });
        d2r[i].setWantsKeyboardFocus(true);
        d2r[i].setExplicitFocusOrder(++tabOrder);

        rr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::rr, .title = GuiText::Fm::Op::Rr, .isReset = true, .regType = RegisterType::FmRr });
        rr[i].setWantsKeyboardFocus(true);
        rr[i].setExplicitFocusOrder(++tabOrder);

        rrTo000[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::RrTo000, .isReset = false, .isResized = false });
        rrTo000[i].setWantsKeyboardFocus(true);
        rrTo000[i].setExplicitFocusOrder(++tabOrder);
        rrTo000[i].onClick = [this, index = i] { rr[index].setValue(0.00, juce::sendNotification); };

        rrTo003[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::RrTo003, .isReset = false, .isResized = false });
        rrTo003[i].setWantsKeyboardFocus(true);
        rrTo003[i].setExplicitFocusOrder(++tabOrder);
        rrTo003[i].onClick = [this, index = i] { rr[index].setValue(0.03, juce::sendNotification); };

        tl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::tl, .title = GuiText::Fm::Op::Tl, .isReset = true, .regType = RegisterType::FmTl });
        tl[i].setWantsKeyboardFocus(true);
        tl[i].setExplicitFocusOrder(++tabOrder);

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ks, .title = GuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });
        ks[i].setWantsKeyboardFocus(true);
        ks[i].setExplicitFocusOrder(++tabOrder);

        phaseOffset[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::phaseOffset, .title = GuiText::Fm::Op::phaseOffset, .isReset = true });
        phaseOffset[i].setWantsKeyboardFocus(true);
        phaseOffset[i].setExplicitFocusOrder(++tabOrder);

        catShape[i].setup({ .parent = *this, .title = GuiText::Category::shape });

        ws[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ws, .title = GuiText::Fm::Op::Ws, .items = opzx3WsItems, .isReset = true });
        ws[i].setWantsKeyboardFocus(true);
        ws[i].setExplicitFocusOrder(++tabOrder);

        loadPcmBtn[i].setup({ .parent = *this, .title = GuiText::File::Pcm, .isReset = false, .isResized = false });
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
                        ctx.audioProcessor.loadOpzx3PcmFile(i, file);
                        updatePcmFileName(i, file.getFileName());
                        ctx.audioProcessor.lastSampleDirectory = file.getParentDirectory();
                    }
                }
            );
            };

        clearPcmBtn[i].setup({ .parent = *this, .title = GuiText::File::clear, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = false });
        clearPcmBtn[i].setWantsKeyboardFocus(true);
        clearPcmBtn[i].setExplicitFocusOrder(++tabOrder);
        clearPcmBtn[i].onClick = [this, i] {
            ctx.audioProcessor.unloadOpzx3PcmFile(i);
            updatePcmFileName(i, Io::empty);
            };

        pcmFileNameLabel[i].setup({ .parent = *this, .title = Io::empty });
        if (ctx.audioProcessor.opzx3PcmFilePaths[i].isNotEmpty()) {
            updatePcmFileName(i, juce::File(ctx.audioProcessor.opzx3PcmFilePaths[i]).getFileName());
        }

        pcmOffset[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pcmOffset, .title = GuiText::Fm::Op::PcmOffset, .isReset = true });
        pcmOffset[i].setWantsKeyboardFocus(true);
        pcmOffset[i].setExplicitFocusOrder(++tabOrder);

        pcmRatio[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pcmRatio, .title = GuiText::Fm::Op::PcmRatio, .isReset = true });
        pcmRatio[i].setWantsKeyboardFocus(true);
        pcmRatio[i].setExplicitFocusOrder(++tabOrder);

        se[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::se, .title = GuiText::Fm::Op::SEnv, .items = opnaSeItems, .isReset = true });
        se[i].setWantsKeyboardFocus(true);
        se[i].setExplicitFocusOrder(++tabOrder);

        seFreq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::seFreq, .title = GuiText::Fm::Op::SFreq, .isReset = true });
        seFreq[i].setWantsKeyboardFocus(true);
        seFreq[i].setExplicitFocusOrder(++tabOrder);

        catLfo[i].setup({ .parent = *this, .title = GuiText::Category::lfo });

        pm[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::vib, .title = GuiText::Fm::Op::PmEn, .isReset = true });
        pm[i].setWantsKeyboardFocus(true);
        pm[i].setExplicitFocusOrder(++tabOrder);

        pms[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::pms, .title = GuiText::Fm::Op::Pms, .items = pmsItems, .isReset = true });
        pms[i].setWantsKeyboardFocus(true);
        pms[i].setExplicitFocusOrder(++tabOrder);

        am[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::am, .title = GuiText::Fm::Op::AmEn, .isReset = true });
        am[i].setWantsKeyboardFocus(true);
        am[i].setExplicitFocusOrder(++tabOrder);

        ams[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::ams, .title = GuiText::Fm::Op::Ams, .items = amsItems, .isReset = true });
        ams[i].setWantsKeyboardFocus(true);
        ams[i].setExplicitFocusOrder(++tabOrder);

        cafFix[i].setup({ .parent = *this, .title = GuiText::Category::fix });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fix, .title = GuiText::Fm::Op::Fix, .isReset = true });
        fix[i].setWantsKeyboardFocus(true);
        fix[i].setExplicitFocusOrder(++tabOrder);

        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fixFreq, .title = GuiText::Fm::Op::FFreq, .isReset = true });
        freq[i].setWantsKeyboardFocus(true);
        freq[i].setExplicitFocusOrder(++tabOrder);

        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::Opzx3FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].setWantsKeyboardFocus(true);
        freqToZero[i].setExplicitFocusOrder(++tabOrder);
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };

        freqTo05[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::Opzx3FreqTo05, .isReset = false, .isResized = false });
        freqTo05[i].setWantsKeyboardFocus(true);
        freqTo05[i].setExplicitFocusOrder(++tabOrder);
        freqTo05[i].onClick = [this, index = i] { freq[index].setValue(0.5, juce::sendNotification); };

        freqTo1[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::Opzx3FreqTo1, .isReset = false, .isResized = false });
        freqTo1[i].setWantsKeyboardFocus(true);
        freqTo1[i].setExplicitFocusOrder(++tabOrder);
        freqTo1[i].onClick = [this, index = i] { freq[index].setValue(1, juce::sendNotification); };

        freqTo2[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::Opzx3FreqTo2, .isReset = false, .isResized = false });
        freqTo2[i].setWantsKeyboardFocus(true);
        freqTo2[i].setExplicitFocusOrder(++tabOrder);
        freqTo2[i].onClick = [this, index = i] { freq[index].setValue(2, juce::sendNotification); };

        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::Opzx3FreqTo440, .isReset = false, .isResized = false });
        freqTo440[i].setWantsKeyboardFocus(true);
        freqTo440[i].setExplicitFocusOrder(++tabOrder);
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        catMask[i].setup({ .parent = *this, .title = GuiText::Category::mask });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::mask, .title = GuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        catMml[i].setup({ .parent = *this, .title = GuiText::Category::mml });

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setWantsKeyboardFocus(true);
        mml[i].setExplicitFocusOrder(++tabOrder);
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 D1R:0 D1L:0 D2R:0 RR:15 TL:0 MUL:1 DT1:0 DT2:0",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });
    }
}

void GuiOpzx3::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutMain({ .mainRect = mRect, .label = &presetNameCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = GuiValue::PresetName::paddingBottom });
    layoutMain({ .mainRect = mRect, .label = &qualityCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &bitSelector.label, .component = &bitSelector });
    layoutMain({ .mainRect = mRect, .label = &rateSelector.label, .component = &rateSelector, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &algFbCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });
    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });
    layoutMain({ .mainRect = mRect, .label = &feedback2Slider.label, .component = &feedback2Slider, .paddingBottom = GuiValue::Category::paddingTop });

    layoutMain({ .mainRect = mRect, .label = &lfoCat, .paddingBottom = GuiValue::Category::paddingBotton });

    layoutMain({ .mainRect = mRect, .label = &lfoFreqSlider.label, .component = &lfoFreqSlider });
    layoutMain({ .mainRect = mRect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
    layoutMain({ .mainRect = mRect, .label = &lfoShapeSelector.label, .component = &lfoShapeSelector });
    layoutMain({ .mainRect = mRect, .component = &lfoPmToggle });
    layoutMain({ .mainRect = mRect, .label = &lfoPmsSelector.label, .component = &lfoPmsSelector });
    layoutMain({ .mainRect = mRect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
    layoutMain({ .mainRect = mRect, .component = &lfoAmToggle });
    layoutMain({ .mainRect = mRect, .label = &lfoAmsSelector.label, .component = &lfoAmsSelector });
    layoutMain({ .mainRect = mRect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &monoPolyCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .component = &monoModeToggle, .paddingBottom = 0 });

    auto imgArea = mRect.removeFromBottom(100);
    algImageComp.setBounds(imgArea);
    mRect.removeFromTop(GuiValue::Category::paddingTop);

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

        // 枠線
        opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(GuiValue::Fm::Op::Padding::width, GuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        bool rgMode = rgEn[i].getToggleState();

        layoutRow({ .rowRect = innerRect, .component = &catMain[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &dt1[i].label, .component = &dt1[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &dt2[i].label, .component = &dt2[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &rgEn[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        updateRgDisplayAsOp(i, rgMode);
        if (rgMode)
        {
            layoutRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &rgD1r[i].label, .component = &rgD1r[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &rgD1l[i].label, .component = &rgD1l[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &rgD2r[i].label, .component = &rgD2r[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        }
        else
        {
            layoutRow({ .rowRect = innerRect, .label = &ar[i].label, .component = &ar[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRowTwoComps({ .rect = innerRect, .comp1 = &arTo000[i], .comp2 = &arTo003[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &d1r[i].label, .component = &d1r[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &d1l[i].label, .component = &d1l[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &d2r[i].label, .component = &d2r[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &rr[i].label, .component = &rr[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRowTwoComps({ .rect = innerRect, .comp1 = &rrTo000[i], .comp2 = &rrTo003[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
            layoutRow({ .rowRect = innerRect, .label = &tl[i].label, .component = &tl[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        }
        layoutRow({ .rowRect = innerRect, .label = &ks[i].label, .component = &ks[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &phaseOffset[i].label, .component = &phaseOffset[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &catShape[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutRow({ .rowRect = innerRect, .label = &ws[i].label, .component = &ws[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRowOpzx3Pcm({ .rect = innerRect, .loadPcmBtn = &loadPcmBtn[i], .pcmFileNameLabel = &pcmFileNameLabel[i], .clearPcmBtn = &clearPcmBtn[i] });
        layoutRow({ .rowRect = innerRect, .label = &pcmOffset[i].label, .component = &pcmOffset[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &pcmRatio[i].label, .component = &pcmRatio[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &se[i].label, .component = &se[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &seFreq[i].label, .component = &seFreq[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &catLfo[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutRow({ .rowRect = innerRect, .component = &pm[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &pms[i].label, .component = &pms[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &am[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &ams[i].label, .component = &ams[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &cafFix[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutRow({ .rowRect = innerRect, .component = &fix[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRow({ .rowRect = innerRect, .label = &freq[i].label, .component = &freq[i], .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutRowFiveComps({ .rect = innerRect, .comp1 = &freqToZero[i], .comp2 = &freqTo05[i], .comp3 = &freqTo1[i], .comp4 = &freqTo2[i], .comp5 = &freqTo440[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &catMask[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutRow({ .rowRect = innerRect, .component = &mask[i], .paddingBottom = GuiValue::Category::paddingTop });
        layoutRow({ .rowRect = innerRect, .component = &catMml[i], .paddingBottom = GuiValue::Category::paddingBotton });
        layoutRow({ .rowRect = innerRect, .component = &mml[i], .paddingBottom = 0 });
    }

    updateAlgorithmDisplay();
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpzx3::applyMmlString(const juce::String& mml, int opIndex)
{
    juce::String input = mml.toUpperCase();
    int val;

    // MUL
    val = RegisterConverter::getValue(input, mmlPrefixMul, mmlValues::opzx3::mul);
    if (RegisterConverter::isValidVal(val)) mul[opIndex].setValue((double)RegisterConverter::convertFmMul(val), juce::sendNotification);

    // DT1
    val = RegisterConverter::getValue(input, mmlPrefixDt1, mmlValues::opzx3::dt1);
    if (RegisterConverter::isValidVal(val)) {
        int regVal = RegisterConverter::convertMmlDtToReg(val);

        dt1[opIndex].setSelectedItemIndex((double)regVal, juce::sendNotification);
    }

    // DT2
    val = RegisterConverter::getValue(input, mmlPrefixDt2, mmlValues::opzx3::dt2);
    if (RegisterConverter::isValidVal(val)) dt2[opIndex].setValue((double)val, juce::sendNotification);

    // KS
    val = RegisterConverter::getValue(input, mmlPrefixKs, mmlValues::opzx3::ks);
    if (RegisterConverter::isValidVal(val)) ks[opIndex].setSelectedItemIndex(RegisterConverter::convertFmKs(val), juce::sendNotification);

    // MASK
    val = RegisterConverter::getValue(input, mmlPrefixMask, mmlValues::opzx3::mask);
    if (RegisterConverter::isValidVal(val)) mask[opIndex].setToggleState(RegisterConverter::convertFmMask(val), juce::sendNotification);

    bool rgMode = rgEn[opIndex].getToggleState();

    if (rgMode)
    {
        // TL
        val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opzx3::tl);
        if (RegisterConverter::isValidVal(val)) rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(val), juce::sendNotification);

        // AR
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opzx3::ar);
        if (RegisterConverter::isValidVal(val)) rgAr[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // DR
        val = RegisterConverter::getValue(input, mmlPrefixD1r, mmlValues::opzx3::d1r);
        if (RegisterConverter::isValidVal(val)) rgD1r[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // SR
        val = RegisterConverter::getValue(input, mmlPrefixD2r, mmlValues::opzx3::d2r);
        if (RegisterConverter::isValidVal(val)) rgD2r[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // SL
        val = RegisterConverter::getValue(input, mmlPrefixD1l, mmlValues::opzx3::d1l);
        if (RegisterConverter::isValidVal(val)) rgD1l[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        // RR
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opzx3::rr);
        if (RegisterConverter::isValidVal(val)) rgRr[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        return;
    }

    // TL
    val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opzx3::tl);
    if (RegisterConverter::isValidVal(val)) tl[opIndex].setValue(RegisterConverter::convertFmTl(val), juce::sendNotification);

    // AR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRar, mmlValues::opzx3::ar);
    if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(mmlValues::opzx3::ar - val), juce::sendNotification);
    // AR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opzx3::ar);
        if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(val), juce::sendNotification);
    }

    // D1R(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRd1r, mmlValues::opzx3::d1r);
    if (RegisterConverter::isValidVal(val)) d1r[opIndex].setValue(RegisterConverter::convertFmDr(mmlValues::opzx3::d1r - val), juce::sendNotification);
    // D1R
    else {
        val = RegisterConverter::getValue(input, mmlPrefixD1r, mmlValues::opzx3::d1r);
        if (RegisterConverter::isValidVal(val)) d1r[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
    }

    // D1L
    val = RegisterConverter::getValue(input, mmlPrefixD1l, mmlValues::opzx3::d1l);
    if (RegisterConverter::isValidVal(val)) d1l[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);

    // D2R(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRd2r, mmlValues::opzx3::d2r);
    if (RegisterConverter::isValidVal(val)) d2r[opIndex].setValue(RegisterConverter::convertFmSr(mmlValues::opzx3::d2r - val), juce::sendNotification);
    // D2R
    else {
        val = RegisterConverter::getValue(input, mmlPrefixD2r, mmlValues::opzx3::d2r);
        if (RegisterConverter::isValidVal(val)) d2r[opIndex].setValue(RegisterConverter::convertFmSr(val), juce::sendNotification);
    }

    // RR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRrr, mmlValues::opzx3::rr);
    if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(mmlValues::opzx3::rr - val), juce::sendNotification);
    // RR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opzx3::rr);
        if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(val), juce::sendNotification);
    }
}

void GuiOpzx3::updateOpEnable(int idx, bool enable)
{
    opGroups[idx].setEnabled(enable);
    catMain[idx].setEnabled(enable);
    mul[idx].setEnabled(enable);
    mul[idx].label.setEnabled(enable);
    dt1[idx].setEnabled(enable);
    dt1[idx].label.setEnabled(enable);
    dt2[idx].setEnabled(enable);
    dt2[idx].label.setEnabled(enable);
    ar[idx].setEnabled(enable);
    ar[idx].label.setEnabled(enable);
    arTo000[idx].setEnabled(enable);
    arTo003[idx].setEnabled(enable);
    d1r[idx].setEnabled(enable);
    d1r[idx].label.setEnabled(enable);
    d1l[idx].setEnabled(enable);
    d1l[idx].label.setEnabled(enable);
    rr[idx].setEnabled(enable);
    rr[idx].label.setEnabled(enable);
    rrTo000[idx].setEnabled(enable);
    rrTo003[idx].setEnabled(enable);
    d2r[idx].setEnabled(enable);
    d2r[idx].label.setEnabled(enable);
    tl[idx].setEnabled(enable);
    tl[idx].label.setEnabled(enable);
    ks[idx].setEnabled(enable);
    ks[idx].label.setEnabled(enable);
    phaseOffset[idx].setEnabled(enable);
    phaseOffset[idx].label.setEnabled(enable);
    catShape[idx].setEnabled(enable);
    se[idx].setEnabled(enable);
    se[idx].label.setEnabled(enable);
    seFreq[idx].setEnabled(enable);
    seFreq[idx].label.setEnabled(enable);
    cafFix[idx].setEnabled(enable);
    fix[idx].setEnabled(enable);
    freq[idx].setEnabled(enable);
    freq[idx].label.setEnabled(enable);
    freqToZero[idx].setEnabled(enable);
    freqTo05[idx].setEnabled(enable);
    freqTo1[idx].setEnabled(enable);
    freqTo2[idx].setEnabled(enable);
    freqTo440[idx].setEnabled(enable);
    catLfo[idx].setEnabled(enable);
    pm[idx].setEnabled(enable);
    pms[idx].setEnabled(enable);
    pms[idx].label.setEnabled(enable);
    am[idx].setEnabled(enable);
    ams[idx].setEnabled(enable);
    ams[idx].label.setEnabled(enable);
    ws[idx].setEnabled(enable);
    ws[idx].label.setEnabled(enable);
    loadPcmBtn[idx].setEnabled(enable);
    clearPcmBtn[idx].setEnabled(enable);
    pcmFileNameLabel[idx].setEnabled(enable);
    pcmOffset[idx].setEnabled(enable);
    pcmOffset[idx].label.setEnabled(enable);
    pcmRatio[idx].setEnabled(enable);
    pcmRatio[idx].label.setEnabled(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    catMml[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
}

void GuiOpzx3::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > 35) return;

    for (int i = 0; i < 4; ++i)
    {
        juce::String newTitle = GuiText::Group::opPrefix + juce::String(i + 1) + algOpPrefix[algIndex][i];

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

void GuiOpzx3::updateRgDisplayAsOp(int idx, bool rgMode)
{
    rgAr[idx].label.setVisible(rgMode);
    rgAr[idx].setVisible(rgMode);
    rgD1r[idx].label.setVisible(rgMode);
    rgD1r[idx].setVisible(rgMode);
    rgD1l[idx].label.setVisible(rgMode);
    rgD1l[idx].setVisible(rgMode);
    rgD2r[idx].label.setVisible(rgMode);
    rgD2r[idx].setVisible(rgMode);
    rgRr[idx].label.setVisible(rgMode);
    rgRr[idx].setVisible(rgMode);
    rgTl[idx].label.setVisible(rgMode);
    rgTl[idx].setVisible(rgMode);

    ar[idx].label.setVisible(!rgMode);
    ar[idx].setVisible(!rgMode);
    arTo000[idx].setVisible(!rgMode);
    arTo003[idx].setVisible(!rgMode);
    d1r[idx].label.setVisible(!rgMode);
    d1r[idx].setVisible(!rgMode);
    d1l[idx].label.setVisible(!rgMode);
    d1l[idx].setVisible(!rgMode);
    d2r[idx].label.setVisible(!rgMode);
    d2r[idx].setVisible(!rgMode);
    rr[idx].label.setVisible(!rgMode);
    rr[idx].setVisible(!rgMode);
    rrTo000[idx].setVisible(!rgMode);
    rrTo003[idx].setVisible(!rgMode);
    tl[idx].label.setVisible(!rgMode);
    tl[idx].setVisible(!rgMode);
}

void GuiOpzx3::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}
