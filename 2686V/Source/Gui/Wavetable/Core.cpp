#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Wavetable/Keys.h"
#include "../../Processor/Wavetable/Values.h"
#include "../../Core/Const/FileValues.h"

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

static std::vector<SelectItem> wtWsItems = {
    {.name = "0: Sine",          .value = 1 },
    {.name = "1: Triangle",      .value = 2 },
    {.name = "2: Saw Up",        .value = 3 },
    {.name = "3: Saw Down",      .value = 4 },
    {.name = "4: Square (50%)",  .value = 5 },
    {.name = "5: Pulse (25%)",   .value = 6 },
    {.name = "6: Pulse (12.5%)", .value = 7 },
    {.name = "7: Digital Noise", .value = 8 },
    {.name = "8: Custom(Draw)",  .value = 9 },
};

static std::vector<SelectItem> wtTsItems = {
    {.name = "0:  32 Samples",  .value = 1 },
    {.name = "1:  64 Samples",  .value = 2 },
    {.name = "2: 128 Samples",  .value = 3 },
    {.name = "3: 256 Samples",  .value = 4 },
};

static std::vector<SelectItem> wtStepsItems = {
    {.name = " 0: Free",   .value =  1 },
    {.name = " 1: 16(+)",  .value =  2 },
    {.name = " 2: 32(+)",  .value =  3 },
    {.name = " 3: 64(+)",  .value =  4 },
    {.name = " 4: 128(+)", .value =  5 },
    {.name = " 5: 256(+)", .value =  6 },
    {.name = " 6: 16(-)",  .value =  7 },
    {.name = " 7: 32(-)",  .value =  8 },
    {.name = " 8: 64(-)",  .value =  9 },
    {.name = " 9: 128(-)", .value = 10 },
    {.name = "10: 256(-)", .value = 11 },
};

