#include "GuiSsg.h"
#include "../processor/PluginProcessor.h"
#include "../gui/GuiConstants.h"
#include "../gui/LabelConstants.h"
#include "../fm/OpConstants.h"
#include "../fm/OpValueRange.h"

void GuiSsg::setup()
{
    const juce::String code = codeSsg;

    std::vector<SelectItem> wsItems = {
        {.name = "0: Pulse(Rect)", .value = 1 },
        {.name = "1: Triangle / Saw", .value = 2 },
    };

    std::vector<SelectItem> dmItems = {
        {.name = "0: Preset Ratio", .value = 1 },
        {.name = "1: Variable (Slider)", .value = 2 },
    };

    std::vector<SelectItem> prItems = {
        {.name = "0: 1:1 (50%)", .value = 1 },
        {.name = "1: 3:5 (37.5%)", .value = 2 },
        {.name = "2: 5:11 (31.25%)", .value = 3 },
        {.name = "3: 1:3 (25%)", .value = 4 },
        {.name = "4: 1:4 (20%)", .value = 5 },
        {.name = "5: 3:13 (18.75%)", .value = 6 },
        {.name = "6: 1:7 (12.5%)", .value = 7 },
        {.name = "7: 1:15 (6.25%)", .value = 8 },
    };

    std::vector<SelectItem> envItems = {
        {.name = "0: Saw Down", .value = 1 },
        {.name = "1: Saw Down & Hold", .value = 2 },
        {.name = "2: Triangle", .value = 3 },
        {.name = "3: Alternative Saw Down & Hold", .value = 4 },
        {.name = "4: Saw Up", .value = 5 },
        {.name = "5: Saw Up & Hold", .value = 6 },
        {.name = "6: Triangle Invert", .value = 7 },
        {.name = "7: Alternative Saw Up & Hold", .value = 8 },
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
    bitSelector.setup({ .parent = *this, .id = code + postBit, .title = mBitTitle, .items = bdItems, .isReset = true });
    rateSelector.setup({ .parent = *this, .id = code + postRate, .title = mRateTitle, .items = rateItems, .isReset = true });

	voiceGroup.setup(*this, "Voice");
	waveSelector.setup({ .parent = *this, .id = code + postWaveform, .title = "Form", .items = wsItems, .isReset = true, .isResized = true });
	levelSlider.setup({ .parent = *this, .id = code + postLevel, .title = "Tone", .isReset = true, .regType = RegisterType::SsgVol });
    noiseSlider.setup({ .parent = *this, .id = code + postNoise, .title = "Noise", .isReset = true, .regType = RegisterType::SsgVol });
	noiseFreqSlider.setup({ .parent = *this, .id = code + postNoiseFreq, .title = "Freq", .isReset = true });
	noiseOnNoteButton.setup({ .parent = *this, .id = code + postNoiseOnNote, .title = "Noise On Note", .isReset = true });

    // 初期状態反映
	mixSlider.setup({ .parent = *this, .id = code + postMix, .title = "Mix", .isReset = true });
	mixSetTone.setup({ .parent = *this, .title = "Tone", .isReset = false, .isResized = false });
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };
    mixSetMix.setup({ .parent = *this, .title = "Mix", .isReset = false, .isResized = false });
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };
    mixSetNoise.setup({ .parent = *this, .title = "Noise", .isReset = false, .isResized = false });
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

	adsrBypassButton.setup({ .parent = *this, .id = code + codeAdsr + postBypass, .title = "Bypass ADSR", .isReset = true });

    attackSlider.setup({ .parent = *this, .id = code + postAr, .title = mArLabel, .isReset = true });
    decaySlider.setup({ .parent = *this, .id = code + postDr, .title = mDrLabel, .isReset = true });
    sustainSlider.setup({ .parent = *this, .id = code + postSl, .title = mSlLabel, .isReset = true });
    releaseSlider.setup({ .parent = *this, .id = code + postRr, .title = mRrLabel, .isReset = true });

    masterVolSlider.setup({ .parent = *this, .id = codeMasterVol, .title = masterVolumeLabel, .isReset = true });

    // Duty Controls Setup
	dutyGroup.setup(*this, "Pulse Width (Duty)");
	dutyModeSelector.setup({ .parent = *this, .id = code + postDutyMode, .title = "Mode", .items = dmItems, .isReset = true, .isResized = true });
	dutyPresetSelector.setup({ .parent = *this, .id = code + postDutyPreset, .title = "Preset", .items = prItems, .isReset = true, .isResized = true });
	dutyVarSlider.setup({ .parent = *this, .id = code + postDutyVar, .title = "Ratio", .isReset = true });
	dutyInvertButton.setup({ .parent = *this, .id = code + postDutyInv, .title = "Invert Phase", .isReset = true, .isResized = true });

	triGroup.setup(*this, "Triangle Property");
	triKeyTrackButton.setup({ .parent = *this, .id = code + postTriKeyTrk, .title = "Key Track (Pitch)", .isReset = true, .isResized = true });
	triFreqSlider.setup({ .parent = *this, .id = code + postTriFreq, .title = "Freq", .isReset = true });
	triPeakSlider.setup({ .parent = *this, .id = code + postTriPeak, .title = "Peak", .isReset = true });
	triSetSawDown.setup({ .parent = *this, .title = "0.0 (Down)", .isReset = false, .isResized = false });
    triSetSawDown.onClick = [this] { triPeakSlider.setValue(0.0, juce::sendNotification); };
	triSetTri.setup({ .parent = *this, .title = "0.5 (Tri)", .isReset = false, .isResized = false });
    triSetTri.onClick = [this] { triPeakSlider.setValue(0.5, juce::sendNotification); };
	triSetSawUp.setup({ .parent = *this, .title = "1.0 (Up)", .isReset = false, .isResized = false });
    triSetSawUp.onClick = [this] { triPeakSlider.setValue(1.0, juce::sendNotification); };

    // HW Env Group
	envGroup.setup(*this, "Hardware Envelope");
	envEnableButton.setup({ .parent = *this, .id = code + postEnvEnable, .title = "Enable HW Env", .isReset = true });
	shapeSelector.setup({ .parent = *this, .id = code + postEnvShape, .title = "Shape", .items = envItems, .isReset = true });
	periodSlider.setup({ .parent = *this, .id = code + postEnvPeriod, .title = "Speed", .isReset = true, .regType = RegisterType::SsgEnv });
}

