#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Opn/Keys.h"
#include "../../Processor/Opn/Values.h"
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
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    qualityCat.setupHwCategory({ .parent = *this, .title = OpnGuiText::Category::visibleQuality, .invisibleTitle = OpnGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = *this, .id = code + OpnPrKey::bit, .title = OpnGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + OpnPrKey::rate, .title = OpnGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    algFbCat.setupHwCategory({ .parent = *this, .title = OpnGuiText::Category::algFb });

    algSelector.setup({ .parent = *this, .id = code + OpnPrKey::alg, .title = OpnGuiText::Fm::alg, .items = opnAlgItems, .isReset = true });
    algSelector.setWantsKeyboardFocus(true);
    algSelector.setExplicitFocusOrder(++tabOrder);
    algSelector.onChange = [this] {
        updateAlgorithmDisplay();
        };

    feedbackSlider.setup({ .parent = *this, .id = code + OpnPrKey::fb, .title = OpnGuiText::Fm::fb, .isReset = true });
    feedbackSlider.setWantsKeyboardFocus(true);
    feedbackSlider.setExplicitFocusOrder(++tabOrder);

    lfoCat.setupSwCategory({ .parent = *this, .title = OpnGuiText::Category::visibleN88Lfo, .invisibleTitle = OpnGuiText::Category::invisibleN88Lfo, .enableChangeDetailVisible = true });

    lfoFreqSlider.setup({ .parent = *this, .id = code + OpnPrKey::N88Lfo::freq, .title = OpnGuiText::Fm::lfoSpeed, .isReset = true });
    lfoFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoFreqSlider.setWantsKeyboardFocus(true);
    lfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoShapeSelector.setup({ .parent = *this, .id = code + OpnPrKey::N88Lfo::shape, .title = OpnGuiText::Fm::lfoShape, .items = lfoShapeItems, .isReset = true });
    lfoShapeSelector.setWantsKeyboardFocus(true);
    lfoShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + OpnPrKey::N88Lfo::amSmoothRatio, .title = OpnGuiText::Fm::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = *this, .id = code + OpnPrKey::N88Lfo::syncDelay, .title = OpnGuiText::Fm::lfoSyncDelay, .isReset = true });
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

    lfoPmToggle.setup({ .parent = *this, .id = code + OpnPrKey::N88Lfo::pm, .title = OpnGuiText::Fm::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = *this, .id = code + OpnPrKey::N88Lfo::pmd, .title = OpnGuiText::Fm::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = *this, .id = code + OpnPrKey::N88Lfo::pms, .title = OpnGuiText::Fm::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = *this, .id = code + OpnPrKey::N88Lfo::am, .title = OpnGuiText::Fm::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = *this, .id = code + OpnPrKey::N88Lfo::amd, .title = OpnGuiText::Fm::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    unisonCat.setupOtherCategory({ .parent = *this, .title = OpnGuiText::Category::invisibleUnison, .invisibleTitle = OpnGuiText::Category::invisibleUnison, .enableChangeDetailVisible = true });

    unisonVoicesSlider.setup({ .parent = *this, .id = code + OpnPrKey::Unison::voices, .title = OpnGuiText::Unison::voices, .isReset = true });
    unisonVoicesSlider.setWantsKeyboardFocus(true);
    unisonVoicesSlider.setExplicitFocusOrder(++tabOrder);

    unisonDetuneSlider.setup({ .parent = *this, .id = code + OpnPrKey::Unison::detune, .title = OpnGuiText::Unison::detune, .isReset = true });
    unisonDetuneSlider.setWantsKeyboardFocus(true);
    unisonDetuneSlider.setExplicitFocusOrder(++tabOrder);

    unisonSpreadSlider.setup({ .parent = *this, .id = code + OpnPrKey::Unison::spread, .title = OpnGuiText::Unison::spread, .isReset = true });
    unisonSpreadSlider.setWantsKeyboardFocus(true);
    unisonSpreadSlider.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setupOtherCategory({ .parent = *this, .title = OpnGuiText::Category::visibleMonoMode, .invisibleTitle = OpnGuiText::Category::invisibleMonoMode, .enableChangeDetailVisible = true });

    monoModeToggle.setup({ .parent = *this, .id = OpnPrKey::monoMode, .title = OpnGuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setupOtherCategory({ .parent = *this, .title = OpnGuiText::Category::visibleMvol, .invisibleTitle = OpnGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

    masterVolSlider.setup({ .parent = *this, .id = OpnPrKey::masterVol, .title = OpnGuiText::MasterVol::title, .isReset = true });
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
    const juce::String opCode = code + OpnPrKey::op;

    for (int i = 0; i < 4; ++i)
    {
        opGroups[i].setup(*this, OpnGuiText::Group::opPrefix + juce::String(i + 1));

        juce::String paramPrefix = opCode + juce::String(i);

        mul[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::mul, .title = OpnGuiText::Fm::Op::Mul, .items = multems, .isReset = true, .regType = RegisterType::FmMul });
        mul[i].setWantsKeyboardFocus(true);
        mul[i].setExplicitFocusOrder(++tabOrder);

        dt[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::dt, .title = OpnGuiText::Fm::Op::Dt, .items = dtItems, .isReset = true, .regType = RegisterType::FmDt });
        dt[i].setWantsKeyboardFocus(true);
        dt[i].setExplicitFocusOrder(++tabOrder);

        rgAr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::rgAr, .title = OpnGuiText::Fm::Op::Ar, .isReset = true });
        rgAr[i].setWantsKeyboardFocus(true);
        rgAr[i].setExplicitFocusOrder(++tabOrder);

        rgDr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::rgDr, .title = OpnGuiText::Fm::Op::Dr, .isReset = true });
        rgDr[i].setWantsKeyboardFocus(true);
        rgDr[i].setExplicitFocusOrder(++tabOrder);

        rgSl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::rgSl, .title = OpnGuiText::Fm::Op::Sl, .isReset = true });
        rgSl[i].setWantsKeyboardFocus(true);
        rgSl[i].setExplicitFocusOrder(++tabOrder);

        rgSr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::rgSr, .title = OpnGuiText::Fm::Op::Sr, .isReset = true });
        rgSr[i].setWantsKeyboardFocus(true);
        rgSr[i].setExplicitFocusOrder(++tabOrder);

        rgRr[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::rgRr, .title = OpnGuiText::Fm::Op::Rr, .isReset = true });
        rgRr[i].setWantsKeyboardFocus(true);
        rgRr[i].setExplicitFocusOrder(++tabOrder);

        rgTl[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::rgTl, .title = OpnGuiText::Fm::Op::Tl, .isReset = true });
        rgTl[i].setWantsKeyboardFocus(true);
        rgTl[i].setExplicitFocusOrder(++tabOrder);

        ks[i].setup(GuiComboBox::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::ks, .title = OpnGuiText::Fm::Op::Ks, .items = ksItems, .isReset = true });
        ks[i].setWantsKeyboardFocus(true);
        ks[i].setExplicitFocusOrder(++tabOrder);

        catOptional[i].setupSwCategory({ .parent = *this, .title = OpnGuiText::Category::visibleOptional, .invisibleTitle = OpnGuiText::Category::invisibleOptional, .enableChangeDetailVisible = true });

        xof[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::xof, .title = OpnGuiText::Fm::Op::xof, .isReset = true });
        xof[i].setWantsKeyboardFocus(true);
        xof[i].setExplicitFocusOrder(++tabOrder);

        bypass[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::ampBypass, .title = OpnGuiText::Fm::Op::bypass, .isReset = true });
        bypass[i].setWantsKeyboardFocus(true);
        bypass[i].setExplicitFocusOrder(++tabOrder);

        catPitchEnv[i].setupSwCategory({ .parent = *this, .title = OpnGuiText::Category::visiblePitchAdsr, .invisibleTitle = OpnGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

        pitchEnvEnable[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::PitchAdsr::enable, .title = OpnGuiText::PitchAdsr::enable, .isReset = true });
        pitchEnvEnable[i].setWantsKeyboardFocus(true);
        pitchEnvEnable[i].setExplicitFocusOrder(++tabOrder);

        pitchAttack[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::PitchAdsr::ar, .title = OpnGuiText::PitchAdsr::ar, .isReset = true });
        pitchAttack[i].setWantsKeyboardFocus(true);
        pitchAttack[i].setExplicitFocusOrder(++tabOrder);

        pitchDecay[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::PitchAdsr::dr , .title = OpnGuiText::PitchAdsr::dr, .isReset = true });
        pitchDecay[i].setWantsKeyboardFocus(true);
        pitchDecay[i].setExplicitFocusOrder(++tabOrder);

        pitchRelease[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::PitchAdsr::rr, .title = OpnGuiText::PitchAdsr::rr, .isReset = true });
        pitchRelease[i].setWantsKeyboardFocus(true);
        pitchRelease[i].setExplicitFocusOrder(++tabOrder);

        pitchStartLevel[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::PitchAdsr::stl, .title = OpnGuiText::PitchAdsr::stl, .isReset = true });
        pitchStartLevel[i].setWantsKeyboardFocus(true);
        pitchStartLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchAttackLevel[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::PitchAdsr::atl, .title = OpnGuiText::PitchAdsr::atl, .isReset = true });
        pitchAttackLevel[i].setWantsKeyboardFocus(true);
        pitchAttackLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchSustainLevel[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::PitchAdsr::ssl, .title = OpnGuiText::PitchAdsr::ssl, .isReset = true });
        pitchSustainLevel[i].setWantsKeyboardFocus(true);
        pitchSustainLevel[i].setExplicitFocusOrder(++tabOrder);

        pitchReleaseLevel[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::PitchAdsr::rll, .title = OpnGuiText::PitchAdsr::rll, .isReset = true });
        pitchReleaseLevel[i].setWantsKeyboardFocus(true);
        pitchReleaseLevel[i].setExplicitFocusOrder(++tabOrder);

        catSsgSwEnv[i].setupSwCategory({ .parent = *this, .title = OpnGuiText::Category::visibleSwEnv, .invisibleTitle = OpnGuiText::Category::invisibleSwEnv, .enableChangeDetailVisible = true });

        ssgSwEnvEnable[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::enable, .title = OpnGuiText::SsgSwEnv::enable, .isReset = true });
        ssgSwEnvEnable[i].setWantsKeyboardFocus(true);
        ssgSwEnvEnable[i].setExplicitFocusOrder(++tabOrder);

        ssgSwSteps[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::steps, .title = OpnGuiText::SsgSwEnv::steps, .isReset = true });
        ssgSwSteps[i].setWantsKeyboardFocus(true);
        ssgSwSteps[i].setExplicitFocusOrder(++tabOrder);
        ssgSwSteps[i].onValueChange = [this, i] {
            bool ssgEnvLoopEnable = ssgSwEnvLoop[i].getToggleState();

            applyOpSsgSwEnvLoopValues(i, ssgEnvLoopEnable);
            };

        ssgSwEnvLoop[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::loop, .title = OpnGuiText::SsgSwEnv::loop, .isReset = true });
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

        ssgSwLoopTo[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::loopTo, .title = OpnGuiText::SsgSwEnv::loopTo, .isReset = true });
        ssgSwLoopTo[i].setWantsKeyboardFocus(true);
        ssgSwLoopTo[i].setExplicitFocusOrder(++tabOrder);
        ssgSwLoopTo[i].onValueChange = [this, i] {
            bool ssgEnvLoopEnable = ssgSwEnvLoop[i].getToggleState();

            applyOpSsgSwEnvLoopValues(i, ssgEnvLoopEnable);
            };

        ssgSwLoopCount[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::loopCount, .title = OpnGuiText::SsgSwEnv::loopCount, .isReset = true });
        ssgSwLoopCount[i].setWantsKeyboardFocus(true);
        ssgSwLoopCount[i].setExplicitFocusOrder(++tabOrder);

        bool ssgEnvLoopEnable = ssgSwEnvLoop[i].getToggleState();

        ssgSwLoopTo[i].setEnabled(ssgEnvLoopEnable);
        ssgSwLoopTo[i].label.setEnabled(ssgEnvLoopEnable);
        ssgSwLoopCount[i].setEnabled(ssgEnvLoopEnable);
        ssgSwLoopCount[i].label.setEnabled(ssgEnvLoopEnable);

        ssgSwStartLevel[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::stl, .title = OpnGuiText::SsgSwEnv::stl, .isReset = true });
        ssgSwStartLevel[i].setWantsKeyboardFocus(true);
        ssgSwStartLevel[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR1[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::r1, .title = OpnGuiText::SsgSwEnv::r1, .isReset = true });
        ssgSwR1[i].setWantsKeyboardFocus(true);
        ssgSwR1[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL1[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::l1, .title = OpnGuiText::SsgSwEnv::l1, .isReset = true });
        ssgSwL1[i].setWantsKeyboardFocus(true);
        ssgSwL1[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR2[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::r2, .title = OpnGuiText::SsgSwEnv::r2, .isReset = true });
        ssgSwR2[i].setWantsKeyboardFocus(true);
        ssgSwR2[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL2[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::l2, .title = OpnGuiText::SsgSwEnv::l2, .isReset = true });
        ssgSwL2[i].setWantsKeyboardFocus(true);
        ssgSwL2[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR3[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::r3, .title = OpnGuiText::SsgSwEnv::r3, .isReset = true });
        ssgSwR3[i].setWantsKeyboardFocus(true);
        ssgSwR3[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL3[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::l3, .title = OpnGuiText::SsgSwEnv::l3, .isReset = true });
        ssgSwL3[i].setWantsKeyboardFocus(true);
        ssgSwL3[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR4[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::r4, .title = OpnGuiText::SsgSwEnv::r4, .isReset = true });
        ssgSwR4[i].setWantsKeyboardFocus(true);
        ssgSwR4[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL4[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::l4, .title = OpnGuiText::SsgSwEnv::l4, .isReset = true });
        ssgSwL4[i].setWantsKeyboardFocus(true);
        ssgSwL4[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR5[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::r5, .title = OpnGuiText::SsgSwEnv::r5, .isReset = true });
        ssgSwR5[i].setWantsKeyboardFocus(true);
        ssgSwR5[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL5[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::l5, .title = OpnGuiText::SsgSwEnv::l5, .isReset = true });
        ssgSwL5[i].setWantsKeyboardFocus(true);
        ssgSwL5[i].setExplicitFocusOrder(++tabOrder);

        ssgSwR6[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::r6, .title = OpnGuiText::SsgSwEnv::r6, .isReset = true });
        ssgSwR6[i].setWantsKeyboardFocus(true);
        ssgSwR6[i].setExplicitFocusOrder(++tabOrder);

        ssgSwL6[i].setup({ .parent = *this, .id = paramPrefix + OpnPrKey::SsgSwEnv::l6, .title = OpnGuiText::SsgSwEnv::l6, .isReset = true });
        ssgSwL6[i].setWantsKeyboardFocus(true);
        ssgSwL6[i].setExplicitFocusOrder(++tabOrder);

        catN88Lfo[i].setupSwCategory({ .parent = *this, .title = OpnGuiText::Category::visibleN88Lfo, .invisibleTitle = OpnGuiText::Category::invisibleN88Lfo, .enableChangeDetailVisible = true });

        n88Ams[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::n88Ams, .title = OpnGuiText::Fm::Op::Ams, .isReset = true });
        n88Ams[i].setWantsKeyboardFocus(true);
        n88Ams[i].setExplicitFocusOrder(++tabOrder);

        cafFix[i].setupHwCategory({ .parent = *this, .title = OpnGuiText::Category::visibleFix, .invisibleTitle = OpnGuiText::Category::invisibleFix, .enableChangeDetailVisible = true });

        fix[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::fix, .title = OpnGuiText::Fm::Op::Fix, .isReset = true });
        fix[i].setWantsKeyboardFocus(true);
        fix[i].setExplicitFocusOrder(++tabOrder);

        freq[i].setup(GuiSlider::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::fixFreq, .title = OpnGuiText::Fm::Op::FFreq, .isReset = true });
        freq[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        freq[i].setWantsKeyboardFocus(true);
        freq[i].setExplicitFocusOrder(++tabOrder);

        freqToZero[i].setup(GuiTextButton::Config{ .parent = *this, .title = OpnGuiText::Fm::Op::FreqTo0, .isReset = false, .isResized = false });
        freqToZero[i].setWantsKeyboardFocus(true);
        freqToZero[i].setExplicitFocusOrder(++tabOrder);
        freqToZero[i].onClick = [this, index = i] { freq[index].setValue(0, juce::sendNotification); };

        freqTo440[i].setup(GuiTextButton::Config{ .parent = *this, .title = OpnGuiText::Fm::Op::FreqTo440, .isReset = false, .isResized = false });
        freqTo440[i].setWantsKeyboardFocus(true);
        freqTo440[i].setExplicitFocusOrder(++tabOrder);
        freqTo440[i].onClick = [this, index = i] { freq[index].setValue(440, juce::sendNotification); };

        catMask[i].setupHwCategory({ .parent = *this, .title = OpnGuiText::Category::visibleMask, .invisibleTitle = OpnGuiText::Category::invisibleMask, .enableChangeDetailVisible = true });

        mask[i].setup(GuiToggleButton::Config{ .parent = *this, .id = paramPrefix + OpnPrKey::mask, .title = OpnGuiText::Fm::Op::Mask, .isReset = true });
        mask[i].setWantsKeyboardFocus(true);
        mask[i].setExplicitFocusOrder(++tabOrder);

        addAndMakeVisible(mmlSeparator[i]);
        mmlSeparator[i].setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

        mml[i].setup({ .parent = *this, .title = juce::String("") + "MML入力", .isReset = false, .isResized = false });
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
    auto mRect = mainArea.reduced(OpnGuiValue::Group::Padding::width, OpnGuiValue::Group::Padding::height);

    mRect.removeFromTop(OpnGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mRect.removeFromTop(OpnGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    layoutMainCategory({ .mainRect = mRect, .label = &algFbCat });
    layoutMain({ .mainRect = mRect, .label = &algSelector.label, .component = &algSelector });

    mRect.removeFromTop(OpnGuiValue::Category::paddingTop);

    auto imgArea = mRect.removeFromTop(120);
    algImageComp.setBounds(imgArea);

    mRect.removeFromTop(OpnGuiValue::Category::paddingTop);

    layoutMain({ .mainRect = mRect, .label = &feedbackSlider.label, .component = &feedbackSlider });

    layoutN88LfoCat(mRect);

    layoutUnisonCat(mRect);

    layoutQualityCat(mRect);

    layoutMonoModeCat(mRect);

    layoutMvolCat(mRect);

    // --- B. Operators Section (Bottom) ---
    for (int i = 0; i < 4; ++i)
    {
        auto opArea = pageArea.removeFromLeft(OpnGuiValue::Fm::Op::width);

        // 枠線
        opGroups[i].setBounds(opArea);

        // 枠線の内側
        auto innerRect = opArea.reduced(OpnGuiValue::Fm::Op::Padding::width, OpnGuiValue::Fm::Op::Padding::height);
        innerRect.removeFromTop(OpnGuiValue::Group::TitlePaddingTop);

        // グラフ用の区画を確保
        layoutOpGraph(i, innerRect);
        updateOpGraph(i);

        layoutRow({ .rowRect = innerRect, .label = &mul[i].label, .component = &mul[i] });
        layoutRow({ .rowRect = innerRect, .label = &dt[i].label, .component = &dt[i] });
        updateRgDisplayAsOp(i, true);
        layoutRow({ .rowRect = innerRect, .label = &rgAr[i].label, .component = &rgAr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgDr[i].label, .component = &rgDr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgSr[i].label, .component = &rgSr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgSl[i].label, .component = &rgSl[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgRr[i].label, .component = &rgRr[i] });
        layoutRow({ .rowRect = innerRect, .label = &rgTl[i].label, .component = &rgTl[i] });
        layoutRow({ .rowRect = innerRect, .label = &ks[i].label, .component = &ks[i] });

        layoutOpOptionalCat(i, innerRect);

        layoutOpPitchEnvCat(i, innerRect);

        layoutOpSsgSwEnvCat(i, innerRect);

        layoutOpN88LfoCat(i, innerRect);

        layoutOpFixCat(i, innerRect);

        layoutOpMaskCat(i, innerRect);

        // 区切り線エリアを確保
        auto mmlSeparatorArea = innerRect.removeFromTop(OpnGuiValue::ParamGroup::Separator::height);
        mmlSeparator[i].setBounds(mmlSeparatorArea);

        layoutRow({ .rowRect = innerRect, .component = &mml[i] });
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
    bypass[idx].setEnabled(enable);
    cafFix[idx].setEnabled(enable);
    fix[idx].setEnabled(enable);
    freq[idx].setEnabledWithLabel(enable);
    freqToZero[idx].setEnabled(enable);
    freqTo440[idx].setEnabled(enable);
    catMask[idx].setEnabled(enable);
    mask[idx].setEnabled(enable);
    mmlSeparator[idx].setEnabled(enable);
    mml[idx].setEnabled(enable);
}

void GuiOpn::updateAlgorithmDisplay()
{
    int algIndex = algSelector.getSelectedItemIndex();

    if (algIndex < 0 || algIndex > 7) return;

    for (int i = 0; i < 4; ++i)
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

void GuiOpn::layoutOpFixCat(int opIndex, juce::Rectangle<int>& rect) {
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

void GuiOpn::layoutMonoModeCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &monoPolyCat });

    bool visible = monoPolyCat.isDetailVisible();

    monoModeToggle.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoModeToggle });
    }
}