static std::vector<SelectItem> multems = {
    {.name = " 0: x  0.5",    .value = 1 },
    {.name = " 1: x  0.891",  .value = 2 },
    {.name = " 2: x  1",      .value = 3 },
    {.name = " 3: x  1.414",  .value = 4 },
    {.name = " 4: x  1.498",  .value = 5 },
    {.name = " 5: x  1.581",  .value = 6 },
    {.name = " 6: x  1.781",  .value = 7 },
    {.name = " 7: x  2",      .value = 8 },
    {.name = " 8: x  3",      .value = 9 },
    {.name = " 9: x  4",      .value = 10 },
    {.name = "10: x  5",      .value = 11 },
    {.name = "11: x  6",      .value = 12 },
    {.name = "12: x  7",      .value = 13 },
    {.name = "13: x  8",      .value = 14 },
    {.name = "14: x  9",      .value = 15 },
    {.name = "15: x 10",      .value = 16 },
    {.name = "16: x 11",      .value = 17 },
    {.name = "17: x 12",      .value = 18 },
    {.name = "18: x 13",      .value = 19 },
    {.name = "19: x 14",      .value = 20 },
    {.name = "20: x 15",      .value = 21 },
    {.name = "21: Use Ratio", .value = 22 }
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

void GuiWt::setup()
{
    auto applySteps = [this] {
        int stepsIndex = stepsSelector.getSelectedId() - 1;
        int steps = 0;

        switch (stepsIndex)
        {
        case 1:
            steps = 16;
            break;
        case 2:
            steps = 32;
            break;
        case 3:
            steps = 64;
            break;
        case 4:
            steps = 128;
            break;
        case 5:
            steps = 256;
            break;
        case 6:
            steps = -16;
            break;
        case 7:
            steps = -32;
            break;
        case 8:
            steps = -64;
            break;
        case 9:
            steps = -128;
            break;
        case 10:
            steps = -256;
            break;
        }

        customSliders32.steps = steps;
        customSliders64.steps = steps;
        customSliders128.steps = steps;
        customSliders256.steps = steps;
        };
    const juce::String code = WtPrKey::prefix;
    int tabOrder = 1;

    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    mainGroup.setup(*this, WtGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    qualityCat.setupHwCategory({ .parent = *this, .title = WtGuiText::Category::visibleQuality, .invisibleTitle = WtGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = *this, .id = code + WtPrKey::bit, .title = WtGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + WtPrKey::rate, .title = WtGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    levelSlider.setup({ .parent = *this, .id = code + WtPrKey::level, .title = WtGuiText::Wt::level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    // Waveform
    waveSelector.setup({ .parent = *this, .id = code + WtPrKey::wave, .title = WtGuiText::Wt::form, .items = wtWsItems, .isReset = true, .isResized = true });
    waveSelector.setWantsKeyboardFocus(true);
    waveSelector.setExplicitFocusOrder(++tabOrder);
    waveSelector.onChange = [this] {
        updateCustomWaveCatOnChange();

        ctx.editor.resized();
        };

    // Custom Wave Size
    sizeSelector.setup({ .parent = *this, .id = code + WtPrKey::sampleSize, .title = WtGuiText::Wt::size, .items = wtTsItems, .isReset = true, .isResized = true });
    sizeSelector.setWantsKeyboardFocus(true);
    sizeSelector.setExplicitFocusOrder(++tabOrder);

    // Steps
    stepsSelector.setup({ .parent = *this, .id = code + WtPrKey::steps, .title = WtGuiText::Wt::steps, .items = wtStepsItems, .isReset = true, .isResized = true });
    stepsSelector.setWantsKeyboardFocus(true);
    stepsSelector.setExplicitFocusOrder(++tabOrder);
    stepsSelector.onChange = [this] {
        auto applySteps = [this] {
            int stepsIndex = stepsSelector.getSelectedId() - 1;
            int steps = 0;

            switch (stepsIndex)
            {
            case 1:
                steps = 16;
                break;
            case 2:
                steps = 32;
                break;
            case 3:
                steps = 64;
                break;
            case 4:
                steps = 128;
                break;
            case 5:
                steps = 256;
                break;
            case 6:
                steps = -16;
                break;
            case 7:
                steps = -32;
                break;
            case 8:
                steps = -64;
                break;
            case 9:
                steps = -128;
                break;
            case 10:
                steps = -256;
                break;
            }

            customSliders32.steps = steps;
            customSliders64.steps = steps;
            customSliders128.steps = steps;
            customSliders256.steps = steps;
            };

        applySteps();

        ctx.editor.resized();
        };

    modCat.setupHwCategory({ .parent = *this, .title = WtGuiText::Category::visibleMod, .invisibleTitle = WtGuiText::Category::invisibileMod, .enableChangeDetailVisible = true });

    // Modulation
    modEnableButton.setup({ .parent = *this, .id = code + WtPrKey::Mod::enable, .title = WtGuiText::Wt::Mod::enable, .isReset = true, .isResized = true });
    modEnableButton.setWantsKeyboardFocus(true);
    modEnableButton.setExplicitFocusOrder(++tabOrder);

    modDepthSlider.setup({ .parent = *this, .id = code + WtPrKey::Mod::depth, .title = WtGuiText::Wt::Mod::depth, .isReset = true });
    modDepthSlider.setWantsKeyboardFocus(true);
    modDepthSlider.setExplicitFocusOrder(++tabOrder);

    modSpeedSlider.setup({ .parent = *this, .id = code + WtPrKey::Mod::speed, .title = WtGuiText::Wt::Mod::speed, .isReset = true });
    modSpeedSlider.setWantsKeyboardFocus(true);
    modSpeedSlider.setExplicitFocusOrder(++tabOrder);

    adsrCat.setupSwCategory({ .parent = *this, .title = WtGuiText::Category::visibleAdsr, .invisibleTitle = WtGuiText::Category::invisibleAdsr, .enableChangeDetailVisible = true });

    adsrBypassButton.setup({ .parent = *this, .id = code + WtPrKey::adsr + WtPrKey::bypass, .title = WtGuiText::Adsr::bypass, .isReset = true });
    adsrBypassButton.setWantsKeyboardFocus(true);
    adsrBypassButton.setExplicitFocusOrder(++tabOrder);

    startLevelSlider.setup({ .parent = *this, .id = code + WtPrKey::Adsr::stl, .title = WtGuiText::Adsr::stl, .isReset = true });
    startLevelSlider.setWantsKeyboardFocus(true);
    startLevelSlider.setExplicitFocusOrder(++tabOrder);

    attackSlider.setup({ .parent = *this, .id = code + WtPrKey::Adsr::ar, .title = WtGuiText::Adsr::ar, .isReset = true });
    attackSlider.setWantsKeyboardFocus(true);
    attackSlider.setExplicitFocusOrder(++tabOrder);

    decaySlider.setup({ .parent = *this, .id = code + WtPrKey::Adsr::dr, .title = WtGuiText::Adsr::dr, .isReset = true });
    decaySlider.setWantsKeyboardFocus(true);
    decaySlider.setExplicitFocusOrder(++tabOrder);

    sustainSlider.setup({ .parent = *this, .id = code + WtPrKey::Adsr::sl, .title = WtGuiText::Adsr::sl, .isReset = true });
    sustainSlider.setWantsKeyboardFocus(true);
    sustainSlider.setExplicitFocusOrder(++tabOrder);

    releaseSlider.setup({ .parent = *this, .id = code + WtPrKey::Adsr::rr, .title = WtGuiText::Adsr::rr, .isReset = true });
    releaseSlider.setWantsKeyboardFocus(true);
    releaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchAdsrCat.setupSwCategory({ .parent = *this, .title = WtGuiText::Category::visiblePitchAdsr, .invisibleTitle = WtGuiText::Category::invisiblePitchAdsr, .enableChangeDetailVisible = true });

    pitchAdsrBypassButton.setup({ .parent = *this, .id = code + WtPrKey::pitchAdsr + WtPrKey::bypass, .title = WtGuiText::PitchAdsr::bypass, .isReset = true });
    pitchAdsrBypassButton.setWantsKeyboardFocus(true);
    pitchAdsrBypassButton.setExplicitFocusOrder(++tabOrder);

    pitchAttackSlider.setup({ .parent = *this, .id = code + WtPrKey::PitchAdsr::ar, .title = WtGuiText::PitchAdsr::ar, .isReset = true });
    pitchAttackSlider.setWantsKeyboardFocus(true);
    pitchAttackSlider.setExplicitFocusOrder(++tabOrder);

    pitchDecaySlider.setup({ .parent = *this, .id = code + WtPrKey::PitchAdsr::dr , .title = WtGuiText::PitchAdsr::dr, .isReset = true });
    pitchDecaySlider.setWantsKeyboardFocus(true);
    pitchDecaySlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseSlider.setup({ .parent = *this, .id = code + WtPrKey::PitchAdsr::rr, .title = WtGuiText::PitchAdsr::rr, .isReset = true });
    pitchReleaseSlider.setWantsKeyboardFocus(true);
    pitchReleaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchStartLevelSlider.setup({ .parent = *this, .id = code + WtPrKey::PitchAdsr::stl, .title = WtGuiText::PitchAdsr::stl, .isReset = true });
    pitchStartLevelSlider.setWantsKeyboardFocus(true);
    pitchStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchAttackLevelSlider.setup({ .parent = *this, .id = code + WtPrKey::PitchAdsr::atl, .title = WtGuiText::PitchAdsr::atl, .isReset = true });
    pitchAttackLevelSlider.setWantsKeyboardFocus(true);
    pitchAttackLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchSustainLevelSlider.setup({ .parent = *this, .id = code + WtPrKey::PitchAdsr::ssl, .title = WtGuiText::PitchAdsr::ssl, .isReset = true });
    pitchSustainLevelSlider.setWantsKeyboardFocus(true);
    pitchSustainLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseLevelSlider.setup({ .parent = *this, .id = code + WtPrKey::PitchAdsr::rll, .title = WtGuiText::PitchAdsr::rll, .isReset = true });
    pitchReleaseLevelSlider.setWantsKeyboardFocus(true);
    pitchReleaseLevelSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwEnvCat.setupSwCategory({ .parent = *this, .title = WtGuiText::Category::visibleSwEnv, .invisibleTitle = WtGuiText::Category::invisibleSwEnv, .enableChangeDetailVisible = true });

    ssgSwEnvBypassButton.setup({ .parent = *this, .id = code + WtPrKey::ssgSwEnv + WtPrKey::bypass, .title = WtGuiText::SsgSwEnv::bypass, .isReset = true });
    ssgSwEnvBypassButton.setWantsKeyboardFocus(true);
    ssgSwEnvBypassButton.setExplicitFocusOrder(++tabOrder);

    ssgSwStepsSlider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::steps, .title = WtGuiText::SsgSwEnv::steps, .isReset = true });
    ssgSwStepsSlider.setWantsKeyboardFocus(true);
    ssgSwStepsSlider.setExplicitFocusOrder(++tabOrder);
    ssgSwStepsSlider.onValueChange = [this] {
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        };

    ssgSwEnvLoopButton.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::loop, .title = WtGuiText::SsgSwEnv::loop, .isReset = true });
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

    ssgSwLoopToSlider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::loopTo, .title = WtGuiText::SsgSwEnv::loopTo, .isReset = true });
    ssgSwLoopToSlider.setWantsKeyboardFocus(true);
    ssgSwLoopToSlider.setExplicitFocusOrder(++tabOrder);
    ssgSwLoopToSlider.onValueChange = [this] {
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        };

    ssgSwLoopCountSlider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::loopCount, .title = WtGuiText::SsgSwEnv::loopCount, .isReset = true });
    ssgSwLoopCountSlider.setWantsKeyboardFocus(true);
    ssgSwLoopCountSlider.setExplicitFocusOrder(++tabOrder);

    bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();

    ssgSwLoopToSlider.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopToSlider.label.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopCountSlider.setEnabled(ssgEnvLoopEnable);
    ssgSwLoopCountSlider.label.setEnabled(ssgEnvLoopEnable);

    ssgSwStartLevelSlider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::stl, .title = WtGuiText::SsgSwEnv::stl, .isReset = true });
    ssgSwStartLevelSlider.setWantsKeyboardFocus(true);
    ssgSwStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    ssgSwR1Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::r1, .title = WtGuiText::SsgSwEnv::r1, .isReset = true });
    ssgSwR1Slider.setWantsKeyboardFocus(true);
    ssgSwR1Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL1Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::l1, .title = WtGuiText::SsgSwEnv::l1, .isReset = true });
    ssgSwL1Slider.setWantsKeyboardFocus(true);
    ssgSwL1Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR2Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::r2, .title = WtGuiText::SsgSwEnv::r2, .isReset = true });
    ssgSwR2Slider.setWantsKeyboardFocus(true);
    ssgSwR2Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL2Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::l2, .title = WtGuiText::SsgSwEnv::l2, .isReset = true });
    ssgSwL2Slider.setWantsKeyboardFocus(true);
    ssgSwL2Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR3Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::r3, .title = WtGuiText::SsgSwEnv::r3, .isReset = true });
    ssgSwR3Slider.setWantsKeyboardFocus(true);
    ssgSwR3Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL3Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::l3, .title = WtGuiText::SsgSwEnv::l3, .isReset = true });
    ssgSwL3Slider.setWantsKeyboardFocus(true);
    ssgSwL3Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR4Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::r4, .title = WtGuiText::SsgSwEnv::r4, .isReset = true });
    ssgSwR4Slider.setWantsKeyboardFocus(true);
    ssgSwR4Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL4Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::l4, .title = WtGuiText::SsgSwEnv::l4, .isReset = true });
    ssgSwL4Slider.setWantsKeyboardFocus(true);
    ssgSwL4Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR5Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::r5, .title = WtGuiText::SsgSwEnv::r5, .isReset = true });
    ssgSwR5Slider.setWantsKeyboardFocus(true);
    ssgSwR5Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL5Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::l5, .title = WtGuiText::SsgSwEnv::l5, .isReset = true });
    ssgSwL5Slider.setWantsKeyboardFocus(true);
    ssgSwL5Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwR6Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::r6, .title = WtGuiText::SsgSwEnv::r6, .isReset = true });
    ssgSwR6Slider.setWantsKeyboardFocus(true);
    ssgSwR6Slider.setExplicitFocusOrder(++tabOrder);

    ssgSwL6Slider.setup({ .parent = *this, .id = code + WtPrKey::SsgSwEnv::l6, .title = WtGuiText::SsgSwEnv::l6, .isReset = true });
    ssgSwL6Slider.setWantsKeyboardFocus(true);
    ssgSwL6Slider.setExplicitFocusOrder(++tabOrder);

    detuneCat.setupSwCategory({ .parent = *this, .title = WtGuiText::Category::visibleDetune, .invisibleTitle = WtGuiText::Category::invisibleDetune, .enableChangeDetailVisible = true });

    mul.setup({ .parent = *this, .id = code + WtPrKey::mul, .title = WtGuiText::Wt::Detune::Mul, .items = multems, .isReset = true });
    mul.setWantsKeyboardFocus(true);
    mul.setExplicitFocusOrder(++tabOrder);

    mulRatio.setup({ .parent = *this, .id = code + WtPrKey::mulRatio, .title = WtGuiText::Wt::Detune::MulRatio, .isReset = true });
    mulRatio.setWantsKeyboardFocus(true);
    mulRatio.setExplicitFocusOrder(++tabOrder);

    dt1.setup({ .parent = *this, .id = code + WtPrKey::dt, .title = WtGuiText::Wt::Detune::Dt1, .items = dtItems, .isReset = true });
    dt1.setWantsKeyboardFocus(true);
    dt1.setExplicitFocusOrder(++tabOrder);

    dt2.setup({ .parent = *this, .id = code + WtPrKey::dt2, .title = WtGuiText::Wt::Detune::Dt2, .isReset = true });
    dt2.setWantsKeyboardFocus(true);
    dt2.setExplicitFocusOrder(++tabOrder);

    lfoCat.setupSwCategory({ .parent = *this, .title = WtGuiText::Category::visibleLfo, .invisibleTitle = WtGuiText::Category::invisibleLfo, .enableChangeDetailVisible = true });

    lfoPmFreqSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::pmFreq, .title = WtGuiText::Lfo::pmSpeed, .isReset = true });
    lfoPmFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoPmFreqSlider.setWantsKeyboardFocus(true);
    lfoPmFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmFreqSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::amFreq, .title = WtGuiText::Lfo::amSpeed, .isReset = true });
    lfoAmFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoAmFreqSlider.setWantsKeyboardFocus(true);
    lfoAmFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::amSmoothRatio, .title = WtGuiText::Lfo::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::syncDelay, .title = WtGuiText::Lfo::syncDelay, .isReset = true });
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

    lfoPmToggle.setup({ .parent = *this, .id = code + WtPrKey::Lfo::pm, .title = WtGuiText::Lfo::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmShapeSelector.setup({ .parent = *this, .id = code + WtPrKey::Lfo::pmShape, .title = WtGuiText::Lfo::pmShape, .items = lfoPmShapeItems, .isReset = true });
    lfoPmShapeSelector.setWantsKeyboardFocus(true);
    lfoPmShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::pms, .title = WtGuiText::Lfo::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::pmd, .title = WtGuiText::Lfo::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = *this, .id = code + WtPrKey::Lfo::am, .title = WtGuiText::Lfo::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmShapeSelector.setup({ .parent = *this, .id = code + WtPrKey::Lfo::amShape, .title = WtGuiText::Lfo::amShape, .items = lfoAmShapeItems, .isReset = true });
    lfoAmShapeSelector.setWantsKeyboardFocus(true);
    lfoAmShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmsSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::ams, .title = WtGuiText::Lfo::ams, .isReset = true });
    lfoAmsSlider.setWantsKeyboardFocus(true);
    lfoAmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = *this, .id = code + WtPrKey::Lfo::amd, .title = WtGuiText::Lfo::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setupOtherCategory({ .parent = *this, .title = WtGuiText::Category::visibleMonoMode, .invisibleTitle = WtGuiText::Category::invisibleMonoMode, .enableChangeDetailVisible = true });

    monoModeToggle.setup({ .parent = *this, .id = WtPrKey::monoMode, .title = WtGuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setupOtherCategory({ .parent = *this, .title = WtGuiText::Category::visibleMvol, .invisibleTitle = WtGuiText::Category::invisibleMvol, .enableChangeDetailVisible = true });

    // Master Volume
	masterVolSlider.setup({ .parent = *this, .id = WtPrKey::masterVol, .title = WtGuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    // Custom Wave Group
	customWaveGroup.setup(*this, WtGuiText::Group::wtCustom);

    // Custom Wave Sliders
	customSliders32.setup({ .parent = *this, .idPrefix = code + WtPrKey::custom32 });
    customSliders64.setup({ .parent = *this, .idPrefix = code + WtPrKey::custom64 });
    customSliders128.setup({ .parent = *this, .idPrefix = code + WtPrKey::custom128 });
    customSliders256.setup({ .parent = *this, .idPrefix = code + WtPrKey::custom256 });

	customWaveResetTo0Btn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::to0, .bgColor = GuiColor::WaveformContainer::ResetBtn::To0, .isReset = false, .isResized = false });
    customWaveResetTo0Btn.setWantsKeyboardFocus(true);
    customWaveResetTo0Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetTo0Btn.onClick = [this] {
        customSliders32.setAllValues(0.0f);
        customSliders64.setAllValues(0.0f);
        customSliders128.setAllValues(0.0f);
        customSliders256.setAllValues(0.0f);
        ctx.editor.resized();
    };

    customWaveResetTo1Btn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::to1, .bgColor = GuiColor::WaveformContainer::ResetBtn::To1, .isReset = false, .isResized = false });
    customWaveResetTo1Btn.setWantsKeyboardFocus(true);
    customWaveResetTo1Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetTo1Btn.onClick = [this] {
        customSliders32.setAllValues(1.0f);
        customSliders64.setAllValues(1.0f);
        customSliders128.setAllValues(1.0f);
        customSliders256.setAllValues(1.0f);
        ctx.editor.resized();
        };

    customWaveResetToM1Btn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::toM1, .bgColor = GuiColor::WaveformContainer::ResetBtn::ToM1, .isReset = false, .isResized = false });
    customWaveResetToM1Btn.setWantsKeyboardFocus(true);
    customWaveResetToM1Btn.setExplicitFocusOrder(++tabOrder);
    customWaveResetToM1Btn.onClick = [this] {
        customSliders32.setAllValues(-1.0f);
        customSliders64.setAllValues(-1.0f);
        customSliders128.setAllValues(-1.0f);
        customSliders256.setAllValues(-1.0f);
        resized();
        };

    customWaveSmoothBtn.setup({ .parent = *this, .title = WtGuiText::Wt::Custom::smooth, .bgColor = juce::Colours::darkcyan, .isReset = false, .isResized = false });
    customWaveSmoothBtn.setWantsKeyboardFocus(true);
    customWaveSmoothBtn.setExplicitFocusOrder(++tabOrder);
    customWaveSmoothBtn.onClick = [this] {
        // 現在選択されている波形サイズに応じてスムージングを実行
        int sizeId = sizeSelector.getSelectedId();
        if (sizeId == 1) customSliders32.applySmoothing();
        else if (sizeId == 2) customSliders64.applySmoothing();
        else if (sizeId == 3) customSliders128.applySmoothing();
        else if (sizeId == 4) customSliders256.applySmoothing();
        };

    waveFileCat.setup({ .parent = *this, .title = WtGuiText::Category::visibleWaveFile, .invisibleTitle = WtGuiText::Category::invisibleWaveFile, .enableChangeDetailVisible = true });

    customWaveImportBtn.setup({ .parent = *this, .title = WtGuiText::Wt::fileImport, .bgColor = juce::Colours::darkgrey, .isReset = false, .isResized = false });
    customWaveImportBtn.setWantsKeyboardFocus(true);
    customWaveImportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveImportBtn.onClick = [this] { importWavetable(); };

    customWaveExportBtn.setup({ .parent = *this, .title = WtGuiText::Wt::fileExport, .bgColor = juce::Colours::darkgrey, .isReset = false, .isResized = false });
    customWaveExportBtn.setWantsKeyboardFocus(true);
    customWaveExportBtn.setExplicitFocusOrder(++tabOrder);
    customWaveExportBtn.onClick = [this] { exportWavetable(); };

    applySteps();

    setupGraph();
    updateGraph();
}

