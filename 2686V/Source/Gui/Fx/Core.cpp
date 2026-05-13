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
    const juce::String code = PrKey::prefix;
    int tabOrder = 1;
    const juce::Colour groupBgColour = juce::Colours::darkblue.withAlpha(0.25f);

    // MainGroup
    mainGroup.setup(*this, GuiText::Group::mainGroup);
    mainGroup.setBackgroundColor(groupBgColour);

	bypassToggle.setup({ .parent = *this, .id = code + PrKey::bypass, .title = GuiText::Fx::masterBypass, .isReset = true });
    bypassToggle.setWantsKeyboardFocus(true);
    bypassToggle.setExplicitFocusOrder(++tabOrder);

    resetBtn.setup({ .parent = *this, .title = GuiText::Fx::reset });
    resetBtn.setWantsKeyboardFocus(true);
    resetBtn.setExplicitFocusOrder(++tabOrder);
    resetBtn.onClick = [&] { this->ctx.audioProcessor.initParams("FX_"); };

    // Filter Group
    filterGroup.setup(*this, GuiText::Group::fxFilter);
    filterGroup.setBackgroundColor(groupBgColour);
    const juce::String filterPrefix = code + PrKey::fil;

    flBypassBtn.setup({ .parent = *this, .id = filterPrefix + PrKey::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    flBypassBtn.setWantsKeyboardFocus(true);
    flBypassBtn.setExplicitFocusOrder(++tabOrder);

    flMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    flTypeSelector.setup({ .parent = *this, .id = filterPrefix + PrKey::Filter::type, .title = GuiText::Fx::Filter::type, .items = flTypeItems, .isReset = true });
    flTypeSelector.setWantsKeyboardFocus(true);
    flTypeSelector.setExplicitFocusOrder(++tabOrder);

    flFreqSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::Filter::freq, .title = GuiText::Fx::Filter::freq, .isReset = true });
    flFreqSlider.setWantsKeyboardFocus(true);
    flFreqSlider.setExplicitFocusOrder(++tabOrder);

    flQSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::Filter::q, .title = GuiText::Fx::Filter::q, .isReset = true });
    flQSlider.setWantsKeyboardFocus(true);
    flQSlider.setExplicitFocusOrder(++tabOrder);

    flMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    flMixSlider.setup({ .parent = *this, .id = filterPrefix + PrKey::mix, .title = GuiText::Fx::mix, .isReset = true });
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

    // Tremolo Group
	tremGroup.setup(*this, GuiText::Group::fxTremolo);
    tremGroup.setBackgroundColor(groupBgColour);
    const juce::String trmPrefix = code + PrKey::trm;

    tBypassBtn.setup({ .parent = *this, .id = trmPrefix + PrKey::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    tBypassBtn.setWantsKeyboardFocus(true);
    tBypassBtn.setExplicitFocusOrder(++tabOrder);

    tMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    tRateSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::Tremolo::rate, .title = GuiText::Fx::Tremolo::rate, .isReset = true });
    tRateSlider.setWantsKeyboardFocus(true);
    tRateSlider.setExplicitFocusOrder(++tabOrder);

    tDepthSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::Tremolo::depth, .title = GuiText::Fx::Tremolo::depth, .isReset = true });
    tDepthSlider.setWantsKeyboardFocus(true);
    tDepthSlider.setExplicitFocusOrder(++tabOrder);

    tMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    tMixSlider.setup({ .parent = *this, .id = trmPrefix + PrKey::mix, .title = GuiText::Fx::mix, .isReset = true });
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
    vibGroup.setBackgroundColor(groupBgColour);
    const juce::String vibPrefix = code + PrKey::vib;

    vBypassBtn.setup({ .parent = *this, .id = vibPrefix + PrKey::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    vBypassBtn.setWantsKeyboardFocus(true);
    vBypassBtn.setExplicitFocusOrder(++tabOrder);

    vMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    vRateSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::Vibrato::rate, .title = GuiText::Fx::Vibrate::rate, .isReset = true });
    vRateSlider.setWantsKeyboardFocus(true);
    vRateSlider.setExplicitFocusOrder(++tabOrder);

    vDepthSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::Vibrato::depth, .title = GuiText::Fx::Vibrate::depth, .isReset = true });
    vDepthSlider.setWantsKeyboardFocus(true);
    vDepthSlider.setExplicitFocusOrder(++tabOrder);

    vMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    vMixSlider.setup({ .parent = *this, .id = vibPrefix + PrKey::mix, .title = GuiText::Fx::mix, .isReset = true });
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
    mbcGroup.setBackgroundColor(groupBgColour);
    const juce::String mbcPrefix = code + PrKey::mbc;

    mbcBypassBtn.setup({ .parent = *this, .id = mbcPrefix + PrKey::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    mbcBypassBtn.setWantsKeyboardFocus(true);
    mbcBypassBtn.setExplicitFocusOrder(++tabOrder);

    mbcMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    mbcBitsSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::Mbc::bit, .title = GuiText::Fx::Mbc::bit, .isReset = true });
    mbcBitsSlider.setWantsKeyboardFocus(true);
    mbcBitsSlider.setExplicitFocusOrder(++tabOrder);

    mbcRateSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::Mbc::rate, .title = GuiText::Fx::Mbc::rate, .isReset = true });
    mbcRateSlider.setWantsKeyboardFocus(true);
    mbcRateSlider.setExplicitFocusOrder(++tabOrder);

    mbcMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    mbcMixSlider.setup({ .parent = *this, .id = mbcPrefix + PrKey::mix, .title = GuiText::Fx::mix, .isReset = true });
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
    delayGroup.setBackgroundColor(groupBgColour);
    const juce::String dlyPrefix = code + PrKey::dly;

    dBypassBtn.setup({ .parent = *this, .id = dlyPrefix + PrKey::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    dBypassBtn.setWantsKeyboardFocus(true);
    dBypassBtn.setExplicitFocusOrder(++tabOrder);

    dMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    dTimeSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::Delay::time, .title = GuiText::Fx::Delay::time, .isReset = true });
    dTimeSlider.setWantsKeyboardFocus(true);
    dTimeSlider.setExplicitFocusOrder(++tabOrder);

    dFbSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::Delay::fb, .title = GuiText::Fx::Delay::fb, .isReset = true });
    dFbSlider.setWantsKeyboardFocus(true);
    dFbSlider.setExplicitFocusOrder(++tabOrder);

    dMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    dMixSlider.setup({ .parent = *this, .id = dlyPrefix + PrKey::mix, .title = GuiText::Fx::mix, .isReset = true });
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
    reverbGroup.setBackgroundColor(groupBgColour);
    const juce::String rvbPrefix = code + PrKey::rvb;

    rBypassBtn.setup({ .parent = *this, .id = rvbPrefix + PrKey::bypass, .title = GuiText::Fx::bypass, .isReset = true });
    rBypassBtn.setWantsKeyboardFocus(true);
    rBypassBtn.setExplicitFocusOrder(++tabOrder);

    rMainCat.setup({ .parent = *this, .title = GuiText::Category::m });

    rSizeSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::Reverb::size, .title = GuiText::Fx::Reverb::size, .isReset = true });
    rSizeSlider.setWantsKeyboardFocus(true);
    rSizeSlider.setExplicitFocusOrder(++tabOrder);

    rDampSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::Reverb::damp, .title = GuiText::Fx::Reverb::damp, .isReset = true });
    rDampSlider.setWantsKeyboardFocus(true);
    rDampSlider.setExplicitFocusOrder(++tabOrder);

    rMixCat.setup({ .parent = *this, .title = GuiText::Category::mix });

    rMixSlider.setup({ .parent = *this, .id = rvbPrefix + PrKey::mix, .title = GuiText::Fx::mix, .isReset = true });
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
}

