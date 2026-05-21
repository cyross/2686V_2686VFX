#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Ssg/Keys.h"
#include "../../Processor/Ssg/Values.h"

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

static std::vector<SelectItem> ssgEnvItems = {
    {.name = "0: Saw Down",                    .value = 1 },
    {.name = "1: Saw Down & Hold",             .value = 2 },
    {.name = "2: Triangle",                    .value = 3 },
    {.name = "3: Alternative Saw Down & Hold", .value = 4 },
    {.name = "4: Saw Up",                      .value = 5 },
    {.name = "5: Saw Up & Hold",               .value = 6 },
    {.name = "6: Triangle Invert",             .value = 7 },
    {.name = "7: Alternative Saw Up & Hold",   .value = 8 },
};

static std::vector<SelectItem> ssgPrItems = {
    {.name = "0: 1:1 (50%)",     .value = 1 },
    {.name = "1: 3:5 (37.5%)",   .value = 2 },
    {.name = "2: 5:11 (31.25%)", .value = 3 },
    {.name = "3: 1:3 (25%)",     .value = 4 },
    {.name = "4: 1:4 (20%)",     .value = 5 },
    {.name = "5: 3:13 (18.75%)", .value = 6 },
    {.name = "6: 1:7 (12.5%)",   .value = 7 },
    {.name = "7: 1:15 (6.25%)",  .value = 8 },
};

static std::vector<SelectItem> ssgWsItems = {
    {.name = "0: Pulse(Rect)",    .value = 1 },
    {.name = "1: Triangle / Saw", .value = 2 },
};

