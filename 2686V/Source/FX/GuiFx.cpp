#include "GuiFx.h"

#include "../core/PrKeys.h"
#include "../core/PrValues.h"

#include "../gui/GuiHelpers.h"
#include "../gui/GuiValues.h"
#include "../gui/GuiText.h"
#include "../gui/GuiStructs.h"

static std::vector<SelectItem> qualityItems = {
    {.name = "1: Raw (32bit)", .value = 1 },
    {.name = "2: 24-bit PCM",  .value = 2 },
    {.name = "3: 16-bit PCM",  .value = 3 },
    {.name = "4: 8-bit PCM",   .value = 4 },
    {.name = "5: 5-bit PCM",   .value = 5 },
    {.name = "6: 4-bit PCM",   .value = 6 },
    {.name = "7: 4-bit ADPCM", .value = 7 },
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

static std::vector<SelectItem> flTypeItems = {
    {.name = "LPF (Low Pass)",  .value = 1 },
    {.name = "HPF (High Pass)", .value = 2 },
    {.name = "BPF (Band Pass)", .value = 3 }
};

static std::vector<SelectItem> rlfoWaves = {
    {"Saw",      1},
    {"Square",   2},
    {"Triangle", 3},
    {"Noise",    4}
};

void GuiFx::setup()
{
    const juce::String code = PrKey::Prefix::fx;

    // MainGroup
    mainGroup.setup(*this, GuiText::Group::mainGroup);

    bypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });

	bypassToggle.setup({ .parent = *this, .id = code + PrKey::Post::bypass, .title = GuiText::Fx::masterBypass, .isReset = true });

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

	masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });

    // Retro LFO Group
    rlfoGroup.setup(*this, GuiText::Group::fxRLfo);
    const juce::String rlfoPrefix = code + PrKey::Innder::Fx::rlfo;

    rlfoBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });
    rlfoBypassBtn.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });

    rlfoMainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    rlfoWaveSelector.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::wave, .title = GuiText::Fx::Rlfo::wave, .items = rlfoWaves, .isReset = true });
    rlfoFreqSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::freq, .title = GuiText::Fx::Rlfo::freq, .isReset = true });
    rlfoAmsSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::ams, .title = GuiText::Fx::Rlfo::ams, .isReset = true });
    rlfoPmsSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::pms, .title = GuiText::Fx::Rlfo::pms, .isReset = true });
    rlfoAmdSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::amd, .title = GuiText::Fx::Rlfo::amd, .isReset = true });
    rlfoPmdSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::pmd, .title = GuiText::Fx::Rlfo::pmd, .isReset = true });
    rlfoMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });
    rlfoMixSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    rlfoDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry }); rlfoDryBtn.onClick = [&] { rlfoMixSlider.setValue(0.0f); };
    rlfoHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix }); rlfoHalfBtn.onClick = [&] { rlfoMixSlider.setValue(0.5f); };
    rlfoWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet }); rlfoWetBtn.onClick = [&] { rlfoMixSlider.setValue(1.0f); };

    // Tremolo Group
	tremGroup.setup(*this, GuiText::Group::fxTremolo);
    const juce::String trmPrefix = code + PrKey::Innder::Fx::trm;
    tBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });
    tBypassBtn.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    tMainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    tRateSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::Fx::Tremolo::rate, .title = GuiText::Fx::Tremolo::rate, .isReset = true });
    tDepthSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::Fx::Tremolo::depth, .title = GuiText::Fx::Tremolo::depth, .isReset = true });
    tMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });
    tMixSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
	tDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    tDryBtn.onClick = [&] { tMixSlider.setValue(0.0f); };
    tHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    tHalfBtn.onClick = [&] { tMixSlider.setValue(0.5f); };
    tWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    tWetBtn.onClick = [&] { tMixSlider.setValue(1.0f); };

    // Vibrato Group
	vibGroup.setup(*this, GuiText::Group::fxVibrato);
    const juce::String vibPrefix = code + PrKey::Innder::Fx::vib;
    vBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });
    vBypassBtn.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    vMainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    vRateSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::Fx::Vibrato::rate, .title = GuiText::Fx::Vibrate::rate, .isReset = true });
    vDepthSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::Fx::Vibrato::depth, .title = GuiText::Fx::Vibrate::depth, .isReset = true });
    vMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });
    vMixSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    vDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    vDryBtn.onClick = [&] { vMixSlider.setValue(0.0f); };
    vHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    vHalfBtn.onClick = [&] { vMixSlider.setValue(0.5f); };
    vWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    vWetBtn.onClick = [&] { vMixSlider.setValue(1.0f); };

    // Modern Bit Crusher Group
	mbcGroup.setup(*this, GuiText::Group::fxMbc);
    const juce::String mbcPrefix = code + PrKey::Innder::Fx::mbc;
    mbcBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });
    mbcBypassBtn.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    mbcMainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    mbcRateSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::Fx::Mbc::rate, .title = GuiText::Fx::Mbc::rate, .isReset = true });
    mbcBitsSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::Fx::Mbc::bit, .title = GuiText::Fx::Mbc::bit, .isReset = true });
    mbcMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });
    mbcMixSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    mbcDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    mbcDryBtn.onClick = [&] { mbcMixSlider.setValue(0.0f); };
    mbcHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    mbcHalfBtn.onClick = [&] { mbcMixSlider.setValue(0.5f); };
    mbcWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    mbcWetBtn.onClick = [&] { mbcMixSlider.setValue(1.0f); };

    // Delay Group
	delayGroup.setup(*this, GuiText::Group::fxDelay);
    const juce::String dlyPrefix = code + PrKey::Innder::Fx::dly;
    dBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });
    dBypassBtn.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    dMainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    dTimeSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::Fx::Delay::time, .title = GuiText::Fx::Delay::time, .isReset = true });
    dFbSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::Fx::Delay::fb, .title = GuiText::Fx::Delay::fb, .isReset = true });
    dMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });
    dMixSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    dDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    dDryBtn.onClick = [&] { dMixSlider.setValue(0.0f); };
    dHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    dHalfBtn.onClick = [&] { dMixSlider.setValue(0.5f); };
    dWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    dWetBtn.onClick = [&] { dMixSlider.setValue(1.0f); };

    // Reverb Group
	reverbGroup.setup(*this, GuiText::Group::fxReverb);
    const juce::String rvbPrefix = code + PrKey::Innder::Fx::rvb;
    rBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });
    rBypassBtn.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    rMainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    rSizeSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::Fx::Reverb::size, .title = GuiText::Fx::Reverb::size, .isReset = true });
    rDampSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::Fx::Reverb::damp, .title = GuiText::Fx::Reverb::damp, .isReset = true });
    rMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });
    rMixSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    rDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    rDryBtn.onClick = [&] { rMixSlider.setValue(0.0f); };
    rHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    rHalfBtn.onClick = [&] { rMixSlider.setValue(0.5f); };
    rWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    rWetBtn.onClick = [&] { rMixSlider.setValue(1.0f); };

    // Retro Bit Crusher Group
	rbcGroup.setup(*this, GuiText::Group::fxRbc);
    const juce::String rbcPrefix = code + PrKey::Innder::Fx::rbc;
    rbcBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });
    rbcBypassBtn.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    rbcMainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    rbcRateSelector.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::Fx::Rbc::rate, .title = GuiText::Fx::Rbc::rate, .items = rateItems, .isReset = true });
    rbcBitsSelector.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::Fx::Rbc::bit, .title = GuiText::Fx::Rbc::quality, .items = qualityItems, .isReset = true });
    rbcMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });
    rbcMixSlider.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    rbcDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    rbcDryBtn.onClick = [&] { rbcMixSlider.setValue(0.0f); };
    rbcHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    rbcHalfBtn.onClick = [&] { rbcMixSlider.setValue(0.5f); };
    rbcWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    rbcWetBtn.onClick = [&] { rbcMixSlider.setValue(1.0f); };

    // Filter Group
    filterGroup.setup(*this, GuiText::Group::fxFilter);
    const juce::String filterPrefix = code + PrKey::Innder::Fx::fil;
    flBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });
    flBypassBtn.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    flMainCat.setup({ .parent = *this, .title = GuiText::Category::m });
    flTypeSelector.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::Filter::type, .title = GuiText::Fx::Filter::type, .items = flTypeItems, .isReset = true });
    flFreqSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::Filter::freq, .title = GuiText::Fx::Filter::freq, .isReset = true });
    flQSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::Filter::q, .title = GuiText::Fx::Filter::q, .isReset = true });
    flMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });
    flMixSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    flDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry }); flDryBtn.onClick = [&] { flMixSlider.setValue(0.0f); };
    flHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix }); flHalfBtn.onClick = [&] { flMixSlider.setValue(0.5f); };
    flWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet }); flWetBtn.onClick = [&] { flMixSlider.setValue(1.0f); };

    // Soft Clipper Group
    softClipperGroup.setup(*this, GuiText::Group::fxSoftClipper);
    const juce::String sclPrefix = code + PrKey::Innder::Fx::scl;
    scBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });
    scBypassBtn.setup({ .parent = *this, .id = sclPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    scMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });
    scMixSlider.setup({ .parent = *this, .id = sclPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    scDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry }); scDryBtn.onClick = [&] { scMixSlider.setValue(0.0f); };
    scHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix }); scHalfBtn.onClick = [&] { scMixSlider.setValue(0.5f); };
    scWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet }); scWetBtn.onClick = [&] { scMixSlider.setValue(1.0f); };
}

