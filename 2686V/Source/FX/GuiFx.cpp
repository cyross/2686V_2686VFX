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
    int tabOrder = 1;

    // MainGroup
    mainGroup.setup(*this, GuiText::Group::mainGroup);

    bypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });

	bypassToggle.setup({ .parent = *this, .id = code + PrKey::Post::bypass, .title = GuiText::Fx::masterBypass, .isReset = true });
    bypassToggle.setWantsKeyboardFocus(true);
    bypassToggle.setExplicitFocusOrder(++tabOrder);

    mvolCat.setup({ .parent = *this, .title = GuiText::Category::mvol });

	masterVolSlider.setup({ .parent = *this, .id = PrKey::masterVol, .title = GuiText::MasterVol::title, .isReset = true });
    masterVolSlider.setWantsKeyboardFocus(true);
    masterVolSlider.setExplicitFocusOrder(++tabOrder);

    // Filter Group
    filterGroup.setup(*this, GuiText::Group::fxFilter);
    const juce::String filterPrefix = code + PrKey::Innder::Fx::fil;
    flBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });

    flBypassBtn.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    flBypassBtn.setWantsKeyboardFocus(true);
    flBypassBtn.setExplicitFocusOrder(++tabOrder);

    flMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    flTypeSelector.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::Filter::type, .title = GuiText::Fx::Filter::type, .items = flTypeItems, .isReset = true });
    flTypeSelector.setWantsKeyboardFocus(true);
    flTypeSelector.setExplicitFocusOrder(++tabOrder);

    flFreqSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::Filter::freq, .title = GuiText::Fx::Filter::freq, .isReset = true });
    flFreqSlider.setWantsKeyboardFocus(true);
    flFreqSlider.setExplicitFocusOrder(++tabOrder);

    flQSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::Filter::q, .title = GuiText::Fx::Filter::q, .isReset = true });
    flQSlider.setWantsKeyboardFocus(true);
    flQSlider.setExplicitFocusOrder(++tabOrder);

    flMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    flMixSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    flMixSlider.setWantsKeyboardFocus(true);
    flMixSlider.setExplicitFocusOrder(++tabOrder);

    flDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    flDryBtn.onClick = [&] { flMixSlider.setValue(0.0f); };
    flDryBtn.setWantsKeyboardFocus(true);
    flDryBtn.setExplicitFocusOrder(++tabOrder);

    flHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    flHalfBtn.onClick = [&] { flMixSlider.setValue(0.5f); };
    flHalfBtn.setWantsKeyboardFocus(true);
    flHalfBtn.setExplicitFocusOrder(++tabOrder);

    flWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    flWetBtn.onClick = [&] { flMixSlider.setValue(1.0f); };
    flWetBtn.setWantsKeyboardFocus(true);
    flWetBtn.setExplicitFocusOrder(++tabOrder);

    // Retro LFO Group
    rlfoGroup.setup(*this, GuiText::Group::fxRLfo);
    const juce::String rlfoPrefix = code + PrKey::Innder::Fx::rlfo;

    rlfoBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });
    rlfoBypassBtn.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    rlfoBypassBtn.setWantsKeyboardFocus(true);
    rlfoBypassBtn.setExplicitFocusOrder(++tabOrder);

    rlfoMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    rlfoWaveSelector.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::wave, .title = GuiText::Fx::Rlfo::wave, .items = rlfoWaves, .isReset = true });
    rlfoWaveSelector.setWantsKeyboardFocus(true);
    rlfoWaveSelector.setExplicitFocusOrder(++tabOrder);

    rlfoFreqSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::freq, .title = GuiText::Fx::Rlfo::freq, .isReset = true });
    rlfoFreqSlider.setWantsKeyboardFocus(true);
    rlfoFreqSlider.setExplicitFocusOrder(++tabOrder);

    rlfoPmsSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::pms, .title = GuiText::Fx::Rlfo::pms, .isReset = true });
    rlfoPmsSlider.setWantsKeyboardFocus(true);
    rlfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    rlfoPmdSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::pmd, .title = GuiText::Fx::Rlfo::pmd, .isReset = true });
    rlfoPmdSlider.setWantsKeyboardFocus(true);
    rlfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    rlfoAmsSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::ams, .title = GuiText::Fx::Rlfo::ams, .isReset = true });
    rlfoAmsSlider.setWantsKeyboardFocus(true);
    rlfoAmsSlider.setExplicitFocusOrder(++tabOrder);

    rlfoAmdSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::RLfo::amd, .title = GuiText::Fx::Rlfo::amd, .isReset = true });
    rlfoAmdSlider.setWantsKeyboardFocus(true);
    rlfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    rlfoMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    rlfoMixSlider.setup({ .parent = *this, .id = rlfoPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    rlfoMixSlider.setWantsKeyboardFocus(true);
    rlfoMixSlider.setExplicitFocusOrder(++tabOrder);

    rlfoDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry }); rlfoDryBtn.onClick = [&] { rlfoMixSlider.setValue(0.0f); };
    rlfoDryBtn.setWantsKeyboardFocus(true);
    rlfoDryBtn.setExplicitFocusOrder(++tabOrder);

    rlfoHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix }); rlfoHalfBtn.onClick = [&] { rlfoMixSlider.setValue(0.5f); };
    rlfoHalfBtn.setWantsKeyboardFocus(true);
    rlfoHalfBtn.setExplicitFocusOrder(++tabOrder);

    rlfoWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet }); rlfoWetBtn.onClick = [&] { rlfoMixSlider.setValue(1.0f); };
    rlfoWetBtn.setWantsKeyboardFocus(true);
    rlfoWetBtn.setExplicitFocusOrder(++tabOrder);

    // Tremolo Group
	tremGroup.setup(*this, GuiText::Group::fxTremolo);
    const juce::String trmPrefix = code + PrKey::Innder::Fx::trm;

    tBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });

    tBypassBtn.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    tBypassBtn.setWantsKeyboardFocus(true);
    tBypassBtn.setExplicitFocusOrder(++tabOrder);

    tMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    tRateSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::Fx::Tremolo::rate, .title = GuiText::Fx::Tremolo::rate, .isReset = true });
    tRateSlider.setWantsKeyboardFocus(true);
    tRateSlider.setExplicitFocusOrder(++tabOrder);

    tDepthSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::Fx::Tremolo::depth, .title = GuiText::Fx::Tremolo::depth, .isReset = true });
    tDepthSlider.setWantsKeyboardFocus(true);
    tDepthSlider.setExplicitFocusOrder(++tabOrder);

    tMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    tMixSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    tMixSlider.setWantsKeyboardFocus(true);
    tMixSlider.setExplicitFocusOrder(++tabOrder);

    tDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    tDryBtn.setWantsKeyboardFocus(true);
    tDryBtn.setExplicitFocusOrder(++tabOrder);
    tDryBtn.onClick = [&] { tMixSlider.setValue(0.0f); };

    tHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    tHalfBtn.setWantsKeyboardFocus(true);
    tHalfBtn.setExplicitFocusOrder(++tabOrder);
    tHalfBtn.onClick = [&] { tMixSlider.setValue(0.5f); };

    tWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    tWetBtn.setWantsKeyboardFocus(true);
    tWetBtn.setExplicitFocusOrder(++tabOrder);
    tWetBtn.onClick = [&] { tMixSlider.setValue(1.0f); };

    // Vibrato Group
	vibGroup.setup(*this, GuiText::Group::fxVibrato);
    const juce::String vibPrefix = code + PrKey::Innder::Fx::vib;
    vBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });

    vBypassBtn.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    vBypassBtn.setWantsKeyboardFocus(true);
    vBypassBtn.setExplicitFocusOrder(++tabOrder);

    vMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    vRateSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::Fx::Vibrato::rate, .title = GuiText::Fx::Vibrate::rate, .isReset = true });
    vRateSlider.setWantsKeyboardFocus(true);
    vRateSlider.setExplicitFocusOrder(++tabOrder);

    vDepthSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::Fx::Vibrato::depth, .title = GuiText::Fx::Vibrate::depth, .isReset = true });
    vDepthSlider.setWantsKeyboardFocus(true);
    vDepthSlider.setExplicitFocusOrder(++tabOrder);

    vMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    vMixSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    vMixSlider.setWantsKeyboardFocus(true);
    vMixSlider.setExplicitFocusOrder(++tabOrder);

    vDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    vDryBtn.setWantsKeyboardFocus(true);
    vDryBtn.setExplicitFocusOrder(++tabOrder);
    vDryBtn.onClick = [&] { vMixSlider.setValue(0.0f); };

    vHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    vHalfBtn.setWantsKeyboardFocus(true);
    vHalfBtn.setExplicitFocusOrder(++tabOrder);
    vHalfBtn.onClick = [&] { vMixSlider.setValue(0.5f); };

    vWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    vWetBtn.setWantsKeyboardFocus(true);
    vWetBtn.setExplicitFocusOrder(++tabOrder);
    vWetBtn.onClick = [&] { vMixSlider.setValue(1.0f); };

    // Modern Bit Crusher Group
	mbcGroup.setup(*this, GuiText::Group::fxMbc);
    const juce::String mbcPrefix = code + PrKey::Innder::Fx::mbc;
    mbcBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });

    mbcBypassBtn.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    mbcBypassBtn.setWantsKeyboardFocus(true);
    mbcBypassBtn.setExplicitFocusOrder(++tabOrder);

    mbcMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    mbcBitsSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::Fx::Mbc::bit, .title = GuiText::Fx::Mbc::bit, .isReset = true });
    mbcBitsSlider.setWantsKeyboardFocus(true);
    mbcBitsSlider.setExplicitFocusOrder(++tabOrder);

    mbcRateSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::Fx::Mbc::rate, .title = GuiText::Fx::Mbc::rate, .isReset = true });
    mbcRateSlider.setWantsKeyboardFocus(true);
    mbcRateSlider.setExplicitFocusOrder(++tabOrder);

    mbcMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    mbcMixSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    mbcMixSlider.setWantsKeyboardFocus(true);
    mbcMixSlider.setExplicitFocusOrder(++tabOrder);

    mbcDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    mbcDryBtn.setWantsKeyboardFocus(true);
    mbcDryBtn.setExplicitFocusOrder(++tabOrder);

    mbcDryBtn.onClick = [&] { mbcMixSlider.setValue(0.0f); };
    mbcDryBtn.setWantsKeyboardFocus(true);
    mbcDryBtn.setExplicitFocusOrder(++tabOrder);

    mbcHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    mbcHalfBtn.setWantsKeyboardFocus(true);
    mbcHalfBtn.setExplicitFocusOrder(++tabOrder);
    mbcHalfBtn.onClick = [&] { mbcMixSlider.setValue(0.5f); };

    mbcWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    mbcWetBtn.setWantsKeyboardFocus(true);
    mbcWetBtn.setExplicitFocusOrder(++tabOrder);
    mbcWetBtn.onClick = [&] { mbcMixSlider.setValue(1.0f); };

    // Delay Group
	delayGroup.setup(*this, GuiText::Group::fxDelay);
    const juce::String dlyPrefix = code + PrKey::Innder::Fx::dly;
    dBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });

    dBypassBtn.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    dBypassBtn.setWantsKeyboardFocus(true);
    dBypassBtn.setExplicitFocusOrder(++tabOrder);

    dMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    dTimeSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::Fx::Delay::time, .title = GuiText::Fx::Delay::time, .isReset = true });
    dTimeSlider.setWantsKeyboardFocus(true);
    dTimeSlider.setExplicitFocusOrder(++tabOrder);

    dFbSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::Fx::Delay::fb, .title = GuiText::Fx::Delay::fb, .isReset = true });
    dFbSlider.setWantsKeyboardFocus(true);
    dFbSlider.setExplicitFocusOrder(++tabOrder);

    dMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    dMixSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    dMixSlider.setWantsKeyboardFocus(true);
    dMixSlider.setExplicitFocusOrder(++tabOrder);

    dDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    dDryBtn.setWantsKeyboardFocus(true);
    dDryBtn.setExplicitFocusOrder(++tabOrder);
    dDryBtn.onClick = [&] { dMixSlider.setValue(0.0f); };

    dHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    dHalfBtn.setWantsKeyboardFocus(true);
    dHalfBtn.setExplicitFocusOrder(++tabOrder);
    dHalfBtn.onClick = [&] { dMixSlider.setValue(0.5f); };

    dWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    dWetBtn.setWantsKeyboardFocus(true);
    dWetBtn.setExplicitFocusOrder(++tabOrder);
    dWetBtn.onClick = [&] { dMixSlider.setValue(1.0f); };

    // Reverb Group
	reverbGroup.setup(*this, GuiText::Group::fxReverb);
    const juce::String rvbPrefix = code + PrKey::Innder::Fx::rvb;
    rBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });

    rBypassBtn.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    rBypassBtn.setWantsKeyboardFocus(true);
    rBypassBtn.setExplicitFocusOrder(++tabOrder);

    rMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    rSizeSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::Fx::Reverb::size, .title = GuiText::Fx::Reverb::size, .isReset = true });
    rSizeSlider.setWantsKeyboardFocus(true);
    rSizeSlider.setExplicitFocusOrder(++tabOrder);

    rDampSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::Fx::Reverb::damp, .title = GuiText::Fx::Reverb::damp, .isReset = true });
    rDampSlider.setWantsKeyboardFocus(true);
    rDampSlider.setExplicitFocusOrder(++tabOrder);

    rMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    rMixSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    rMixSlider.setWantsKeyboardFocus(true);
    rMixSlider.setExplicitFocusOrder(++tabOrder);

    rDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    rDryBtn.setWantsKeyboardFocus(true);
    rDryBtn.setExplicitFocusOrder(++tabOrder);
    rDryBtn.onClick = [&] { rMixSlider.setValue(0.0f); };

    rHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    rHalfBtn.setWantsKeyboardFocus(true);
    rHalfBtn.setExplicitFocusOrder(++tabOrder);
    rHalfBtn.onClick = [&] { rMixSlider.setValue(0.5f); };

    rWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    rWetBtn.setWantsKeyboardFocus(true);
    rWetBtn.setExplicitFocusOrder(++tabOrder);
    rWetBtn.onClick = [&] { rMixSlider.setValue(1.0f); };

    // Retro Bit Crusher Group
	rbcGroup.setup(*this, GuiText::Group::fxRbc);
    const juce::String rbcPrefix = code + PrKey::Innder::Fx::rbc;
    rbcBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });

    rbcBypassBtn.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    rbcBypassBtn.setWantsKeyboardFocus(true);
    rbcBypassBtn.setExplicitFocusOrder(++tabOrder);

    rbcMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    rbcBitsSelector.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::Fx::Rbc::bit, .title = GuiText::Fx::Rbc::quality, .items = qualityItems, .isReset = true });
    rbcBitsSelector.setWantsKeyboardFocus(true);
    rbcBitsSelector.setExplicitFocusOrder(++tabOrder);

    rbcRateSelector.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::Fx::Rbc::rate, .title = GuiText::Fx::Rbc::rate, .items = rateItems, .isReset = true });
    rbcRateSelector.setWantsKeyboardFocus(true);
    rbcRateSelector.setExplicitFocusOrder(++tabOrder);

    rbcMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    rbcMixSlider.setup({ .parent = *this, .id = rbcPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    rbcMixSlider.setWantsKeyboardFocus(true);
    rbcMixSlider.setExplicitFocusOrder(++tabOrder);

    rbcDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    rbcDryBtn.setWantsKeyboardFocus(true);
    rbcDryBtn.setExplicitFocusOrder(++tabOrder);
    rbcDryBtn.onClick = [&] { rbcMixSlider.setValue(0.0f); };

    rbcHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    rbcHalfBtn.setWantsKeyboardFocus(true);
    rbcHalfBtn.setExplicitFocusOrder(++tabOrder);
    rbcHalfBtn.onClick = [&] { rbcMixSlider.setValue(0.5f); };

    rbcWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    rbcWetBtn.setWantsKeyboardFocus(true);
    rbcWetBtn.setExplicitFocusOrder(++tabOrder);
    rbcWetBtn.onClick = [&] { rbcMixSlider.setValue(1.0f); };

    // Soft Clipper Group
    softClipperGroup.setup(*this, GuiText::Group::fxSoftClipper);
    const juce::String sclPrefix = code + PrKey::Innder::Fx::scl;
    scBypassCat.setup({ .parent = *this, .title = GuiText::Category::bypass });

    scBypassBtn.setup({ .parent = *this, .id = sclPrefix + PrKey::Post::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    scBypassBtn.setWantsKeyboardFocus(true);
    scBypassBtn.setExplicitFocusOrder(++tabOrder);

    scMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    scMixSlider.setup({ .parent = *this, .id = sclPrefix + PrKey::Post::Fx::mix, .title = GuiText::Fx::mix, .isReset = true });
    scMixSlider.setWantsKeyboardFocus(true);
    scMixSlider.setExplicitFocusOrder(++tabOrder);

    scDryBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::dry });
    scDryBtn.onClick = [&] { scMixSlider.setValue(0.0f); };
    scDryBtn.setWantsKeyboardFocus(true);
    scDryBtn.setExplicitFocusOrder(++tabOrder);

    scHalfBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::mix });
    scHalfBtn.onClick = [&] { scMixSlider.setValue(0.5f); };
    scHalfBtn.setWantsKeyboardFocus(true);
    scHalfBtn.setExplicitFocusOrder(++tabOrder);

    scWetBtn.setup({ .parent = *this, .title = GuiText::Fx::Mix::wet });
    scWetBtn.onClick = [&] { scMixSlider.setValue(1.0f); };
    scWetBtn.setWantsKeyboardFocus(true);
    scWetBtn.setExplicitFocusOrder(++tabOrder);
}

