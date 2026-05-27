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

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    mainGroup.setup(*this, OpmGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    qualityCat.setupHwCategory({ .parent = *this, .title = OpmGuiText::Category::visibleQuality, .invisibleTitle = OpmGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = *this, .id = code + OpmPrKey::bit, .title = OpmGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + OpmPrKey::rate, .title = OpmGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setupHwCategory({ .parent = *this, .title = OpmGuiText::Category::algFb });

    algSelector.setup({ .parent = *this, .id = code + OpmPrKey::alg, .title = OpmGuiText::Fm::alg, .items = opmAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    feedbackSlider.setup({ .parent = *this, .id = code + OpmPrKey::fb, .title = OpmGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    panCat.setupHwCategory({ .parent = *this, .title = OpmGuiText::Category::visiblePan, .invisibleTitle = OpmGuiText::Category::invisiblePan, .enableChangeDetailVisible = true });

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

    lfoCat.setupHwCategory({ .parent = *this, .title = OpmGuiText::Category::visibleHwLfo, .invisibleTitle = OpmGuiText::Category::invisibileHwLfo, .enableChangeDetailVisible = true });

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

    monoPolyCat.setupOtherCategory({ .parent = *this, .title = OpmGuiText::Category::visibleMonoMode, .invisibleTitle = OpmGuiText::Category::invisibleMonoMode, .enableChangeDetailVisible = true });

    monoModeToggle.setup({ .parent = *this, .id = OpmPrKey::monoMode, .title = OpmGuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setupOtherCategory({ .parent = *this, .title = OpmGuiText::Category::visibleMvol, .invisibleTitle = OpmGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

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

        catPitchEnv[i].setupSwCategory({ .parent = *this, .title = OpmGuiText::Category::visiblePitchAdsr, .invisibleTitle = OpmGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

        pitchEnvEnable[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::PitchAdsr::enable, .title = OpmGuiText::PitchAdsr::enable, .isReset = true });
        pitchEnvEnable[i].setWantsKeyboardFocus(true);
        pitchEnvEnable[i].setExplicitFocusOrder(++tabOrder);

        pitchAttack[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::PitchAdsr::ar, .title = OpmGuiText::PitchAdsr::ar, .isReset = true });
        pitchAttack[i].setWantsKeyboardFocus(true);
        pitchAttack[i].setExplicitFocusOrder(++tabOrder);

        pitchDecay[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::PitchAdsr::dr , .title = OpmGuiText::PitchAdsr::dr, .isReset = true });
        pitchDecay[i].setWantsKeyboardFocus(true);
        pitchDecay[i].setExplicitFocusOrder(++tabOrder);

        pitchRelease[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::PitchAdsr::rr, .title = OpmGuiText::PitchAdsr::rr, .isReset = true });
        pitchRelease[i].setWantsKeyboardFocus(true);
        pitchRelease[i].setExplicitFocusOrder(++tabOrder);

        pitchStartLevel[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::PitchAdsr::stl, .title = OpmGuiText::PitchAdsr::stl, .isReset = true });
        pitchStartLevel[i].setWantsKeyboardFocus(true);
        pitchStartLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchAttackLevel[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::PitchAdsr::atl, .title = OpmGuiText::PitchAdsr::atl, .isReset = true });
        pitchAttackLevel[i].setWantsKeyboardFocus(true);
        pitchAttackLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchSustainLevel[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::PitchAdsr::ssl, .title = OpmGuiText::PitchAdsr::ssl, .isReset = true });
        pitchSustainLevel[i].setWantsKeyboardFocus(true);
        pitchSustainLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchReleaseLevel[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::PitchAdsr::rll, .title = OpmGuiText::PitchAdsr::rll, .isReset = true });
        pitchReleaseLevel[i].setWantsKeyboardFocus(true);
        pitchReleaseLevel[i].setExplicitFocusOrder(++tabOrder);

        catSsgSwEnv[i].setupSwCategory({ .parent = *this, .title = OpmGuiText::Category::visibleSwEnv, .invisibleTitle = OpmGuiText::Category::invisibleSwEnv, .enableChangeDetailVisible = true });

        ssgSwEnvEnable[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::enable, .title = OpmGuiText::SsgSwEnv::enable, .isReset = true });
        ssgSwEnvEnable[i].setWantsKeyboardFocus(true);
        ssgSwEnvEnable[i].setExplicitFocusOrder(++tabOrder);

        ssgSwSteps[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::steps, .title = OpmGuiText::SsgSwEnv::steps, .isReset = true });
        ssgSwSteps[i].setWantsKeyboardFocus(true);
        ssgSwSteps[i].setExplicitFocusOrder(++tabOrder);
        ssgSwSteps[i].onValueChange = [this, i] {
            bool ssgEnvLoopEnable = ssgSwEnvLoop[i].getToggleState();

            applyOpSsgSwEnvLoopValues(i, ssgEnvLoopEnable);
            };

        ssgSwEnvLoop[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::loop, .title = OpmGuiText::SsgSwEnv::loop, .isReset = true });
        ssgSwEnvLoop[i].setWantsKeyboardFocus(true);
        ssgSwEnvLoop[i].setExplicitFocusOrder(++tabOrder);
        ssgSwEnvLoop[i].onClick = [this, i] {
            bool ssgEnvLoopEnable = ssgSwEnvLoop[i].getToggleState();

            ssgSwLoopTo[i].setEnabled(ssgEnvLoopEnable);
            ssgSwLoopTo[i].label.setEnabled(ssgEnvLoopEnable);
            ssgSwLoopCount[i].setEnabled(ssgEnvLoopEnable);
            ssgSwLoopCount[i].label.setEnabled(ssgEnvLoopEnable);

            applyOpSsgSwEnvLoopValues(i, ssgEnvLoopEnable);
            };

        ssgSwLoopTo[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::loopTo, .title = OpmGuiText::SsgSwEnv::loopTo, .isReset = true });
        ssgSwLoopTo[i].setWantsKeyboardFocus(true);
        ssgSwLoopTo[i].setExplicitFocusOrder(++tabOrder);
        ssgSwLoopTo[i].onValueChange = [this, i] {
            bool ssgEnvLoopEnable = ssgSwEnvLoop[i].getToggleState();

            applyOpSsgSwEnvLoopValues(i, ssgEnvLoopEnable);
            };

        ssgSwLoopCount[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::loopCount, .title = OpmGuiText::SsgSwEnv::loopCount, .isReset = true });
        ssgSwLoopCount[i].setWantsKeyboardFocus(true);
        ssgSwLoopCount[i].setExplicitFocusOrder(++tabOrder);

        bool ssgEnvLoopEnable = ssgSwEnvLoop[i].getToggleState();

        ssgSwLoopTo[i].setEnabled(ssgEnvLoopEnable);
        ssgSwLoopTo[i].label.setEnabled(ssgEnvLoopEnable);
        ssgSwLoopCount[i].setEnabled(ssgEnvLoopEnable);
        ssgSwLoopCount[i].label.setEnabled(ssgEnvLoopEnable);

        ssgSwStartLevel[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::stl, .title = OpmGuiText::SsgSwEnv::stl, .isReset = true });
        ssgSwStartLevel[i].setWantsKeyboardFocus(true);
        ssgSwStartLevel[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR1[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::r1, .title = OpmGuiText::SsgSwEnv::r1, .isReset = true });
        ssgSwR1[i].setWantsKeyboardFocus(true);
        ssgSwR1[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL1[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::l1, .title = OpmGuiText::SsgSwEnv::l1, .isReset = true });
        ssgSwL1[i].setWantsKeyboardFocus(true);
        ssgSwL1[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR2[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::r2, .title = OpmGuiText::SsgSwEnv::r2, .isReset = true });
        ssgSwR2[i].setWantsKeyboardFocus(true);
        ssgSwR2[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL2[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::l2, .title = OpmGuiText::SsgSwEnv::l2, .isReset = true });
        ssgSwL2[i].setWantsKeyboardFocus(true);
        ssgSwL2[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR3[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::r3, .title = OpmGuiText::SsgSwEnv::r3, .isReset = true });
        ssgSwR3[i].setWantsKeyboardFocus(true);
        ssgSwR3[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL3[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::l3, .title = OpmGuiText::SsgSwEnv::l3, .isReset = true });
        ssgSwL3[i].setWantsKeyboardFocus(true);
        ssgSwL3[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR4[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::r4, .title = OpmGuiText::SsgSwEnv::r4, .isReset = true });
        ssgSwR4[i].setWantsKeyboardFocus(true);
        ssgSwR4[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL4[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::l4, .title = OpmGuiText::SsgSwEnv::l4, .isReset = true });
        ssgSwL4[i].setWantsKeyboardFocus(true);
        ssgSwL4[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR5[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::r5, .title = OpmGuiText::SsgSwEnv::r5, .isReset = true });
        ssgSwR5[i].setWantsKeyboardFocus(true);
        ssgSwR5[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL5[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::l5, .title = OpmGuiText::SsgSwEnv::l5, .isReset = true });
        ssgSwL5[i].setWantsKeyboardFocus(true);
        ssgSwL5[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR6[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::r6, .title = OpmGuiText::SsgSwEnv::r6, .isReset = true });
        ssgSwR6[i].setWantsKeyboardFocus(true);
        ssgSwR6[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL6[i].setup({ .parent = *this, .id = paramPrefix + OpmPrKey::SsgSwEnv::l6, .title = OpmGuiText::SsgSwEnv::l6, .isReset = true });
        ssgSwL6[i].setWantsKeyboardFocus(true);
        ssgSwL6[i].setExplicitFocusOrder(++tabOrder);

        cafLfo[i].setupHwCategory({ .parent = *this, .title = OpmGuiText::Category::visibleHwLfo, .invisibleTitle = OpmGuiText::Category::invisibileHwLfo, .enableChangeDetailVisible = true });

        amsEnable[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::amsEn, .title = OpmGuiText::Fm::Op::AmsEn, .isReset = true });
        amsEnable[i].setWantsKeyboardFocus(true);
        amsEnable[i].setExplicitFocusOrder(++tabOrder);

        cafFix[i].setupHwCategory({ .parent = *this, .title = OpmGuiText::Category::visibleFix, .invisibleTitle = OpmGuiText::Category::invisibleFix, .enableChangeDetailVisible = true });

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

        catMask[i].setupHwCategory({ .parent = *this, .title = OpmGuiText::Category::visibleMask, .invisibleTitle = OpmGuiText::Category::invisibleMask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpmPrKey::mask, .title = OpmGuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        catMml[i].setupOtherCategory({ .parent = *this, .title = OpmGuiText::Category::mml });

        mml[i].setup({ .parent = *this, .title = "MML", .isReset = false, .isResized = false });
        mml[i].setWantsKeyboardFocus(true);
        mml[i].setExplicitFocusOrder(++tabOrder);
        mml[i].setupMml({
            .opIndex = i,
            .hintMessage = "e.g. AR:31/RAR:0 D1R:0 D1L:0 D2R:0 RR:15 TL:0 MUL:1 DT:10 DT:20",
            .onMmlApplied = [this, i](juce::String mml) { this->applyMmlString(mml, i); }
            });

        setupGraph(i);
        updateOpGraph(i);
    }
}

