#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"


#include "../../Processor/Opm/Keys.h"
#include "../../Processor/Opm/Values.h"
#include "../../Core/Const/MmlKeys.h"
#include "../../Core/Const/MmlValues.h"

#include "../../Core/Fm/RegisterConverter.h"
#include "../../Core/Fm/FmMmlFormatter.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiValues.h"
#include "./GuiText.h"
#include "../../Core/Gui/GuiStructs.h"

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
    auto setupPanBtn = [this](GuiTextButton& btn, const juce::String& text, int& tabOrder)
        {
            addAndMakeVisible(btn);
            btn.setButtonText(text);
            btn.setWantsKeyboardFocus(true);
            btn.setExplicitFocusOrder(++tabOrder);
        };

    // このタブ(Component)がキーボードフォーカスを受け取れるようにする
    setWantsKeyboardFocus(true);

    const juce::String code = OpmPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, OpmGuiText::Group::mainGroup);

    presetNameCat.setup({ .parent = *this, .title = OpmGuiText::Category::preset });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    qualityCat.setup({ .parent = *this, .title = OpmGuiText::Category::visibleQuality, .invisibleTitle = OpmGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = *this, .id = code + OpmPrKey::bit, .title = OpmGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + OpmPrKey::rate, .title = OpmGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setup({ .parent = *this, .title = OpmGuiText::Category::algFb });

    algSelector.setup({ .parent = *this, .id = code + OpmPrKey::alg, .title = OpmGuiText::Fm::alg, .items = opmAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    feedbackSlider.setup({ .parent = *this, .id = code + OpmPrKey::fb0, .title = OpmGuiText::Fm::fb0, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    feedback2Slider.setup({ .parent = *this, .id = code + OpmPrKey::fb2, .title = OpmGuiText::Fm::fb2, .isReset = true });
    feedback2Slider.setWantsKeyboardFocus(true);
    feedback2Slider.setExplicitFocusOrder(++tabOrder);

    panCat.setup({ .parent = *this, .title = OpmGuiText::Category::visiblePan, .invisibleTitle = OpmGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

    panSlider.setup({ .parent = *this, .id = code + OpmPrKey::pan, .title = OpmGuiText::Fm::pan, .isReset = true });
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

    lfoCat.setup({ .parent = *this, .title = OpmGuiText::Category::visibleHwLfo, .invisibleTitle = OpmGuiText::Category::invisibileHwLfo, .enableChangeDetailVisible = true });

    lfoFreqSlider.setup({ .parent = *this, .id = code + OpmPrKey::Lfo::freq, .title = OpmGuiText::Fm::lfoFreq, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + OpmPrKey::Lfo::amSmoothRatio, .title = OpmGuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = *this, .id = code + OpmPrKey::Lfo::syncDelay, .title = OpmGuiText::Fm::lfoSyncDelay, .isReset = true });
    lfoSyncDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoSyncDelaySlider.setWantsKeyboardFocus(true);
    lfoSyncDelaySlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelayToZeroBtn.setup({ .parent = *this, .title = "Async", .isReset = false, .isResized = false });
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

    lfoPmToggle.setup({ .parent = *this, .id = code + OpmPrKey::Lfo::pm, .title = OpmGuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPgShapeSelector.setup({ .parent = *this, .id = code + OpmPrKey::Lfo::pgShape, .title = OpmGuiText::Fm::lfoPgShape, .items = lfoPgShapeItems, .isReset = true });
    lfoPgShapeSelector.setWantsKeyboardFocus(true);
    lfoPgShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmsSelector.setup({ .parent = *this, .id = code + OpmPrKey::Lfo::pms, .title = OpmGuiText::Fm::pms, .items = pmsItems, .isReset = true });
    lfoPmsSelector.setWantsKeyboardFocus(true);
    lfoPmsSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = *this, .id = code + OpmPrKey::Lfo::pmd, .title = OpmGuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = *this, .id = code + OpmPrKey::Lfo::am, .title = OpmGuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoEgShapeSelector.setup({ .parent = *this, .id = code + OpmPrKey::Lfo::egShape, .title = OpmGuiText::Fm::lfoEgShape, .items = lfoEgShapeItems, .isReset = true });
    lfoEgShapeSelector.setWantsKeyboardFocus(true);
    lfoEgShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmsSelector.setup({ .parent = *this, .id = code + OpmPrKey::Lfo::ams, .title = OpmGuiText::Fm::ams, .items = amsItems, .isReset = true });
    lfoAmsSelector.setWantsKeyboardFocus(true);
    lfoAmsSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = *this, .id = code + OpmPrKey::Lfo::amd, .title = OpmGuiText::Fm::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setup({ .parent = *this, .title = OpmGuiText::Category::visibleMonoMode, .invisibleTitle = OpmGuiText::Category::invisibleMonoMode, .enableChangeDetailVisible = true });

    monoModeToggle.setup({ .parent = *this, .id = OpmPrKey::monoMode, .title = OpmGuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = OpmGuiText::Category::visibleMvol, .invisibleTitle = OpmGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

    masterVolSlider.setup({ .parent = *this, .id = OpmPrKey::masterVol, .title = OpmGuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

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
    const juce::String opCode = code + OpmPrKey::op;

    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, OpmGuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        catMain[i].setup({ .parent = *this, .title = OpmGuiText::Category::m });

        mul[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::mul, .title = OpmGuiText::Fm::Op::Mul, .items = multems, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);

        dt1[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::dt, .title = OpmGuiText::Fm::Op::Dt1, .isReset = true, .regType = RegisterType::FmDt });
        dt1[i].setWantsKeyboardFocus(true);
        dt1[i].setExplicitFocusOrder(++tabOrder);

        dt2[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::dt2, .title = OpmGuiText::Fm::Op::Dt2, .isReset = true, .regType = RegisterType::FmDt2 });
        dt2[i].setWantsKeyboardFocus(true);
        dt2[i].setExplicitFocusOrder(++tabOrder);

        rgAr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::rgAr, .title = OpmGuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgD1r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::rgD1r, .title = OpmGuiText::Fm::Op::D1r, .isReset = true });
        rgD1r[i].setWantsKeyboardFocus(true);
        rgD1r[i].setExplicitFocusOrder(++tabOrder);

        rgD1l[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::rgD1l, .title = OpmGuiText::Fm::Op::D1l, .isReset = true });
        rgD1l[i].setWantsKeyboardFocus(true);
        rgD1l[i].setExplicitFocusOrder(++tabOrder);

        rgD2r[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::rgD2r, .title = OpmGuiText::Fm::Op::D2r, .isReset = true });
        rgD2r[i].setWantsKeyboardFocus(true);
        rgD2r[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::rgRr, .title = OpmGuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::rgTl, .title = OpmGuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::ks, .title = OpmGuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });
        ks[i].setWantsKeyboardFocus(true);
        ks[i].setExplicitFocusOrder(++tabOrder);

        cafLfo[i].setup({ .parent = *this, .title = OpmGuiText::Category::visibleHwLfo, .invisibleTitle = OpmGuiText::Category::invisibileHwLfo, .enableChangeDetailVisible = true });

        amsEnable[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::amsEn, .title = OpmGuiText::Fm::Op::AmsEn, .isReset = true });
        amsEnable[i].setWantsKeyboardFocus(true);
        amsEnable[i].setExplicitFocusOrder(++tabOrder);

        cafFix[i].setup({ .parent = *this, .title = OpmGuiText::Category::visibleFix, .invisibleTitle = OpmGuiText::Category::invisibleFix, .enableChangeDetailVisible = true });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::fix, .title = OpmGuiText::Fm::Op::Fix, .isReset = true });
        fix[i].setWantsKeyboardFocus(true);
        fix[i].setExplicitFocusOrder(++tabOrder);

        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::fixFreq, .title = OpmGuiText::Fm::Op::FFreq, .isReset = true });
        freq[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        freq[i].setWantsKeyboardFocus(true);
        freq[i].setExplicitFocusOrder(++tabOrder);

        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = OpmGuiText::Fm::Op::FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].setWantsKeyboardFocus(true);
        freqToZero[i].setExplicitFocusOrder(++tabOrder);
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };

        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = OpmGuiText::Fm::Op::FreqTo440, .isReset = false, .isResized = false });
        freqTo440[i].setWantsKeyboardFocus(true);
        freqTo440[i].setExplicitFocusOrder(++tabOrder);
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        catMask[i].setup({ .parent = *this, .title = OpmGuiText::Category::visibleMask, .invisibleTitle = OpmGuiText::Category::invisibleMask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::mask, .title = OpmGuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        catMml[i].setup({ .parent = *this, .title = OpmGuiText::Category::mml });

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

    auto mainArea = pageArea.removeFromLeft(OpmGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(OpmGuiValue::Group::Padding::width, OpmGuiValue::Group::Padding::height);
    mRect.removeFromTop(OpmGuiValue::Group::TitlePaddingTop);

    layoutMainCategory({ .mainRect = mRect, .label = &presetNameCat });
    layoutMain({ .mainRect = mRect, .label = &presetNameLabel, .paddingBottom = OpmGuiValue::PresetName::paddingBottom });

    layoutQualityCat(mRect);

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });
    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });
    layoutMain({ .mainRect = mRect, .label = &feedback2Slider.label, .component = &feedback2Slider, });

    layoutPanCat(mRect);

    layoutHwLfoCat(mRect);

    layoutMonoModeCat(mRect);

    layoutMvolCat(mRect);

    auto imgArea = mRect.removeFromBottom(100);
    algImageComp.setBounds(imgArea);
    mRect.removeFromTop(OpmGuiValue::Category::paddingTop);

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(OpmGuiValue::Fm::Op::width);

        // 枠線
        opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(OpmGuiValue::Fm::Op::Padding::width, OpmGuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(OpmGuiValue::Group::TitlePaddingTop);

        layoutRowCategory({ .rowRect = innerRect, .component = &catMain[i] });
        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt1[i].label, .component = &dt1[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt2[i].label, .component = &dt2[i] });
        updateRgDisplayAsOp(i, true);
        layoutRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgD1r[i].label, .component = &rgD1r[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgD1l[i].label, .component = &rgD1l[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgD2r[i].label, .component = &rgD2r[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i] });
        layoutRow({ .rowRect = innerRect, .label = &ks[i].label, .component = &ks[i], });

        layoutOpHwLfoCat(i, innerRect);

        layoutOpFixCat(i, innerRect);

        layoutOpMaskCat(i, innerRect);

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
    catMain[idx].setEnabled(enable);
    mul[idx].setEnabledWithLabel(enable);
    dt1[idx].setEnabledWithLabel(enable);
    dt2[idx].setEnabledWithLabel(enable);
    ks[idx].setEnabledWithLabel(enable);
    cafFix[idx].setEnabled(enable);
    fix[idx].setEnabled(enable);
    freq[idx].setEnabledWithLabel(enable);
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
            //     ALG   FB0   FB2   MSK  FRQ  SYC   PG   EG  PMS  PMD  AMS  AMD
            u8"    %1d,  %1d,  %1d,  %2d, %3d, %3d, %1d, %1d, %1d, %3d, %1d, %3d\n",
            algSelector.getSelectedId() - 1,        // ALG
            (int)feedbackSlider.getValue(),         // FB0
            (int)feedback2Slider.getValue(),        // FB2
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