static std::vector<SelectItem> ssgDmItems = {
    {.name = "0: Preset Ratio",      .value = 1 },
    {.name = "1: Variable (Slider)", .value = 2 },
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

static std::vector<SelectItem> lfoPmShapeItems = {
    {.name = "0: Sine",                .value = 1 },
    {.name = "1: Saw Up",              .value = 2 },
    {.name = "2: Saw Down",            .value = 3 },
    {.name = "3: Square",              .value = 4 },
    {.name = "4: Triangle",            .value = 5 },
    {.name = "5: Sample & Hold",       .value = 6 },
    {.name = "6: Saw Down & One Shot", .value = 7 },
    {.name = "7: Triangle & One Shot", .value = 8 },
};

static std::vector<SelectItem> lfoAmShapeItems = {
    {.name = "0: Sine",                .value = 1 },
    {.name = "1: Saw Up",              .value = 2 },
    {.name = "2: Saw Down",            .value = 3 },
    {.name = "3: Square",              .value = 4 },
    {.name = "4: Triangle",            .value = 5 },
    {.name = "5: Sample & Hold",       .value = 6 },
    {.name = "6: Saw Down & One Shot", .value = 7 },
    {.name = "7: Triangle & One Shot", .value = 8 },
};

void GuiSsg::setup()
{
    const juce::String code = SsgPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, SsgGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    qualityCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::visibleQuality, .invisibleTitle = SsgGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = *this, .id = code + SsgPrKey::bit, .title = SsgGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + SsgPrKey::rate, .title = SsgGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    adsrCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::visibleAdsr, .invisibleTitle = SsgGuiText::Category::invisibleAdsr, .enableChangeDetailVisible = true });

    adsrBypassButton.setup({ .parent = *this, .id = code + SsgPrKey::adsr + SsgPrKey::bypass, .title = SsgGuiText::Adsr::bypass, .isReset = true });
    adsrBypassButton.setWantsKeyboardFocus(true);
    adsrBypassButton.setExplicitFocusOrder(++tabOrder);

    startLevelSlider.setup({ .parent = *this, .id = code + SsgPrKey::Adsr::stl, .title = SsgGuiText::Adsr::stl, .isReset = true });
    startLevelSlider.setWantsKeyboardFocus(true);
    startLevelSlider.setExplicitFocusOrder(++tabOrder);

    attackSlider.setup({ .parent = *this, .id = code + SsgPrKey::Adsr::ar, .title = SsgGuiText::Adsr::ar, .isReset = true });
    attackSlider.setWantsKeyboardFocus(true);
    attackSlider.setExplicitFocusOrder(++tabOrder);

    decaySlider.setup({ .parent = *this, .id = code + SsgPrKey::Adsr::dr , .title = SsgGuiText::Adsr::dr, .isReset = true });
    decaySlider.setWantsKeyboardFocus(true);
    decaySlider.setExplicitFocusOrder(++tabOrder);

    sustainSlider.setup({ .parent = *this, .id = code + SsgPrKey::Adsr::sl, .title = SsgGuiText::Adsr::sl, .isReset = true });
    sustainSlider.setWantsKeyboardFocus(true);
    sustainSlider.setExplicitFocusOrder(++tabOrder);

    releaseSlider.setup({ .parent = *this, .id = code + SsgPrKey::Adsr::rr, .title = SsgGuiText::Adsr::rr, .isReset = true });
    releaseSlider.setWantsKeyboardFocus(true);
    releaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchAdsrCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::visiblePitchAdsr, .invisibleTitle = SsgGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

    pitchAdsrBypassButton.setup({ .parent = *this, .id = code + SsgPrKey::pitchAdsr + SsgPrKey::bypass, .title = SsgGuiText::PitchAdsr::bypass, .isReset = true });
    pitchAdsrBypassButton.setWantsKeyboardFocus(true);
    pitchAdsrBypassButton.setExplicitFocusOrder(++tabOrder);

    pitchAttackSlider.setup({ .parent = *this, .id = code + SsgPrKey::PitchAdsr::ar, .title = SsgGuiText::PitchAdsr::ar, .isReset = true });
    pitchAttackSlider.setWantsKeyboardFocus(true);
    pitchAttackSlider.setExplicitFocusOrder(++tabOrder);

    pitchDecaySlider.setup({ .parent = *this, .id = code + SsgPrKey::PitchAdsr::dr , .title = SsgGuiText::PitchAdsr::dr, .isReset = true });
    pitchDecaySlider.setWantsKeyboardFocus(true);
    pitchDecaySlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseSlider.setup({ .parent = *this, .id = code + SsgPrKey::PitchAdsr::rr, .title = SsgGuiText::PitchAdsr::rr, .isReset = true });
    pitchReleaseSlider.setWantsKeyboardFocus(true);
    pitchReleaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchStartLevelSlider.setup({ .parent = *this, .id = code + SsgPrKey::PitchAdsr::stl, .title = SsgGuiText::PitchAdsr::stl, .isReset = true });
    pitchStartLevelSlider.setWantsKeyboardFocus(true);
    pitchStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchAttackLevelSlider.setup({ .parent = *this, .id = code + SsgPrKey::PitchAdsr::atl, .title = SsgGuiText::PitchAdsr::atl, .isReset = true });
    pitchAttackLevelSlider.setWantsKeyboardFocus(true);
    pitchAttackLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchSustainLevelSlider.setup({ .parent = *this, .id = code + SsgPrKey::PitchAdsr::ssl, .title = SsgGuiText::PitchAdsr::ssl, .isReset = true });
    pitchSustainLevelSlider.setWantsKeyboardFocus(true);
    pitchSustainLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseLevelSlider.setup({ .parent = *this, .id = code + SsgPrKey::PitchAdsr::rll, .title = SsgGuiText::PitchAdsr::rll, .isReset = true });
    pitchReleaseLevelSlider.setWantsKeyboardFocus(true);
    pitchReleaseLevelSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwEnvCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::visibleSwEnv, .invisibleTitle = SsgGuiText::Category::invisibleSwEnv, .enableChangeDetailVisible = true });

    ssgSwEnvBypassButton.setup({ .parent = *this, .id = code + SsgPrKey::ssgSwEnv + SsgPrKey::bypass, .title = SsgGuiText::SsgSwEnv::bypass, .isReset = true });
    ssgSwEnvBypassButton.setWantsKeyboardFocus(true);
    ssgSwEnvBypassButton.setExplicitFocusOrder(++tabOrder);

    ssgSwStepsSlider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::steps, .title = SsgGuiText::SsgSwEnv::steps, .isReset = true });
    ssgSwStepsSlider.setWantsKeyboardFocus(true);
    ssgSwStepsSlider.setExplicitFocusOrder(++tabOrder);
    ssgSwStepsSlider.onValueChange = [this] {
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        };

    ssgSwEnvLoopButton.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::loop, .title = SsgGuiText::SsgSwEnv::loop, .isReset = true });
    ssgSwEnvLoopButton.setWantsKeyboardFocus(true);
    ssgSwEnvLoopButton.setExplicitFocusOrder(++tabOrder);
    ssgSwEnvLoopButton.onClick = [this] {
		bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

        ssgSwLoopToSlider.setEnabled(ssgEnvLoopEnable);
        ssgSwLoopToSlider.label.setEnabled(ssgEnvLoopEnable);
        ssgSwLoopCountSlider.setEnabled(ssgEnvLoopEnable);
        ssgSwLoopCountSlider.label.setEnabled(ssgEnvLoopEnable);

        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
    };

    ssgSwLoopToSlider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::loopTo, .title = SsgGuiText::SsgSwEnv::loopTo, .isReset = true });
    ssgSwLoopToSlider.setWantsKeyboardFocus(true);
    ssgSwLoopToSlider.setExplicitFocusOrder(++tabOrder);
    ssgSwLoopToSlider.onValueChange = [this] {
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        };

    ssgSwLoopCountSlider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::loopCount, .title = SsgGuiText::SsgSwEnv::loopCount, .isReset = true });
    ssgSwLoopCountSlider.setWantsKeyboardFocus(true);
    ssgSwLoopCountSlider.setExplicitFocusOrder(++tabOrder);

    bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

    ssgSwLoopToSlider.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopToSlider.label.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopCountSlider.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopCountSlider.label.setEnabled(ssgEnvLoopEnable);

    ssgSwStartLevelSlider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::stl, .title = SsgGuiText::SsgSwEnv::stl, .isReset = true });
    ssgSwStartLevelSlider.setWantsKeyboardFocus(true);
    ssgSwStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwR1Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::r1, .title = SsgGuiText::SsgSwEnv::r1, .isReset = true });
    ssgSwR1Slider.setWantsKeyboardFocus(true);
    ssgSwR1Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL1Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::l1, .title = SsgGuiText::SsgSwEnv::l1, .isReset = true });
    ssgSwL1Slider.setWantsKeyboardFocus(true);
    ssgSwL1Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR2Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::r2, .title = SsgGuiText::SsgSwEnv::r2, .isReset = true });
    ssgSwR2Slider.setWantsKeyboardFocus(true);
    ssgSwR2Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL2Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::l2, .title = SsgGuiText::SsgSwEnv::l2, .isReset = true });
    ssgSwL2Slider.setWantsKeyboardFocus(true);
    ssgSwL2Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR3Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::r3, .title = SsgGuiText::SsgSwEnv::r3, .isReset = true });
    ssgSwR3Slider.setWantsKeyboardFocus(true);
    ssgSwR3Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL3Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::l3, .title = SsgGuiText::SsgSwEnv::l3, .isReset = true });
    ssgSwL3Slider.setWantsKeyboardFocus(true);
    ssgSwL3Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR4Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::r4, .title = SsgGuiText::SsgSwEnv::r4, .isReset = true });
    ssgSwR4Slider.setWantsKeyboardFocus(true);
    ssgSwR4Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL4Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::l4, .title = SsgGuiText::SsgSwEnv::l4, .isReset = true });
    ssgSwL4Slider.setWantsKeyboardFocus(true);
    ssgSwL4Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR5Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::r5, .title = SsgGuiText::SsgSwEnv::r5, .isReset = true });
    ssgSwR5Slider.setWantsKeyboardFocus(true);
    ssgSwR5Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL5Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::l5, .title = SsgGuiText::SsgSwEnv::l5, .isReset = true });
    ssgSwL5Slider.setWantsKeyboardFocus(true);
    ssgSwL5Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR6Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::r6, .title = SsgGuiText::SsgSwEnv::r6, .isReset = true });
    ssgSwR6Slider.setWantsKeyboardFocus(true);
    ssgSwR6Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL6Slider.setup({ .parent = *this, .id = code + SsgPrKey::SsgSwEnv::l6, .title = SsgGuiText::SsgSwEnv::l6, .isReset = true });
    ssgSwL6Slider.setWantsKeyboardFocus(true);
    ssgSwL6Slider.setExplicitFocusOrder(++tabOrder);

    detuneCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::visibleDetune, .invisibleTitle = SsgGuiText::Category::invisibleDetune, .enableChangeDetailVisible = true });

    dt1.setup({ .parent = *this, .id = code + SsgPrKey::dt, .title = SsgGuiText::Ssg::Detune::Dt1, .items = dtItems, .isReset = true });
    dt1.setWantsKeyboardFocus(true);
    dt1.setExplicitFocusOrder(++tabOrder);

    dt2.setup({ .parent = *this, .id = code + SsgPrKey::dt2, .title = SsgGuiText::Ssg::Detune::Dt2, .isReset = true });
    dt2.setWantsKeyboardFocus(true);
    dt2.setExplicitFocusOrder(++tabOrder);

    hwEnvCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::visibleHwEnv, .invisibleTitle = SsgGuiText::Category::invisibleHwEnv, .enableChangeDetailVisible = true });

    envEnableButton.setup({ .parent = *this, .id = code + SsgPrKey::HwEnv::enable, .title = SsgGuiText::Ssg::HwEnv::enable, .isReset = true });
    envEnableButton.setWantsKeyboardFocus(true);
    envEnableButton.setExplicitFocusOrder(++tabOrder);

    shapeSelector.setup({ .parent = *this, .id = code + SsgPrKey::HwEnv::shape, .title = SsgGuiText::Ssg::HwEnv::shape, .items = ssgEnvItems, .isReset = true });
    shapeSelector.setWantsKeyboardFocus(true);
    shapeSelector.setExplicitFocusOrder(++tabOrder);

    periodSlider.setup({ .parent = *this, .id = code + SsgPrKey::HwEnv::period, .title = SsgGuiText::Ssg::HwEnv::speed, .isReset = true, .regType = RegisterType::SsgEnv });
    periodSlider.setWantsKeyboardFocus(true);
    periodSlider.setExplicitFocusOrder(++tabOrder);

    lfoCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::visibleLfo, .invisibleTitle = SsgGuiText::Category::invisibleLfo, .enableChangeDetailVisible = true });

    lfoPmFreqSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::pmFreq, .title = SsgGuiText::Lfo::pmSpeed, .isReset = true });
    lfoPmFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoPmFreqSlider.setWantsKeyboardFocus(true);
    lfoPmFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmFreqSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::amFreq, .title = SsgGuiText::Lfo::amSpeed, .isReset = true });
    lfoAmFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoAmFreqSlider.setWantsKeyboardFocus(true);
    lfoAmFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::amSmoothRatio, .title = SsgGuiText::Lfo::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::syncDelay, .title = SsgGuiText::Lfo::syncDelay, .isReset = true });
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

    lfoPmToggle.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::pm, .title = SsgGuiText::Lfo::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmShapeSelector.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::pmShape, .title = SsgGuiText::Lfo::pmShape, .items = lfoPmShapeItems, .isReset = true });
    lfoPmShapeSelector.setWantsKeyboardFocus(true);
    lfoPmShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::pms, .title = SsgGuiText::Lfo::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::pmd, .title = SsgGuiText::Lfo::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::am, .title = SsgGuiText::Lfo::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmShapeSelector.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::amShape, .title = SsgGuiText::Lfo::amShape, .items = lfoAmShapeItems, .isReset = true });
    lfoAmShapeSelector.setWantsKeyboardFocus(true);
    lfoAmShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmsSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::ams, .title = SsgGuiText::Lfo::ams, .isReset = true });
    lfoAmsSlider.setWantsKeyboardFocus(true);
    lfoAmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::amd, .title = SsgGuiText::Lfo::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setupOtherCategory({ .parent = *this, .title = SsgGuiText::Category::visibleMonoMode, .invisibleTitle = SsgGuiText::Category::invisibleMonoMode, .enableChangeDetailVisible = true });

    monoModeToggle.setup({ .parent = *this, .id = SsgPrKey::monoMode, .title = SsgGuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setupOtherCategory({ .parent = *this, .title = SsgGuiText::Category::visibleMvol, .invisibleTitle = SsgGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

    masterVolSlider.setup({ .parent = *this, .id = SsgPrKey::masterVol, .title = SsgGuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

	voiceGroup.setup(*this, SsgGuiText::Group::voice);

    shapeCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::shape });

    waveSelector.setup({ .parent = *this, .id = code + SsgPrKey::wveform, .title = SsgGuiText::Ssg::Voice::form, .items = ssgWsItems, .isReset = true, .isResized = true });
    waveSelector.setWantsKeyboardFocus(true);
    waveSelector.setExplicitFocusOrder(++tabOrder);

    toneCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::ssgTone });

    levelSlider.setup({ .parent = *this, .id = code + SsgPrKey::tone, .title = SsgGuiText::Ssg::Voice::tone, .isReset = true, .regType = RegisterType::SsgVol });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    noiseCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::ssgNoise });

    noiseSlider.setup({ .parent = *this, .id = code + SsgPrKey::noise, .title = SsgGuiText::Ssg::Voice::noise, .isReset = true, .regType = RegisterType::SsgVol });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = *this, .id = code + SsgPrKey::noiseFreq, .title = SsgGuiText::Ssg::Voice::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    noiseOnNoteButton.setup({ .parent = *this, .id = code + SsgPrKey::noiseOnNote, .title = SsgGuiText::Ssg::Voice::noiseOnNote, .isReset = true });
    noiseOnNoteButton.setWantsKeyboardFocus(true);
    noiseOnNoteButton.setExplicitFocusOrder(++tabOrder);

    mixCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::mix });

    // 初期状態反映
    mixSlider.setup({ .parent = *this, .id = code + SsgPrKey::mix , .title = SsgGuiText::Ssg::Voice::mix, .isReset = true });
    mixSlider.setWantsKeyboardFocus(true);
    mixSlider.setExplicitFocusOrder(++tabOrder);

    mixSetTone.setup({ .parent = *this, .title = SsgGuiText::Ssg::Voice::tone, .isReset = false, .isResized = false });
    mixSetTone.setWantsKeyboardFocus(true);
    mixSetTone.setExplicitFocusOrder(++tabOrder);
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };

    mixSetMix.setup({ .parent = *this, .title = SsgGuiText::Ssg::Voice::mix, .isReset = false, .isResized = false });
    mixSetMix.setWantsKeyboardFocus(true);
    mixSetMix.setExplicitFocusOrder(++tabOrder);
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };

    mixSetNoise.setup({ .parent = *this, .title = SsgGuiText::Ssg::Voice::noise, .isReset = false, .isResized = false });
    mixSetNoise.setWantsKeyboardFocus(true);
    mixSetNoise.setExplicitFocusOrder(++tabOrder);
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

    // Duty Controls Setup
    dutyGroup.setup(*this, SsgGuiText::Group::ssgDuty);

    dutyModeSelector.setup({ .parent = *this, .id = code + SsgPrKey::Duty::mode, .title = SsgGuiText::Ssg::Duty::mode, .items = ssgDmItems, .isReset = true, .isResized = true });
    dutyModeSelector.setWantsKeyboardFocus(true);
    dutyModeSelector.setExplicitFocusOrder(++tabOrder);

    dutyPresetSelector.setup({ .parent = *this, .id = code + SsgPrKey::Duty::preset, .title = SsgGuiText::Ssg::Duty::preset, .items = ssgPrItems, .isReset = true, .isResized = true });
    dutyPresetSelector.setWantsKeyboardFocus(true);
    dutyPresetSelector.setExplicitFocusOrder(++tabOrder);

    dutyVarSlider.setup({ .parent = *this, .id = code + SsgPrKey::Duty::var, .title = SsgGuiText::Ssg::Duty::var, .isReset = true });
    dutyVarSlider.setWantsKeyboardFocus(true);
    dutyVarSlider.setExplicitFocusOrder(++tabOrder);

    pulseInvCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::invert });

    dutyInvertButton.setup({ .parent = *this, .id = code + SsgPrKey::Duty::inv, .title = SsgGuiText::Ssg::Duty::invert, .isReset = true, .isResized = true });
    dutyInvertButton.setWantsKeyboardFocus(true);
    dutyInvertButton.setExplicitFocusOrder(++tabOrder);

    triGroup.setup(*this, SsgGuiText::Group::ssgTri);

    triKeyTrackButton.setup({ .parent = *this, .id = code + SsgPrKey::Tri::keyTrk, .title = SsgGuiText::Ssg::Tri::keyTrack, .isReset = true, .isResized = true });
    triKeyTrackButton.setWantsKeyboardFocus(true);
    triKeyTrackButton.setExplicitFocusOrder(++tabOrder);

    triFreqSlider.setup({ .parent = *this, .id = code + SsgPrKey::Tri::freq, .title = SsgGuiText::Ssg::Tri::manualFreq, .isReset = true });
    triFreqSlider.setWantsKeyboardFocus(true);
    triFreqSlider.setExplicitFocusOrder(++tabOrder);

    triPeakCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::peak });

    triPeakSlider.setup({ .parent = *this, .id = code + SsgPrKey::Tri::peak, .title = SsgGuiText::Ssg::Tri::peak, .isReset = true });
    triPeakSlider.setWantsKeyboardFocus(true);
    triPeakSlider.setExplicitFocusOrder(++tabOrder);

    triSetSawDown.setup({ .parent = *this, .title = SsgGuiText::Ssg::Tri::peakTo00, .isReset = false, .isResized = false });
    triSetSawDown.setWantsKeyboardFocus(true);
    triSetSawDown.setExplicitFocusOrder(++tabOrder);
    triSetSawDown.onClick = [this] { triPeakSlider.setValue(0.0, juce::sendNotification); };

    triSetTri.setup({ .parent = *this, .title = SsgGuiText::Ssg::Tri::peakTo05, .isReset = false, .isResized = false });
    triSetTri.setWantsKeyboardFocus(true);
    triSetTri.setExplicitFocusOrder(++tabOrder);
    triSetTri.onClick = [this] { triPeakSlider.setValue(0.5, juce::sendNotification); };

    triSetSawUp.setup({ .parent = *this, .title = SsgGuiText::Ssg::Tri::peakTo10, .isReset = false, .isResized = false });
    triSetSawUp.setWantsKeyboardFocus(true);
    triSetSawUp.setExplicitFocusOrder(++tabOrder);
    triSetSawUp.onClick = [this] { triPeakSlider.setValue(1.0, juce::sendNotification); };
}

