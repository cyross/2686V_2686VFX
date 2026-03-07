#include "GuiSsg.h"

#include <vector>

#include "../processor/PluginProcessor.h"

#include "../core/PrKeys.h"
#include "../core/PrValues.h"

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

void GuiSsg::setup()
{
    const juce::String code = PrKey::Prefix::ssg;

    mainGroup.setup(*this, GuiText::Group::mainGroup);
    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::bit, .title = GuiText::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::rate, .title = GuiText::rate, .items = rateItems, .isReset = true });

	voiceGroup.setup(*this, GuiText::Group::voice);
	waveSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::wveform, .title = GuiText::Ssg::Voice::form, .items = ssgWsItems, .isReset = true, .isResized = true });
	levelSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::tone, .title = GuiText::Ssg::Voice::tone, .isReset = true, .regType = RegisterType::SsgVol });
    noiseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::noise, .title = GuiText::Ssg::Voice::noise, .isReset = true, .regType = RegisterType::SsgVol });
	noiseFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::noiseFreq, .title = GuiText::Ssg::Voice::noiseFreq, .isReset = true });
	noiseOnNoteButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::noiseOnNote, .title = GuiText::Ssg::Voice::noiseOnNote, .isReset = true });

    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });
    shapeCat.setup({ .parent = *this, .title = GuiText::Category::shape });
    toneCat.setup({ .parent = *this, .title = GuiText::Category::ssgTone });
    noiseCat.setup({ .parent = *this, .title = GuiText::Category::ssgNoise });
    mixCat.setup({ .parent = *this, .title = GuiText::Category::mix });
    pulseDutyCat.setup({ .parent = *this, .title = GuiText::Category::m });
    pulseInvCat.setup({ .parent = *this, .title = GuiText::Category::invert });
    triCat.setup({ .parent = *this, .title = GuiText::Category::m });
    triPeakCat.setup({ .parent = *this, .title = GuiText::Category::peak });
    hwEnvCat.setup({ .parent = *this, .title = GuiText::Category::m });
    adsrCat.setup({ .parent = *this, .title = GuiText::Category::adsr });

    // 初期状態反映
    mixSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::mix , .title = GuiText::Ssg::Voice::mix, .isReset = true });
	mixSetTone.setup({ .parent = *this, .title = GuiText::Ssg::Voice::tone, .isReset = false, .isResized = false });
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };
    mixSetMix.setup({ .parent = *this, .title = GuiText::Ssg::Voice::mix, .isReset = false, .isResized = false });
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };
    mixSetNoise.setup({ .parent = *this, .title = GuiText::Ssg::Voice::noise, .isReset = false, .isResized = false });
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

	adsrBypassButton.setup({ .parent = *this, .id = code + PrKey::Innder::adsr + PrKey::Post::bypass, .title = GuiText::Adsr::bypass, .isReset = true });

    attackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::ar, .title = GuiText::Adsr::ar, .isReset = true });
    decaySlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::dr , .title = GuiText::Adsr::dr, .isReset = true });
    sustainSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::sl, .title = GuiText::Adsr::sl, .isReset = true });
    releaseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::rr, .title = GuiText::Adsr::rr, .isReset = true });

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    // Duty Controls Setup
	dutyGroup.setup(*this, GuiText::Group::ssgDuty);
	dutyModeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::mode, .title = GuiText::Ssg::Duty::mode, .items = ssgDmItems, .isReset = true, .isResized = true });
	dutyPresetSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::preset, .title = GuiText::Ssg::Duty::preset, .items = ssgPrItems, .isReset = true, .isResized = true });
	dutyVarSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::var, .title = GuiText::Ssg::Duty::var, .isReset = true });
	dutyInvertButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Duty::inv, .title = GuiText::Ssg::Duty::invert, .isReset = true, .isResized = true });

	triGroup.setup(*this, GuiText::Group::ssgTri);
	triKeyTrackButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Tri::keyTrk, .title = GuiText::Ssg::Tri::keyTrack, .isReset = true, .isResized = true });
	triFreqSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Tri::freq, .title = GuiText::Ssg::Tri::manualFreq, .isReset = true });
	triPeakSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::Tri::peak, .title = GuiText::Ssg::Tri::peak, .isReset = true });
	triSetSawDown.setup({ .parent = *this, .title = GuiText::Ssg::Tri::peakTo00, .isReset = false, .isResized = false });
    triSetSawDown.onClick = [this] { triPeakSlider.setValue(0.0, juce::sendNotification); };
	triSetTri.setup({ .parent = *this, .title = GuiText::Ssg::Tri::peakTo05, .isReset = false, .isResized = false });
    triSetTri.onClick = [this] { triPeakSlider.setValue(0.5, juce::sendNotification); };
	triSetSawUp.setup({ .parent = *this, .title = GuiText::Ssg::Tri::peakTo10, .isReset = false, .isResized = false });
    triSetSawUp.onClick = [this] { triPeakSlider.setValue(1.0, juce::sendNotification); };

    // HW Env Group
	envGroup.setup(*this, GuiText::Group::ssgHwEnv);
	envEnableButton.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::HwEnv::enable, .title = GuiText::Ssg::HwEnv::enable, .isReset = true });
	shapeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::HwEnv::shape, .title = GuiText::Ssg::HwEnv::shape, .items = ssgEnvItems, .isReset = true });
	periodSlider.setup({ .parent = *this, .id = code + PrKey::Post::Ssg::HwEnv::period, .title = GuiText::Ssg::HwEnv::speed, .isReset = true, .regType = RegisterType::SsgEnv });
}