void GuiWt::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(WtGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(WtGuiValue::Group::Padding::width, WtGuiValue::Group::Padding::height);
    mRect.removeFromTop(WtGuiValue::Group::TitlePaddingTop);

    // グラフ用の区画を確保
    layoutGraph(mRect);
    updateGraph();

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider, });

    layoutMain({ .mainRect = mRect, .label = &waveSelector.label, .component = &waveSelector });

    int index = waveSelector.getSelectedId();
    bool visible = index == 9; // custom

    if (visible)
    {
        layoutMain({ .mainRect = mRect, .label = &sizeSelector.label, .component = &sizeSelector, });
        layoutMain({ .mainRect = mRect, .label = &stepsSelector.label, .component = &stepsSelector, });
    }

    layoutModulationCat(mRect);

    layoutAdsrCat(mRect);

    layoutPitchEnvCat(mRect);

	layoutSsgSwEnvCat(mRect);

    layoutDetuneCat(mRect);

    layoutLfoCat(mRect);

    layoutWavefileCat(mRect);

    layoutQualityCat(mRect);

    layoutMonoModeCat(mRect);

    layoutMvolCat(mRect);

    bool isMod = modEnableButton.getToggleState();
    modDepthSlider.setEnabledWithLabel(isMod);
    modSpeedSlider.setEnabledWithLabel(isMod);

    // 波形がカスタム以外の時は波形精度選択を Disabled に
    bool isCustomWave = (waveSelector.getSelectedId() == 9);

    sizeSelector.setEnabledWithLabel(isCustomWave);
    customSliders32.setEnabled(isCustomWave);
    customSliders64.setEnabled(isCustomWave);
    customSliders128.setEnabled(isCustomWave);
    customSliders256.setEnabled(isCustomWave);
    customWaveResetTo0Btn.setEnabled(isCustomWave);
    customWaveResetTo1Btn.setEnabled(isCustomWave);
    customWaveResetToM1Btn.setEnabled(isCustomWave);
    customWaveSmoothBtn.setEnabled(isCustomWave);

    // Custom Wave
    // Custom Mode Layout
    // 中央に32/64本のスライダーを配置
    // 波形選択が Custom 以外の時は Disabled 表示
    auto rightArea = pageArea.removeFromLeft(WtGuiValue::RightWidth);

    customWaveGroup.setBounds(rightArea);

    auto cwRect = customWaveGroup.getBounds().reduced(WtGuiValue::Group::Padding::width, WtGuiValue::Group::Padding::height);
    cwRect.removeFromTop(WtGuiValue::Group::TitlePaddingTop);

    auto containerArea = cwRect.removeFromTop(WtGuiValue::Custom::Slider::Height);

    customSliders32.setBounds(containerArea);
    customSliders64.setBounds(containerArea);
    customSliders128.setBounds(containerArea);
    customSliders256.setBounds(containerArea);

    // Check if Custom Mode is selected (Index 8 -> ID 9 based on addItem?)
    int waveSize = 0;

    switch (sizeSelector.getSelectedId())
    {
    case 1:
        waveSize = 32;
        break;
    case 2:
        waveSize = 64;
        break;
    case 3:
        waveSize = 128;
        break;
    case 4:
        waveSize = 256;
        break;
    }

    customWaveGroup.setEnabled(isCustomWave);

    customSliders32.setVisible(false);
    customSliders32.setCustomEnabled(false);
    customSliders64.setVisible(false);
    customSliders64.setCustomEnabled(false);
    customSliders128.setVisible(false);
    customSliders128.setCustomEnabled(false);
    customSliders256.setVisible(false);
    customSliders256.setCustomEnabled(false);

    if (isCustomWave) {
        if (waveSize == 32) {
            customSliders32.setVisible(true);
            customSliders32.setCustomEnabled(true);
        }
        else if (waveSize == 64) {
            customSliders64.setVisible(true);
            customSliders64.setCustomEnabled(true);
        }
        else if (waveSize == 128) {
            customSliders128.setVisible(true);
            customSliders128.setCustomEnabled(true);
        }
        else {
            customSliders256.setVisible(true);
            customSliders256.setCustomEnabled(true);
        }
    }
    else {
        if (waveSize == 32) {
            customSliders32.setVisible(true);
        }
        else if (waveSize == 64) {
            customSliders64.setVisible(true);
        }
        else if (waveSize == 128) {
            customSliders128.setVisible(true);
        }
        else {
            customSliders256.setVisible(true);
        }
    }

    cwRect.removeFromTop(WtGuiValue::Custom::ResetBtn::Padding::Top);
    
    auto resetRect = cwRect.removeFromTop(WtGuiValue::Custom::ResetBtn::height);
    
    layoutRowWtWaveValueUpdate({
        .rect = resetRect,
        .resetTo0Btn = &customWaveResetTo0Btn,
        .resetTo1Btn = &customWaveResetTo1Btn,
        .resetToM1Btn = &customWaveResetToM1Btn,
        .rowHeight = WtGuiValue::Custom::ResetBtn::height,
        .paddingRight = WtGuiValue::Custom::ResetBtn::Padding::Right
        });

    cwRect.removeFromTop(WtGuiValue::Custom::ResetBtn::Padding::Bottom + WtGuiValue::Custom::ResetBtn::Padding::Top);

    auto smoothRect = cwRect.removeFromTop(WtGuiValue::Custom::ResetBtn::height);

    customWaveSmoothBtn.setBounds(smoothRect.reduced(2, 0));

    updateCustomWaveCatOnChange();
}

