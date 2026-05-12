#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Core/Const/PrKeys.h"
#include "../../Core/Const/PrValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiText.h"
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
    {.name = "1: 96kHz",    .value = 1 },
    {.name = "2: 55.5kHz",  .value = 2 },
    {.name = "3: 48kHz",    .value = 3 },
    {.name = "4: 44.1kHz",  .value = 4 },
    {.name = "5: 22.05kHz", .value = 5 },
    {.name = "6: 16kHz",    .value = 6 },
    {.name = "7: 8kHz",     .value = 7 },
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

void GuiSsg::setup()
{
    const juce::String code = PrKey::Prefix::ssg;
    int tabOrder = 1;

    mainGroup.setup(*this, GuiText::Group::mainGroup);

    presetNameCat.setup({ .parent = *this, .title = GuiText::Category::preset });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });

    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::bit, .title = GuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::rate, .title = GuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    adsrCat.setup({ .parent = *this, .title = GuiText::Category::adsr });

    adsrBypassButton.setup({ .parent = *this, .id = code + PrKey::Innder::adsr + PrKey::Post::bypass, .title = GuiText::Adsr::bypass, .isReset = true });
    adsrBypassButton.setWantsKeyboardFocus(true);
    adsrBypassButton.setExplicitFocusOrder(++tabOrder);

    attackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::ar, .title = GuiText::Adsr::ar, .isReset = true });
    attackSlider.setWantsKeyboardFocus(true);
    attackSlider.setExplicitFocusOrder(++tabOrder);

    decaySlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::dr , .title = GuiText::Adsr::dr, .isReset = true });
    decaySlider.setWantsKeyboardFocus(true);
    decaySlider.setExplicitFocusOrder(++tabOrder);

    sustainSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::sl, .title = GuiText::Adsr::sl, .isReset = true });
    sustainSlider.setWantsKeyboardFocus(true);
    sustainSlider.setExplicitFocusOrder(++tabOrder);

    releaseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::rr, .title = GuiText::Adsr::rr, .isReset = true });
    releaseSlider.setWantsKeyboardFocus(true);
    releaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchAdsrCat.setup({ .parent = *this, .title = GuiText::Category::pitchAdsr });

    pitchAdsrBypassButton.setup({ .parent = *this, .id = code + PrKey::Innder::pitchAdsr + PrKey::Post::bypass, .title = GuiText::PitchAdsr::bypass, .isReset = true });
    pitchAdsrBypassButton.setWantsKeyboardFocus(true);
    pitchAdsrBypassButton.setExplicitFocusOrder(++tabOrder);

    pitchAttackSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::ar, .title = GuiText::PitchAdsr::ar, .isReset = true });
    pitchAttackSlider.setWantsKeyboardFocus(true);
    pitchAttackSlider.setExplicitFocusOrder(++tabOrder);

    pitchDecaySlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::dr , .title = GuiText::PitchAdsr::dr, .isReset = true });
    pitchDecaySlider.setWantsKeyboardFocus(true);
    pitchDecaySlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::rr, .title = GuiText::PitchAdsr::rr, .isReset = true });
    pitchReleaseSlider.setWantsKeyboardFocus(true);
    pitchReleaseSlider.setExplicitFocusOrder(++tabOrder);

    pitchStartLevelSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::stl, .title = GuiText::PitchAdsr::stl, .isReset = true });
    pitchStartLevelSlider.setWantsKeyboardFocus(true);
    pitchStartLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchAttackLevelSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::atl, .title = GuiText::PitchAdsr::atl, .isReset = true });
    pitchAttackLevelSlider.setWantsKeyboardFocus(true);
    pitchAttackLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchSustainLevelSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::ssl, .title = GuiText::PitchAdsr::ssl, .isReset = true });
    pitchSustainLevelSlider.setWantsKeyboardFocus(true);
    pitchSustainLevelSlider.setExplicitFocusOrder(++tabOrder);

    pitchReleaseLevelSlider.setup({ .parent = *this, .id = code + PrKey::Post::PitchAdsr::rll, .title = GuiText::PitchAdsr::rll, .isReset = true });
    pitchReleaseLevelSlider.setWantsKeyboardFocus(true);
    pitchReleaseLevelSlider.setExplicitFocusOrder(++tabOrder);

    detuneCat.setup({ .parent = *this, .title = GuiText::Category::detune });

    dt1.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Op::dt, .title = GuiText::Fm::Op::Dt1, .items = dtItems, .isReset = true });
    dt1.setWantsKeyboardFocus(true);
    dt1.setExplicitFocusOrder(++tabOrder);

    dt2.setup({ .parent = *this, .id = code + PrKey::Post::Fm::Op::dt2, .title = GuiText::Fm::Op::Dt2, .isReset = true });
    dt2.setWantsKeyboardFocus(true);
    dt2.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setup({ .parent = *this, .title = GuiText::Category::monoMode });

    monoModeToggle.setup({ .parent = *this, .id = PrKey::monoMode, .title = GuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

	voiceGroup.setup(*this, GuiText::Group::voice);

    shapeCat.setup({ .parent = *this, .title = GuiText::Category::shape });

    waveSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::wveform, .title = GuiText::Ssg::Voice::form, .items = ssgWsItems, .isReset = true, .isResized = true });
    waveSelector.setWantsKeyboardFocus(true);
    waveSelector.setExplicitFocusOrder(++tabOrder);

    toneCat.setup({ .parent = *this, .title = GuiText::Category::ssgTone });

    levelSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::tone, .title = GuiText::Ssg::Voice::tone, .isReset = true, .regType = RegisterType::SsgVol });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    noiseCat.setup({ .parent = *this, .title = GuiText::Category::ssgNoise });

    noiseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::noise, .title = GuiText::Ssg::Voice::noise, .isReset = true, .regType = RegisterType::SsgVol });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::noiseFreq, .title = GuiText::Ssg::Voice::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    noiseOnNoteButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::noiseOnNote, .title = GuiText::Ssg::Voice::noiseOnNote, .isReset = true });
    noiseOnNoteButton.setWantsKeyboardFocus(true);
    noiseOnNoteButton.setExplicitFocusOrder(++tabOrder);

    mixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    // 初期状態反映
    mixSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::mix , .title = GuiText::Ssg::Voice::mix, .isReset = true });
    mixSlider.setWantsKeyboardFocus(true);
    mixSlider.setExplicitFocusOrder(++tabOrder);

    mixSetTone.setup({ .parent = *this, .title = GuiText::Ssg::Voice::tone, .isReset = false, .isResized = false });
    mixSetTone.setWantsKeyboardFocus(true);
    mixSetTone.setExplicitFocusOrder(++tabOrder);
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };

    mixSetMix.setup({ .parent = *this, .title = GuiText::Ssg::Voice::mix, .isReset = false, .isResized = false });
    mixSetMix.setWantsKeyboardFocus(true);
    mixSetMix.setExplicitFocusOrder(++tabOrder);
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };

    mixSetNoise.setup({ .parent = *this, .title = GuiText::Ssg::Voice::noise, .isReset = false, .isResized = false });
    mixSetNoise.setWantsKeyboardFocus(true);
    mixSetNoise.setExplicitFocusOrder(++tabOrder);
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

    // Duty Controls Setup
    dutyGroup.setup(*this, GuiText::Group::ssgDuty);

    pulseDutyCat.setup({ .parent = *this, .title = GuiText::Category::m });

    dutyModeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::mode, .title = GuiText::Ssg::Duty::mode, .items = ssgDmItems, .isReset = true, .isResized = true });
    dutyModeSelector.setWantsKeyboardFocus(true);
    dutyModeSelector.setExplicitFocusOrder(++tabOrder);

    dutyPresetSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::preset, .title = GuiText::Ssg::Duty::preset, .items = ssgPrItems, .isReset = true, .isResized = true });
    dutyPresetSelector.setWantsKeyboardFocus(true);
    dutyPresetSelector.setExplicitFocusOrder(++tabOrder);

    dutyVarSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::var, .title = GuiText::Ssg::Duty::var, .isReset = true });
    dutyVarSlider.setWantsKeyboardFocus(true);
    dutyVarSlider.setExplicitFocusOrder(++tabOrder);

    pulseInvCat.setup({ .parent = *this, .title = GuiText::Category::invert });

    dutyInvertButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::inv, .title = GuiText::Ssg::Duty::invert, .isReset = true, .isResized = true });
    dutyInvertButton.setWantsKeyboardFocus(true);
    dutyInvertButton.setExplicitFocusOrder(++tabOrder);

    triGroup.setup(*this, GuiText::Group::ssgTri);

    triCat.setup({ .parent = *this, .title = GuiText::Category::m });

    triKeyTrackButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Tri::keyTrk, .title = GuiText::Ssg::Tri::keyTrack, .isReset = true, .isResized = true });
    triKeyTrackButton.setWantsKeyboardFocus(true);
    triKeyTrackButton.setExplicitFocusOrder(++tabOrder);

    triFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Tri::freq, .title = GuiText::Ssg::Tri::manualFreq, .isReset = true });
    triFreqSlider.setWantsKeyboardFocus(true);
    triFreqSlider.setExplicitFocusOrder(++tabOrder);

    triPeakCat.setup({ .parent = *this, .title = GuiText::Category::peak });

    triPeakSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Tri::peak, .title = GuiText::Ssg::Tri::peak, .isReset = true });
    triPeakSlider.setWantsKeyboardFocus(true);
    triPeakSlider.setExplicitFocusOrder(++tabOrder);

    triSetSawDown.setup({ .parent = *this, .title = GuiText::Ssg::Tri::peakTo00, .isReset = false, .isResized = false });
    triSetSawDown.setWantsKeyboardFocus(true);
    triSetSawDown.setExplicitFocusOrder(++tabOrder);
    triSetSawDown.onClick = [this] { triPeakSlider.setValue(0.0, juce::sendNotification); };

    triSetTri.setup({ .parent = *this, .title = GuiText::Ssg::Tri::peakTo05, .isReset = false, .isResized = false });
    triSetTri.setWantsKeyboardFocus(true);
    triSetTri.setExplicitFocusOrder(++tabOrder);
    triSetTri.onClick = [this] { triPeakSlider.setValue(0.5, juce::sendNotification); };

    triSetSawUp.setup({ .parent = *this, .title = GuiText::Ssg::Tri::peakTo10, .isReset = false, .isResized = false });
    triSetSawUp.setWantsKeyboardFocus(true);
    triSetSawUp.setExplicitFocusOrder(++tabOrder);
    triSetSawUp.onClick = [this] { triPeakSlider.setValue(1.0, juce::sendNotification); };

    // HW Env Group
	envGroup.setup(*this, GuiText::Group::ssgHwEnv);

    hwEnvCat.setup({ .parent = *this, .title = GuiText::Category::m });

    envEnableButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::HwEnv::enable, .title = GuiText::Ssg::HwEnv::enable, .isReset = true });
    envEnableButton.setWantsKeyboardFocus(true);
    envEnableButton.setExplicitFocusOrder(++tabOrder);
    
    shapeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::HwEnv::shape, .title = GuiText::Ssg::HwEnv::shape, .items = ssgEnvItems, .isReset = true });
    shapeSelector.setWantsKeyboardFocus(true);
    shapeSelector.setExplicitFocusOrder(++tabOrder);
    
    periodSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::HwEnv::period, .title = GuiText::Ssg::HwEnv::speed, .isReset = true, .regType = RegisterType::SsgEnv });
    periodSlider.setWantsKeyboardFocus(true);
    periodSlider.setExplicitFocusOrder(++tabOrder);
}

