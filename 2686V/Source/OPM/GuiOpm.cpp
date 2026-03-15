#include "GuiOpm.h"

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

void GuiOpm::setup()
{
    const juce::String code = PrKey::Prefix::opm;
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

    algSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::alg, .title = GuiText::Fm::alg, .items = opmAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    feedbackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb0, .title = GuiText::Fm::fb0, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    feedback2Slider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::fb2, .title = GuiText::Fm::fb2, .isReset = true });
    feedback2Slider.setWantsKeyboardFocus(true);
    feedback2Slider.setExplicitFocusOrder(++tabOrder);

    lfoCat.setup({ .parent = *this, .title = GuiText::Category::hwLfo });

    lfoFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::freq, .title = GuiText::Fm::lfoFreq, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::amSmoothRatio, .title = GuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmToggle.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pm, .title = GuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPgShapeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pgShape, .title = GuiText::Fm::lfoPgShape, .items = lfoPgShapeItems, .isReset = true });
    lfoPgShapeSelector.setWantsKeyboardFocus(true);
    lfoPgShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmsSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pms, .title = GuiText::Fm::pms, .items = pmsItems, .isReset = true });
    lfoPmsSelector.setWantsKeyboardFocus(true);
    lfoPmsSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::pmd, .title = GuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::am, .title = GuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoEgShapeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::egShape, .title = GuiText::Fm::lfoEgShape, .items = lfoEgShapeItems, .isReset = true });
    lfoEgShapeSelector.setWantsKeyboardFocus(true);
    lfoEgShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmsSelector.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::ams, .title = GuiText::Fm::ams, .items = amsItems, .isReset = true });
    lfoAmsSelector.setWantsKeyboardFocus(true);
    lfoAmsSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Lfo::amd, .title = GuiText::Fm::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setup({ .parent = *this, .title = GuiText::Category::monoMode });

    monoModeToggle.setup({ .parent = *this, .id = PrKey::monoMode, .title = GuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    auto docDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);

    for (int i = 0; i < 8; ++i)
    {
        juce::String fileName = juce::String::formatted(Io::Folder::asset + "/" + Io::Folder::resource + "/ALG_OPNA_OPN_OPM_%02d.png", i);
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

        cafFix[i].setup({ .parent = *this, .title = GuiText::Category::fix });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fix, .title = GuiText::Fm::Op::Fix, .isReset = true });
        fix[i].setWantsKeyboardFocus(true);
        fix[i].setExplicitFocusOrder(++tabOrder);

        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + PrKey::Post::Fm::Op::fixFreq, .title = GuiText::Fm::Op::FFreq, .isReset = true });
        freq[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        freq[i].setWantsKeyboardFocus(true);
        freq[i].setExplicitFocusOrder(++tabOrder);

        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].setWantsKeyboardFocus(true);
        freqToZero[i].setExplicitFocusOrder(++tabOrder);
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };

        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = GuiText::Fm::Op::FreqTo440, .isReset = false, .isResized = false });
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
            .hintMessage = "e.g. AR:31/RAR:0 D1R:0 D1L:0 D2R:0 RR:15 TL:0 MUL:1 DT:10 DT:20",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });
    }
}