void GuiFx::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &bypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .component = &bypassToggle, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

    auto topCol = pageArea.removeFromTop(GuiValue::Fx::AreaHeightBig);
    pageArea.removeFromTop(GuiValue::PaddingBottom::block);
    auto centerCol = pageArea.removeFromTop(GuiValue::Fx::AreaHeight);
    pageArea.removeFromTop(GuiValue::PaddingBottom::block);
    auto bottomCol = pageArea.removeFromTop(GuiValue::Fx::AreaHeightMini);

    // 1st Row

    // Filter
    auto flArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    filterGroup.setBounds(flArea);

    auto flRect = flArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    flRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = flRect, .label = &flBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = flRect, .component = &flBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = flRect, .label = &flMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = flRect, .label = &flTypeSelector.label, .component = &flTypeSelector, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = flRect, .label = &flFreqSlider.label, .component = &flFreqSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = flRect, .label = &flQSlider.label, .component = &flQSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = flRect, .label = &flMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = flRect, .label = &flMixSlider.label, .component = &flMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRFxMixRow({ .rect = flRect, .dryBtn = &flDryBtn, .HalfBtn = &flHalfBtn, .wetBtn = &flWetBtn, .paddingBottom = 0});

    // Retro LFO
    auto rlfoArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    rlfoGroup.setBounds(rlfoArea);

    auto rlfoRect = rlfoArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rlfoRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .component = &rlfoBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoWaveSelector.label, .component = &rlfoWaveSelector, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoFreqSlider.label, .component = &rlfoFreqSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoPmsSlider.label, .component = &rlfoPmsSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoPmdSlider.label, .component = &rlfoPmdSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoAmsSlider.label, .component = &rlfoAmsSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoAmdSlider.label, .component = &rlfoAmdSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = rlfoRect, .label = &rlfoMixSlider.label, .component = &rlfoMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRFxMixRow({ .rect = rlfoRect, .dryBtn = &rlfoDryBtn, .HalfBtn = &rlfoHalfBtn, .wetBtn = &rlfoWetBtn, .paddingBottom = 0 });

    // Tremolo
    auto trmArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    tremGroup.setBounds(trmArea);

    auto trmRect = trmArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    trmRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = trmRect, .label = &tBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = trmRect, .component = &tBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = trmRect, .label = &tMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = trmRect, .label = &tRateSlider.label, .component = &tRateSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = trmRect, .label = &tDepthSlider.label, .component = &tDepthSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = trmRect, .label = &tMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = trmRect, .label = &tMixSlider.label, .component = &tMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRFxMixRow({ .rect = trmRect, .dryBtn = &tDryBtn, .HalfBtn = &tHalfBtn, .wetBtn = &tWetBtn, .paddingBottom = 0 });

    // Vibrato
    auto vibArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    vibGroup.setBounds(vibArea);

    auto vibRect = vibArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    vibRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = vibRect, .label = &vBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = vibRect, .component = &vBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = vibRect, .label = &vMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = vibRect, .label = &vRateSlider.label, .component = &vRateSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = vibRect, .label = &vDepthSlider.label, .component = &vDepthSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = vibRect, .label = &vMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = vibRect, .label = &vMixSlider.label, .component = &vMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRFxMixRow({ .rect = vibRect, .dryBtn = &vDryBtn, .HalfBtn = &vHalfBtn, .wetBtn = &vWetBtn, .paddingBottom = 0 });


    // 2nd Row

    // Modern Bit Crusher
    auto mbcArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    mbcGroup.setBounds(mbcArea);

    auto mbcRect = mbcArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mbcRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = mbcRect, .label = &mbcBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = mbcRect, .component = &mbcBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = mbcRect, .label = &mbcMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = mbcRect, .label = &mbcBitsSlider.label, .component = &mbcBitsSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = mbcRect, .label = &mbcRateSlider.label, .component = &mbcRateSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = mbcRect, .label = &mbcMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = mbcRect, .label = &mbcMixSlider.label, .component = &mbcMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRFxMixRow({ .rect = mbcRect, .dryBtn = &mbcDryBtn, .HalfBtn = &mbcHalfBtn, .wetBtn = &mbcWetBtn, .paddingBottom = 0 });

    // Delay
    auto dlyArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    delayGroup.setBounds(dlyArea);

    auto dlyRect = dlyArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    dlyRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = dlyRect, .label = &dBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = dlyRect, .component = &dBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = dlyRect, .label = &dMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = dlyRect, .label = &dTimeSlider.label, .component = &dTimeSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = dlyRect, .label = &dFbSlider.label, .component = &dFbSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = dlyRect, .label = &dMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = dlyRect, .label = &dMixSlider.label, .component = &dMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRFxMixRow({ .rect = dlyRect, .dryBtn = &dDryBtn, .HalfBtn = &dHalfBtn, .wetBtn = &dWetBtn, .paddingBottom = 0 });

    // Reverb
    auto rvbArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    reverbGroup.setBounds(rvbArea);

    auto rvbRect = rvbArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rvbRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = rvbRect, .label = &rBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = rvbRect, .component = &rBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rvbRect, .label = &rMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = rvbRect, .label = &rSizeSlider.label, .component = &rSizeSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rvbRect, .label = &rDampSlider.label, .component = &rDampSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rvbRect, .label = &rMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = rvbRect, .label = &rMixSlider.label, .component = &rMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRFxMixRow({ .rect = rvbRect, .dryBtn = &rDryBtn, .HalfBtn = &rHalfBtn, .wetBtn = &rWetBtn, .paddingBottom = 0 });

    // Retro Bit Crusher
    auto rbcArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    rbcGroup.setBounds(rbcArea);

    auto rbcRect = rbcArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rbcRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = rbcRect, .label = &rbcBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = rbcRect, .component = &rbcBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rbcRect, .label = &rbcMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = rbcRect, .label = &rbcBitsSelector.label, .component = &rbcBitsSelector, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rbcRect, .label = &rbcRateSelector.label, .component = &rbcRateSelector, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = rbcRect, .label = &rbcMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = rbcRect, .label = &rbcMixSlider.label, .component = &rbcMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRFxMixRow({ .rect = rbcRect, .dryBtn = &rbcDryBtn, .HalfBtn = &rbcHalfBtn, .wetBtn = &rbcWetBtn, .paddingBottom = 0 });


    // 3rd Row

    // Soft Clipper
    auto scArea = bottomCol.removeFromLeft(GuiValue::Fm::Op::width);

    softClipperGroup.setBounds(scArea);

    auto scRect = scArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    scRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutComponentsLtoRRow({ .rowRect = scRect, .label = &scBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = scRect, .component = &scBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutComponentsLtoRRow({ .rowRect = scRect, .label = &scMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutComponentsLtoRRow({ .rowRect = scRect, .label = &scMixSlider.label, .component = &scMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutComponentsLtoRFxMixRow({ .rect = scRect, .dryBtn = &scDryBtn, .HalfBtn = &scHalfBtn, .wetBtn = &scWetBtn, .paddingBottom = 0 });
}