void GuiSsg::layout(juce::Rectangle<int> content)
{
    juce::String code = SsgPrKey::prefix;
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(SsgGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);
    mRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    layoutAdsrCat(mRect);

    layoutPitchEnvCat(mRect);

    layoutDetuneCat(mRect);

    layoutHwEnvCat(mRect);

	layoutSwEnvCat(mRect);

    layoutLfoCat(mRect);

    layoutQualityCat(mRect);

    layoutMonoModeCat(mRect);

    layoutMvolCat(mRect);

    auto paramArea = pageArea.removeFromLeft(SsgGuiValue::ParamGroup::width);

    // --- Voice Group ---
    auto voiceArea = paramArea.removeFromTop(230);

    voiceGroup.setBounds(voiceArea);
    auto vRect = voiceGroup.getBounds().reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);

    vRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

    layoutRowCategory({ .rowRect = vRect, .label = &shapeCat });
    layoutRow({ .rowRect = vRect, .label = &waveSelector.label, .component = &waveSelector, });
    layoutRowCategory({ .rowRect = vRect, .label = &toneCat });
    layoutRow({ .rowRect = vRect, .label = &levelSlider.label, .component = &levelSlider, });
    layoutRowCategory({ .rowRect = vRect, .label = &noiseCat });
    layoutRow({ .rowRect = vRect, .label = &noiseSlider.label, .component = &noiseSlider });
    layoutRow({ .rowRect = vRect, .label = &noiseFreqSlider.label, .component = &noiseFreqSlider });
    layoutRow({ .rowRect = vRect, .component = &noiseOnNoteButton, });
    layoutRowCategory({ .rowRect = vRect, .label = &mixCat });
    layoutRow({ .rowRect = vRect, .label = &mixSlider.label, .component = &mixSlider });
    layoutRowThreeComps({ .rect = vRect, .comp1 = &mixSetTone, .comp2 = &mixSetMix, .comp3 = &mixSetNoise, .paddingBottom = 0 });

    // Wave Group
    float waveParam = *ctx.audioProcessor.apvts.getRawParameterValue(code + SsgPrKey::wveform);
    int waveMode = (waveParam > SsgPrValue::boolThread) ? 1 : 0;
    auto waveArea = paramArea.removeFromTop(140);

    if (waveMode == 0) // Pulse
    {
        dutyGroup.setVisible(true);
        triGroup.setVisible(false);

        triKeyTrackButton.setVisible(false);
        triPeakCat.setVisible(false);
        triPeakSlider.setVisibleWithLabel(false);
        triFreqSlider.setVisibleWithLabel(false);
        triSetSawDown.setVisible(false);
        triSetTri.setVisible(false);
        triSetSawUp.setVisible(false);

        pulseInvCat.setVisible(true);
        dutyModeSelector.setVisibleWithLabel(true);
        dutyVarSlider.setValue(true);
        dutyVarSlider.label.setVisible(true);
        dutyInvertButton.setVisible(true);

        dutyGroup.setBounds(waveArea);
        auto dRect = dutyGroup.getBounds().reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);

        dRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

        layoutRow({ .rowRect = dRect, .label = &dutyModeSelector.label, .component = &dutyModeSelector });

        float dutyModeVal = *ctx.audioProcessor.apvts.getRawParameterValue(code + SsgPrKey::Duty::mode);
        if (dutyModeVal < 0.5f) {
            dutyPresetSelector.setVisibleWithLabel(true);
            dutyVarSlider.setVisibleWithLabel(false);
            layoutRow({ .rowRect = dRect, .label = &dutyPresetSelector.label, .component = &dutyPresetSelector, });
        }
        else {
            dutyPresetSelector.setVisibleWithLabel(false);
            dutyVarSlider.setVisibleWithLabel(true);
            layoutRow({ .rowRect = dRect, .label = &dutyVarSlider.label, .component = &dutyVarSlider, });
        }

        layoutRowCategory({ .rowRect = dRect, .label = &pulseInvCat });
        layoutRow({ .rowRect = dRect, .component = &dutyInvertButton, .paddingBottom = 0 });
    }
    else // Triangle
    {
        dutyGroup.setVisible(false);
        triGroup.setVisible(true);

        pulseInvCat.setVisible(false);
        dutyModeSelector.setVisibleWithLabel(false);
        dutyInvertButton.setVisible(false);
        dutyPresetSelector.setVisibleWithLabel(false);
        dutyVarSlider.setVisible(false);
        dutyVarSlider.label.setVisible(false);

        triKeyTrackButton.setVisible(true);
        triFreqSlider.setVisibleWithLabel(true);
        triPeakCat.setVisible(true);
        triPeakSlider.setVisibleWithLabel(true);
        triSetSawDown.setVisible(true);
        triSetTri.setVisible(true);
        triSetSawUp.setVisible(true);

        triGroup.setBounds(waveArea);
        auto tRect = triGroup.getBounds().reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);

        tRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

        layoutRow({ .rowRect = tRect, .component = &triKeyTrackButton });

        bool isKeyTrack = triKeyTrackButton.getToggleState();
        triFreqSlider.setVisibleWithLabel(!isKeyTrack);

        if (!isKeyTrack) {
            layoutRow({ .rowRect = tRect, .label = &triFreqSlider.label, .component = &triFreqSlider, });
        }
        else
        {
            tRect.removeFromTop(SsgGuiValue::Category::paddingTop);
        }

        layoutRowCategory({ .rowRect = tRect, .label = &triPeakCat });
        layoutRow({ .rowRect = tRect, .label = &triPeakSlider.label, .component = &triPeakSlider });
        layoutRowThreeComps({ .rect = tRect, .comp1 = &triSetSawDown, .comp2 = &triSetTri, .comp3 = &triSetSawUp, .paddingBottom = 0 });
    }
}

