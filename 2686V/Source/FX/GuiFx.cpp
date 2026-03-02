#include "GuiFx.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"

#include "../gui/GuiHelpers.h"

void GuiFx::setup()
{
    const juce::String code = PrKey::Prefix::fx;

    // MainGroup
    mainGroup.setup(*this, GuiText::Group::mainGroup);

	bypassToggle.setup({ .parent = *this, .id = code + PrKey::Post::bypass, .title = GuiText::Group::Fx::masterBypass, .isReset = true });

	masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    // Retro LFO Group
    rlfoGroup.setup(*this, "Software LFO");
    const juce::String rlfoPrefix = code + PrKey::Innder::Fx::rlfo;
    rlfoBypassBtn.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::bypass, .title = "Bypass", .isReset = true });

    // Wave 選択用アイテム（Saw, Square, Tri, Noise）を直接生成してセット
    rlfoWaveSelector.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::wave, .title = "Wave", .items = rlfoWaves, .isReset = true });
    rlfoFreqSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::freq, .title = "Freq", .isReset = true });
    rlfoAmsSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::ams, .title = "AMS", .isReset = true });
    rlfoPmsSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::pms, .title = "PMS", .isReset = true });
    rlfoAmdSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::amd, .title = "AMD", .isReset = true });
    rlfoPmdSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::pmd, .title = "PMD", .isReset = true });
    rlfoMixSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::mix, .title = "Mix", .isReset = true });
    rlfoDryBtn.setup({ .parent = *this, .title = "Dry" }); rlfoDryBtn.onClick = [&] { rlfoMixSlider.setValue(0.0f); };
    rlfoHalfBtn.setup({ .parent = *this, .title = "50%" }); rlfoHalfBtn.onClick = [&] { rlfoMixSlider.setValue(0.5f); };
    rlfoWetBtn.setup({ .parent = *this, .title = "Wet" }); rlfoWetBtn.onClick = [&] { rlfoMixSlider.setValue(1.0f); };

    // Tremolo Group
	tremGroup.setup(*this, "Tremolo");
    const juce::String trmPrefix = code + PrKey::Innder::Fx::trm;
    tBypassBtn.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::bypass, .title = "Bypass", .isReset = true });
	tRateSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::Fx::Tremolo::rate, .title = "Rate", .isReset = true });
    tDepthSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::Fx::Tremolo::depth, .title = "Depth", .isReset = true });
    tMixSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::Fx::mix, .title = "Mix", .isReset = true });
	tDryBtn.setup({ .parent = *this, .title = "Dry" });
    tDryBtn.onClick = [&] { tMixSlider.setValue(0.0f); };
    tHalfBtn.setup({ .parent = *this, .title = "50%" });
    tHalfBtn.onClick = [&] { tMixSlider.setValue(0.5f); };
    tWetBtn.setup({ .parent = *this, .title = "Wet" });
    tWetBtn.onClick = [&] { tMixSlider.setValue(1.0f); };

    // Vibrato Group
	vibGroup.setup(*this, "Vibrato");
    const juce::String vibPrefix = code + PrKey::Innder::Fx::vib;
    vBypassBtn.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::bypass, .title = "Bypass", .isReset = true });
    vRateSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::Fx::Vibrato::rate, .title = "Rate", .isReset = true });
    vDepthSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::Fx::Vibrato::depth, .title = "Depth", .isReset = true });
    vMixSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::Fx::mix, .title = "Mix", .isReset = true });
    vDryBtn.setup({ .parent = *this, .title = "Dry" });
    vDryBtn.onClick = [&] { vMixSlider.setValue(0.0f); };
    vHalfBtn.setup({ .parent = *this, .title = "50%" });
    vHalfBtn.onClick = [&] { vMixSlider.setValue(0.5f); };
    vWetBtn.setup({ .parent = *this, .title = "Wet" });
    vWetBtn.onClick = [&] { vMixSlider.setValue(1.0f); };

    // Modern Bit Crusher Group
	mbcGroup.setup(*this, "Modern Bit Crusher");
    const juce::String mbcPrefix = code + PrKey::Innder::Fx::mbc;
    mbcBypassBtn.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::bypass, .title = "Bypass", .isReset = true });
    mbcRateSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::Fx::Mbc::rate, .title = "Rate", .isReset = true });
    mbcBitsSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::Fx::Mbc::bit, .title = "Depth", .isReset = true });
    mbcMixSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::Fx::mix, .title = "Mix", .isReset = true });
    mbcDryBtn.setup({ .parent = *this, .title = "Dry" });
    mbcDryBtn.onClick = [&] { mbcMixSlider.setValue(0.0f); };
    mbcHalfBtn.setup({ .parent = *this, .title = "50%" });
    mbcHalfBtn.onClick = [&] { mbcMixSlider.setValue(0.5f); };
    mbcWetBtn.setup({ .parent = *this, .title = "Wet" });
    mbcWetBtn.onClick = [&] { mbcMixSlider.setValue(1.0f); };

    // Delay Group
	delayGroup.setup(*this, "Delay");
    const juce::String dlyPrefix = code + PrKey::Innder::Fx::dly;
    dBypassBtn.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::bypass, .title = "Bypass", .isReset = true });
    dTimeSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::Fx::Delay::time, .title = "Time (ms)", .isReset = true });
    dFbSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::Fx::Delay::fb, .title = "Feedback", .isReset = true });
    dMixSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::Fx::mix, .title = "Mix", .isReset = true });
    dDryBtn.setup({ .parent = *this, .title = "Dry" });
    dDryBtn.onClick = [&] { dMixSlider.setValue(0.0f); };
    dHalfBtn.setup({ .parent = *this, .title = "50%" });
    dHalfBtn.onClick = [&] { dMixSlider.setValue(0.5f); };
    dWetBtn.setup({ .parent = *this, .title = "Wet" });
    dWetBtn.onClick = [&] { dMixSlider.setValue(1.0f); };

    // Reverb Group
	reverbGroup.setup(*this, "Reverb");
    const juce::String rvbPrefix = code + PrKey::Innder::Fx::rvb;
    rBypassBtn.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::bypass, .title = "Bypass", .isReset = true });
    rSizeSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::Fx::Reverb::size, .title = "Size", .isReset = true });
    rDampSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::Fx::Reverb::damp, .title = "Damp", .isReset = true });
    rMixSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::Fx::mix, .title = "Mix", .isReset = true });
    rDryBtn.setup({ .parent = *this, .title = "Dry" });
    rDryBtn.onClick = [&] { rMixSlider.setValue(0.0f); };
    rHalfBtn.setup({ .parent = *this, .title = "50%" });
    rHalfBtn.onClick = [&] { rMixSlider.setValue(0.5f); };
    rWetBtn.setup({ .parent = *this, .title = "Wet" });
    rWetBtn.onClick = [&] { rMixSlider.setValue(1.0f); };

    // Retro Bit Crusher Group
	rbcGroup.setup(*this, "Retro Bit Crusher");
    const juce::String rbcPrefix = code + PrKey::Innder::Fx::rbc;
    rbcBypassBtn.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::bypass, .title = "Bypass", .isReset = true });
    rbcRateSelector.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::Fx::Rbc::rate, .title = "Rate", .items = rateItems, .isReset = true });
    rbcBitsSelector.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::Fx::Rbc::bit, .title = "Quality", .items = qualityItems, .isReset = true });
    rbcMixSlider.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::Fx::mix, .title = "Mix", .isReset = true });
    rbcDryBtn.setup({ .parent = *this, .title = "Dry" });
    rbcDryBtn.onClick = [&] { rbcMixSlider.setValue(0.0f); };
    rbcHalfBtn.setup({ .parent = *this, .title = "50%" });
    rbcHalfBtn.onClick = [&] { rbcMixSlider.setValue(0.5f); };
    rbcWetBtn.setup({ .parent = *this, .title = "Wet" });
    rbcWetBtn.onClick = [&] { rbcMixSlider.setValue(1.0f); };

    // Filter Group
    filterGroup.setup(*this, "Filter");
    const juce::String filterPrefix = code + PrKey::Innder::Fx::fil;
    flBypassBtn.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::bypass, .title = "Bypass", .isReset = true });
    flTypeSelector.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::Filter::type, .title = "Type", .items = flTypeItems, .isReset = true });
    flFreqSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::Filter::freq, .title = "Cutoff", .isReset = true });
    flQSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::Filter::q, .title = "Reso/Q", .isReset = true });
    flMixSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::mix, .title = "Mix", .isReset = true });
    flDryBtn.setup({ .parent = *this, .title = "Dry" }); flDryBtn.onClick = [&] { flMixSlider.setValue(0.0f); };
    flHalfBtn.setup({ .parent = *this, .title = "50%" }); flHalfBtn.onClick = [&] { flMixSlider.setValue(0.5f); };
    flWetBtn.setup({ .parent = *this, .title = "Wet" }); flWetBtn.onClick = [&] { flMixSlider.setValue(1.0f); };

    // Soft Clipper Group
    softClipperGroup.setup(*this, "Soft Clipper");
    const juce::String sclPrefix = code + PrKey::Innder::Fx::scl;
    scBypassBtn.setup({ .parent = *this, .id = sclPrefix + PrKey::Post::bypass, .title = "Bypass", .isReset = true });
    scMixSlider.setup({ .parent = *this, .id = sclPrefix + PrKey::Post::Fx::mix, .title = "Mix", .isReset = true });
    scDryBtn.setup({ .parent = *this, .title = "Dry" }); scDryBtn.onClick = [&] { scMixSlider.setValue(0.0f); };
    scHalfBtn.setup({ .parent = *this, .title = "50%" }); scHalfBtn.onClick = [&] { scMixSlider.setValue(0.5f); };
    scWetBtn.setup({ .parent = *this, .title = "Wet" }); scWetBtn.onClick = [&] { scMixSlider.setValue(1.0f); };
}