void GuiOpm::layoutOpMaskCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &catMask[opIndex] });

    bool visibleMask = catMask[opIndex].isDetailVisible();

    mask[opIndex].setVisible(visibleMask);

    if (visibleMask)
    {
        layoutRow({ .rowRect = rect, .component = &mask[opIndex] });
    }
}

void GuiOpm::layoutOpFixCat(int opIndex, juce::Rectangle<int>& rect) {
    layoutRowCategory({ .rowRect = rect, .component = &cafFix[opIndex] });

    bool visibleFix = cafFix[opIndex].isDetailVisible();

    fix[opIndex].setVisible(visibleFix);
    freq[opIndex].setVisible(visibleFix);
    freq[opIndex].label.setVisible(visibleFix);
    freqToZero[opIndex].setVisible(visibleFix);
    freqTo440[opIndex].setVisible(visibleFix);

    if (visibleFix)
    {
        layoutRow({ .rowRect = rect, .component = &fix[opIndex] });
        layoutRow({ .rowRect = rect, .label = &freq[opIndex].label, .component = &freq[opIndex] });
        layoutRowTwoComps({ .rect = rect, .comp1 = &freqToZero[opIndex], .comp2 = &freqTo440[opIndex] });
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

void GuiOpm::layoutMonoModeCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &monoPolyCat });

    bool visible = monoPolyCat.isDetailVisible();

    monoModeToggle.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoModeToggle });
    }
}

void GuiOpm::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
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