void GuiSsg::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

void GuiSsg::initParams()
{
    this->ctx.audioProcessor.initParams("SSG_");
}

void GuiSsg::layoutQualityCat(juce::Rectangle<int>& rect) {
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

void GuiSsg::layoutMonoModeCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &monoPolyCat });

    bool visible = monoPolyCat.isDetailVisible();

    monoModeToggle.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoModeToggle });
    }
}

void GuiSsg::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
    }
}

void GuiSsg::layoutAdsrCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &adsrCat });

    bool visible = adsrCat.isDetailVisible();

    adsrBypassButton.setVisible(visible);
    startLevelSlider.setVisibleWithLabel(visible);
    attackSlider.setVisibleWithLabel(visible);
    decaySlider.setVisibleWithLabel(visible);
    sustainSlider.setVisibleWithLabel(visible);
    releaseSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &adsrBypassButton });
        layoutMain({ .mainRect = rect, .label = &startLevelSlider.label, .component = &startLevelSlider });
        layoutMain({ .mainRect = rect, .label = &attackSlider.label, .component = &attackSlider });
        layoutMain({ .mainRect = rect, .label = &decaySlider.label, .component = &decaySlider });
        layoutMain({ .mainRect = rect, .label = &sustainSlider.label, .component = &sustainSlider });
        layoutMain({ .mainRect = rect, .label = &releaseSlider.label, .component = &releaseSlider });
    }
}