void GuiSsg::layout(juce::Rectangle<int> content)
{
    juce::String code = PrKey::Prefix::ssg;
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &qualityCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &bitSelector.label, .component = &bitSelector });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &rateSelector.label, .component = &rateSelector, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &adsrCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &adsrBypassButton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &attackSlider.label, .component = &attackSlider });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &decaySlider.label, .component = &decaySlider });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &sustainSlider.label, .component = &sustainSlider });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &releaseSlider.label, .component = &releaseSlider, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

    // --- Voice Group ---
    auto voiceArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

    voiceGroup.setBounds(voiceArea);
    auto vRect = voiceGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    vRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = vRect, .label = &shapeCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = vRect, .label = &waveSelector.label, .component = &waveSelector, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = vRect, .label = &toneCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = vRect, .label = &levelSlider.label, .component = &levelSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = vRect, .label = &noiseCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = vRect, .label = &noiseSlider.label, .component = &noiseSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = vRect, .label = &noiseFreqSlider.label, .component = &noiseFreqSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = vRect, .component = &noiseOnNoteButton, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = vRect, .label = &mixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = vRect, .label = &mixSlider.label, .component = &mixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRSsgMixRow({ .rect = vRect, .toneBtn = &mixSetTone, .mixBtn =  &mixSetMix, .noizeBtn = &mixSetNoise, .paddingBottom = 0 });

    // Wave Group
    float waveParam = *ctx.audioProcessor.apvts.getRawParameterValue(code + PrKey::Post::Ssg::wveform);
    int waveMode = (waveParam > PrValue::boolThread) ? 1 : 0;
    auto waveArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);

    if (waveMode == 0) // Pulse
    {
        dutyGroup.setVisible(true);
        triGroup.setVisible(false);

        triCat.setVisible(false);
        triKeyTrackButton.setVisible(false);
        triPeakCat.setVisible(false);
        triPeakSlider.setVisible(false);
        triPeakSlider.label.setVisible(false);
        triFreqSlider.setVisible(false);
        triFreqSlider.label.setVisible(false);
        triSetSawDown.setVisible(false);
        triSetTri.setVisible(false);
        triSetSawUp.setVisible(false);

        pulseDutyCat.setVisible(true);
        pulseInvCat.setVisible(true);
        dutyModeSelector.setVisible(true);
        dutyModeSelector.label.setVisible(true);
        dutyVarSlider.setValue(true);
        dutyVarSlider.label.setVisible(true);
        dutyInvertButton.setVisible(true);

        dutyGroup.setBounds(waveArea);
        auto dRect = dutyGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

        dRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        layoutComponentsLtoRRow({ .rowRect = dRect, .label = &pulseDutyCat, .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = dRect, .label = &dutyModeSelector.label, .component = &dutyModeSelector, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });

        float dutyModeVal = *ctx.audioProcessor.apvts.getRawParameterValue(code + PrKey::Post::Ssg::Duty::mode);
        if (dutyModeVal < 0.5f) {
            dutyPresetSelector.setVisible(true);
            dutyPresetSelector.label.setVisible(true);
            dutyVarSlider.setVisible(false);
            dutyVarSlider.label.setVisible(false);
            layoutComponentsLtoRRow({ .rowRect = dRect, .label = &dutyPresetSelector.label, .component = &dutyPresetSelector, .paddingBottom = GuiValue::Category::paddingTop });
        }
        else {
            dutyPresetSelector.setVisible(false);
            dutyPresetSelector.label.setVisible(false);
            dutyVarSlider.setVisible(true);
            dutyVarSlider.label.setVisible(true);
            layoutComponentsLtoRRow({ .rowRect = dRect, .label = &dutyVarSlider.label, .component = &dutyVarSlider, .paddingBottom = GuiValue::Category::paddingTop });
        }

        layoutComponentsLtoRRow({ .rowRect = dRect, .label = &pulseInvCat, .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = dRect, .component = &dutyInvertButton, .paddingBottom = 0 });
    }
    else // Triangle
    {
        dutyGroup.setVisible(false);
        triGroup.setVisible(true);

        pulseDutyCat.setVisible(false);
        pulseInvCat.setVisible(false);
        dutyModeSelector.setVisible(false);
        dutyModeSelector.label.setVisible(false);
        dutyInvertButton.setVisible(false);
        dutyPresetSelector.setVisible(false);
        dutyPresetSelector.label.setVisible(false);
        dutyVarSlider.setVisible(false);
        dutyVarSlider.label.setVisible(false);

        triCat.setVisible(true);
        triKeyTrackButton.setVisible(true);
        triFreqSlider.setVisible(true);
        triFreqSlider.label.setVisible(true);
        triPeakCat.setVisible(true);
        triPeakSlider.setVisible(true);
        triPeakSlider.label.setVisible(true);
        triSetSawDown.setVisible(true);
        triSetTri.setVisible(true);
        triSetSawUp.setVisible(true);

        triGroup.setBounds(waveArea);
        auto tRect = triGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

        tRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

        layoutComponentsLtoRRow({ .rowRect = tRect, .label = &triCat, .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = tRect, .component = &triKeyTrackButton, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });

        bool isKeyTrack = triKeyTrackButton.getToggleState();
        triFreqSlider.setVisible(!isKeyTrack);
        triFreqSlider.label.setVisible(!isKeyTrack);

        if (!isKeyTrack) {
            layoutComponentsLtoRRow({ .rowRect = tRect, .label = &triFreqSlider.label, .component = &triFreqSlider, .paddingBottom = GuiValue::Category::paddingTop });
        }
        else
        {
            tRect.removeFromTop(GuiValue::Category::paddingTop);
        }

        layoutComponentsLtoRRow({ .rowRect = tRect, .label = &triPeakCat, .paddingBottom = GuiValue::Category::paddingBotton });
        layoutComponentsLtoRRow({ .rowRect = tRect, .label = &triPeakSlider.label, .component = &triPeakSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
        layoutComponentsLtoRSsgTriPosRow({ .rect = tRect, .sawDownBtn = &triSetSawDown, .triBtn = &triSetTri, .sawUpBtn = &triSetSawUp, .paddingBottom = 0 });
    }

    // HW Env Group
    auto envArea = pageArea.removeFromLeft(GuiValue::Fm::Op::width);
    envGroup.setBounds(envArea);
    auto eRect = envGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    eRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = eRect, .label = &hwEnvCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = eRect, .component = &envEnableButton, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = eRect, .label = &shapeSelector.label, .component = &shapeSelector, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = eRect, .label = &periodSlider.label, .component = &periodSlider, .paddingBottom = 0 });
}