void GuiOpn::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
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

void GuiOpn::layoutOpPitchEnvCat(int opIndex, juce::Rectangle<int>& rect)
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

void GuiOpn::layoutOpSsgSwEnvCat(int opIndex, juce::Rectangle<int>& rect)
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

void GuiOpn::applyOpSsgSwEnvLoopValues(int opIndex, bool enabled)
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
    pitchEnvEnable[opIndex].onStateChange = repaintGraph;
    ssgSwEnvEnable[opIndex].onStateChange = repaintGraph;
    xof[opIndex].onStateChange = repaintGraph;

    rgAr[opIndex].onValueChange = repaintGraph;
    rgDr[opIndex].onValueChange = repaintGraph;
    rgSr[opIndex].onValueChange = repaintGraph;
    rgSl[opIndex].onValueChange = repaintGraph;
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
        opGraphs[opIndex].updateBypass(!(pitchEnvEnable[opIndex].getToggleState()));

        opGraphs[opIndex].updatePitchEnv(
            pitchAttack[opIndex],
            pitchDecay[opIndex],
            pitchRelease[opIndex],
            pitchStartLevel[opIndex],
            pitchAttackLevel[opIndex],
            pitchSustainLevel[opIndex],
            pitchReleaseLevel[opIndex],
            p_curveCore,
            isCurveMode,
            posIdx
        );
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        opGraphs[opIndex].updateBypass(!(ssgSwEnvEnable[opIndex].getToggleState()));

        opGraphs[opIndex].updateSsgSwEnv(
            ssgSwSteps[opIndex],
            ssgSwEnvLoop[opIndex],
            ssgSwLoopTo[opIndex],
            ssgSwLoopCount[opIndex],
            { nullptr, &ssgSwR1[opIndex], &ssgSwR2[opIndex], &ssgSwR3[opIndex], &ssgSwR4[opIndex], &ssgSwR5[opIndex], &ssgSwR6[opIndex] },
            { &ssgSwStartLevel[opIndex], &ssgSwL1[opIndex], &ssgSwL2[opIndex], &ssgSwL3[opIndex], &ssgSwL4[opIndex], &ssgSwL5[opIndex], &ssgSwL6[opIndex] },
            p_curveCore,
            isCurveMode,
            posIdx
        );
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
    bypass[opIndex].setVisible(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .component = &xof[opIndex] });
        layoutRow({ .rowRect = rect, .component = &bypass[opIndex] });
    }
}

void GuiOpn::layoutUnisonCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &unisonCat });

    bool visibleUnison = unisonCat.isDetailVisible();

    unisonVoicesSlider.setVisibleWithLabel(visibleUnison);
    unisonDetuneSlider.setVisibleWithLabel(visibleUnison);
    unisonSpreadSlider.setVisibleWithLabel(visibleUnison);

    if (visibleUnison)
    {
        layoutMain({ .mainRect = rect, .label = &unisonVoicesSlider.label, .component = &unisonVoicesSlider });
        layoutMain({ .mainRect = rect, .label = &unisonDetuneSlider.label, .component = &unisonDetuneSlider });
        layoutMain({ .mainRect = rect, .label = &unisonSpreadSlider.label, .component = &unisonSpreadSlider });
    }
}
