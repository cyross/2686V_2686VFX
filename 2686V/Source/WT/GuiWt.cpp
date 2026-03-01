#include "GuiWt.h"
#include "../processor/PluginProcessor.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"

void GuiWt::setup()
{
    const juce::String code = PrKey::Prefix::wt;

    mainGroup.setup(*this, GuiText::Group::mainGroup);

    bitSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::bit, .title = GuiText::Group::Fm::bit, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::rate, .title = GuiText::Group::Fm::alg, .items = rateItems, .isReset = true });

    levelSlider.setup({ .parent = *this, .id = code + PrKey::Post::Wt::level, .title = "Level", .isReset = true });

    attackSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::ar, .title = GuiText::Group::Ssg::Post::Adsr::ar, .isReset = true });
    decaySlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::dr, .title = GuiText::Group::Ssg::Post::Adsr::dr, .isReset = true });
    sustainSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::sl, .title = GuiText::Group::Ssg::Post::Adsr::sl, .isReset = true });
    releaseSlider.setup({ .parent = *this, .id = code + PrKey::Post::Adsr::rr, .title = GuiText::Group::Ssg::Post::Adsr::rr, .isReset = true });

    // Waveform
	waveSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::wave, .title = "Form", .items = wtWsItems, .isReset = true, .isResized = true });

    // Custom Wave Size
    sizeSelector.setup({ .parent = *this, .id = code + PrKey::Post::Wt::sampleSize, .title = "Size", .items = wtTsItems, .isReset = true, .isResized = true });

    // Modulation
    modEnableButton.setup({ .parent = *this, .id = code + PrKey::Post::Wt::Mod::enable, .title = "Mod", .isReset = true, .isResized = true });
	modDepthSlider.setup({ .parent = *this, .id = code + PrKey::Post::Wt::Mod::depth, .title = "Depth", .isReset = true });
	modSpeedSlider.setup({ .parent = *this, .id = code + PrKey::Post::Wt::Mod::speed, .title = "Speed", .isReset = true });

    // Master Volume
	masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    // Custom Wave Group
	customWaveGroup.setup(*this, "Custom Wave");

    // Custom Wave Sliders
	customSliders32.setup({ .parent = *this, .idPrefix = code + PrKey::Innder::custom32 });
    customSliders64.setup({ .parent = *this, .idPrefix = code + PrKey::Innder::custom64 });

	customWaveResetTo0Btn.setup({ .parent = *this, .title = "-> 0.0", .bgColor = GuiColor::WaveformContainer::ResetBtn::To0, .isReset = false, .isResized = false });
    customWaveResetTo0Btn.onClick = [this] {
        customSliders32.setAllValues(0.0f);
        customSliders64.setAllValues(0.0f);
        resized();
    };

    customWaveResetTo1Btn.setup({ .parent = *this, .title = "-> 1.0", .bgColor = GuiColor::WaveformContainer::ResetBtn::To1, .isReset = false, .isResized = false });
    customWaveResetTo1Btn.onClick = [this] {
        customSliders32.setAllValues(1.0f);
        customSliders64.setAllValues(1.0f);
        resized();
        };

    customWaveResetToM1Btn.setup({ .parent = *this, .title = "-> -1.0", .bgColor = GuiColor::WaveformContainer::ResetBtn::ToM1, .isReset = false, .isResized = false });
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

    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &bitSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &bitSelector, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &rateSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &rateSelector, { GuiValue::MainGroup::Value::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &levelSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right}}, { &levelSlider, { GuiValue::MainGroup::Value::width, 0}} });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &attackSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right}}, { &attackSlider, { GuiValue::MainGroup::Value::width, 0}} });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &decaySlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right}}, { &decaySlider, { GuiValue::MainGroup::Value::width, 0}} });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &sustainSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right}}, { &sustainSlider, { GuiValue::MainGroup::Value::width, 0}} });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &releaseSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right}}, { &releaseSlider, { GuiValue::MainGroup::Value::width, 0}} });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &waveSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right}}, { &waveSelector, { GuiValue::MainGroup::Value::width, 0}} });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &sizeSelector.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right}}, { &sizeSelector, { GuiValue::MainGroup::Value::width, 0}} });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &modEnableButton, { GuiValue::MainGroup::Button::width, 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &modDepthSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right}}, { &modDepthSlider, { GuiValue::MainGroup::Value::width, 0}} });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &modSpeedSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right}}, { &modSpeedSlider, { GuiValue::MainGroup::Value::width, 0}} });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::MainVol::height, 0, { { &masterVolSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} }, { &masterVolSlider, { GuiValue::MainGroup::Value::width, 0} } });

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
    layoutComponentsLtoR(cwRect, GuiValue::Wt::Custom::ResetBtn::Height, 0, {
        { &customWaveResetTo0Btn, { GuiValue::Wt::Custom::ResetBtn::To0Width, GuiValue::Wt::Custom::ResetBtn::Padding::Right} },
        { &customWaveResetTo1Btn, { GuiValue::Wt::Custom::ResetBtn::To1Width, GuiValue::Wt::Custom::ResetBtn::Padding::Right} },
        { &customWaveResetToM1Btn, { GuiValue::Wt::Custom::ResetBtn::ToM1Width, 0} }
    });
}