void GuiSsg::layoutPitchEnvCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &pitchAdsrCat });

    bool visible = pitchAdsrCat.isDetailVisible();

    pitchAdsrBypassButton.setVisible(visible);
    pitchAttackSlider.setVisibleWithLabel(visible);
    pitchDecaySlider.setVisibleWithLabel(visible);
    pitchReleaseSlider.setVisibleWithLabel(visible);
    pitchStartLevelSlider.setVisibleWithLabel(visible);
    pitchAttackLevelSlider.setVisibleWithLabel(visible);
    pitchSustainLevelSlider.setVisibleWithLabel(visible);
    pitchReleaseLevelSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &pitchAdsrBypassButton });
        layoutMain({ .mainRect = rect, .label = &pitchAttackSlider.label, .component = &pitchAttackSlider });
        layoutMain({ .mainRect = rect, .label = &pitchDecaySlider.label, .component = &pitchDecaySlider });
        layoutMain({ .mainRect = rect, .label = &pitchReleaseSlider.label, .component = &pitchReleaseSlider });
        layoutMain({ .mainRect = rect, .label = &pitchStartLevelSlider.label, .component = &pitchStartLevelSlider });
        layoutMain({ .mainRect = rect, .label = &pitchAttackLevelSlider.label, .component = &pitchAttackLevelSlider });
        layoutMain({ .mainRect = rect, .label = &pitchSustainLevelSlider.label, .component = &pitchSustainLevelSlider });
        layoutMain({ .mainRect = rect, .label = &pitchReleaseLevelSlider.label, .component = &pitchReleaseLevelSlider });
    }
}