void GuiSsg::layout(juce::Rectangle<int> content)
{
    juce::String code = PrKey::Prefix::ssg;
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

    layoutMainCategory({ .mainRect = mRect, .label = &adsrCat });
    layoutMain({ .mainRect = mRect, .component = &adsrBypassButton });
    layoutMain({ .mainRect = mRect, .label = &attackSlider.label, .component = &attackSlider });
    layoutMain({ .mainRect = mRect, .label = &decaySlider.label, .component = &decaySlider });
    layoutMain({ .mainRect = mRect, .label = &sustainSlider.label, .component = &sustainSlider });
    layoutMain({ .mainRect = mRect, .label = &releaseSlider.label, .component = &releaseSlider });

    layoutMainCategory({ .mainRect = mRect, .label = &pitchAdsrCat });
    layoutMain({ .mainRect = mRect, .component = &pitchAdsrBypassButton });
    layoutMain({ .mainRect = mRect, .label = &pitchAttackSlider.label, .component = &pitchAttackSlider });
    layoutMain({ .mainRect = mRect, .label = &pitchDecaySlider.label, .component = &pitchDecaySlider });
    layoutMain({ .mainRect = mRect, .label = &pitchReleaseSlider.label, .component = &pitchReleaseSlider });
    layoutMain({ .mainRect = mRect, .label = &pitchStartLevelSlider.label, .component = &pitchStartLevelSlider });
    layoutMain({ .mainRect = mRect, .label = &pitchAttackLevelSlider.label, .component = &pitchAttackLevelSlider });
    layoutMain({ .mainRect = mRect, .label = &pitchSustainLevelSlider.label, .component = &pitchSustainLevelSlider });
    layoutMain({ .mainRect = mRect, .label = &pitchReleaseLevelSlider.label, .component = &pitchReleaseLevelSlider });

    layoutMainCategory({ .mainRect = mRect, .label = &detuneCat });
    layoutMain({ .mainRect = mRect, .label = &dt1.label, .component = &dt1 });
    layoutMain({ .mainRect = mRect, .label = &dt2.label, .component = &dt2 });

    layoutMainCategory({ .mainRect = mRect, .label = &monoPolyCat });
    layoutMain({ .mainRect = mRect, .component = &monoModeToggle });
    layoutMainCategory({ .mainRect = mRect, .label = &mvolCat });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

    auto paramArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

    // --- Voice Group ---
    auto voiceArea = paramArea.removeFromTop(230);

    voiceGroup.setBounds(voiceArea);
    auto vRect = voiceGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    vRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

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
    float waveParam = *ctx.audioProcessor.apvts.getRawParameterValue(code + PrKey::Post::Ssg::wveform);
    int waveMode = (waveParam > PrValue::boolThread) ? 1 : 0;
    auto waveArea = paramArea.removeFromTop(140);

    if (waveMode == 0) // Pulse
    {
        dutyGroup.setVisible(true);
        triGroup.setVisible(false);

        triCat.setVisible(false);
        triKeyTrackButton.setVisible(false);
        triPeakCat.setVisible(false);
        triPeakSlider.setVisibleWithLabel(false);
        triFreqSlider.setVisibleWithLabel(false);
        triSetSawDown.setVisible(false);
        triSetTri.setVisible(false);
        triSetSawUp.setVisible(false);

        pulseDutyCat.setVisible(true);
        pulseInvCat.setVisible(true);
        dutyModeSelector.setVisibleWithLabel(true);
        dutyVarSlider.setValue(true);
        dutyVarSlider.label.setVisible(true);
        dutyInvertButton.setVisible(true);

        dutyGroup.setBounds(waveArea);
        auto dRect = dutyGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

        dRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        layoutRowCategory({ .rowRect = dRect, .label = &pulseDutyCat });
        layoutRow({ .rowRect = dRect, .label = &dutyModeSelector.label, .component = &dutyModeSelector });

        float dutyModeVal = *ctx.audioProcessor.apvts.getRawParameterValue(code + PrKey::Post::Ssg::Duty::mode);
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

        pulseDutyCat.setVisible(false);
        pulseInvCat.setVisible(false);
        dutyModeSelector.setVisibleWithLabel(false);
        dutyInvertButton.setVisible(false);
        dutyPresetSelector.setVisibleWithLabel(false);
        dutyVarSlider.setVisible(false);
        dutyVarSlider.label.setVisible(false);

        triCat.setVisible(true);
        triKeyTrackButton.setVisible(true);
        triFreqSlider.setVisibleWithLabel(true);
        triPeakCat.setVisible(true);
        triPeakSlider.setVisibleWithLabel(true);
        triSetSawDown.setVisible(true);
        triSetTri.setVisible(true);
        triSetSawUp.setVisible(true);

        triGroup.setBounds(waveArea);
        auto tRect = triGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

        tRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        layoutRowCategory({ .rowRect = tRect, .label = &triCat });
        layoutRow({ .rowRect = tRect, .component = &triKeyTrackButton });

        bool isKeyTrack = triKeyTrackButton.getToggleState();
        triFreqSlider.setVisibleWithLabel(!isKeyTrack);

        if (!isKeyTrack) {
            layoutRow({ .rowRect = tRect, .label = &triFreqSlider.label, .component = &triFreqSlider, });
        }
        else
        {
            tRect.removeFromTop(GuiValue::Category::paddingTop);
        }

        layoutRowCategory({ .rowRect = tRect, .label = &triPeakCat });
        layoutRow({ .rowRect = tRect, .label = &triPeakSlider.label, .component = &triPeakSlider });
        layoutRowThreeComps({ .rect = tRect, .comp1 = &triSetSawDown, .comp2 = &triSetTri, .comp3 = &triSetSawUp, .paddingBottom = 0 });
    }

    // HW Env Group
    auto envArea = paramArea.removeFromTop(120);
    envGroup.setBounds(envArea);
    auto eRect = envGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    eRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRowCategory({ .rowRect = eRect, .label = &hwEnvCat });
    layoutRow({ .rowRect = eRect, .component = &envEnableButton });
    layoutRow({ .rowRect = eRect, .label = &shapeSelector.label, .component = &shapeSelector });
    layoutRow({ .rowRect = eRect, .label = &periodSlider.label, .component = &periodSlider, .paddingBottom = 0 });
}

void GuiSsg::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

void GuiSsg::initParams()
{
    this->ctx.audioProcessor.initParams("SSG_");
}
