#include "GuiWt.h"

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
    {.name = "0: 32 Samples",  .value = 1 },
    {.name = "1: 64 Samples",  .value = 2 },
};

void GuiWt::setup()
{
    const juce::String code = PrKey::Prefix::wt;

    mainGroup.setup(*this, GuiText::Group::mainGroup);

    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::bit, .title = GuiText::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::rate, .title = GuiText::rate, .items = rateItems, .isReset = true });

    levelSlider.setup({ .parent = *this, .id = code + PrKey::Post::Wt::level, .title = GuiText::Wt::level, .isReset = true });

    monoPolyCat.setup({ .parent = *this, .title = GuiText::Category::monoMode });
    presetNameCat.setup({ .parent = *this, .title = GuiText::Category::preset });
    qualityCat.setup({ .parent = *this, .title = GuiText::Category::quality });
    mainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    adsrCat.setup({ .parent = *this, .title = GuiText::Category::adsr });
    modCat.setup({ .parent = *this, .title = GuiText::Category::mod });

    attackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::ar, .title = GuiText::Adsr::ar, .isReset = true });
    decaySlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::dr, .title = GuiText::Adsr::dr, .isReset = true });
    sustainSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::sl, .title = GuiText::Adsr::sl, .isReset = true });
    releaseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::rr, .title = GuiText::Adsr::rr, .isReset = true });

    // Waveform
	waveSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::wave, .title = GuiText::Wt::form, .items = wtWsItems, .isReset = true, .isResized = true });

    // Custom Wave Size
    sizeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::sampleSize, .title = GuiText::Wt::size, .items = wtTsItems, .isReset = true, .isResized = true });

    // Modulation
    modEnableButton.setup({ .parent = *this, .id = code + PrKey::Post::Wt::Mod::enable, .title = GuiText::Wt::Mod::enable, .isReset = true, .isResized = true });
	modDepthSlider.setup({ .parent = *this, .id = code + PrKey::Post::Wt::Mod::depth, .title = GuiText::Wt::Mod::depth, .isReset = true });
	modSpeedSlider.setup({ .parent = *this, .id = code + PrKey::Post::Wt::Mod::speed, .title = GuiText::Wt::Mod::speed, .isReset = true });

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

    // Master Volume
	masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    monoModeToggle.setup({ .parent = *this, .id = PrKey::monoMode, .title = GuiText::monoPoly, .isReset = true });

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.5f));

    // Custom Wave Group
	customWaveGroup.setup(*this, GuiText::Group::wtCustom);

    // Custom Wave Sliders
	customSliders32.setup({ .parent = *this, .idPrefix = code + PrKey::Innder::custom32 });
    customSliders64.setup({ .parent = *this, .idPrefix = code + PrKey::Innder::custom64 });

	customWaveResetTo0Btn.setup({ .parent = *this, .title = GuiText::Wt::Custom::to0, .bgColor = GuiColor::WaveformContainer::ResetBtn::To0, .isReset = false, .isResized = false });
    customWaveResetTo0Btn.onClick = [this] {
        customSliders32.setAllValues(0.0f);
        customSliders64.setAllValues(0.0f);
        resized();
    };

    customWaveResetTo1Btn.setup({ .parent = *this, .title = GuiText::Wt::Custom::to1, .bgColor = GuiColor::WaveformContainer::ResetBtn::To1, .isReset = false, .isResized = false });
    customWaveResetTo1Btn.onClick = [this] {
        customSliders32.setAllValues(1.0f);
        customSliders64.setAllValues(1.0f);
        resized();
        };

    customWaveResetToM1Btn.setup({ .parent = *this, .title = GuiText::Wt::Custom::toM1, .bgColor = GuiColor::WaveformContainer::ResetBtn::ToM1, .isReset = false, .isResized = false });
    customWaveResetToM1Btn.onClick = [this] {
        customSliders32.setAllValues(-1.0f);
        customSliders64.setAllValues(-1.0f);
        resized();
        };
}

void GuiWt::layout(juce::Rectangle<int> content)
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
    layoutMain({ .mainRect = mRect, .label = &mainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &waveSelector.label, .component = &waveSelector });
    layoutMain({ .mainRect = mRect, .label = &sizeSelector.label, .component = &sizeSelector, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &modCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .component = &modEnableButton });
    layoutMain({ .mainRect = mRect, .label = &modDepthSlider.label, .component = &modDepthSlider });
    layoutMain({ .mainRect = mRect, .label = &modSpeedSlider.label, .component = &modSpeedSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &adsrCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &attackSlider.label, .component = &attackSlider });
    layoutMain({ .mainRect = mRect, .label = &decaySlider.label, .component = &decaySlider });
    layoutMain({ .mainRect = mRect, .label = &sustainSlider.label, .component = &sustainSlider });
    layoutMain({ .mainRect = mRect, .label = &releaseSlider.label, .component = &releaseSlider, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &monoPolyCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .component = &monoModeToggle, .paddingBottom = 0 });

    bool isMod = modEnableButton.getToggleState();
    modDepthSlider.setEnabled(isMod);
    modDepthSlider.label.setEnabled(isMod);
    modSpeedSlider.setEnabled(isMod);
    modSpeedSlider.label.setEnabled(isMod);

    // 波形がカスタム以外の時は波形精度選択を Disabled に
    bool isCustomWave = (waveSelector.getSelectedId() == 9);

    sizeSelector.setEnabled(isCustomWave);
    sizeSelector.label.setEnabled(isCustomWave);
    customSliders32.setEnabled(isCustomWave);
    customSliders64.setEnabled(isCustomWave);
    customWaveResetTo0Btn.setEnabled(isCustomWave);
    customWaveResetTo1Btn.setEnabled(isCustomWave);
    customWaveResetToM1Btn.setEnabled(isCustomWave);

    // Custom Wave
    // Custom Mode Layout
    // 中央に32/64本のスライダーを配置
    // 波形選択が Custom 以外の時は Disabled 表示
    auto rightArea = pageArea.removeFromLeft(GuiValue::Wt::RightWidth).removeFromTop(GuiValue::Wt::RightHeight);

    customWaveGroup.setBounds(rightArea);

    auto cwRect = customWaveGroup.getBounds().reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);
    cwRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    auto containerArea = cwRect.removeFromTop(GuiValue::Wt::Custom::Slider::Height);

    customSliders32.setBounds(containerArea);
    customSliders64.setBounds(containerArea);

    // Check if Custom Mode is selected (Index 8 -> ID 9 based on addItem?)
    int waveSize = sizeSelector.getSelectedId() == 2 ? 64 : 32;

    customWaveGroup.setEnabled(isCustomWave);

    customSliders32.setVisible(false);
    customSliders32.setCustomEnabled(false);
    customSliders64.setVisible(false);
    customSliders64.setCustomEnabled(false);

    if (isCustomWave) {
        if (waveSize == 32) {
            customSliders32.setVisible(true);
            customSliders32.setCustomEnabled(true);
        }
        else {
            customSliders64.setVisible(true);
            customSliders64.setCustomEnabled(true);
        }
    }
    else {
        if (waveSize == 32) {
            customSliders32.setVisible(true);
        }
        else {
            customSliders64.setVisible(true);
        }
    }

    cwRect.removeFromTop(GuiValue::Wt::Custom::ResetBtn::Padding::Top);
    layoutRowWtWaveValueUpdate({ .rect = cwRect, .resetTo0Btn = &customWaveResetTo0Btn, .resetTo1Btn = &customWaveResetTo1Btn, .resetToM1Btn = &customWaveResetToM1Btn });
}

void GuiWt::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}
