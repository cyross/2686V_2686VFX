#include "./Core.h"

#include "../../Core/Const/PrKeys.h"
#include "../../Core/Const/PrValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "../../Core/Gui/GuiValues.h"
#include "../../Core/Gui/GuiText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> flTypeItems = {
    {.name = "LPF (Low Pass)",  .value = 1 },
    {.name = "HPF (High Pass)", .value = 2 },
    {.name = "BPF (Band Pass)", .value = 3 }
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
}

void GuiFx::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(GuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutMainCategory({ .mainRect = mRect, .label = &bypassCat });
    layoutMain({ .mainRect = mRect, .component = &bypassToggle });
    layoutMainCategory({ .mainRect = mRect, .label = &mvolCat });
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

    layoutRowCategory({ .rowRect = flRect, .label = &flBypassCat });
    layoutRow({ .rowRect = flRect, .component = &flBypassBtn });
    layoutRowCategory({ .rowRect = flRect, .label = &flMainCat });
    layoutRow({ .rowRect = flRect, .label = &flTypeSelector.label, .component = &flTypeSelector });
    layoutRow({ .rowRect = flRect, .label = &flFreqSlider.label, .component = &flFreqSlider });
    layoutRow({ .rowRect = flRect, .label = &flQSlider.label, .component = &flQSlider });
    layoutRowCategory({ .rowRect = flRect, .label = &flMixCat });
    layoutRow({ .rowRect = flRect, .label = &flMixSlider.label, .component = &flMixSlider });
    layoutRowThreeComps({ .rect = flRect, .comp1 = &flDryBtn, .comp2 = &flHalfBtn, .comp3 = &flWetBtn, .paddingBottom = 0});

    // Tremolo
    auto trmArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    tremGroup.setBounds(trmArea);

    auto trmRect = trmArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    trmRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRowCategory({ .rowRect = trmRect, .label = &tBypassCat });
    layoutRow({ .rowRect = trmRect, .component = &tBypassBtn });
    layoutRowCategory({ .rowRect = trmRect, .label = &tMainCat });
    layoutRow({ .rowRect = trmRect, .label = &tRateSlider.label, .component = &tRateSlider });
    layoutRow({ .rowRect = trmRect, .label = &tDepthSlider.label, .component = &tDepthSlider });
    layoutRowCategory({ .rowRect = trmRect, .label = &tMixCat });
    layoutRow({ .rowRect = trmRect, .label = &tMixSlider.label, .component = &tMixSlider });
    layoutRowThreeComps({ .rect = trmRect, .comp1 = &tDryBtn, .comp2 = &tHalfBtn, .comp3 = &tWetBtn, .paddingBottom = 0 });

    // Vibrato
    auto vibArea = topCol.removeFromLeft(GuiValue::Fm::Op::width);

    vibGroup.setBounds(vibArea);

    auto vibRect = vibArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    vibRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRowCategory({ .rowRect = vibRect, .label = &vBypassCat });
    layoutRow({ .rowRect = vibRect, .component = &vBypassBtn });
    layoutRowCategory({ .rowRect = vibRect, .label = &vMainCat });
    layoutRow({ .rowRect = vibRect, .label = &vRateSlider.label, .component = &vRateSlider });
    layoutRow({ .rowRect = vibRect, .label = &vDepthSlider.label, .component = &vDepthSlider });
    layoutRowCategory({ .rowRect = vibRect, .label = &vMixCat });
    layoutRow({ .rowRect = vibRect, .label = &vMixSlider.label, .component = &vMixSlider });
    layoutRowThreeComps({ .rect = vibRect, .comp1 = &vDryBtn, .comp2 = &vHalfBtn, .comp3 = &vWetBtn, .paddingBottom = 0 });


    // 2nd Row

    // Modern Bit Crusher
    auto mbcArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    mbcGroup.setBounds(mbcArea);

    auto mbcRect = mbcArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mbcRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRowCategory({ .rowRect = mbcRect, .label = &mbcBypassCat });
    layoutRow({ .rowRect = mbcRect, .component = &mbcBypassBtn });
    layoutRowCategory({ .rowRect = mbcRect, .label = &mbcMainCat });
    layoutRow({ .rowRect = mbcRect, .label = &mbcBitsSlider.label, .component = &mbcBitsSlider });
    layoutRow({ .rowRect = mbcRect, .label = &mbcRateSlider.label, .component = &mbcRateSlider });
    layoutRowCategory({ .rowRect = mbcRect, .label = &mbcMixCat });
    layoutRow({ .rowRect = mbcRect, .label = &mbcMixSlider.label, .component = &mbcMixSlider });
    layoutRowThreeComps({ .rect = mbcRect, .comp1 = &mbcDryBtn, .comp2 = &mbcHalfBtn, .comp3 = &mbcWetBtn, .paddingBottom = 0 });

    // Delay
    auto dlyArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    delayGroup.setBounds(dlyArea);

    auto dlyRect = dlyArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    dlyRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRowCategory({ .rowRect = dlyRect, .label = &dBypassCat });
    layoutRow({ .rowRect = dlyRect, .component = &dBypassBtn });
    layoutRowCategory({ .rowRect = dlyRect, .label = &dMainCat });
    layoutRow({ .rowRect = dlyRect, .label = &dTimeSlider.label, .component = &dTimeSlider });
    layoutRow({ .rowRect = dlyRect, .label = &dFbSlider.label, .component = &dFbSlider });
    layoutRowCategory({ .rowRect = dlyRect, .label = &dMixCat });
    layoutRow({ .rowRect = dlyRect, .label = &dMixSlider.label, .component = &dMixSlider });
    layoutRowThreeComps({ .rect = dlyRect, .comp1 = &dDryBtn, .comp2 = &dHalfBtn, .comp3 = &dWetBtn, .paddingBottom = 0 });

    // Reverb
    auto rvbArea = centerCol.removeFromLeft(GuiValue::Fm::Op::width);

    reverbGroup.setBounds(rvbArea);

    auto rvbRect = rvbArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    rvbRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    layoutRowCategory({ .rowRect = rvbRect, .label = &rBypassCat });
    layoutRow({ .rowRect = rvbRect, .component = &rBypassBtn });
    layoutRowCategory({ .rowRect = rvbRect, .label = &rMainCat });
    layoutRow({ .rowRect = rvbRect, .label = &rSizeSlider.label, .component = &rSizeSlider });
    layoutRow({ .rowRect = rvbRect, .label = &rDampSlider.label, .component = &rDampSlider });
    layoutRowCategory({ .rowRect = rvbRect, .label = &rMixCat });
    layoutRow({ .rowRect = rvbRect, .label = &rMixSlider.label, .component = &rMixSlider });
    layoutRowThreeComps({ .rect = rvbRect, .comp1 = &rDryBtn, .comp2 = &rHalfBtn, .comp3 = &rWetBtn, .paddingBottom = 0 });
}