void GuiSsg::layout(juce::Rectangle<int> content)
{
    juce::String code = codeSsg;
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &bitSelector.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &bitSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &rateSelector.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &rateSelector, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &adsrBypassButton, { MainRegButtonWidth, MainRegPaddingRight} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &attackSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &attackSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &decaySlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &decaySlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &sustainSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &sustainSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &releaseSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &releaseSlider, { MainRegValueWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &masterVolSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &masterVolSlider, { MainRegValueWidth, 0} } });

    // --- Voice Group ---
    auto voiceArea = pageArea.removeFromLeft(FmOpWidth);

    voiceGroup.setBounds(voiceArea);
    auto vRect = voiceGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

    vRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &levelSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &levelSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &noiseSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &noiseSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &noiseFreqSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &noiseFreqSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &noiseOnNoteButton, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, { { &mixSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &mixSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(vRect, OpRowHeight, OpRowPaddingBottom, {
        { &mixSetTone, { OpRegMixChangeButtonWidth, 0} },
        { &mixSetMix, { OpRegMixChangeButtonWidth, 0} },
        { &mixSetNoise, { OpRegMixChangeButtonWidth, 0} }
        });
    layoutComponentsLtoR(vRect, OpRowHeight, OpLastRowPaddingBottom, { { &waveSelector.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &waveSelector, { OpRegValueWidth, 0} } });

    // Wave Group
    float waveParam = *ctx.audioProcessor.apvts.getRawParameterValue(code + postWaveform);
    int waveMode = (waveParam > opBoolThread) ? 1 : 0;
    auto waveArea = pageArea.removeFromLeft(FmOpWidth);

    if (waveMode == 0) // Pulse
    {
        dutyGroup.setVisible(true);
        triGroup.setVisible(false);

        triKeyTrackButton.setVisible(false);
        triPeakSlider.setVisible(false);
        triPeakSlider.label.setVisible(false);
        triFreqSlider.setVisible(false);
        triFreqSlider.label.setVisible(false);
        triSetSawDown.setVisible(false);
        triSetTri.setVisible(false);
        triSetSawUp.setVisible(false);

        dutyModeSelector.setVisible(true);
        dutyModeSelector.label.setVisible(true);
        dutyVarSlider.setValue(true);
        dutyVarSlider.label.setVisible(true);
        dutyInvertButton.setVisible(true);

        dutyGroup.setBounds(waveArea);
        auto dRect = dutyGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

        dRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(dRect, OpRowHeight, OpRowPaddingBottom, { { &dutyModeSelector.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dutyModeSelector, { OpRegValueWidth, 0} } });
        layoutComponentsLtoR(dRect, OpRowHeight, OpRowPaddingBottom, { { &dutyInvertButton, { OpRegButtonWidth, 0} } });

        float dutyModeVal = *ctx.audioProcessor.apvts.getRawParameterValue(code + postDutyMode);
        if (dutyModeVal < 0.5f) {
            dutyPresetSelector.setVisible(true);
            dutyPresetSelector.label.setVisible(true);
            dutyVarSlider.setVisible(false);
            dutyVarSlider.label.setVisible(false);
            layoutComponentsLtoR(dRect, OpRowHeight, OpLastRowPaddingBottom, { { &dutyPresetSelector.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dutyPresetSelector, { OpRegValueWidth, 0} } });
        }
        else {
            dutyPresetSelector.setVisible(false);
            dutyPresetSelector.label.setVisible(false);
            dutyVarSlider.setVisible(true);
            dutyVarSlider.label.setVisible(true);
            layoutComponentsLtoR(dRect, OpRowHeight, OpLastRowPaddingBottom, { { &dutyVarSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dutyVarSlider, { OpRegValueWidth, 0} } });
        }
    }
    else // Triangle
    {
        dutyGroup.setVisible(false);
        triGroup.setVisible(true);

        dutyModeSelector.setVisible(false);
        dutyModeSelector.label.setVisible(false);
        dutyInvertButton.setVisible(false);
        dutyPresetSelector.setVisible(false);
        dutyPresetSelector.label.setVisible(false);
        dutyVarSlider.setVisible(false);
        dutyVarSlider.label.setVisible(false);

        triKeyTrackButton.setVisible(true);
        triFreqSlider.setVisible(true);
        triFreqSlider.label.setVisible(true);
        triPeakSlider.setVisible(true);
        triPeakSlider.label.setVisible(true);
        triSetSawDown.setVisible(true);
        triSetTri.setVisible(true);
        triSetSawUp.setVisible(true);

        triGroup.setBounds(waveArea);
        auto tRect = triGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

        tRect.removeFromTop(TitlePaddingTop);

        layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &triKeyTrackButton, { OpRegButtonWidth, 0} } });

        bool isKeyTrack = triKeyTrackButton.getToggleState();
        triFreqSlider.setVisible(!isKeyTrack);
        triFreqSlider.label.setVisible(!isKeyTrack);

        if (!isKeyTrack) {
            layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &triFreqSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &triFreqSlider, { OpRegValueWidth, 0} } });
            layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &triPeakSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &triPeakSlider, { OpRegValueWidth, 0} } });
        }
        else {
            layoutComponentsLtoR(tRect, OpRowHeight, OpRowPaddingBottom, { { &triPeakSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &triPeakSlider, { OpRegValueWidth, 0} } });
        }

        layoutComponentsLtoR(tRect, OpRowHeight, OpLastRowPaddingBottom, {
            { &triSetSawDown, { OpRegSsgTriPeakButtonWidth, 0} },
            { &triSetTri, { OpRegSsgTriPeakButtonWidth, 0} },
            { &triSetSawUp, { OpRegSsgTriPeakButtonWidth, 0} }
            });
    }

    // HW Env Group
    auto envArea = pageArea.removeFromLeft(FmOpWidth);
    envGroup.setBounds(envArea);
    auto eRect = envGroup.getBounds().reduced(GroupPaddingWidth, GroupPaddingHeight);

    eRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(eRect, OpRowHeight, OpRowPaddingBottom, { { &envEnableButton, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(eRect, OpRowHeight, OpRowPaddingBottom, { { &shapeSelector.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &shapeSelector, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(eRect, OpRowHeight, OpRowPaddingBottom, { { &periodSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &periodSlider, { OpRegValueWidth, 0} } });
}