void GuiSsg::layoutDetuneCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &detuneCat });

    bool visible = detuneCat.isDetailVisible();

    dt1.setVisibleWithLabel(visible);
    dt2.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &dt1.label, .component = &dt1 });
        layoutMain({ .mainRect = rect, .label = &dt2.label, .component = &dt2 });
    }
}

void GuiSsg::layoutLfoCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &lfoCat });

    bool visible = lfoCat.isDetailVisible();

    lfoPmFreqSlider.setVisibleWithLabel(visible);
    lfoAmFreqSlider.setVisibleWithLabel(visible);
    lfoAmSmRtSlider.setVisibleWithLabel(visible);
    lfoSyncDelaySlider.setVisibleWithLabel(visible);
    lfoSyncDelayToZeroBtn.setVisible(visible);
    lfoSyncDelayToOneBtn.setVisible(visible);
    lfoPmToggle.setVisible(visible);
    lfoPmShapeSelector.setVisibleWithLabel(visible);
    lfoPmsSlider.setVisibleWithLabel(visible);
    lfoPmdSlider.setVisibleWithLabel(visible);
    lfoAmToggle.setVisible(visible);
    lfoAmShapeSelector.setVisibleWithLabel(visible);
    lfoAmsSlider.setVisibleWithLabel(visible);
    lfoAmdSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &lfoPmFreqSlider.label, .component = &lfoPmFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmFreqSlider.label, .component = &lfoAmFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
        layoutMain({ .mainRect = rect, .label = &lfoSyncDelaySlider.label, .component = &lfoSyncDelaySlider });
        layoutMainTwoComps({ .rect = rect, .comp1 = &lfoSyncDelayToZeroBtn, .comp2 = &lfoSyncDelayToOneBtn });
        layoutMain({ .mainRect = rect, .component = &lfoPmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoPmShapeSelector.label, .component = &lfoPmShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoPmsSlider.label, .component = &lfoPmsSlider });
        layoutMain({ .mainRect = rect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
        layoutMain({ .mainRect = rect, .component = &lfoAmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoAmShapeSelector.label, .component = &lfoAmShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoAmsSlider.label, .component = &lfoAmsSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });
    }
}