void GuiWt::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

void GuiWt::updateCustomWaveCatOnChange()
{
    int index = waveSelector.getSelectedId();
    bool visible = index == 9; // custom

    sizeSelector.setVisibleWithLabel(visible);
    stepsSelector.setVisibleWithLabel(visible);
}

// ==============================================================================
// Import / Export Logic
// ==============================================================================
void GuiWt::importWavetable()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importWavetableFile, defaultDir, Io::ExtensionGlob::wavetable);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                if (lines.size() == 0) return;

                // 1行目のサンプル数を取得
                int sampleCount = lines[0].trim().getIntValue();

                // サンプル数の検証
                if (sampleCount != 32 && sampleCount != 64 && sampleCount != 128 && sampleCount != 256) {
                    juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
                        "Invalid WT File", "Sample count must be 32, 64, 128, or 256.");
                    return;
                }

                // デフォルト0.0で配列を初期化（足りない部分は0.0で埋まる）
                std::vector<float> values(sampleCount, 0.0f);

                // 2行目以降の値を読み込み、-1.0 ~ 1.0 にクランプして格納
                for (int i = 0; i < sampleCount; ++i) {
                    if (i + 1 < lines.size()) {
                        float val = lines[i + 1].getFloatValue();
                        values[i] = std::clamp(val, -1.0f, 1.0f);
                    }
                }

                // --- UIの更新 ---
                // 波形を「8: Custom(Draw)」に変更 (ID: 9)
                waveSelector.setSelectedId(9, juce::sendNotification);

                // サンプルサイズを選択
                int sizeId = 1;
                if (sampleCount == 32) sizeId = 1;
                else if (sampleCount == 64) sizeId = 2;
                else if (sampleCount == 128) sizeId = 3;
                else if (sampleCount == 256) sizeId = 4;
                sizeSelector.setSelectedId(sizeId, juce::sendNotification);

                // --- 値をAPVTS(スライダー)に反映 ---
                if (sampleCount == 32) customSliders32.setValues(values);
                else if (sampleCount == 64) customSliders64.setValues(values);
                else if (sampleCount == 128) customSliders128.setValues(values);
                else if (sampleCount == 256) customSliders256.setValues(values);
            }
        });
}