void GuiFx::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutMain({ .mainRect = mRect, .label = &bypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .component = &bypassToggle, .paddingBottom = GuiValue::MVol::paddingTop });
    layoutMain({ .mainRect = mRect, .label = &mvolCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutMain({ .mainRect = mRect, .label = &masterVolSlider.label, .component = &masterVolSlider, .paddingBottom = 0 });

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

    layoutRow({ .rowRect = flRect, .label = &flBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = flRect, .component = &flBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = flRect, .label = &flMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = flRect, .label = &flTypeSelector.label, .component = &flTypeSelector, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = flRect, .label = &flFreqSlider.label, .component = &flFreqSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = flRect, .label = &flQSlider.label, .component = &flQSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = flRect, .label = &flMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = flRect, .label = &flMixSlider.label, .component = &flMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRowThreeComps({ .rect = flRect, .comp1 = &flDryBtn, .comp2 = &flHalfBtn, .comp3 = &flWetBtn, .paddingBottom = 0});

    // Retro LFO
    auto rlfoArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    rlfoGroup.setBounds(rlfoArea);

    auto rlfoRect = rlfoArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rlfoRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = rlfoRect, .label = &rlfoBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = rlfoRect, .component = &rlfoBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = rlfoRect, .label = &rlfoMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = rlfoRect, .label = &rlfoWaveSelector.label, .component = &rlfoWaveSelector, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = rlfoRect, .label = &rlfoFreqSlider.label, .component = &rlfoFreqSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = rlfoRect, .label = &rlfoPmsSlider.label, .component = &rlfoPmsSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = rlfoRect, .label = &rlfoPmdSlider.label, .component = &rlfoPmdSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = rlfoRect, .label = &rlfoAmsSlider.label, .component = &rlfoAmsSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = rlfoRect, .label = &rlfoAmdSlider.label, .component = &rlfoAmdSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = rlfoRect, .label = &rlfoMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = rlfoRect, .label = &rlfoMixSlider.label, .component = &rlfoMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRowThreeComps({ .rect = rlfoRect, .comp1 = &rlfoDryBtn, .comp2 = &rlfoHalfBtn, .comp3 = &rlfoWetBtn, .paddingBottom = 0 });

    // Tremolo
    auto trmArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    tremGroup.setBounds(trmArea);

    auto trmRect = trmArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    trmRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = trmRect, .label = &tBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = trmRect, .component = &tBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = trmRect, .label = &tMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = trmRect, .label = &tRateSlider.label, .component = &tRateSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = trmRect, .label = &tDepthSlider.label, .component = &tDepthSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = trmRect, .label = &tMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = trmRect, .label = &tMixSlider.label, .component = &tMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRowThreeComps({ .rect = trmRect, .comp1 = &tDryBtn, .comp2 = &tHalfBtn, .comp3 = &tWetBtn, .paddingBottom = 0 });

    // Vibrato
    auto vibArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    vibGroup.setBounds(vibArea);

    auto vibRect = vibArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    vibRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = vibRect, .label = &vBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = vibRect, .component = &vBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = vibRect, .label = &vMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = vibRect, .label = &vRateSlider.label, .component = &vRateSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = vibRect, .label = &vDepthSlider.label, .component = &vDepthSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = vibRect, .label = &vMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = vibRect, .label = &vMixSlider.label, .component = &vMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRowThreeComps({ .rect = vibRect, .comp1 = &vDryBtn, .comp2 = &vHalfBtn, .comp3 = &vWetBtn, .paddingBottom = 0 });


    // 2nd Row

    // Modern Bit Crusher
    auto mbcArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    mbcGroup.setBounds(mbcArea);

    auto mbcRect = mbcArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mbcRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = mbcRect, .label = &mbcBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = mbcRect, .component = &mbcBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = mbcRect, .label = &mbcMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = mbcRect, .label = &mbcBitsSlider.label, .component = &mbcBitsSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = mbcRect, .label = &mbcRateSlider.label, .component = &mbcRateSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = mbcRect, .label = &mbcMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = mbcRect, .label = &mbcMixSlider.label, .component = &mbcMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRowThreeComps({ .rect = mbcRect, .comp1 = &mbcDryBtn, .comp2 = &mbcHalfBtn, .comp3 = &mbcWetBtn, .paddingBottom = 0 });

    // Delay
    auto dlyArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    delayGroup.setBounds(dlyArea);

    auto dlyRect = dlyArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    dlyRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = dlyRect, .label = &dBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = dlyRect, .component = &dBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = dlyRect, .label = &dMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = dlyRect, .label = &dTimeSlider.label, .component = &dTimeSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = dlyRect, .label = &dFbSlider.label, .component = &dFbSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = dlyRect, .label = &dMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = dlyRect, .label = &dMixSlider.label, .component = &dMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRowThreeComps({ .rect = dlyRect, .comp1 = &dDryBtn, .comp2 = &dHalfBtn, .comp3 = &dWetBtn, .paddingBottom = 0 });

    // Reverb
    auto rvbArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    reverbGroup.setBounds(rvbArea);

    auto rvbRect = rvbArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rvbRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = rvbRect, .label = &rBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = rvbRect, .component = &rBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = rvbRect, .label = &rMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = rvbRect, .label = &rSizeSlider.label, .component = &rSizeSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = rvbRect, .label = &rDampSlider.label, .component = &rDampSlider, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = rvbRect, .label = &rMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = rvbRect, .label = &rMixSlider.label, .component = &rMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRowThreeComps({ .rect = rvbRect, .comp1 = &rDryBtn, .comp2 = &rHalfBtn, .comp3 = &rWetBtn, .paddingBottom = 0 });

    // Retro Bit Crusher
    auto rbcArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    rbcGroup.setBounds(rbcArea);

    auto rbcRect = rbcArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rbcRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = rbcRect, .label = &rbcBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = rbcRect, .component = &rbcBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = rbcRect, .label = &rbcMainCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = rbcRect, .label = &rbcBitsSelector.label, .component = &rbcBitsSelector, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRow({ .rowRect = rbcRect, .label = &rbcRateSelector.label, .component = &rbcRateSelector, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = rbcRect, .label = &rbcMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = rbcRect, .label = &rbcMixSlider.label, .component = &rbcMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRowThreeComps({ .rect = rbcRect, .comp1 = &rbcDryBtn, .comp2 = &rbcHalfBtn, .comp3 = &rbcWetBtn, .paddingBottom = 0 });


    // 3rd Row

    // Soft Clipper
    auto scArea = bottomCol.removeFromLeft(GuiValue::Fm::Op::width);

    softClipperGroup.setBounds(scArea);

    auto scRect = scArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    scRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = scRect, .label = &scBypassCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = scRect, .component = &scBypassBtn, .paddingBottom = GuiValue::Category::paddingTop });
    layoutRow({ .rowRect = scRect, .label = &scMixCat, .paddingBottom = GuiValue::Category::paddingBotton });
    layoutRow({ .rowRect = scRect, .label = &scMixSlider.label, .component = &scMixSlider, .paddingBottom = GuiValue::ParamGroup::Row::paddingTop });
    layoutRowThreeComps({ .rect = scRect, .comp1 = &scDryBtn, .comp2 = &scHalfBtn, .comp3 = &scWetBtn, .paddingBottom = 0 });
}
