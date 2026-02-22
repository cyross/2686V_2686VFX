#include "GuiWt.h"
#include "../processor/PluginProcessor.h"
#include "../gui/GuiConstants.h"
#include "../gui/LabelConstants.h"
#include "../fm/OpConstants.h"
#include "../fm/OpValueRange.h"

void GuiWt::setup()
{
    const juce::String code = codeWt;

    std::vector<SelectItem> wsItems = {
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

    std::vector<SelectItem> tsItems = {
        {.name = "0: 32 Samples",  .value = 1 },
        {.name = "1: 64 Samples",  .value = 2 },
    };

    std::vector<SelectItem> bdItems = {
        {.name = "0: 4-bit (16 steps)",  .value = 1 },
        {.name = "1: 5-bit (32 steps)",  .value = 2 },
        {.name = "2: 6-bit (64 steps)",  .value = 3 },
        {.name = "3: 8-bit (256 steps)", .value = 4 },
        {.name = "4: Raw",               .value = 5 },
    };

    std::vector<SelectItem> rateItems = {
        {.name = "0: 96kHz",    .value = 1 },
        {.name = "1: 55.5kHz",  .value = 2 },
        {.name = "2: 48kHz",    .value = 3 },
        {.name = "3: 44.1kHz",  .value = 4 },
        {.name = "4: 22.05kHz", .value = 5 },
        {.name = "5: 16kHz",    .value = 6 },
        {.name = "6: 8kHz",     .value = 7 },
    };

    mainGroup.setup(*this, mGroupTitle);

    bitSelector.setup({ .parent = *this, .id = code + postBit, .title = mBitTitle, .items = bdItems });
    rateSelector.setup({ .parent = *this, .id = code + postRate, .title = mRateTitle, .items = rateItems });

    levelSlider.setup({ .parent = *this, .id = code + postLevel, .title = "Level"});

    attackSlider.setup({ .parent = *this, .id = code + postAr, .title = mArLabel });
    decaySlider.setup({ .parent = *this, .id = code + postDr, .title = mDrLabel });
    sustainSlider.setup({ .parent = *this, .id = code + postSl, .title = mSlLabel });
    releaseSlider.setup({ .parent = *this, .id = code + postRr, .title = mRrLabel });

    // Waveform
	waveSelector.setup({ .parent = *this, .id = code + postWave, .title = "Form", .items = wsItems, .isReset = true, .isResized = true });

    // Modulation
    modEnableButton.setup({ .parent = *this, .id = code + postModEnable, .title = "Mod", .isReset = true, .isResized = true });
	modDepthSlider.setup({ .parent = *this, .id = code + postModDepth, .title = "Depth" });
	modSpeedSlider.setup({ .parent = *this, .id = code + postModSpeed, .title = "Speed" });

    // Master Volume
	masterVolSlider.setup({ .parent = *this, .id = codeMasterVol, .title = "Master Volume" });

    // Custom Wave Group
	customWaveGroup.setup(*this, "Custom Wave");

    // Custom Wave Size
	sizeSelector.setup({ .parent = *this, .id = code + postSize, .title = "Wave Size", .items = tsItems, .isReset = true, .isResized = true });

    // Custom Wave Sliders
	customSliders32.setup({ .parent = *this, .idPrefix = code + codeCustom32 });
    customSliders64.setup({ .parent = *this, .idPrefix = code + codeCustom64 });

	customWaveResetBtn.setup({ .parent = *this, .title = "Reset", .isReset = false, .isResized = false });
    customWaveResetBtn.onClick = [this] {
        customSliders32.resetValues();
        customSliders64.resetValues();
        resized();
    };
}

void GuiWt::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &bitSelector.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &bitSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &rateSelector.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &rateSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &levelSlider.label, { MainRegLabelWidth, MainRegPaddingRight}}, { &levelSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &attackSlider.label, { MainRegLabelWidth, MainRegPaddingRight}}, { &attackSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &decaySlider.label, { MainRegLabelWidth, MainRegPaddingRight}}, { &decaySlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &sustainSlider.label, { MainRegLabelWidth, MainRegPaddingRight}}, { &sustainSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &releaseSlider.label, { MainRegLabelWidth, MainRegPaddingRight}}, { &releaseSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &waveSelector.label, { MainRegLabelWidth, MainRegPaddingRight}}, { &waveSelector, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &sizeSelector.label, { MainRegLabelWidth, MainRegPaddingRight}}, { &sizeSelector, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &modEnableButton, { MainRegButtonWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &modDepthSlider.label, { MainRegLabelWidth, MainRegPaddingRight}}, { &modDepthSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &modSpeedSlider.label, { MainRegLabelWidth, MainRegPaddingRight}}, { &modSpeedSlider, { MainRegValueWidth, 0}} });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &masterVolSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &masterVolSlider, { MainRegValueWidth, 0} } });

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
    customWaveResetBtn.setEnabled(isCustomWave);

    // Custom Wave
    // Custom Mode Layout
    // 中央に32/64本のスライダーを配置
    // 波形選択が Custom 以外の時は Disabled 表示
    auto rightArea = pageArea.removeFromLeft(WtRightWidth).removeFromTop(WtRightHeight);

    customWaveGroup.setBounds(rightArea);

    auto cwRect = customWaveGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);
    cwRect.removeFromTop(TitlePaddingTop);

    auto containerArea = cwRect.removeFromTop(WtCustomSliderHeight);

    customSliders32.setBounds(containerArea);
    customSliders64.setBounds(containerArea);

    // Check if Custom Mode is selected (Index 8 -> ID 9 based on addItem?)
    int waveSize = sizeSelector.getSelectedId() == 2 ? 64 : 32;

    customWaveGroup.setEnabled(isCustomWave);

    if (isCustomWave) {
        if (waveSize == 32) {
            customSliders32.setVisible(true);
            customSliders32.setCustomEnabled(true);
            customSliders64.setVisible(false);
            customSliders64.setCustomEnabled(false);
        }
        else {
            customSliders32.setVisible(false);
            customSliders32.setCustomEnabled(false);
            customSliders64.setVisible(true);
            customSliders64.setCustomEnabled(true);
        }
    }
    else {
        customSliders32.setVisible(false);
        customSliders64.setVisible(false);
    }

    cwRect.removeFromTop(WtCustomSliderResetBtnPaddingTop);
    layoutComponentsLtoR(cwRect, WtCustomSliderResetBtnHeight, MainLastRowPaddingBottom, { { &customWaveResetBtn, { MainRegButtonWidth, 0} } });
}
