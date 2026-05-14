#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Fx/Keys.h"
#include "../../Processor/Fx/Values.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiValues.h"
#include "./GuiText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> flTypeItems = {
    {.name = "LPF", .value = 1 },
    {.name = "HPF", .value = 2 },
    {.name = "BPF", .value = 3 }
};

void GuiFx::setup()
{
    const juce::String code = FxPrKey::prefix;
    int tabOrder = 1;
    const juce::Colour groupBgColour = juce::Colours::darkblue.withAlpha(0.25f);

    // MainGroup
    mainGroup.setup(*this, FxGuiText::Group::mainGroup);
    mainGroup.setBackgroundColor(groupBgColour);

	bypassToggle.setup({ .parent = *this, .id = code + FxPrKey::bypass, .title = FxGuiText::Fx::masterBypass, .isReset = true });
    bypassToggle.setWantsKeyboardFocus(true);
    bypassToggle.setExplicitFocusOrder(++tabOrder);

    resetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::reset });
    resetBtn.setWantsKeyboardFocus(true);
    resetBtn.setExplicitFocusOrder(++tabOrder);
    resetBtn.onClick = [&] { this->ctx.audioProcessor.initParams("FX_"); };

    // Filter Group
    filterGroup.setup(*this, FxGuiText::Group::fxFilter);
    filterGroup.setBackgroundColor(groupBgColour);
    const juce::String filterPrefix = code + FxPrKey::fil;

    flBypassBtn.setup({ .parent = *this, .id = filterPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    flBypassBtn.setWantsKeyboardFocus(true);
    flBypassBtn.setExplicitFocusOrder(++tabOrder);

    flMainCat.setup({ .parent = *this, .title = FxGuiText::Category::m });

    flTypeSelector.setup({ .parent = *this, .id = filterPrefix + FxPrKey::Filter::type, .title = FxGuiText::Fx::Filter::type, .items = flTypeItems, .isReset = true });
    flTypeSelector.setWantsKeyboardFocus(true);
    flTypeSelector.setExplicitFocusOrder(++tabOrder);

    flFreqSlider.setup({ .parent = *this, .id = filterPrefix + FxPrKey::Filter::freq, .title = FxGuiText::Fx::Filter::freq, .isReset = true });
    flFreqSlider.setWantsKeyboardFocus(true);
    flFreqSlider.setExplicitFocusOrder(++tabOrder);

    flQSlider.setup({ .parent = *this, .id = filterPrefix + FxPrKey::Filter::q, .title = FxGuiText::Fx::Filter::q, .isReset = true });
    flQSlider.setWantsKeyboardFocus(true);
    flQSlider.setExplicitFocusOrder(++tabOrder);

    flMixCat.setup({ .parent = *this, .title = FxGuiText::Category::mix });

    flMixSlider.setup({ .parent = *this, .id = filterPrefix + FxPrKey::mix, .title = FxGuiText::Fx::mix, .isReset = true });
    flMixSlider.setWantsKeyboardFocus(true);
    flMixSlider.setExplicitFocusOrder(++tabOrder);

    flDryBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::dry });
    flDryBtn.onClick = [&] { flMixSlider.setValue(0.0f); };
    flDryBtn.setWantsKeyboardFocus(true);
    flDryBtn.setExplicitFocusOrder(++tabOrder);

    flHalfBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::mix });
    flHalfBtn.onClick = [&] { flMixSlider.setValue(0.5f); };
    flHalfBtn.setWantsKeyboardFocus(true);
    flHalfBtn.setExplicitFocusOrder(++tabOrder);

    flWetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::wet });
    flWetBtn.onClick = [&] { flMixSlider.setValue(1.0f); };
    flWetBtn.setWantsKeyboardFocus(true);
    flWetBtn.setExplicitFocusOrder(++tabOrder);

    // Tremolo Group
	tremGroup.setup(*this, FxGuiText::Group::fxTremolo);
    tremGroup.setBackgroundColor(groupBgColour);
    const juce::String trmPrefix = code + FxPrKey::trm;

    tBypassBtn.setup({ .parent = *this, .id = trmPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    tBypassBtn.setWantsKeyboardFocus(true);
    tBypassBtn.setExplicitFocusOrder(++tabOrder);

    tMainCat.setup({ .parent = *this, .title = FxGuiText::Category::m });

    tRateSlider.setup({ .parent = *this, .id = trmPrefix + FxPrKey::Tremolo::rate, .title = FxGuiText::Fx::Tremolo::rate, .isReset = true });
    tRateSlider.setWantsKeyboardFocus(true);
    tRateSlider.setExplicitFocusOrder(++tabOrder);

    tDepthSlider.setup({ .parent = *this, .id = trmPrefix + FxPrKey::Tremolo::depth, .title = FxGuiText::Fx::Tremolo::depth, .isReset = true });
    tDepthSlider.setWantsKeyboardFocus(true);
    tDepthSlider.setExplicitFocusOrder(++tabOrder);

    tMixCat.setup({ .parent = *this, .title = FxGuiText::Category::mix });

    tMixSlider.setup({ .parent = *this, .id = trmPrefix + FxPrKey::mix, .title = FxGuiText::Fx::mix, .isReset = true });
    tMixSlider.setWantsKeyboardFocus(true);
    tMixSlider.setExplicitFocusOrder(++tabOrder);

    tDryBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::dry });
    tDryBtn.setWantsKeyboardFocus(true);
    tDryBtn.setExplicitFocusOrder(++tabOrder);
    tDryBtn.onClick = [&] { tMixSlider.setValue(0.0f); };

    tHalfBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::mix });
    tHalfBtn.setWantsKeyboardFocus(true);
    tHalfBtn.setExplicitFocusOrder(++tabOrder);
    tHalfBtn.onClick = [&] { tMixSlider.setValue(0.5f); };

    tWetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::wet });
    tWetBtn.setWantsKeyboardFocus(true);
    tWetBtn.setExplicitFocusOrder(++tabOrder);
    tWetBtn.onClick = [&] { tMixSlider.setValue(1.0f); };

    // Vibrato Group
	vibGroup.setup(*this, FxGuiText::Group::fxVibrato);
    vibGroup.setBackgroundColor(groupBgColour);
    const juce::String vibPrefix = code + FxPrKey::vib;

    vBypassBtn.setup({ .parent = *this, .id = vibPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    vBypassBtn.setWantsKeyboardFocus(true);
    vBypassBtn.setExplicitFocusOrder(++tabOrder);

    vMainCat.setup({ .parent = *this, .title = FxGuiText::Category::m });

    vRateSlider.setup({ .parent = *this, .id = vibPrefix + FxPrKey::Vibrato::rate, .title = FxGuiText::Fx::Vibrate::rate, .isReset = true });
    vRateSlider.setWantsKeyboardFocus(true);
    vRateSlider.setExplicitFocusOrder(++tabOrder);

    vDepthSlider.setup({ .parent = *this, .id = vibPrefix + FxPrKey::Vibrato::depth, .title = FxGuiText::Fx::Vibrate::depth, .isReset = true });
    vDepthSlider.setWantsKeyboardFocus(true);
    vDepthSlider.setExplicitFocusOrder(++tabOrder);

    vMixCat.setup({ .parent = *this, .title = FxGuiText::Category::mix });

    vMixSlider.setup({ .parent = *this, .id = vibPrefix + FxPrKey::mix, .title = FxGuiText::Fx::mix, .isReset = true });
    vMixSlider.setWantsKeyboardFocus(true);
    vMixSlider.setExplicitFocusOrder(++tabOrder);

    vDryBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::dry });
    vDryBtn.setWantsKeyboardFocus(true);
    vDryBtn.setExplicitFocusOrder(++tabOrder);
    vDryBtn.onClick = [&] { vMixSlider.setValue(0.0f); };

    vHalfBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::mix });
    vHalfBtn.setWantsKeyboardFocus(true);
    vHalfBtn.setExplicitFocusOrder(++tabOrder);
    vHalfBtn.onClick = [&] { vMixSlider.setValue(0.5f); };

    vWetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::wet });
    vWetBtn.setWantsKeyboardFocus(true);
    vWetBtn.setExplicitFocusOrder(++tabOrder);
    vWetBtn.onClick = [&] { vMixSlider.setValue(1.0f); };

    // Modern Bit Crusher Group
	mbcGroup.setup(*this, FxGuiText::Group::fxMbc);
    mbcGroup.setBackgroundColor(groupBgColour);
    const juce::String mbcPrefix = code + FxPrKey::mbc;

    mbcBypassBtn.setup({ .parent = *this, .id = mbcPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    mbcBypassBtn.setWantsKeyboardFocus(true);
    mbcBypassBtn.setExplicitFocusOrder(++tabOrder);

    mbcMainCat.setup({ .parent = *this, .title = FxGuiText::Category::m });

    mbcBitsSlider.setup({ .parent = *this, .id = mbcPrefix + FxPrKey::Mbc::bit, .title = FxGuiText::Fx::Mbc::bit, .isReset = true });
    mbcBitsSlider.setWantsKeyboardFocus(true);
    mbcBitsSlider.setExplicitFocusOrder(++tabOrder);

    mbcRateSlider.setup({ .parent = *this, .id = mbcPrefix + FxPrKey::Mbc::rate, .title = FxGuiText::Fx::Mbc::rate, .isReset = true });
    mbcRateSlider.setWantsKeyboardFocus(true);
    mbcRateSlider.setExplicitFocusOrder(++tabOrder);

    mbcMixCat.setup({ .parent = *this, .title = FxGuiText::Category::mix });

    mbcMixSlider.setup({ .parent = *this, .id = mbcPrefix + FxPrKey::mix, .title = FxGuiText::Fx::mix, .isReset = true });
    mbcMixSlider.setWantsKeyboardFocus(true);
    mbcMixSlider.setExplicitFocusOrder(++tabOrder);

    mbcDryBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::dry });
    mbcDryBtn.setWantsKeyboardFocus(true);
    mbcDryBtn.setExplicitFocusOrder(++tabOrder);

    mbcDryBtn.onClick = [&] { mbcMixSlider.setValue(0.0f); };
    mbcDryBtn.setWantsKeyboardFocus(true);
    mbcDryBtn.setExplicitFocusOrder(++tabOrder);

    mbcHalfBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::mix });
    mbcHalfBtn.setWantsKeyboardFocus(true);
    mbcHalfBtn.setExplicitFocusOrder(++tabOrder);
    mbcHalfBtn.onClick = [&] { mbcMixSlider.setValue(0.5f); };

    mbcWetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::wet });
    mbcWetBtn.setWantsKeyboardFocus(true);
    mbcWetBtn.setExplicitFocusOrder(++tabOrder);
    mbcWetBtn.onClick = [&] { mbcMixSlider.setValue(1.0f); };

    // Delay Group
	delayGroup.setup(*this, FxGuiText::Group::fxDelay);
    delayGroup.setBackgroundColor(groupBgColour);
    const juce::String dlyPrefix = code + FxPrKey::dly;

    dBypassBtn.setup({ .parent = *this, .id = dlyPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    dBypassBtn.setWantsKeyboardFocus(true);
    dBypassBtn.setExplicitFocusOrder(++tabOrder);

    dMainCat.setup({ .parent = *this, .title = FxGuiText::Category::m });

    dTimeSlider.setup({ .parent = *this, .id = dlyPrefix + FxPrKey::Delay::time, .title = FxGuiText::Fx::Delay::time, .isReset = true });
    dTimeSlider.setWantsKeyboardFocus(true);
    dTimeSlider.setExplicitFocusOrder(++tabOrder);

    dFbSlider.setup({ .parent = *this, .id = dlyPrefix + FxPrKey::Delay::fb, .title = FxGuiText::Fx::Delay::fb, .isReset = true });
    dFbSlider.setWantsKeyboardFocus(true);
    dFbSlider.setExplicitFocusOrder(++tabOrder);

    dMixCat.setup({ .parent = *this, .title = FxGuiText::Category::mix });

    dMixSlider.setup({ .parent = *this, .id = dlyPrefix + FxPrKey::mix, .title = FxGuiText::Fx::mix, .isReset = true });
    dMixSlider.setWantsKeyboardFocus(true);
    dMixSlider.setExplicitFocusOrder(++tabOrder);

    dDryBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::dry });
    dDryBtn.setWantsKeyboardFocus(true);
    dDryBtn.setExplicitFocusOrder(++tabOrder);
    dDryBtn.onClick = [&] { dMixSlider.setValue(0.0f); };

    dHalfBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::mix });
    dHalfBtn.setWantsKeyboardFocus(true);
    dHalfBtn.setExplicitFocusOrder(++tabOrder);
    dHalfBtn.onClick = [&] { dMixSlider.setValue(0.5f); };

    dWetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::wet });
    dWetBtn.setWantsKeyboardFocus(true);
    dWetBtn.setExplicitFocusOrder(++tabOrder);
    dWetBtn.onClick = [&] { dMixSlider.setValue(1.0f); };

    // Reverb Group
	reverbGroup.setup(*this, FxGuiText::Group::fxReverb);
    reverbGroup.setBackgroundColor(groupBgColour);
    const juce::String rvbPrefix = code + FxPrKey::rvb;

    rBypassBtn.setup({ .parent = *this, .id = rvbPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    rBypassBtn.setWantsKeyboardFocus(true);
    rBypassBtn.setExplicitFocusOrder(++tabOrder);

    rMainCat.setup({ .parent = *this, .title = FxGuiText::Category::m });

    rSizeSlider.setup({ .parent = *this, .id = rvbPrefix + FxPrKey::Reverb::size, .title = FxGuiText::Fx::Reverb::size, .isReset = true });
    rSizeSlider.setWantsKeyboardFocus(true);
    rSizeSlider.setExplicitFocusOrder(++tabOrder);

    rDampSlider.setup({ .parent = *this, .id = rvbPrefix + FxPrKey::Reverb::damp, .title = FxGuiText::Fx::Reverb::damp, .isReset = true });
    rDampSlider.setWantsKeyboardFocus(true);
    rDampSlider.setExplicitFocusOrder(++tabOrder);

    rMixCat.setup({ .parent = *this, .title = FxGuiText::Category::mix });

    rMixSlider.setup({ .parent = *this, .id = rvbPrefix + FxPrKey::mix, .title = FxGuiText::Fx::mix, .isReset = true });
    rMixSlider.setWantsKeyboardFocus(true);
    rMixSlider.setExplicitFocusOrder(++tabOrder);

    rDryBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::dry });
    rDryBtn.setWantsKeyboardFocus(true);
    rDryBtn.setExplicitFocusOrder(++tabOrder);
    rDryBtn.onClick = [&] { rMixSlider.setValue(0.0f); };

    rHalfBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::mix });
    rHalfBtn.setWantsKeyboardFocus(true);
    rHalfBtn.setExplicitFocusOrder(++tabOrder);
    rHalfBtn.onClick = [&] { rMixSlider.setValue(0.5f); };

    rWetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::wet });
    rWetBtn.setWantsKeyboardFocus(true);
    rWetBtn.setExplicitFocusOrder(++tabOrder);
    rWetBtn.onClick = [&] { rMixSlider.setValue(1.0f); };
}