void GuiFx::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &bypassToggle });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

    auto topCol = pageArea.removeFromTop(GuiValue::Fx::AreaHeightBig);
    pageArea.removeFromTop(GuiValue::PaddingBottom::block);
    auto centerCol = pageArea.removeFromTop(GuiValue::Fx::AreaHeight);
    pageArea.removeFromTop(GuiValue::PaddingBottom::block);
    auto bottomCol = pageArea.removeFromTop(GuiValue::Fx::AreaHeight);

    // 1st Row

    // Filter
    auto flArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    filterGroup.setBounds(flArea);

    auto flRect = flArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    flRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = flRect, .component = &flBypassBtn });
    layoutComponentsLtoRRow({ .rowRect = flRect, .label = &flTypeSelector.label, .component = &flTypeSelector });
    layoutComponentsLtoRRow({ .rowRect = flRect, .label = &flFreqSlider.label, .component = &flFreqSlider });
    layoutComponentsLtoRRow({ .rowRect = flRect, .label = &flQSlider.label, .component = &flQSlider });
    layoutComponentsLtoRRow({ .rowRect = flRect, .label = &flMixSlider.label, .component = &flMixSlider });
    layoutComponentsLtoRFxMixRow({ .rect = flRect, .dryBtn = &flDryBtn, .HalfBtn = &flHalfBtn, .wetBtn = &flWetBtn, .paddingBottom = 0});

    // Retro LFO
    auto rlfoArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    rlfoGroup.setBounds(rlfoArea);

    auto rlfoRect = rlfoArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rlfoRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .component = &rlfoBypassBtn });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoWaveSelector.label, .component = &rlfoWaveSelector });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoFreqSlider.label, .component = &rlfoFreqSlider });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoAmsSlider.label, .component = &rlfoAmsSlider });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoPmsSlider.label, .component = &rlfoPmsSlider });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoAmdSlider.label, .component = &rlfoAmdSlider });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoPmdSlider.label, .component = &rlfoPmdSlider });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoMixSlider.label, .component = &rlfoMixSlider });
    layoutComponentsLtoRFxMixRow({ .rect = rlfoRect, .dryBtn = &rlfoDryBtn, .HalfBtn = &rlfoHalfBtn, .wetBtn = &rlfoWetBtn, .paddingBottom = 0 });

    // Tremolo
    auto trmArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    tremGroup.setBounds(trmArea);

    auto trmRect = trmArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    trmRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = trmRect, .component = &tBypassBtn });
    layoutComponentsLtoRRow({ .rowRect = trmRect, .label = &tRateSlider.label, .component = &tRateSlider });
    layoutComponentsLtoRRow({ .rowRect = trmRect, .label = &tDepthSlider.label, .component = &tDepthSlider });
    layoutComponentsLtoRRow({ .rowRect = trmRect, .label = &tMixSlider.label, .component = &tMixSlider });
    layoutComponentsLtoRFxMixRow({ .rect = trmRect, .dryBtn = &tDryBtn, .HalfBtn = &tHalfBtn, .wetBtn = &tWetBtn, .paddingBottom = 0 });

    // Vibrato
    auto vibArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    vibGroup.setBounds(vibArea);

    auto vibRect = vibArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    vibRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = vibRect, .component = &vBypassBtn });
    layoutComponentsLtoRRow({ .rowRect = vibRect, .label = &vRateSlider.label, .component = &vRateSlider });
    layoutComponentsLtoRRow({ .rowRect = vibRect, .label = &vDepthSlider.label, .component = &vDepthSlider });
    layoutComponentsLtoRRow({ .rowRect = vibRect, .label = &vMixSlider.label, .component = &vMixSlider });
    layoutComponentsLtoRFxMixRow({ .rect = vibRect, .dryBtn = &vDryBtn, .HalfBtn = &vHalfBtn, .wetBtn = &vWetBtn, .paddingBottom = 0 });


    // 2nd Row

    // Modern Bit Crusher
    auto mbcArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    mbcGroup.setBounds(mbcArea);

    auto mbcRect = mbcArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mbcRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = mbcRect, .component = &mbcBypassBtn });
    layoutComponentsLtoRRow({ .rowRect = mbcRect, .label = &mbcRateSlider.label, .component = &mbcRateSlider });
    layoutComponentsLtoRRow({ .rowRect = mbcRect, .label = &mbcBitsSlider.label, .component = &mbcBitsSlider });
    layoutComponentsLtoRRow({ .rowRect = mbcRect, .label = &mbcMixSlider.label, .component = &mbcMixSlider });
    layoutComponentsLtoRFxMixRow({ .rect = mbcRect, .dryBtn = &mbcDryBtn, .HalfBtn = &mbcHalfBtn, .wetBtn = &mbcWetBtn, .paddingBottom = 0 });

    // Delay
    auto dlyArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    delayGroup.setBounds(dlyArea);

    auto dlyRect = dlyArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    dlyRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = dlyRect, .component = &dBypassBtn });
    layoutComponentsLtoRRow({ .rowRect = dlyRect, .label = &dTimeSlider.label, .component = &dTimeSlider });
    layoutComponentsLtoRRow({ .rowRect = dlyRect, .label = &dFbSlider.label, .component = &dFbSlider });
    layoutComponentsLtoRRow({ .rowRect = dlyRect, .label = &dMixSlider.label, .component = &dMixSlider });
    layoutComponentsLtoRFxMixRow({ .rect = dlyRect, .dryBtn = &dDryBtn, .HalfBtn = &dHalfBtn, .wetBtn = &dWetBtn, .paddingBottom = 0 });

    // Reverb
    auto rvbArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    reverbGroup.setBounds(rvbArea);

    auto rvbRect = rvbArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rvbRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = rvbRect, .component = &rBypassBtn });
    layoutComponentsLtoRRow({ .rowRect = rvbRect, .label = &rSizeSlider.label, .component = &rSizeSlider });
    layoutComponentsLtoRRow({ .rowRect = rvbRect, .label = &rDampSlider.label, .component = &rDampSlider });
    layoutComponentsLtoRRow({ .rowRect = rvbRect, .label = &rMixSlider.label, .component = &rMixSlider });
    layoutComponentsLtoRFxMixRow({ .rect = rvbRect, .dryBtn = &rDryBtn, .HalfBtn = &rHalfBtn, .wetBtn = &rWetBtn, .paddingBottom = 0 });

    // Retro Bit Crusher
    auto rbcArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    rbcGroup.setBounds(rbcArea);

    auto rbcRect = rbcArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rbcRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = rbcRect, .component = &rbcBypassBtn });
    layoutComponentsLtoRRow({ .rowRect = rbcRect, .label = &rbcBitsSelector.label, .component = &rbcBitsSelector });
    layoutComponentsLtoRRow({ .rowRect = rbcRect, .label = &rbcRateSelector.label, .component = &rbcRateSelector });
    layoutComponentsLtoRRow({ .rowRect = rbcRect, .label = &rbcMixSlider.label, .component = &rbcMixSlider });
    layoutComponentsLtoRFxMixRow({ .rect = rbcRect, .dryBtn = &rbcDryBtn, .HalfBtn = &rbcHalfBtn, .wetBtn = &rbcWetBtn, .paddingBottom = 0 });


    // 3rd Row

    // Soft Clipper
    auto scArea = bottomCol.removeFromLeft(GuiValue::Fm::Op::width);

    softClipperGroup.setBounds(scArea);

    auto scRect = scArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    scRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = scRect, .component = &scBypassBtn });
    layoutComponentsLtoRRow({ .rowRect = scRect, .label = &scMixSlider.label, .component = &scMixSlider });
    layoutComponentsLtoRFxMixRow({ .rect = scRect, .dryBtn = &scDryBtn, .HalfBtn = &scHalfBtn, .wetBtn = &scWetBtn, .paddingBottom = 0 });
}