void GuiWt::exportWavetable()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportWavetableFile, defaultDir.getChildFile("custom_wave.wt"), Io::ExtensionGlob::wavetable);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();

                // 現在のサイズIDを取得
                int sizeId = sizeSelector.getSelectedId();
                std::vector<float> values;

                if (sizeId == 1) values = customSliders32.getValues();
                else if (sizeId == 2) values = customSliders64.getValues();
                else if (sizeId == 3) values = customSliders128.getValues();
                else if (sizeId == 4) values = customSliders256.getValues();

                if (values.empty()) return;

                // 1行目にサンプル数
                juce::String content = juce::String(values.size()) + "\n";

                // 2行目以降に値を書き込む
                for (float v : values) {
                    content += juce::String(v, 6) + "\n"; // 小数点以下6桁まで保存
                }

                file.replaceWithText(content);
            }
        });
}

void GuiWt::initParams()
{
    this->ctx.audioProcessor.initParams("WT_");
}

void GuiWt::layoutQualityCat(juce::Rectangle<int>& rect) {
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

void GuiWt::layoutMonoModeCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &monoPolyCat });

    bool visible = monoPolyCat.isDetailVisible();

    monoModeToggle.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoModeToggle });
    }
}

void GuiWt::layoutMvolCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &mvolCat });

    bool visible = mvolCat.isDetailVisible();

    masterVolSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });
    }
}