void GuiSsg::layoutHwEnvCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &hwEnvCat });

    bool visible = hwEnvCat.isDetailVisible();

    envEnableButton.setVisible(visible);
    shapeSelector.setVisibleWithLabel(visible);
    periodSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &envEnableButton });
        layoutMain({ .mainRect = rect, .label = &shapeSelector.label, .component = &shapeSelector });
        layoutMain({ .mainRect = rect, .label = &periodSlider.label, .component = &periodSlider, .paddingBottom = 0 });
    }
}

void GuiSsg::layoutSwEnvCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &ssgSwEnvCat });

    bool visible = ssgSwEnvCat.isDetailVisible();

	ssgSwEnvBypassButton.setVisible(visible);
	ssgSwStepsSlider.setVisibleWithLabel(visible);
	ssgSwEnvLoopButton.setVisible(visible);
	ssgSwLoopToSlider.setVisibleWithLabel(visible);
	ssgSwLoopCountSlider.setVisibleWithLabel(visible);
	ssgSwStartLevelSlider.setVisibleWithLabel(visible);
	ssgSwR1Slider.setVisibleWithLabel(visible);
	ssgSwL1Slider.setVisibleWithLabel(visible);
	ssgSwR2Slider.setVisibleWithLabel(visible);
	ssgSwL2Slider.setVisibleWithLabel(visible);
	ssgSwR3Slider.setVisibleWithLabel(visible);
	ssgSwL3Slider.setVisibleWithLabel(visible);
    ssgSwR4Slider.setVisibleWithLabel(visible);
	ssgSwL4Slider.setVisibleWithLabel(visible);
	ssgSwR5Slider.setVisibleWithLabel(visible);
    ssgSwL5Slider.setVisibleWithLabel(visible);
    ssgSwR6Slider.setVisibleWithLabel(visible);
    ssgSwL6Slider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &ssgSwEnvBypassButton });
        layoutMain({ .mainRect = rect, .label = &ssgSwStepsSlider.label, .component = &ssgSwStepsSlider });
        layoutMain({ .mainRect = rect, .component = &ssgSwEnvLoopButton });
        layoutMain({ .mainRect = rect, .label = &ssgSwLoopToSlider.label, .component = &ssgSwLoopToSlider });
        layoutMain({ .mainRect = rect, .label = &ssgSwLoopCountSlider.label, .component = &ssgSwLoopCountSlider });
        layoutMain({ .mainRect = rect, .label = &ssgSwStartLevelSlider.label, .component = &ssgSwStartLevelSlider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR1Slider.label, .component = &ssgSwR1Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL1Slider.label, .component = &ssgSwL1Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR2Slider.label, .component = &ssgSwR2Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL2Slider.label, .component = &ssgSwL2Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR3Slider.label, .component = &ssgSwR3Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL3Slider.label, .component = &ssgSwL3Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR4Slider.label, .component = &ssgSwR4Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL4Slider.label, .component = &ssgSwL4Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR5Slider.label, .component = &ssgSwR5Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL5Slider.label, .component = &ssgSwL5Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwR6Slider.label, .component = &ssgSwR6Slider });
        layoutMain({ .mainRect = rect, .label = &ssgSwL6Slider.label, .component = &ssgSwL6Slider });
    }
}

void GuiSsg::applySsgSwEnvLoopValues(bool enabled)
{
    if (enabled)
    {
        int steps = static_cast<int>(ssgSwStepsSlider.getValue());

        // Steps が 1 のときはループできないため、Steps を 2 にする
        if (steps < 2) {
            steps = 2;
            ssgSwStepsSlider.setValue(steps);
        }

        int loopTo = static_cast<int>(ssgSwLoopToSlider.getValue());

        // Steps - LoopTo が 2未満のときは、LoopTo を Steps - 2 にする
        if (steps - loopTo < 2) {
            ssgSwLoopToSlider.setValue(steps - 2);
        }
    }
}
