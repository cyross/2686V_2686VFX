#include "GuiFx.h"

#include "../gui/GuiConstants.h"
#include "../gui/LabelConstants.h"
#include "../fm/OpConstants.h"

void GuiFx::setup()
{
    const juce::String code = codeFx;

    std::vector<SelectItem> qualityItems = {
        {.name = "0: Raw (32bit)", .value = 1 },
        {.name = "1: 24-bit PCM",  .value = 2 },
        {.name = "2: 16-bit PCM",  .value = 3 },
        {.name = "3: 8-bit PCM",   .value = 4 },
        {.name = "4: 5-bit PCM",   .value = 5 },
        {.name = "5: 4-bit PCM",   .value = 6 },
        {.name = "6: 4-bit ADPCM", .value = 7 },
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

    // MainGroup
    mainGroup.setup(*this, mGroupTitle);

	bypassToggle.setup({ .parent = *this, .id = code + postBypass, .title = masterBypassLabel });

	masterVolSlider.setup({ .parent = *this, .id = codeMasterVol, .title = masterVolumeLabel, .isReset = false });

    // Tremolo Group
	tremGroup.setup(*this, "Tremolo");
    tBypassBtn.setup({ .parent = *this, .id = code + codeFxTrm + postBypass, .title = "Bypass" });
	tRateSlider.setup({ .parent = *this, .id = code + codeFxTrm + postRate, .title = "Rate" });
    tDepthSlider.setup({ .parent = *this, .id = code + codeFxTrm + postDepth, .title = "Depth" });
    tMixSlider.setup({ .parent = *this, .id = code + codeFxTrm + postMix, .title = "Mix" });
	tDryBtn.setup({ .parent = *this, .title = "Dry" });
    tDryBtn.onClick = [&] { tMixSlider.setValue(0.0f); };
    tHalfBtn.setup({ .parent = *this, .title = "50%" });
    tHalfBtn.onClick = [&] { tMixSlider.setValue(0.5f); };
    tWetBtn.setup({ .parent = *this, .title = "Wet" });
    tWetBtn.onClick = [&] { tMixSlider.setValue(1.0f); };

    // Vibrato Group
	vibGroup.setup(*this, "Vibrato");
    vBypassBtn.setup({ .parent = *this, .id = code + codeFxVib + postBypass, .title = "Bypass" });
    vRateSlider.setup({ .parent = *this, .id = code + codeFxVib + postRate, .title = "Rate" });
    vDepthSlider.setup({ .parent = *this, .id = code + codeFxVib + postDepth, .title = "Depth" });
    vMixSlider.setup({ .parent = *this, .id = code + codeFxVib + postMix, .title = "Mix" });
    vDryBtn.setup({ .parent = *this, .title = "Dry" });
    vDryBtn.onClick = [&] { vMixSlider.setValue(0.0f); };
    vHalfBtn.setup({ .parent = *this, .title = "50%" });
    vHalfBtn.onClick = [&] { vMixSlider.setValue(0.5f); };
    vWetBtn.setup({ .parent = *this, .title = "Wet" });
    vWetBtn.onClick = [&] { vMixSlider.setValue(1.0f); };

    // Modern Bit Crusher Group
	mbcGroup.setup(*this, "Modern Bit Crusher");
    mbcBypassBtn.setup({ .parent = *this, .id = code + codeFxMbc + postBypass, .title = "Bypass" });
    mbcRateSlider.setup({ .parent = *this, .id = code + codeFxMbc + postRate, .title = "Rate" });
    mbcBitsSlider.setup({ .parent = *this, .id = code + codeFxMbc + postDepth, .title = "Depth" });
    mbcMixSlider.setup({ .parent = *this, .id = code + codeFxMbc + postMix, .title = "Mix" });
    mbcDryBtn.setup({ .parent = *this, .title = "Dry" });
    mbcDryBtn.onClick = [&] { mbcMixSlider.setValue(0.0f); };
    mbcHalfBtn.setup({ .parent = *this, .title = "50%" });
    mbcHalfBtn.onClick = [&] { mbcMixSlider.setValue(0.5f); };
    mbcWetBtn.setup({ .parent = *this, .title = "Wet" });
    mbcWetBtn.onClick = [&] { mbcMixSlider.setValue(1.0f); };

    // Delay Group
	delayGroup.setup(*this, "Delay");
    dBypassBtn.setup({ .parent = *this, .id = code + codeFxDly + postBypass, .title = "Bypass" });
    dTimeSlider.setup({ .parent = *this, .id = code + codeFxDly + postFxTime, .title = "Time (ms)" });
    dFbSlider.setup({ .parent = *this, .id = code + codeFxDly + postFxFb, .title = "Feedback" });
    dMixSlider.setup({ .parent = *this, .id = code + codeFxDly + postMix, .title = "Mix" });
    dDryBtn.setup({ .parent = *this, .title = "Dry" });
    dDryBtn.onClick = [&] { dMixSlider.setValue(0.0f); };
    dHalfBtn.setup({ .parent = *this, .title = "50%" });
    dHalfBtn.onClick = [&] { dMixSlider.setValue(0.5f); };
    dWetBtn.setup({ .parent = *this, .title = "Wet" });
    dWetBtn.onClick = [&] { dMixSlider.setValue(1.0f); };

    // Reverb Group
	reverbGroup.setup(*this, "Reverb");
    rBypassBtn.setup({ .parent = *this, .id = code + codeFxRvb + postBypass, .title = "Bypass" });
    rSizeSlider.setup({ .parent = *this, .id = code + codeFxRvb + postFxSize, .title = "Size" });
    rDampSlider.setup({ .parent = *this, .id = code + codeFxRvb + postFxDamp, .title = "Damp" });
    rMixSlider.setup({ .parent = *this, .id = code + codeFxRvb + postMix, .title = "Mix" });
    rDryBtn.setup({ .parent = *this, .title = "Dry" });
    rDryBtn.onClick = [&] { rMixSlider.setValue(0.0f); };
    rHalfBtn.setup({ .parent = *this, .title = "50%" });
    rHalfBtn.onClick = [&] { rMixSlider.setValue(0.5f); };
    rWetBtn.setup({ .parent = *this, .title = "Wet" });
    rWetBtn.onClick = [&] { rMixSlider.setValue(1.0f); };

    // Retro Bit Crusher Group
	rbcGroup.setup(*this, "Retro Bit Crusher");
	rbcBypassBtn.setup({ .parent = *this, .id = code + codeFxRbc + postBypass, .title = "Bypass" });
    rbcRateSelector.setup({ .parent = *this, .id = code + codeFxRbc + postRate, .title = "Rate", .items = rateItems });
    rbcBitsSelector.setup({ .parent = *this, .id = code + codeFxRbc + postFxBit, .title = "Quality", .items = qualityItems });
    tMixSlider.setup({ .parent = *this, .id = code + codeFxRbc + postMix, .title = "Mix" });
    rbcDryBtn.setup({ .parent = *this, .title = "Dry" });
    rbcDryBtn.onClick = [&] { rbcMixSlider.setValue(0.0f); };
    rbcHalfBtn.setup({ .parent = *this, .title = "50%" });
    rbcHalfBtn.onClick = [&] { rbcMixSlider.setValue(0.5f); };
    rbcWetBtn.setup({ .parent = *this, .title = "Wet" });
    rbcWetBtn.onClick = [&] { rbcMixSlider.setValue(1.0f); };
}

void GuiFx::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(MainWidth);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(GroupPaddingWidth, GroupPaddingHeight);
    mRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mRect, MainRowHeight, MainRowPaddingBottom, { { &bypassToggle, { MainRegButtonWidth, 0} } });
    layoutComponentsLtoR(mRect, MainVolHeight, MainLastRowPaddingBottom, { { &masterVolSlider.label, { MainRegLabelWidth, MainRegPaddingRight} }, { &masterVolSlider, { MainRegValueWidth, 0} } });

    auto topCol = pageArea.removeFromTop(FxAreaHeight);
    auto bottomCol = pageArea.removeFromTop(FxAreaHeight);

    // Left

    // 1. Tremolo
    auto trmArea = topCol.removeFromLeft(FmOpWidth);
    tremGroup.setBounds(trmArea);

    auto trmRect = trmArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    trmRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(trmRect, OpRowHeight, OpLastRowPaddingBottom, { { &tBypassBtn, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(trmRect, OpRowHeight, OpRowPaddingBottom, { { &tRateSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &tRateSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(trmRect, OpRowHeight, OpRowPaddingBottom, { { &tDepthSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &tDepthSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(trmRect, OpRowHeight, OpRowPaddingBottom, { { &tMixSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &tMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(trmRect, OpRowHeight, OpRowPaddingBottom, {
        { &tDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &tHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &tWetBtn, { OpRegPanChangeButtonWidth, 0} }
        });

    // 2. Vibrato
    auto vibArea = topCol.removeFromLeft(FmOpWidth);
    vibGroup.setBounds(vibArea);

    auto vibRect = vibArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    vibRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(vibRect, OpRowHeight, OpLastRowPaddingBottom, { { &vBypassBtn, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(vibRect, OpRowHeight, OpRowPaddingBottom, { { &vRateSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &vRateSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(vibRect, OpRowHeight, OpRowPaddingBottom, { { &vDepthSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &vDepthSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(vibRect, OpRowHeight, OpRowPaddingBottom, { { &vMixSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &vMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(vibRect, OpRowHeight, OpRowPaddingBottom, {
        { &vDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &vHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &vWetBtn, { OpRegPanChangeButtonWidth, 0} }
        });

    // 3. Modern Bit Crusher
    auto mbcArea = topCol.removeFromLeft(FmOpWidth);
    mbcGroup.setBounds(mbcArea);

    auto mbcRect = mbcArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    mbcRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(mbcRect, OpRowHeight, OpLastRowPaddingBottom, { { &mbcBypassBtn, { OpRegButtonWidth, OpRegPaddingRight} } });
    layoutComponentsLtoR(mbcRect, OpRowHeight, OpRowPaddingBottom, { { &mbcRateSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &mbcRateSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(mbcRect, OpRowHeight, OpRowPaddingBottom, { { &mbcBitsSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &mbcBitsSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(mbcRect, OpRowHeight, OpRowPaddingBottom, { { &mbcMixSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &mbcMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(mbcRect, OpRowHeight, OpRowPaddingBottom, {
        { &mbcDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &mbcHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &mbcWetBtn, { OpRegPanChangeButtonWidth, 0} }
        });

    // 4. Delay
    auto dlyArea = bottomCol.removeFromLeft(FmOpWidth);
    delayGroup.setBounds(dlyArea);

    auto dlyRect = dlyArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    dlyRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(dlyRect, OpRowHeight, OpLastRowPaddingBottom, { { &dBypassBtn, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(dlyRect, OpRowHeight, OpRowPaddingBottom, { { &dTimeSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dTimeSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(dlyRect, OpRowHeight, OpRowPaddingBottom, { { &dFbSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dFbSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(dlyRect, OpRowHeight, OpRowPaddingBottom, { { &dMixSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &dMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(dlyRect, OpRowHeight, OpRowPaddingBottom, {
        { &dDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &dHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &dWetBtn, { OpRegPanChangeButtonWidth, 0} }
        });

    // 5. Reverb
    auto rvbArea = bottomCol.removeFromLeft(FmOpWidth);
    reverbGroup.setBounds(rvbArea);

    auto rvbRect = rvbArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    rvbRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(rvbRect, OpRowHeight, OpLastRowPaddingBottom, { { &rBypassBtn, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(rvbRect, OpRowHeight, OpRowPaddingBottom, { { &rSizeSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &rSizeSlider, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(rvbRect, OpRowHeight, OpRowPaddingBottom, { { &rDampSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &rDampSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(rvbRect, OpRowHeight, OpRowPaddingBottom, { { &rMixSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &rMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(rvbRect, OpRowHeight, OpRowPaddingBottom, {
        { &rDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &rHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &rWetBtn, { OpRegPanChangeButtonWidth, 0} }
        });

    // 6. Retro Bit Crusher
    auto rbcArea = bottomCol.removeFromLeft(FmOpWidth);
    rbcGroup.setBounds(rbcArea);

    auto rbcRect = rbcArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    rbcRect.removeFromTop(TitlePaddingTop);

    layoutComponentsLtoR(rbcRect, OpRowHeight, OpLastRowPaddingBottom, { { &rbcBypassBtn, { OpRegButtonWidth, 0} } });
    layoutComponentsLtoR(rbcRect, OpRowHeight, OpRowPaddingBottom, { { &rbcBitsSelector.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &rbcBitsSelector, { OpRegValueWidth, 0} } });
    layoutComponentsLtoR(rbcRect, OpRowHeight, OpRowPaddingBottom, { { &rbcRateSelector.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &rbcRateSelector, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(rbcRect, OpRowHeight, OpRowPaddingBottom, { { &rbcMixSlider.label, { OpRegLabelWidth, OpRegPaddingRight} }, { &rbcMixSlider, { OpRegValueWidth, 0}} });
    layoutComponentsLtoR(rbcRect, OpRowHeight, OpRowPaddingBottom, {
        { &rbcDryBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &rbcHalfBtn, { OpRegPanChangeButtonWidth, OpRegPanPaddingHeight} },
        { &rbcWetBtn, { OpRegPanChangeButtonWidth, 0} }
        });
}