void GuiWt::layoutModulationCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &modCat });

    bool visible = modCat.isDetailVisible();

    modEnableButton.setVisible(visible);
    modDepthSlider.setVisibleWithLabel(visible);
    modSpeedSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &modEnableButton });
        layoutMain({ .mainRect = rect, .label = &modDepthSlider.label, .component = &modDepthSlider });
        layoutMain({ .mainRect = rect, .label = &modSpeedSlider.label, .component = &modSpeedSlider, });
    }
}

void GuiWt::layoutAdsrCat(juce::Rectangle<int>& rect)
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

void GuiWt::layoutPitchEnvCat(juce::Rectangle<int>& rect)
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

void GuiWt::layoutDetuneCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &detuneCat });

    bool visible = detuneCat.isDetailVisible();

    mul.setVisibleWithLabel(visible);
    mulRatio.setVisibleWithLabel(visible);
    dt1.setVisibleWithLabel(visible);
    dt2.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &mul.label, .component = &mul });
        layoutMain({ .mainRect = rect, .label = &mulRatio.label, .component = &mulRatio });
        layoutMain({ .mainRect = rect, .label = &dt1.label, .component = &dt1 });
        layoutMain({ .mainRect = rect, .label = &dt2.label, .component = &dt2 });
    }
}

void GuiWt::layoutWavefileCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &waveFileCat });

    bool visible = waveFileCat.isDetailVisible();

    customWaveImportBtn.setVisible(visible);
    customWaveExportBtn.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &customWaveImportBtn });
        layoutMain({ .mainRect = rect, .component = &customWaveExportBtn });
    }
}