void GuiFx::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto fxArea = pageArea.removeFromLeft(GuiValue::Fx::MainWidth);

    auto mainArea = fxArea.removeFromTop(GuiValue::Fx::MainHeight);

    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutMain({ .mainRect = mRect, .component = &bypassToggle });
    layoutMain({ .mainRect = mRect, .component = &resetBtn });

    auto topCol = fxArea.removeFromTop(GuiValue::Fx::AreaHeightBig);
    pageArea.removeFromTop(GuiValue::PaddingBottom::block);
    auto centerCol = fxArea.removeFromTop(GuiValue::Fx::AreaHeight);
    pageArea.removeFromTop(GuiValue::PaddingBottom::block);
    auto bottomCol = fxArea.removeFromTop(GuiValue::Fx::AreaHeight);

    // 1st Row

    // Filter
    auto flArea = topCol.removeFromLeft(GuiValue::Fx::AreaWidth);

    filterGroup.setBounds(flArea);

    auto flRect = flArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    flRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = flRect, .component = &flBypassBtn });
    layoutRowCategory({ .rowRect = flRect, .label = &flMainCat });
    layoutRow({ .rowRect = flRect, .label = &flTypeSelector.label, .component = &flTypeSelector, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = flRect, .label = &flFreqSlider.label, .component = &flFreqSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = flRect, .label = &flQSlider.label, .component = &flQSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = flRect, .label = &flMixCat });
    layoutRow({ .rowRect = flRect, .label = &flMixSlider.label, .component = &flMixSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = flRect, .comp1 = &flDryBtn, .comp2 = &flHalfBtn, .comp3 = &flWetBtn, .paddingBottom = 0, .compWidth = GuiValue::Fx::MixBtnWidth });

    // Tremolo
    auto trmArea = topCol.removeFromLeft(GuiValue::Fx::AreaWidth);

    tremGroup.setBounds(trmArea);

    auto trmRect = trmArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    trmRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = trmRect, .component = &tBypassBtn });
    layoutRowCategory({ .rowRect = trmRect, .label = &tMainCat });
    layoutRow({ .rowRect = trmRect, .label = &tRateSlider.label, .component = &tRateSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = trmRect, .label = &tDepthSlider.label, .component = &tDepthSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = trmRect, .label = &tMixCat });
    layoutRow({ .rowRect = trmRect, .label = &tMixSlider.label, .component = &tMixSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = trmRect, .comp1 = &tDryBtn, .comp2 = &tHalfBtn, .comp3 = &tWetBtn, .paddingBottom = 0, .compWidth = GuiValue::Fx::MixBtnWidth });

    // Vibrato
    auto vibArea = centerCol.removeFromLeft(GuiValue::Fx::AreaWidth);

    vibGroup.setBounds(vibArea);

    auto vibRect = vibArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    vibRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = vibRect, .component = &vBypassBtn });
    layoutRowCategory({ .rowRect = vibRect, .label = &vMainCat });
    layoutRow({ .rowRect = vibRect, .label = &vRateSlider.label, .component = &vRateSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = vibRect, .label = &vDepthSlider.label, .component = &vDepthSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = vibRect, .label = &vMixCat });
    layoutRow({ .rowRect = vibRect, .label = &vMixSlider.label, .component = &vMixSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = vibRect, .comp1 = &vDryBtn, .comp2 = &vHalfBtn, .comp3 = &vWetBtn, .paddingBottom = 0, .compWidth = GuiValue::Fx::MixBtnWidth });


    // 2nd Row

    // Modern Bit Crusher
    auto mbcArea = centerCol.removeFromLeft(GuiValue::Fx::AreaWidth);

    mbcGroup.setBounds(mbcArea);

    auto mbcRect = mbcArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mbcRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = mbcRect, .component = &mbcBypassBtn });
    layoutRowCategory({ .rowRect = mbcRect, .label = &mbcMainCat });
    layoutRow({ .rowRect = mbcRect, .label = &mbcBitsSlider.label, .component = &mbcBitsSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = mbcRect, .label = &mbcRateSlider.label, .component = &mbcRateSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = mbcRect, .label = &mbcMixCat });
    layoutRow({ .rowRect = mbcRect, .label = &mbcMixSlider.label, .component = &mbcMixSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = mbcRect, .comp1 = &mbcDryBtn, .comp2 = &mbcHalfBtn, .comp3 = &mbcWetBtn, .paddingBottom = 0, .compWidth = GuiValue::Fx::MixBtnWidth });

    // Delay
    auto dlyArea = bottomCol.removeFromLeft(GuiValue::Fx::AreaWidth);

    delayGroup.setBounds(dlyArea);

    auto dlyRect = dlyArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    dlyRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = dlyRect, .component = &dBypassBtn });
    layoutRowCategory({ .rowRect = dlyRect, .label = &dMainCat });
    layoutRow({ .rowRect = dlyRect, .label = &dTimeSlider.label, .component = &dTimeSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = dlyRect, .label = &dFbSlider.label, .component = &dFbSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = dlyRect, .label = &dMixCat });
    layoutRow({ .rowRect = dlyRect, .label = &dMixSlider.label, .component = &dMixSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = dlyRect, .comp1 = &dDryBtn, .comp2 = &dHalfBtn, .comp3 = &dWetBtn, .paddingBottom = 0, .compWidth = GuiValue::Fx::MixBtnWidth });

    // Reverb
    auto rvbArea = bottomCol.removeFromLeft(GuiValue::Fx::AreaWidth);

    reverbGroup.setBounds(rvbArea);

    auto rvbRect = rvbArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rvbRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = rvbRect, .component = &rBypassBtn });
    layoutRowCategory({ .rowRect = rvbRect, .label = &rMainCat });
    layoutRow({ .rowRect = rvbRect, .label = &rSizeSlider.label, .component = &rSizeSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = rvbRect, .label = &rDampSlider.label, .component = &rDampSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowCategory({ .rowRect = rvbRect, .label = &rMixCat });
    layoutRow({ .rowRect = rvbRect, .label = &rMixSlider.label, .component = &rMixSlider, .labelWidth = GuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = rvbRect, .comp1 = &rDryBtn, .comp2 = &rHalfBtn, .comp3 = &rWetBtn, .paddingBottom = 0, .compWidth = GuiValue::Fx::MixBtnWidth });
}