void GuiFx::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto fxArea = pageArea.removeFromLeft(FxGuiValue::Fx::MainWidth);

    auto mainArea = fxArea.removeFromTop(FxGuiValue::Fx::MainHeight);

    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    mRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutMain({ .mainRect = mRect, .component = &bypassToggle });
    layoutMain({ .mainRect = mRect, .component = &resetBtn });

    auto topCol = fxArea.removeFromTop(FxGuiValue::Fx::AreaHeightBig);
    pageArea.removeFromTop(FxGuiValue::PaddingBottom::block);
    auto centerCol = fxArea.removeFromTop(FxGuiValue::Fx::AreaHeight);
    pageArea.removeFromTop(FxGuiValue::PaddingBottom::block);
    auto bottomCol = fxArea.removeFromTop(FxGuiValue::Fx::AreaHeight);

    // 1st Row

    // Filter
    auto flArea = topCol.removeFromLeft(FxGuiValue::Fx::AreaWidth);

    filterGroup.setBounds(flArea);

    auto flRect = flArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    flRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = flRect, .component = &flBypassBtn });
    layoutRowCategory({ .rowRect = flRect, .label = &flMainCat });
    layoutRow({ .rowRect = flRect, .label = &flTypeSelector.label, .component = &flTypeSelector, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = flRect, .label = &flFreqSlider.label, .component = &flFreqSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = flRect, .label = &flQSlider.label, .component = &flQSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = flRect, .label = &flMixCat });
    layoutRow({ .rowRect = flRect, .label = &flMixSlider.label, .component = &flMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = flRect, .comp1 = &flDryBtn, .comp2 = &flHalfBtn, .comp3 = &flWetBtn, .paddingBottom = 0, .compWidth = FxGuiValue::Fx::MixBtnWidth });

    // Tremolo
    auto trmArea = topCol.removeFromLeft(FxGuiValue::Fx::AreaWidth);

    tremGroup.setBounds(trmArea);

    auto trmRect = trmArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    trmRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = trmRect, .component = &tBypassBtn });
    layoutRowCategory({ .rowRect = trmRect, .label = &tMainCat });
    layoutRow({ .rowRect = trmRect, .label = &tRateSlider.label, .component = &tRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = trmRect, .label = &tDepthSlider.label, .component = &tDepthSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = trmRect, .label = &tMixCat });
    layoutRow({ .rowRect = trmRect, .label = &tMixSlider.label, .component = &tMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = trmRect, .comp1 = &tDryBtn, .comp2 = &tHalfBtn, .comp3 = &tWetBtn, .paddingBottom = 0, .compWidth = FxGuiValue::Fx::MixBtnWidth });

    // Vibrato
    auto vibArea = centerCol.removeFromLeft(FxGuiValue::Fx::AreaWidth);

    vibGroup.setBounds(vibArea);

    auto vibRect = vibArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    vibRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = vibRect, .component = &vBypassBtn });
    layoutRowCategory({ .rowRect = vibRect, .label = &vMainCat });
    layoutRow({ .rowRect = vibRect, .label = &vRateSlider.label, .component = &vRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = vibRect, .label = &vDepthSlider.label, .component = &vDepthSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = vibRect, .label = &vMixCat });
    layoutRow({ .rowRect = vibRect, .label = &vMixSlider.label, .component = &vMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = vibRect, .comp1 = &vDryBtn, .comp2 = &vHalfBtn, .comp3 = &vWetBtn, .paddingBottom = 0, .compWidth = FxGuiValue::Fx::MixBtnWidth });


    // 2nd Row

    // Modern Bit Crusher
    auto mbcArea = centerCol.removeFromLeft(FxGuiValue::Fx::AreaWidth);

    mbcGroup.setBounds(mbcArea);

    auto mbcRect = mbcArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    mbcRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = mbcRect, .component = &mbcBypassBtn });
    layoutRowCategory({ .rowRect = mbcRect, .label = &mbcMainCat });
    layoutRow({ .rowRect = mbcRect, .label = &mbcBitsSlider.label, .component = &mbcBitsSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = mbcRect, .label = &mbcRateSlider.label, .component = &mbcRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = mbcRect, .label = &mbcMixCat });
    layoutRow({ .rowRect = mbcRect, .label = &mbcMixSlider.label, .component = &mbcMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = mbcRect, .comp1 = &mbcDryBtn, .comp2 = &mbcHalfBtn, .comp3 = &mbcWetBtn, .paddingBottom = 0, .compWidth = FxGuiValue::Fx::MixBtnWidth });

    // Delay
    auto dlyArea = bottomCol.removeFromLeft(FxGuiValue::Fx::AreaWidth);

    delayGroup.setBounds(dlyArea);

    auto dlyRect = dlyArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    dlyRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = dlyRect, .component = &dBypassBtn });
    layoutRowCategory({ .rowRect = dlyRect, .label = &dMainCat });
    layoutRow({ .rowRect = dlyRect, .label = &dTimeSlider.label, .component = &dTimeSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = dlyRect, .label = &dFbSlider.label, .component = &dFbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = dlyRect, .label = &dMixCat });
    layoutRow({ .rowRect = dlyRect, .label = &dMixSlider.label, .component = &dMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = dlyRect, .comp1 = &dDryBtn, .comp2 = &dHalfBtn, .comp3 = &dWetBtn, .paddingBottom = 0, .compWidth = FxGuiValue::Fx::MixBtnWidth });

    // Reverb
    auto rvbArea = bottomCol.removeFromLeft(FxGuiValue::Fx::AreaWidth);

    reverbGroup.setBounds(rvbArea);

    auto rvbRect = rvbArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    rvbRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = rvbRect, .component = &rBypassBtn });
    layoutRowCategory({ .rowRect = rvbRect, .label = &rMainCat });
    layoutRow({ .rowRect = rvbRect, .label = &rSizeSlider.label, .component = &rSizeSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = rvbRect, .label = &rDampSlider.label, .component = &rDampSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = rvbRect, .label = &rMixCat });
    layoutRow({ .rowRect = rvbRect, .label = &rMixSlider.label, .component = &rMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = rvbRect, .comp1 = &rDryBtn, .comp2 = &rHalfBtn, .comp3 = &rWetBtn, .paddingBottom = 0, .compWidth = FxGuiValue::Fx::MixBtnWidth });
}