void GuiWt::layoutLfoCat(juce::Rectangle<int>& rect)
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

void GuiWt::layoutSsgSwEnvCat(juce::Rectangle<int>& rect)
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

void GuiWt::applySsgSwEnvLoopValues(bool enabled)
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

void GuiWt::setupGraph()
{
    addAndMakeVisible(&graph); // グラフを追加

    graphBtnAmp.setup({ .parent = *this, .title = "Amp", .isReset = false, .isResized = false });
    graphBtnAmp.setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp.onClick = [this] { setGraphMode(GraphMode::Amp); };

    graphBtnPitch.setup({ .parent = *this, .title = "Pitch", .isReset = false, .isResized = false });
    graphBtnPitch.onClick = [this] { setGraphMode(GraphMode::Pitch); };

    graphBtnSsg.setup({ .parent = *this, .title = "SSG SW", .isReset = false, .isResized = false });
    graphBtnSsg.onClick = [this] { setGraphMode(GraphMode::SsgSw); };

    auto repaintGraph = [this]() { updateGraph(); };

    startLevelSlider.onValueChange = repaintGraph;
    attackSlider.onValueChange = repaintGraph;
    decaySlider.onValueChange = repaintGraph;
    sustainSlider.onValueChange = repaintGraph;
    releaseSlider.onValueChange = repaintGraph;

    pitchAttackSlider.onValueChange = repaintGraph;
    pitchDecaySlider.onValueChange = repaintGraph;
    pitchReleaseSlider.onValueChange = repaintGraph;
    pitchStartLevelSlider.onValueChange = repaintGraph;
    pitchAttackLevelSlider.onValueChange = repaintGraph;
    pitchSustainLevelSlider.onValueChange = repaintGraph;
    pitchReleaseLevelSlider.onValueChange = repaintGraph;

    ssgSwEnvLoopButton.onStateChange = repaintGraph;

    ssgSwStepsSlider.onValueChange = [this, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();
        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        repaintGraph();
        };
    ssgSwLoopToSlider.onValueChange = [this, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();
        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        repaintGraph();
        };
    ssgSwLoopCountSlider.onValueChange = [this, repaintGraph]() {
        // 既存のループ設定ロジックを呼んだ後に再描画
        bool ssgEnvLoopEnable = ssgSwEnvLoopButton.getToggleState();
        applySsgSwEnvLoopValues(ssgEnvLoopEnable);
        repaintGraph();
        };

    ssgSwR1Slider.onValueChange = repaintGraph;
    ssgSwR2Slider.onValueChange = repaintGraph;
    ssgSwR3Slider.onValueChange = repaintGraph;
    ssgSwR4Slider.onValueChange = repaintGraph;
    ssgSwR5Slider.onValueChange = repaintGraph;
    ssgSwR6Slider.onValueChange = repaintGraph;

    ssgSwStartLevelSlider.onValueChange = repaintGraph;
    ssgSwL1Slider.onValueChange = repaintGraph;
    ssgSwL2Slider.onValueChange = repaintGraph;
    ssgSwL3Slider.onValueChange = repaintGraph;
    ssgSwL4Slider.onValueChange = repaintGraph;
    ssgSwL5Slider.onValueChange = repaintGraph;
    ssgSwL6Slider.onValueChange = repaintGraph;

    addAndMakeVisible(graphSeparator);
    graphSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });
}

