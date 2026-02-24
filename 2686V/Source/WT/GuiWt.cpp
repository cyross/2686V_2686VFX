#include "GuiWt.h"
#include "../processor/PluginProcessor.h"
#include "../gui/GuiConstants.h"
#include "../gui/LabelConstants.h"
#include "../fm/OpConstants.h"
#include "../fm/OpValueRange.h"

void GuiWt::setup()
{
    const juce::String code = codeWt;

    mainGroup.setup(*this, mGroupTitle);

    bitSelector.setup({ .parent = *this, .id = code + postBit, .title = mBitTitle, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + postRate, .title = mRateTitle, .items = rateItems, .isReset = true });

    levelSlider.setup({ .parent = *this, .id = code + postLevel, .title = "Level", .isReset = true });

    attackSlider.setup({ .parent = *this, .id = code + postAr, .title = mArLabel, .isReset = true });
    decaySlider.setup({ .parent = *this, .id = code + postDr, .title = mDrLabel, .isReset = true });
    sustainSlider.setup({ .parent = *this, .id = code + postSl, .title = mSlLabel, .isReset = true });
    releaseSlider.setup({ .parent = *this, .id = code + postRr, .title = mRrLabel, .isReset = true });

    // Waveform
	waveSelector.setup({ .parent = *this, .id = code + postWave, .title = "Form", .items = wtWsItems, .isReset = true, .isResized = true });

    // Custom Wave Size
    sizeSelector.setup({ .parent = *this, .id = code + postSize, .title = "Size", .items = wtTsItems, .isReset = true, .isResized = true });

    // Modulation
    modEnableButton.setup({ .parent = *this, .id = code + postModEnable, .title = "Mod", .isReset = true, .isResized = true });
	modDepthSlider.setup({ .parent = *this, .id = code + postModDepth, .title = "Depth", .isReset = true });
	modSpeedSlider.setup({ .parent = *this, .id = code + postModSpeed, .title = "Speed", .isReset = true });

    // Master Volume
	masterVolSlider.setup({ .parent = *this, .id = codeMasterVol, .title = masterVolumeLabel, .isReset = true });

    // Custom Wave Group
	customWaveGroup.setup(*this, "Custom Wave");

    // Custom Wave Sliders
	customSliders32.setup({ .parent = *this, .idPrefix = code + codeCustom32 });
    customSliders64.setup({ .parent = *this, .idPrefix = code + codeCustom64 });

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
    customWaveResetTo0Btn.setEnabled(isCustomWave);
    customWaveResetTo1Btn.setEnabled(isCustomWave);
    customWaveResetToM1Btn.setEnabled(isCustomWave);

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
        if (waveSize == 32) {
            customSliders32.setVisible(true);
            customSliders32.setCustomEnabled(false);
            customSliders64.setVisible(false);
            customSliders64.setCustomEnabled(false);
        }
        else {
            customSliders32.setVisible(false);
            customSliders32.setCustomEnabled(false);
            customSliders64.setVisible(true);
            customSliders64.setCustomEnabled(false);
        }
    }

    cwRect.removeFromTop(WtCustomSliderResetBtnPaddingTop);
    layoutComponentsLtoR(cwRect, WtCustomSliderResetBtnHeight, MainLastRowPaddingBottom, {
        { &customWaveResetTo0Btn, { WtCustomResetTo0ButtonWidth, WtCustomSliderResetBtnPaddingRight} },
        { &customWaveResetTo1Btn, { WtCustomResetTo1ButtonWidth, WtCustomSliderResetBtnPaddingRight} },
        { &customWaveResetToM1Btn, { WtCustomResetToM1ButtonWidth, 0} }
    });
}