void GuiOpm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutMainCategory({ .mainRect = mRect, .label = &presetNameCat });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = GuiValue::PresetName::paddingBottom });
    layoutMainCategory({ .mainRect = mRect, .label = &qualityCat });
    layoutMain({ .mainRect = mRect, .label = &bitSelector.label, .component = &bitSelector });
    layoutMain({ .mainRect = mRect, .label = &rateSelector.label, .component = &rateSelector, });
    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });
    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });
    layoutMain({ .mainRect = mRect, .label = &feedback2Slider.label, .component = &feedback2Slider, });

    layoutMainCategory({ .mainRect = mRect, .label = &lfoCat });

    layoutMain({ .mainRect = mRect, .label = &lfoFreqSlider.label, .component = &lfoFreqSlider });
    layoutMain({ .mainRect = mRect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
    layoutMain({ .mainRect = mRect, .component = &lfoPmToggle });
    layoutMain({ .mainRect = mRect, .label = &lfoPgShapeSelector.label, .component = &lfoPgShapeSelector });
    layoutMain({ .mainRect = mRect, .label = &lfoPmsSelector.label, .component = &lfoPmsSelector });
    layoutMain({ .mainRect = mRect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
    layoutMain({ .mainRect = mRect, .component = &lfoAmToggle });
    layoutMain({ .mainRect = mRect, .label = &lfoEgShapeSelector.label, .component = &lfoEgShapeSelector });
    layoutMain({ .mainRect = mRect, .label = &lfoAmsSelector.label, .component = &lfoAmsSelector });
    layoutMain({ .mainRect = mRect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });
    layoutMainCategory({ .mainRect = mRect, .label = &mvolCat, .paddingTop = GuiValue::MVol::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, });
    layoutMainCategory({ .mainRect = mRect, .label = &monoPolyCat });
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

        layoutRowCategory({ .rowRect = innerRect, .component = &catMain[i] });
        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
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
            layoutRowTwoComps({ .rect = innerRect, .comp1 = &arTo000[i], .comp2 = &arTo003[i] });
            layoutRow({ .rowRect = innerRect, .label = &d1r[i].label, .component = &d1r[i] });
            layoutRow({ .rowRect = innerRect, .label = &d1l[i].label, .component = &d1l[i] });
            layoutRow({ .rowRect = innerRect, .label = &d2r[i].label, .component = &d2r[i] });
            layoutRow({ .rowRect = innerRect, .label = &rr[i].label, .component = &rr[i] });
            layoutRowTwoComps({ .rect = innerRect, .comp1 = &rrTo000[i], .comp2 = &rrTo003[i] });
            layoutRow({ .rowRect = innerRect, .label = &tl[i].label, .component = &tl[i] });
        }
        layoutRow({ .rowRect = innerRect, .label = &ks[i].label, .component = &ks[i], });
        layoutRowCategory({ .rowRect = innerRect, .component = &cafFix[i] });
        layoutRow({ .rowRect = innerRect, .component = &fix[i] });
        layoutRow({ .rowRect = innerRect, .label = &freq[i].label, .component = &freq[i] });
        layoutRowTwoComps({ .rect = innerRect, .comp1 = &freqToZero[i], .comp2 = &freqTo440[i], });
        layoutRowCategory({ .rowRect = innerRect, .component = &catMask[i] });
        layoutRow({ .rowRect = innerRect, .component = &mask[i], });
        layoutRowCategory({ .rowRect = innerRect, .component = &catMml[i] });
        layoutRow({ .rowRect = innerRect, .component = &mml[i], .paddingBottom = 0 });
    }

    updateAlgorithmDisplay();
}

// ==============================================================================
// MML Parsing Logic (Template to handle different GuiSets)
// ==============================================================================
void GuiOpm::applyMmlString(const juce::String& mml, int opIndex)
{
    juce::String input = mml.toUpperCase();
    int val;

    // MUL
    val = RegisterConverter::getValue(input, mmlPrefixMul, mmlValues::opm::mul);
    if (RegisterConverter::isValidVal(val)) mul[opIndex].setValue((double)RegisterConverter::convertFmMul(val), juce::sendNotification);

    // DT1
    val = RegisterConverter::getValue(input, mmlPrefixDt1, mmlValues::opm::dt1);
    if (RegisterConverter::isValidVal(val)) {
        int regVal = RegisterConverter::convertMmlDtToReg(val);

        dt1[opIndex].setSelectedItemIndex((double)regVal, juce::sendNotification);
    }

    // DT2
    val = RegisterConverter::getValue(input, mmlPrefixDt2, mmlValues::opm::dt2);
    if (RegisterConverter::isValidVal(val)) dt2[opIndex].setValue((double)val, juce::sendNotification);

    // KS
    val = RegisterConverter::getValue(input, mmlPrefixKs, mmlValues::opm::ks);
    if (RegisterConverter::isValidVal(val)) ks[opIndex].setSelectedItemIndex(RegisterConverter::convertFmKs(val), juce::sendNotification);

    // MASK
    val = RegisterConverter::getValue(input, mmlPrefixMask, mmlValues::opm::mask);
    if (RegisterConverter::isValidVal(val)) mask[opIndex].setToggleState(RegisterConverter::convertFmMask(val), juce::sendNotification);

    bool rgMode = rgEn[opIndex].getToggleState();

    if (rgMode)
    {
        // TL
        val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opm::tl);
        if (RegisterConverter::isValidVal(val)) rgTl[opIndex].setValue(RegisterConverter::convertFmRg127(val), juce::sendNotification);

        // AR
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opm::ar);
        if (RegisterConverter::isValidVal(val)) rgAr[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // DR
        val = RegisterConverter::getValue(input, mmlPrefixD1r, mmlValues::opm::d1r);
        if (RegisterConverter::isValidVal(val)) rgD1r[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // SR
        val = RegisterConverter::getValue(input, mmlPrefixD2r, mmlValues::opm::d2r);
        if (RegisterConverter::isValidVal(val)) rgD2r[opIndex].setValue(RegisterConverter::convertFmRg31(val), juce::sendNotification);

        // SL
        val = RegisterConverter::getValue(input, mmlPrefixD1l, mmlValues::opm::d1l);
        if (RegisterConverter::isValidVal(val)) rgD1l[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        // RR
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opm::rr);
        if (RegisterConverter::isValidVal(val)) rgRr[opIndex].setValue(RegisterConverter::convertFmRg15(val), juce::sendNotification);

        return;
    }

    // TL
    val = RegisterConverter::getValue(input, mmlPrefixTl, mmlValues::opm::tl);
    if (RegisterConverter::isValidVal(val)) tl[opIndex].setValue(RegisterConverter::convertFmTl(val), juce::sendNotification);

    // AR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRar, mmlValues::opm::ar);
    if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(mmlValues::opm::ar - val), juce::sendNotification);
    // AR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixAr, mmlValues::opm::ar);
        if (RegisterConverter::isValidVal(val)) ar[opIndex].setValue(RegisterConverter::convertFmAr(val), juce::sendNotification);
    }

    // D1R(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRd1r, mmlValues::opm::d1r);
    if (RegisterConverter::isValidVal(val)) d1r[opIndex].setValue(RegisterConverter::convertFmDr(mmlValues::opm::d1r - val), juce::sendNotification);
    // D1R
    else {
        val = RegisterConverter::getValue(input, mmlPrefixD1r, mmlValues::opm::d1r);
        if (RegisterConverter::isValidVal(val)) d1r[opIndex].setValue(RegisterConverter::convertFmDr(val), juce::sendNotification);
    }

    // D1L
    val = RegisterConverter::getValue(input, mmlPrefixD1l, mmlValues::opm::d1l);
    if (RegisterConverter::isValidVal(val)) d1l[opIndex].setValue(RegisterConverter::convertFmSl(val), juce::sendNotification);

    // D2R(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRd2r, mmlValues::opm::d2r);
    if (RegisterConverter::isValidVal(val)) d2r[opIndex].setValue(RegisterConverter::convertFmSr(mmlValues::opm::d2r - val), juce::sendNotification);
    // D2R
    else {
        val = RegisterConverter::getValue(input, mmlPrefixD2r, mmlValues::opm::d2r);
        if (RegisterConverter::isValidVal(val)) d2r[opIndex].setValue(RegisterConverter::convertFmSr(val), juce::sendNotification);
    }

    // RR(Reverse)
    val = RegisterConverter::getValue(input, mmlPrefixRrr, mmlValues::opm::rr);
    if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(mmlValues::opm::rr - val), juce::sendNotification);
    // RR
    else {
        val = RegisterConverter::getValue(input, mmlPrefixRr, mmlValues::opm::rr);
        if (RegisterConverter::isValidVal(val)) rr[opIndex].setValue(RegisterConverter::convertFmRr(val), juce::sendNotification);
    }
}

void GuiOpm::updateOpEnable(int idx, bool enable)
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
    cafFix[idx].setEnabled(enable);
    fix[idx].setEnabled(enable);
    freq[idx].setEnabled(enable);
    freq[idx].label.setEnabled(enable);
    freqToZero[idx].setEnabled(enable);
    freqTo440[idx].setEnabled(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    catMml[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
}

void GuiOpm::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > 7) return;

    for (int i = 0; i < 4; ++i)
    {
        juce::String newTitle = GuiText::Group::opPrefix + juce::String(i + 1) + algOpPrefix[algIndex][i];

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

void GuiOpm::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}