void GuiWt::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateGraph();
}

void GuiWt::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(WtGuiValue::MainGroup::Graph::height + WtGuiValue::MainGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(WtGuiValue::MainGroup::Separator::height);

    graphSeparator.setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(WtGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg.setBounds(btnArea);

    // 残りをグラフエリアにする
    graph.setBounds(mainArea);
}

// グラフを再計算して描画
void GuiWt::updateGraph()
{
    GraphMode mode = currentGraphMode;

    // カーブモードが有効かどうかを判定
    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();

    // =============================================================
    // Pitch Env
    // =============================================================
    if (mode == GraphMode::Pitch) {
        graph.updatePitchEnv(
            pitchAttackSlider,
            pitchDecaySlider,
            pitchReleaseSlider,
            pitchStartLevelSlider,
            pitchAttackLevelSlider,
            pitchSustainLevelSlider,
            pitchReleaseLevelSlider,
            p_curveCore,
            isCurveMode,
            0
        );
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        graph.updateSsgSwEnv(
            ssgSwStepsSlider,
            ssgSwEnvLoopButton,
            ssgSwLoopToSlider,
            ssgSwLoopCountSlider,
            { nullptr, &ssgSwR1Slider, &ssgSwR2Slider, &ssgSwR3Slider, &ssgSwR4Slider, &ssgSwR5Slider, &ssgSwR6Slider },
            { &ssgSwStartLevelSlider, &ssgSwL1Slider, &ssgSwL2Slider, &ssgSwL3Slider, &ssgSwL4Slider, &ssgSwL5Slider, &ssgSwL6Slider },
            p_curveCore,
            isCurveMode,
            0
        );
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        graph.updateAmpEnv(
            startLevelSlider,
            attackSlider,
            decaySlider,
            sustainSlider,
            releaseSlider,
            p_curveCore,
            isCurveMode,
            0
        );
    }
}