void GuiOpm::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(OpmGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(OpmGuiValue::Group::Padding::width, OpmGuiValue::Group::Padding::height);
    mRect.removeFromTop(OpmGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });
    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    layoutPanCat(mRect);

    layoutHwLfoCat(mRect);

    layoutQualityCat(mRect);

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

        // 一番下にグラフエリアとボタンエリアを確保
        auto graphAndBtnArea = innerRect.removeFromBottom(150);
        layoutOpGraph(i, graphAndBtnArea);
        updateOpGraph(i);

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

        layoutOpPitchEnvCat(i, innerRect);

        layoutOpSsgSwEnvCat(i, innerRect);

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

void GuiOpm::layoutOpPitchEnvCat(int opIndex, juce::Rectangle<int>& rect)
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

void GuiOpm::layoutOpSsgSwEnvCat(int opIndex, juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &catSsgSwEnv[opIndex] });

    bool visible = catSsgSwEnv[opIndex].isDetailVisible();

    ssgSwEnvEnable[opIndex].setVisible(visible);
    ssgSwSteps[opIndex].setVisibleWithLabel(visible);
    ssgSwEnvLoop[opIndex].setVisible(visible);
    ssgSwLoopTo[opIndex].setVisibleWithLabel(visible);
    ssgSwLoopCount[opIndex].setVisibleWithLabel(visible);
    ssgSwStartLevel[opIndex].setVisibleWithLabel(visible);
    ssgSwR1[opIndex].setVisibleWithLabel(visible);
    ssgSwL1[opIndex].setVisibleWithLabel(visible);
    ssgSwR2[opIndex].setVisibleWithLabel(visible);
    ssgSwL2[opIndex].setVisibleWithLabel(visible);
    ssgSwR3[opIndex].setVisibleWithLabel(visible);
    ssgSwL3[opIndex].setVisibleWithLabel(visible);
    ssgSwR4[opIndex].setVisibleWithLabel(visible);
    ssgSwL4[opIndex].setVisibleWithLabel(visible);
    ssgSwR5[opIndex].setVisibleWithLabel(visible);
    ssgSwL5[opIndex].setVisibleWithLabel(visible);
    ssgSwR6[opIndex].setVisibleWithLabel(visible);
    ssgSwL6[opIndex].setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &ssgSwEnvEnable[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwSteps[opIndex].label, .component = &ssgSwSteps[opIndex] });
        layoutRow({ .rowRect = rect, .component = &ssgSwEnvLoop[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwLoopTo[opIndex].label, .component = &ssgSwLoopTo[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwLoopCount[opIndex].label, .component = &ssgSwLoopCount[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwStartLevel[opIndex].label, .component = &ssgSwStartLevel[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR1[opIndex].label, .component = &ssgSwR1[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL1[opIndex].label, .component = &ssgSwL1[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR2[opIndex].label, .component = &ssgSwR2[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL2[opIndex].label, .component = &ssgSwL2[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR3[opIndex].label, .component = &ssgSwR3[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL3[opIndex].label, .component = &ssgSwL3[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR4[opIndex].label, .component = &ssgSwR4[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL4[opIndex].label, .component = &ssgSwL4[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR5[opIndex].label, .component = &ssgSwR5[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL5[opIndex].label, .component = &ssgSwL5[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwR6[opIndex].label, .component = &ssgSwR6[opIndex] });
        layoutRow({ .rowRect = rect, .label = &ssgSwL6[opIndex].label, .component = &ssgSwL6[opIndex] });
    }
}

void GuiOpm::applyOpSsgSwEnvLoopValues(int opIndex, bool enabled)
{
    if (enabled)
    {
        int steps = static_cast<int>(ssgSwSteps[opIndex].getValue());

        // Steps が 1 のときはループできないため、Steps を 2 にする
        if (steps < 2) {
            steps = 2;
            ssgSwSteps[opIndex].setValue(steps);
        }

        int loopTo = static_cast<int>(ssgSwLoopTo[opIndex].getValue());

        // Steps - LoopTo が 2未満のときは、LoopTo を Steps - 2 にする
        if (steps - loopTo < 2) {
            ssgSwLoopTo[opIndex].setValue(steps - 2);
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

    rgAr[opIndex].onValueChange = repaintGraph;
    rgD1r[opIndex].onValueChange = repaintGraph;
    rgD2r[opIndex].onValueChange = repaintGraph;
    rgD1l[opIndex].onValueChange = repaintGraph;
    rgRr[opIndex].onValueChange = repaintGraph;
    rgTl[opIndex].onValueChange = repaintGraph;

    pitchAttack[opIndex].onValueChange = repaintGraph;
    pitchDecay[opIndex].onValueChange = repaintGraph;
    pitchRelease[opIndex].onValueChange = repaintGraph;
    pitchStartLevel[opIndex].onValueChange = repaintGraph;
    pitchAttackLevel[opIndex].onValueChange = repaintGraph;
    pitchSustainLevel[opIndex].onValueChange = repaintGraph;
    pitchReleaseLevel[opIndex].onValueChange = repaintGraph;

    ssgSwEnvLoop[opIndex].onStateChange = repaintGraph;

    ssgSwSteps[opIndex].onValueChange = [this, opIndex, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoop[opIndex].getToggleState();
        applyOpSsgSwEnvLoopValues(opIndex, ssgEnvLoopEnable);
        repaintGraph();
        };
    ssgSwLoopTo[opIndex].onValueChange = [this, opIndex, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoop[opIndex].getToggleState();
        applyOpSsgSwEnvLoopValues(opIndex, ssgEnvLoopEnable);
        repaintGraph();
        };
    ssgSwLoopCount[opIndex].onValueChange = [this, opIndex, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoop[opIndex].getToggleState();
        applyOpSsgSwEnvLoopValues(opIndex, ssgEnvLoopEnable);
        repaintGraph();
        };

    ssgSwR1[opIndex].onValueChange = repaintGraph;
    ssgSwR2[opIndex].onValueChange = repaintGraph;
    ssgSwR3[opIndex].onValueChange = repaintGraph;
    ssgSwR4[opIndex].onValueChange = repaintGraph;
    ssgSwR5[opIndex].onValueChange = repaintGraph;
    ssgSwR6[opIndex].onValueChange = repaintGraph;

    ssgSwStartLevel[opIndex].onValueChange = repaintGraph;
    ssgSwL1[opIndex].onValueChange = repaintGraph;
    ssgSwL2[opIndex].onValueChange = repaintGraph;
    ssgSwL3[opIndex].onValueChange = repaintGraph;
    ssgSwL4[opIndex].onValueChange = repaintGraph;
    ssgSwL5[opIndex].onValueChange = repaintGraph;
    ssgSwL6[opIndex].onValueChange = repaintGraph;
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
    // 一番下にグラフエリアとボタンエリアを確保
    auto graphAndBtnArea = rect.removeFromBottom(150);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = graphAndBtnArea.removeFromBottom(20);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp[opIndex].setBounds(btnArea.removeFromLeft(btnWidth).reduced(2, 0));
    graphBtnPitch[opIndex].setBounds(btnArea.removeFromLeft(btnWidth).reduced(2, 0));
    graphBtnSsg[opIndex].setBounds(btnArea.reduced(2, 0));

    // 残りをグラフエリアにする
    opGraphs[opIndex].setBounds(graphAndBtnArea.reduced(5));
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
        float ar = (float)pitchAttack[opIndex].getValue();
        float dr = (float)pitchDecay[opIndex].getValue();
        float rr = (float)pitchRelease[opIndex].getValue();

        float arMax = (float)pitchAttack[opIndex].getMaximum();
        float drMax = (float)pitchDecay[opIndex].getMaximum();
        float rrMax = (float)pitchRelease[opIndex].getMaximum();

        const float maxCents = 4800.0f; // 仮の最大値
        float stl = (float)pitchStartLevel[opIndex].getValue() / maxCents;
        float atl = (float)pitchAttackLevel[opIndex].getValue() / maxCents;
        float ssl = (float)pitchSustainLevel[opIndex].getValue() / maxCents;
        float rll = (float)pitchReleaseLevel[opIndex].getValue() / maxCents;

        std::vector<GuiEnvelopeGraph::PhaseDef> phases;
        auto color = juce::Colours::orange;
        int targetIdx = (int)CurveParams::Target::PitchEnv;


        // PitchEnv用の幅計算: Rateが0なら即座(0px)、最大ならゆっくり(150px)
        auto pitchRateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
            if (rateValue <= 0.001f) return 2.0f; // 0なら左に詰める(一瞬)
            return (rateValue / maxRate) * maxWidth;
            };

        // Attack
        phases.push_back({
            .widthPx = pitchRateToWidth(ar, arMax),
            .startLevel = stl, .endLevel = atl, .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetPitchEnv::Ar),
            .phaseLineColor = juce::Colours::red
            });

        // Decay
        phases.push_back({
            .widthPx = pitchRateToWidth(dr, drMax),
            .startLevel = atl, .endLevel = ssl, .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetPitchEnv::Dr),
            .phaseLineColor = juce::Colours::blue
            });

        // Sustain
        phases.push_back({ .widthPx = 60.0f, .startLevel = ssl, .endLevel = ssl, .color = color, .phaseLineColor = juce::Colours::green });

        // Release
        phases.push_back({
            .widthPx = pitchRateToWidth(rr, rrMax),
            .startLevel = ssl, .endLevel = rll, .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetPitchEnv::Rr)
            });

        opGraphs[opIndex].setEnvelope(GuiEnvelopeGraph::EnvType::Pitch, "Pitch Env", phases);
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        int steps = (int)ssgSwSteps[opIndex].getValue();
        if (steps < 1) steps = 1;
        bool isLoop = ssgSwEnvLoop[opIndex].getToggleState();
        int loopTo = (int)ssgSwLoopTo[opIndex].getValue();
        int loopCount = (int)ssgSwLoopCount[opIndex].getValue();

        std::vector<GuiEnvelopeGraph::PhaseDef> phases;
        auto color = juce::Colours::lightgreen;
        int targetIdx = (int)CurveParams::Target::SsgSwEnv;

        std::array<juce::Slider*, 7> rSl = { nullptr, &ssgSwR1[opIndex], &ssgSwR2[opIndex], &ssgSwR3[opIndex], &ssgSwR4[opIndex], &ssgSwR5[opIndex], &ssgSwR6[opIndex] };
        std::array<juce::Slider*, 7> lSl = { &ssgSwStartLevel[opIndex], &ssgSwL1[opIndex], &ssgSwL2[opIndex], &ssgSwL3[opIndex], &ssgSwL4[opIndex], &ssgSwL5[opIndex], &ssgSwL6[opIndex] };

        // SSG用の幅計算: Rateが0なら即座(0px)、最大ならゆっくり(150px)
        auto ssgRateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
            if (rateValue <= 0.001f) return 2.0f; // 0なら左に詰める(一瞬)
            return (rateValue / maxRate) * maxWidth;
            };

        float currentTotalWidth = 0.0f;
        float loopToLevel = isLoop ? lSl[loopTo]->getValue() / lSl[loopTo]->getMaximum() : 0.0f;
        float loopEndWidth = 0.0f;

        for (int s = 1; s <= steps; ++s) {
            float rate = rSl[s]->getValue();
            float rateMax = rSl[s]->getMaximum();
            float startL = lSl[s - 1]->getValue() / lSl[s - 1]->getMaximum();
            float endL = lSl[s]->getValue() / lSl[s]->getMaximum();

            int prmIdx = (isLoop && s == steps) ? (int)CurveParams::TargetSsgSwEnv::LoopTo : (s - 1);
            float width = ssgRateToWidth(rate, rateMax);

            if (isLoop && s == steps) {
                phases.push_back({
                    .widthPx = width, .startLevel = startL, .endLevel = loopToLevel, .color = color,
                    .curveFunc = getCurveFunc(posIdx, targetIdx, prmIdx),
                    .phaseLineColor = juce::Colours::lightgreen
                    });

                loopEndWidth = currentTotalWidth;

                if (loopCount > 0) {
                    phases.push_back({
                        .widthPx = 30.0f,
                        .startLevel = loopToLevel,
                        .endLevel = loopToLevel,
                        .isDashed = true,
                        .color = juce::Colours::cyan,
                        .drawPhaseLine = false
                        });
                    phases.push_back({
                        .widthPx = width,
                        .startLevel = startL,
                        .endLevel = endL,
                        .color = juce::Colours::blue,
                        .curveFunc = getCurveFunc(posIdx, targetIdx, prmIdx),
                        .moveToStart = true,
                        .startXOffsetPx = loopEndWidth,
                        .phaseLineColor = juce::Colours::lightgreen
                        });
                }

                loopEndWidth += width;
            }
            else {
                phases.push_back({
                    .widthPx = width, .startLevel = startL, .endLevel = endL, .color = color,
                    .curveFunc = getCurveFunc(posIdx, targetIdx, prmIdx),
                    .phaseLineColor = juce::Colours::lightgreen
                    });
            }
            currentTotalWidth += width;
        }

        // Release
        float rr = rSl[6]->getValue();
        float rrMax = rSl[6]->getMaximum();
        float rStartL = lSl[steps]->getValue() / lSl[steps]->getMaximum();
        float rEndL = lSl[6]->getValue() / lSl[6]->getMaximum();

        phases.push_back({
            .widthPx = ssgRateToWidth(rr, rrMax),
            .startLevel = (isLoop && loopCount == 0) ? loopToLevel : rStartL,
            .endLevel = rEndL,
            .color = juce::Colours::yellow,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetSsgSwEnv::R6),
            .moveToStart = isLoop,
            .startXOffsetPx = loopEndWidth
            });

        opGraphs[opIndex].setEnvelope(GuiEnvelopeGraph::EnvType::SsgSw, "SSG SW Env", phases);
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
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

        if (d2rVal > 0.0f) {
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
        }
        currentTotalWidth += keyOnWidth;
        float noteOffPositionX = currentTotalWidth;

        // 4. Release
        phases.push_back({
            .widthPx = rateToWidth(rrVal, rrMax),
            .startLevel = releaseStartLevel * tlScale,
            .endLevel = 0.0f,
            .color = d2rVal > 0.0f ? juce::Colours::yellow : color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Rr),
            .moveToStart = true,
            .startXOffsetPx = noteOffPositionX
            });

        opGraphs[opIndex].setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
    }
}
