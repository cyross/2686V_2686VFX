#include "GuiFx.h"

#include "../core/GuiValues.h"
#include "../core/GuiText.h"
#include "../core/GuiSelectItems.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"

void GuiFx::setup()
{
    const juce::String code = PrKey::Prefix::fx;

    // MainGroup
    mainGroup.setup(*this, GuiText::Group::mainGroup);

	bypassToggle.setup({ .parent = *this, .id = code + PrKey::Post::bypass, .title = GuiText::Group::Fx::masterBypass, .isReset = true });

	masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    // Retro LFO Group
    rlfoGroup.setup(*this, "Retro LFO (OPNA)");
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

    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::height, GuiValue::MainGroup::Row::Padding::bottom, { { &bypassToggle, { GuiValue::MainGroup::Button::width , 0} } });
    layoutComponentsLtoR(mRect, GuiValue::MainGroup::Row::MainVol::height, 0, {
        { &masterVolSlider.label, { GuiValue::MainGroup::Label::width, GuiValue::MainGroup::Row::Padding::right} },
        { &masterVolSlider, { GuiValue::MainGroup::Value::width, 0} }
        });

    auto topCol = pageArea.removeFromTop(GuiValue::Fx::AreaHeight);
    auto centerCol = pageArea.removeFromTop(GuiValue::Fx::AreaHeight);
    auto bottomCol = pageArea.removeFromTop(GuiValue::Fx::AreaHeight);

    // Left

    // 1. Filter
    auto flArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    filterGroup.setBounds(flArea);

    auto flRect = flArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    flRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(flRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &flBypassBtn, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
    layoutComponentsLtoR(flRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &flTypeSelector.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &flTypeSelector, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(flRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &flFreqSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &flFreqSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(flRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &flQSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &flQSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(flRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &flMixSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &flMixSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(flRect, GuiValue::Fm::Op::Row::height, 0, {
        { &flDryBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &flHalfBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &flWetBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} }
    });

    // 2. Retro LFO
    auto rlfoArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    rlfoGroup.setBounds(rlfoArea);

    auto rlfoRect = rlfoArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rlfoRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(rlfoRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rlfoBypassBtn, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
    layoutComponentsLtoR(rlfoRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rlfoWaveSelector.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rlfoWaveSelector, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(rlfoRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rlfoFreqSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rlfoFreqSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(rlfoRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rlfoAmsSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rlfoAmsSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(rlfoRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rlfoPmsSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rlfoPmsSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(rlfoRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rlfoAmdSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rlfoAmdSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(rlfoRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rlfoPmdSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rlfoPmdSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(rlfoRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rlfoMixSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rlfoMixSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(rlfoRect, GuiValue::Fm::Op::Row::height, 0, {
        { &rlfoDryBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &rlfoHalfBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &rlfoWetBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} }
        });

    // 2. Tremolo
    auto trmArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    tremGroup.setBounds(trmArea);

    auto trmRect = trmArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    trmRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(trmRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &tBypassBtn, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
    layoutComponentsLtoR(trmRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &tRateSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &tRateSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(trmRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &tDepthSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &tDepthSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(trmRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &tMixSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &tMixSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(trmRect, GuiValue::Fm::Op::Row::height, 0, {
        { &tDryBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &tHalfBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &tWetBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} }
        });

    // 3. Vibrato
    auto vibArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    vibGroup.setBounds(vibArea);

    auto vibRect = vibArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    vibRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(vibRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &vBypassBtn, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
    layoutComponentsLtoR(vibRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &vRateSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &vRateSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(vibRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &vDepthSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &vDepthSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(vibRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &vMixSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &vMixSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(vibRect, GuiValue::Fm::Op::Row::height, 0, {
        { &vDryBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &vHalfBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &vWetBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} }
        });

    // 4. Modern Bit Crusher
    auto mbcArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    mbcGroup.setBounds(mbcArea);

    auto mbcRect = mbcArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mbcRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(mbcRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &mbcBypassBtn, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
    layoutComponentsLtoR(mbcRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &mbcRateSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &mbcRateSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(mbcRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &mbcBitsSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &mbcBitsSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(mbcRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &mbcMixSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &mbcMixSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(mbcRect, GuiValue::Fm::Op::Row::height, 0, {
        { &mbcDryBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &mbcHalfBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &mbcWetBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} }
        });

    // 5. Delay
    auto dlyArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    delayGroup.setBounds(dlyArea);

    auto dlyRect = dlyArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    dlyRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(dlyRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &dBypassBtn, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
    layoutComponentsLtoR(dlyRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &dTimeSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &dTimeSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(dlyRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &dFbSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &dFbSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(dlyRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &dMixSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &dMixSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(dlyRect, GuiValue::Fm::Op::Row::height, 0, {
        { &dDryBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &dHalfBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &dWetBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} }
        });

    // 6. Reverb
    auto rvbArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    reverbGroup.setBounds(rvbArea);

    auto rvbRect = rvbArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rvbRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(rvbRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rBypassBtn, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
    layoutComponentsLtoR(rvbRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rSizeSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rSizeSlider, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(rvbRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rDampSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rDampSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(rvbRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rMixSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rMixSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(rvbRect, GuiValue::Fm::Op::Row::height, 0, {
        { &rDryBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &rHalfBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &rWetBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} }
        });

    // 7. Retro Bit Crusher
    auto rbcArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    rbcGroup.setBounds(rbcArea);

    auto rbcRect = rbcArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rbcRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(rbcRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rbcBypassBtn, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
    layoutComponentsLtoR(rbcRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rbcBitsSelector.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rbcBitsSelector, { GuiValue::Fm::Op::Row::Value::width, 0} } });
    layoutComponentsLtoR(rbcRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rbcRateSelector.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rbcRateSelector, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(rbcRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &rbcMixSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &rbcMixSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(rbcRect, GuiValue::Fm::Op::Row::height, 0, {
        { &rbcDryBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &rbcHalfBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &rbcWetBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} }
        });

    // 8. Soft Clipper
    auto scArea = bottomCol.removeFromLeft(GuiValue::Fm::Op::width);

    softClipperGroup.setBounds(scArea);

    auto scRect = scArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    scRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoR(scRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &scBypassBtn, { GuiValue::Fm::Op::Row::Button::wdth, 0} } });
    layoutComponentsLtoR(scRect, GuiValue::Fm::Op::Row::height, GuiValue::Fm::Op::Row::Padding::bottom, { { &scMixSlider.label, { GuiValue::Fm::Op::Row::Label::width, GuiValue::Fm::Op::Row::Padding::right} }, { &scMixSlider, { GuiValue::Fm::Op::Row::Value::width, 0}} });
    layoutComponentsLtoR(scRect, GuiValue::Fm::Op::Row::height, 0, {
        { &scDryBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &scHalfBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, GuiValue::Fm::Op::Row::Button::Mix::Padding::height} },
        { &scWetBtn, { GuiValue::Fm::Op::Row::Button::Mix::width, 0} }
    });
}
