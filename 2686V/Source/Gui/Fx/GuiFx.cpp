#include "./GuiFx.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Fx/ProcessorFxKeys.h"
#include "../../Processor/Fx/ProcessorFxValues.h"
#include "../../Core/Const/ConstFileValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiFxValues.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> flTypeItems = {
    {.name = "LPF", .value = 1 },
    {.name = "HPF", .value = 2 },
    {.name = "BPF", .value = 3 }
};

GuiFx::GuiFx(const GuiContext& context) :
    GuiBase(context),
    mainGroup(context),
    tremGroup(context),
    vibGroup(context),
    mbcGroup(context),
    delayGroup(context),
    reverbGroup(context),
    filterGroup(context),
    eq3bGroup(context),
    sfceGroup(context),
    bypassToggle(context),
    mainSeparator(context),
    resetBtn(context),
    routeSeparator(context),
    showRouteBtn(context),
    routeFx{ GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context) },
    routeUp{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
    routeDown{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
    fileSeparator(context),
    importFxOrderBtn(context),
    exportFxOrderBtn(context),
    importFxParamBtn(context),
    exportFxParamBtn(context),
    tBypassBtn(context),
    tSeparator(context),
    tRateSlider(context),
    tDepthSlider(context),
    tMixSlider(context),
    tDryBtn(context),
    tHalfBtn(context),
    tWetBtn(context),
    vBypassBtn(context),
    vSeparator(context),
    vRateSlider(context),
    vDepthSlider(context),
    vMixSlider(context),
    vDryBtn(context),
    vHalfBtn(context),
    vWetBtn(context),
    mbcBypassBtn(context),
    mbcSeparator(context),
    mbcRateSlider(context),
    mbcBitsSlider(context),
    mbcMixSlider(context),
    mbcDryBtn(context),
    mbcHalfBtn(context),
    mbcWetBtn(context),
    dBypassBtn(context),
    dSeparator(context),
    dTimeSlider(context),
    dFbSlider(context),
    dMixSlider(context),
    dDryBtn(context),
    dHalfBtn(context),
    dWetBtn(context),
    rBypassBtn(context),
    rSeparator(context),
    rSizeSlider(context),
    rDampSlider(context),
    rMixSlider(context),
    rDryBtn(context),
    rHalfBtn(context),
    rWetBtn(context),
    flBypassBtn(context),
    flSeparator(context),
    flTypeSelector(context),
    flFreqSlider(context),
    flQSlider(context),
    flMixSlider(context),
    flDryBtn(context),
    flHalfBtn(context),
    flWetBtn(context),
    eq3bBypassBtn(context),
    eq3bSeparator(context),
    eq3bLowGainDbSlider(context),
    eq3bMidFreqSlider(context),
    eq3bMidGainDbSlider(context),
    eq3bHighGainDbSlider(context),
    eq3bMixSlider(context),
    eq3bDryBtn(context),
    eq3bHalfBtn(context),
    eq3bWetBtn(context),
    sfceBypassBtn(context),
    sfceSeparator(context),
    sfceTimeSlider(context),
    sfceFbSlider(context),
    sfceFirCoef0Slider(context),
    sfceFirCoef1Slider(context),
    sfceFirCoef2Slider(context),
    sfceFirCoef3Slider(context),
    sfceFirCoef4Slider(context),
    sfceFirCoef5Slider(context),
    sfceFirCoef6Slider(context),
    sfceFirCoef7Slider(context),
    sfceMixSlider(context),
    sfceDryBtn(context),
    sfceHalfBtn(context),
    sfceWetBtn(context)
{
    setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    order = ctx.audioProcessor.getFxOrder();
}

void GuiFx::setup()
{
    const juce::String code = FxPrKey::prefix;
    int tabOrder = 1;
    const juce::Colour groupBgColour = juce::Colours::darkblue.darker(0.3f).withAlpha(0.5f);

    // MainGroup
    mainGroup.setup(*this, FxGuiText::Group::mainGroup);
    mainGroup.setBackgroundColor(groupBgColour);

	bypassToggle.setup({ .parent = *this, .id = code + FxPrKey::bypass, .title = FxGuiText::Fx::masterBypass, .isReset = true });
    bypassToggle.setWantsKeyboardFocus(true);
    bypassToggle.setExplicitFocusOrder(++tabOrder);

    addAndMakeVisible(mainSeparator);
    mainSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    resetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::reset, .textColor = juce::Colours::white, .bgColor = juce::Colours::grey });
    resetBtn.setWantsKeyboardFocus(true);
    resetBtn.setExplicitFocusOrder(++tabOrder);
    resetBtn.onClick = [&] { this->ctx.audioProcessor.initParams("FX_"); };

    addAndMakeVisible(routeSeparator);
    routeSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    showRouteBtn.setup({ .parent = *this, .title = juce::String("") + "設定", .textColor = juce::Colours::white, .bgColor = juce::Colours::darkgoldenrod.darker(0.2f), .isReset = false });
    showRouteBtn.setWantsKeyboardFocus(true);
    showRouteBtn.setExplicitFocusOrder(++tabOrder);
    showRouteBtn.onClick = [this] {
        isShowRoute = !isShowRoute;

        ctx.editor.resized();
        };

    for (int fxr = 0; fxr < NumEffects; fxr++) {
        routeFx[fxr].setup({.parent = *this, .title = effectNames[order[fxr]]});
        routeFx[fxr].setWantsKeyboardFocus(true);
        routeFx[fxr].setExplicitFocusOrder(++tabOrder);

        routeUp[fxr].setup({ .parent = *this, .title = juce::String("") + "▲", .isReset = false});
        routeUp[fxr].setEnabled(fxr != 0);
        routeUp[fxr].setWantsKeyboardFocus(true);
        routeUp[fxr].setExplicitFocusOrder(++tabOrder);
        routeUp[fxr].onClick = [this, fxr] {
            int org = order[fxr - 1];

            order[fxr - 1] = order[fxr];
            order[fxr] = org;

            for (int i = 0; i < NumEffects; i++) {
                routeFx[i].setText(effectNames[order[i]], juce::sendNotification);
            }

            ctx.audioProcessor.updateFxOrder(order);

            ctx.editor.resized();
            };

        routeDown[fxr].setup({ .parent = *this, .title = juce::String("") + "▼", .isReset = false });
        routeDown[fxr].setEnabled(fxr != NumEffects - 1);
        routeDown[fxr].setWantsKeyboardFocus(true);
        routeDown[fxr].setExplicitFocusOrder(++tabOrder);
        routeDown[fxr].onClick = [this, fxr] {
            int org = order[fxr + 1];

            order[fxr + 1] = order[fxr];
            order[fxr] = org;

            for (int i = 0; i < NumEffects; i++) {
                routeFx[i].setText(effectNames[order[i]], juce::sendNotification);
            }

            ctx.audioProcessor.updateFxOrder(order);

            ctx.editor.resized();
            };
    }

    addAndMakeVisible(fileSeparator);
    fileSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

    importFxOrderBtn.setup({ .parent = *this, .title = FxGuiText::Fx::orderFileImport, .textColor = juce::Colours::white.darker(0.2f), .textOnColor = juce::Colours::white, .bgColor = juce::Colours::darkgrey, .isReset = false, .isResized = false });
    importFxOrderBtn.setWantsKeyboardFocus(true);
    importFxOrderBtn.setExplicitFocusOrder(++tabOrder);
    importFxOrderBtn.onClick = [this] { importFxOrder(); };

    exportFxOrderBtn.setup({ .parent = *this, .title = FxGuiText::Fx::orderFileExport, .textColor = juce::Colours::white.darker(0.2f), .textOnColor = juce::Colours::white, .bgColor = juce::Colours::darkgrey.darker(0.5f), .isReset = false, .isResized = false});
    exportFxOrderBtn.setWantsKeyboardFocus(true);
    exportFxOrderBtn.setExplicitFocusOrder(++tabOrder);
    exportFxOrderBtn.onClick = [this] { exportFxOrder(); };

    importFxParamBtn.setup({ .parent = *this, .title = FxGuiText::Fx::paramFileImport, .textColor = juce::Colours::white.darker(0.2f), .textOnColor = juce::Colours::white, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    importFxParamBtn.setWantsKeyboardFocus(true);
    importFxParamBtn.setExplicitFocusOrder(++tabOrder);
    importFxParamBtn.onClick = [this] { importFxParam(); };

    exportFxParamBtn.setup({ .parent = *this, .title = FxGuiText::Fx::paramFileExport, .textColor = juce::Colours::white.darker(0.2f), .textOnColor = juce::Colours::white, .bgColor = juce::Colours::darkgreen.darker(0.5f), .isReset = false, .isResized = false });
    exportFxParamBtn.setWantsKeyboardFocus(true);
    exportFxParamBtn.setExplicitFocusOrder(++tabOrder);
    exportFxParamBtn.onClick = [this] { exportFxParam(); };

    // Filter Group
    filterGroup.setup(*this, FxGuiText::Group::fxFilter);
    filterGroup.setBackgroundColor(groupBgColour);
    const juce::String filterPrefix = code + FxPrKey::fil;

    flBypassBtn.setup({ .parent = *this, .id = filterPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    flBypassBtn.setWantsKeyboardFocus(true);
    flBypassBtn.setExplicitFocusOrder(++tabOrder);
    flBypassBtn.onStateChange = [this] {
        updateFilterEnabled();
        };

    addAndMakeVisible(flSeparator);
    flSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    flTypeSelector.setup({ .parent = *this, .id = filterPrefix + FxPrKey::Filter::type, .title = FxGuiText::Fx::Filter::type, .items = flTypeItems, .isReset = true });
    flTypeSelector.setWantsKeyboardFocus(true);
    flTypeSelector.setExplicitFocusOrder(++tabOrder);

    flFreqSlider.setup({ .parent = *this, .id = filterPrefix + FxPrKey::Filter::freq, .title = FxGuiText::Fx::Filter::freq, .isReset = true });
    flFreqSlider.setWantsKeyboardFocus(true);
    flFreqSlider.setExplicitFocusOrder(++tabOrder);

    flQSlider.setup({ .parent = *this, .id = filterPrefix + FxPrKey::Filter::q, .title = FxGuiText::Fx::Filter::q, .isReset = true });
    flQSlider.setWantsKeyboardFocus(true);
    flQSlider.setExplicitFocusOrder(++tabOrder);

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

    // 3Band EQ Group
    eq3bGroup.setup(*this, FxGuiText::Group::fxEq3B);
    eq3bGroup.setBackgroundColor(groupBgColour);
    const juce::String eq3bPrefix = code + FxPrKey::eq3b;

    eq3bBypassBtn.setup({ .parent = *this, .id = eq3bPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    eq3bBypassBtn.setWantsKeyboardFocus(true);
    eq3bBypassBtn.setExplicitFocusOrder(++tabOrder);
    eq3bBypassBtn.onStateChange = [this] {
        updateEq3bEnabled();
        };

    addAndMakeVisible(eq3bSeparator);
    eq3bSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    eq3bLowGainDbSlider.setup({ .parent = *this, .id = eq3bPrefix + FxPrKey::Eq3b::lowGainDb, .title = FxGuiText::Fx::Eq3b::lowGainDb, .isReset = true });
    eq3bLowGainDbSlider.setWantsKeyboardFocus(true);
    eq3bLowGainDbSlider.setExplicitFocusOrder(++tabOrder);

    eq3bMidFreqSlider.setup({ .parent = *this, .id = eq3bPrefix + FxPrKey::Eq3b::midFreq, .title = FxGuiText::Fx::Eq3b::midFreq, .isReset = true });
    eq3bMidFreqSlider.setWantsKeyboardFocus(true);
    eq3bMidFreqSlider.setExplicitFocusOrder(++tabOrder);

    eq3bMidGainDbSlider.setup({ .parent = *this, .id = eq3bPrefix + FxPrKey::Eq3b::midGainDb, .title = FxGuiText::Fx::Eq3b::midGainDb, .isReset = true });
    eq3bMidGainDbSlider.setWantsKeyboardFocus(true);
    eq3bMidGainDbSlider.setExplicitFocusOrder(++tabOrder);

    eq3bHighGainDbSlider.setup({ .parent = *this, .id = eq3bPrefix + FxPrKey::Eq3b::highGainDb, .title = FxGuiText::Fx::Eq3b::highGainDb, .isReset = true });
    eq3bHighGainDbSlider.setWantsKeyboardFocus(true);
    eq3bHighGainDbSlider.setExplicitFocusOrder(++tabOrder);

    eq3bMixSlider.setup({ .parent = *this, .id = eq3bPrefix + FxPrKey::mix, .title = FxGuiText::Fx::mix, .isReset = true });
    eq3bMixSlider.setWantsKeyboardFocus(true);
    eq3bMixSlider.setExplicitFocusOrder(++tabOrder);

    eq3bDryBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::dry });
    eq3bDryBtn.onClick = [&] { eq3bMixSlider.setValue(0.0f); };
    eq3bDryBtn.setWantsKeyboardFocus(true);
    eq3bDryBtn.setExplicitFocusOrder(++tabOrder);

    eq3bHalfBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::mix });
    eq3bHalfBtn.onClick = [&] { eq3bMixSlider.setValue(0.5f); };
    eq3bHalfBtn.setWantsKeyboardFocus(true);
    eq3bHalfBtn.setExplicitFocusOrder(++tabOrder);

    eq3bWetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::wet });
    eq3bWetBtn.onClick = [&] { eq3bMixSlider.setValue(1.0f); };
    eq3bWetBtn.setWantsKeyboardFocus(true);
    eq3bWetBtn.setExplicitFocusOrder(++tabOrder);

    // Tremolo Group
	tremGroup.setup(*this, FxGuiText::Group::fxTremolo);
    tremGroup.setBackgroundColor(groupBgColour);
    const juce::String trmPrefix = code + FxPrKey::trm;

    tBypassBtn.setup({ .parent = *this, .id = trmPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    tBypassBtn.setWantsKeyboardFocus(true);
    tBypassBtn.setExplicitFocusOrder(++tabOrder);
    tBypassBtn.onStateChange = [this] {
        updateTremoloEnabled();
        };

    addAndMakeVisible(tSeparator);
    tSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    tRateSlider.setup({ .parent = *this, .id = trmPrefix + FxPrKey::Tremolo::rate, .title = FxGuiText::Fx::Tremolo::rate, .isReset = true });
    tRateSlider.setWantsKeyboardFocus(true);
    tRateSlider.setExplicitFocusOrder(++tabOrder);

    tDepthSlider.setup({ .parent = *this, .id = trmPrefix + FxPrKey::Tremolo::depth, .title = FxGuiText::Fx::Tremolo::depth, .isReset = true });
    tDepthSlider.setWantsKeyboardFocus(true);
    tDepthSlider.setExplicitFocusOrder(++tabOrder);

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
    vBypassBtn.onStateChange = [this] {
        updateVibratoEnabled();
        };

    addAndMakeVisible(vSeparator);
    vSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    vRateSlider.setup({ .parent = *this, .id = vibPrefix + FxPrKey::Vibrato::rate, .title = FxGuiText::Fx::Vibrate::rate, .isReset = true });
    vRateSlider.setWantsKeyboardFocus(true);
    vRateSlider.setExplicitFocusOrder(++tabOrder);

    vDepthSlider.setup({ .parent = *this, .id = vibPrefix + FxPrKey::Vibrato::depth, .title = FxGuiText::Fx::Vibrate::depth, .isReset = true });
    vDepthSlider.setWantsKeyboardFocus(true);
    vDepthSlider.setExplicitFocusOrder(++tabOrder);

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
    mbcBypassBtn.onStateChange = [this] {
        updateMBCEnabled();
        };

    addAndMakeVisible(mbcSeparator);
    mbcSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    mbcBitsSlider.setup({ .parent = *this, .id = mbcPrefix + FxPrKey::Mbc::bit, .title = FxGuiText::Fx::Mbc::bit, .isReset = true });
    mbcBitsSlider.setWantsKeyboardFocus(true);
    mbcBitsSlider.setExplicitFocusOrder(++tabOrder);

    mbcRateSlider.setup({ .parent = *this, .id = mbcPrefix + FxPrKey::Mbc::rate, .title = FxGuiText::Fx::Mbc::rate, .isReset = true });
    mbcRateSlider.setWantsKeyboardFocus(true);
    mbcRateSlider.setExplicitFocusOrder(++tabOrder);

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
    dBypassBtn.onStateChange = [this] {
        updateDelayEnabled();
        };

    addAndMakeVisible(dSeparator);
    dSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    dTimeSlider.setup({ .parent = *this, .id = dlyPrefix + FxPrKey::Delay::time, .title = FxGuiText::Fx::Delay::time, .isReset = true });
    dTimeSlider.setWantsKeyboardFocus(true);
    dTimeSlider.setExplicitFocusOrder(++tabOrder);

    dFbSlider.setup({ .parent = *this, .id = dlyPrefix + FxPrKey::Delay::fb, .title = FxGuiText::Fx::Delay::fb, .isReset = true });
    dFbSlider.setWantsKeyboardFocus(true);
    dFbSlider.setExplicitFocusOrder(++tabOrder);

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
    rBypassBtn.onStateChange = [this] {
        updateReverbEnabled();
        };

    addAndMakeVisible(rSeparator);
    rSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    rSizeSlider.setup({ .parent = *this, .id = rvbPrefix + FxPrKey::Reverb::size, .title = FxGuiText::Fx::Reverb::size, .isReset = true });
    rSizeSlider.setWantsKeyboardFocus(true);
    rSizeSlider.setExplicitFocusOrder(++tabOrder);

    rDampSlider.setup({ .parent = *this, .id = rvbPrefix + FxPrKey::Reverb::damp, .title = FxGuiText::Fx::Reverb::damp, .isReset = true });
    rDampSlider.setWantsKeyboardFocus(true);
    rDampSlider.setExplicitFocusOrder(++tabOrder);

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

    // SFC Echo Group
    sfceGroup.setup(*this, FxGuiText::Group::sfcEcho);
    sfceGroup.setBackgroundColor(groupBgColour);
    const juce::String sfcePrefix = code + FxPrKey::sfcEcho;

    sfceBypassBtn.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    sfceBypassBtn.setWantsKeyboardFocus(true);
    sfceBypassBtn.setExplicitFocusOrder(++tabOrder);
    sfceBypassBtn.onStateChange = [this] {
        updateSfcEchoEnabled();
        };

    addAndMakeVisible(sfceSeparator);
    sfceSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    sfceTimeSlider.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::SfcEcho::time, .title = FxGuiText::Fx::SfcEcho::time, .isReset = true });
    sfceTimeSlider.setWantsKeyboardFocus(true);
    sfceTimeSlider.setExplicitFocusOrder(++tabOrder);

    sfceFbSlider.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::SfcEcho::fb, .title = FxGuiText::Fx::SfcEcho::fb, .isReset = true });
    sfceFbSlider.setWantsKeyboardFocus(true);
    sfceFbSlider.setExplicitFocusOrder(++tabOrder);

    sfceFirCoef0Slider.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::SfcEcho::firCoef0, .title = FxGuiText::Fx::SfcEcho::firCoef0, .isReset = true });
    sfceFirCoef0Slider.setWantsKeyboardFocus(true);
    sfceFirCoef0Slider.setExplicitFocusOrder(++tabOrder);

    sfceFirCoef1Slider.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::SfcEcho::firCoef1, .title = FxGuiText::Fx::SfcEcho::firCoef1, .isReset = true });
    sfceFirCoef1Slider.setWantsKeyboardFocus(true);
    sfceFirCoef1Slider.setExplicitFocusOrder(++tabOrder);

    sfceFirCoef2Slider.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::SfcEcho::firCoef2, .title = FxGuiText::Fx::SfcEcho::firCoef2, .isReset = true });
    sfceFirCoef2Slider.setWantsKeyboardFocus(true);
    sfceFirCoef2Slider.setExplicitFocusOrder(++tabOrder);

    sfceFirCoef3Slider.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::SfcEcho::firCoef3, .title = FxGuiText::Fx::SfcEcho::firCoef3, .isReset = true });
    sfceFirCoef3Slider.setWantsKeyboardFocus(true);
    sfceFirCoef3Slider.setExplicitFocusOrder(++tabOrder);

    sfceFirCoef4Slider.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::SfcEcho::firCoef4, .title = FxGuiText::Fx::SfcEcho::firCoef4, .isReset = true });
    sfceFirCoef4Slider.setWantsKeyboardFocus(true);
    sfceFirCoef4Slider.setExplicitFocusOrder(++tabOrder);

    sfceFirCoef5Slider.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::SfcEcho::firCoef5, .title = FxGuiText::Fx::SfcEcho::firCoef5, .isReset = true });
    sfceFirCoef5Slider.setWantsKeyboardFocus(true);
    sfceFirCoef5Slider.setExplicitFocusOrder(++tabOrder);

    sfceFirCoef6Slider.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::SfcEcho::firCoef6, .title = FxGuiText::Fx::SfcEcho::firCoef6, .isReset = true });
    sfceFirCoef6Slider.setWantsKeyboardFocus(true);
    sfceFirCoef6Slider.setExplicitFocusOrder(++tabOrder);

    sfceFirCoef7Slider.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::SfcEcho::firCoef7, .title = FxGuiText::Fx::SfcEcho::firCoef7, .isReset = true });
    sfceFirCoef7Slider.setWantsKeyboardFocus(true);
    sfceFirCoef7Slider.setExplicitFocusOrder(++tabOrder);

    sfceMixSlider.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::mix, .title = FxGuiText::Fx::mix, .isReset = true });
    sfceMixSlider.setWantsKeyboardFocus(true);
    sfceMixSlider.setExplicitFocusOrder(++tabOrder);

    sfceDryBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::dry });
    sfceDryBtn.setWantsKeyboardFocus(true);
    sfceDryBtn.setExplicitFocusOrder(++tabOrder);
    sfceDryBtn.onClick = [&] { sfceMixSlider.setValue(0.0f); };

    sfceHalfBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::mix });
    sfceHalfBtn.setWantsKeyboardFocus(true);
    sfceHalfBtn.setExplicitFocusOrder(++tabOrder);
    sfceHalfBtn.onClick = [&] { sfceMixSlider.setValue(0.5f); };

    sfceWetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::wet });
    sfceWetBtn.setWantsKeyboardFocus(true);
    sfceWetBtn.setExplicitFocusOrder(++tabOrder);
    sfceWetBtn.onClick = [&] { sfceMixSlider.setValue(1.0f); };

    updateFilterEnabled();
    updateTremoloEnabled();
    updateVibratoEnabled();
    updateMBCEnabled();
    updateDelayEnabled();
    updateReverbEnabled();
    updateEq3bEnabled();
    updateSfcEchoEnabled();
}

void GuiFx::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    auto fxArea = pageArea.removeFromLeft(FxGuiValue::Fx::MainWidth);

    auto mainArea = fxArea.removeFromTop(isShowRoute ? FxGuiValue::Fx::MainHeightRoute : FxGuiValue::Fx::MainHeight);

    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    mRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutMain({ .mainRect = mRect, .component = &bypassToggle });

    // 区切り線エリアを確保
    auto separatorArea = mRect.removeFromTop(FxGuiValue::Fx::SeparatorHeight);
    mainSeparator.setBounds(separatorArea);

    layoutMain({ .mainRect = mRect, .component = &resetBtn });

    layoutFxOrder(mRect);

    auto row1 = fxArea.removeFromTop(FxGuiValue::Fx::AreaHeightRow1);
    pageArea.removeFromTop(FxGuiValue::PaddingBottom::block);
    auto row2 = fxArea.removeFromTop(FxGuiValue::Fx::AreaHeightRow2);
    pageArea.removeFromTop(FxGuiValue::PaddingBottom::block);
    auto row3 = fxArea.removeFromTop(FxGuiValue::Fx::AreaHeightRow3);
    pageArea.removeFromTop(FxGuiValue::PaddingBottom::block);
    auto row4 = fxArea.removeFromTop(FxGuiValue::Fx::AreaHeightRow4);

    // Filter
    auto rect1 = row1.removeFromLeft(FxGuiValue::Fx::AreaWidth);
    auto flArea = rect1.removeFromTop(FxGuiValue::Fx::HeightFilter);

    filterGroup.setBounds(flArea);

    auto flRect = flArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    flRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = flRect, .component = &flBypassBtn });

    // 区切り線エリアを確保
    auto flSprArea = flRect.removeFromTop(FxGuiValue::Fx::SeparatorHeight);
    flSeparator.setBounds(flSprArea);

    layoutRow({ .rowRect = flRect, .label = &flTypeSelector.label, .component = &flTypeSelector, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = flRect, .label = &flFreqSlider.label, .component = &flFreqSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = flRect, .label = &flQSlider.label, .component = &flQSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    flRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = flRect, .label = &flMixSlider.label, .component = &flMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = flRect, .comp1 = &flDryBtn, .comp2 = &flHalfBtn, .comp3 = &flWetBtn });

    // 3-Band EQ
    auto rect2 = row1.removeFromLeft(FxGuiValue::Fx::AreaWidth);
    auto eq3bArea = rect2.removeFromTop(FxGuiValue::Fx::HeightEq3b);

    eq3bGroup.setBounds(eq3bArea);

    auto eq3bRect = eq3bArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    eq3bRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = eq3bRect, .component = &eq3bBypassBtn });

    // 区切り線エリアを確保
    auto eq3bSprArea = eq3bRect.removeFromTop(FxGuiValue::Fx::SeparatorHeight);
    eq3bSeparator.setBounds(eq3bSprArea);

    layoutRow({ .rowRect = eq3bRect, .label = &eq3bLowGainDbSlider.label, .component = &eq3bLowGainDbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = eq3bRect, .label = &eq3bMidFreqSlider.label, .component = &eq3bMidFreqSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = eq3bRect, .label = &eq3bMidGainDbSlider.label, .component = &eq3bMidGainDbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = eq3bRect, .label = &eq3bHighGainDbSlider.label, .component = &eq3bHighGainDbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    eq3bRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = eq3bRect, .label = &eq3bMixSlider.label, .component = &eq3bMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = eq3bRect, .comp1 = &eq3bDryBtn, .comp2 = &eq3bHalfBtn, .comp3 = &eq3bWetBtn });

    // Tremolo
    auto rect3 = row2.removeFromLeft(FxGuiValue::Fx::AreaWidth);
    auto trmArea = rect3.removeFromTop(FxGuiValue::Fx::HeightTremoro);

    tremGroup.setBounds(trmArea);

    auto trmRect = trmArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    trmRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = trmRect, .component = &tBypassBtn });

    // 区切り線エリアを確保
    auto trmSprArea = trmRect.removeFromTop(FxGuiValue::Fx::SeparatorHeight);
    tSeparator.setBounds(trmSprArea);

    layoutRow({ .rowRect = trmRect, .label = &tRateSlider.label, .component = &tRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = trmRect, .label = &tDepthSlider.label, .component = &tDepthSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    trmRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = trmRect, .label = &tMixSlider.label, .component = &tMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = trmRect, .comp1 = &tDryBtn, .comp2 = &tHalfBtn, .comp3 = &tWetBtn });

    // Vibrato
    auto rect4 = row2.removeFromLeft(FxGuiValue::Fx::AreaWidth);
    auto vibArea = rect4.removeFromTop(FxGuiValue::Fx::HeightVibrato);

    vibGroup.setBounds(vibArea);

    auto vibRect = vibArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    vibRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = vibRect, .component = &vBypassBtn });

    // 区切り線エリアを確保
    auto vibSprArea = vibRect.removeFromTop(FxGuiValue::Fx::SeparatorHeight);
    vSeparator.setBounds(vibSprArea);

    layoutRow({ .rowRect = vibRect, .label = &vRateSlider.label, .component = &vRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = vibRect, .label = &vDepthSlider.label, .component = &vDepthSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    vibRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = vibRect, .label = &vMixSlider.label, .component = &vMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = vibRect, .comp1 = &vDryBtn, .comp2 = &vHalfBtn, .comp3 = &vWetBtn });

    // Modern Bit Crusher
    auto rect5 = row3.removeFromLeft(FxGuiValue::Fx::AreaWidth);
    auto mbcArea = rect5.removeFromTop(FxGuiValue::Fx::HeightMbc);

    mbcGroup.setBounds(mbcArea);

    auto mbcRect = mbcArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    mbcRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = mbcRect, .component = &mbcBypassBtn });

    // 区切り線エリアを確保
    auto mbcSprArea = mbcRect.removeFromTop(FxGuiValue::Fx::SeparatorHeight);
    mbcSeparator.setBounds(mbcSprArea);

    layoutRow({ .rowRect = mbcRect, .label = &mbcBitsSlider.label, .component = &mbcBitsSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = mbcRect, .label = &mbcRateSlider.label, .component = &mbcRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    mbcRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = mbcRect, .label = &mbcMixSlider.label, .component = &mbcMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = mbcRect, .comp1 = &mbcDryBtn, .comp2 = &mbcHalfBtn, .comp3 = &mbcWetBtn });

    // Delay
    auto rect6 = row3.removeFromLeft(FxGuiValue::Fx::AreaWidth);
    auto dlyArea = rect6.removeFromTop(FxGuiValue::Fx::HeightDelay);

    delayGroup.setBounds(dlyArea);

    auto dlyRect = dlyArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    dlyRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = dlyRect, .component = &dBypassBtn });

    // 区切り線エリアを確保
    auto dlySprArea = dlyRect.removeFromTop(FxGuiValue::Fx::SeparatorHeight);
    dSeparator.setBounds(dlySprArea);

    layoutRow({ .rowRect = dlyRect, .label = &dTimeSlider.label, .component = &dTimeSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = dlyRect, .label = &dFbSlider.label, .component = &dFbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    dlyRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = dlyRect, .label = &dMixSlider.label, .component = &dMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = dlyRect, .comp1 = &dDryBtn, .comp2 = &dHalfBtn, .comp3 = &dWetBtn });

    // Reverb
    auto rect7 = row4.removeFromLeft(FxGuiValue::Fx::AreaWidth);
    auto rvbArea = rect7.removeFromTop(FxGuiValue::Fx::HeightReverb);

    reverbGroup.setBounds(rvbArea);

    auto rvbRect = rvbArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    rvbRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = rvbRect, .component = &rBypassBtn });

    // 区切り線エリアを確保
    auto rvbSprArea = rvbRect.removeFromTop(FxGuiValue::Fx::SeparatorHeight);
    rSeparator.setBounds(rvbSprArea);

    layoutRow({ .rowRect = rvbRect, .label = &rSizeSlider.label, .component = &rSizeSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = rvbRect, .label = &rDampSlider.label, .component = &rDampSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    rvbRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = rvbRect, .label = &rMixSlider.label, .component = &rMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = rvbRect, .comp1 = &rDryBtn, .comp2 = &rHalfBtn, .comp3 = &rWetBtn });

    // SfcEcho
    auto rect8 = row4.removeFromLeft(FxGuiValue::Fx::AreaWidth);
    auto sfceArea = rect8.removeFromTop(FxGuiValue::Fx::HeightSfcEcho);

    sfceGroup.setBounds(sfceArea);

    auto sfceRect = sfceArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    sfceRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = sfceRect, .component = &sfceBypassBtn });

    // 区切り線エリアを確保
    auto sfceSprArea = sfceRect.removeFromTop(FxGuiValue::Fx::SeparatorHeight);
    rSeparator.setBounds(sfceSprArea);

    layoutRow({ .rowRect = sfceRect, .label = &sfceTimeSlider.label, .component = &sfceTimeSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = sfceRect, .label = &sfceFbSlider.label, .component = &sfceFbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    sfceRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = sfceRect, .label = &sfceFirCoef0Slider.label, .component = &sfceFirCoef0Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = sfceRect, .label = &sfceFirCoef1Slider.label, .component = &sfceFirCoef1Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = sfceRect, .label = &sfceFirCoef2Slider.label, .component = &sfceFirCoef2Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = sfceRect, .label = &sfceFirCoef3Slider.label, .component = &sfceFirCoef3Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = sfceRect, .label = &sfceFirCoef4Slider.label, .component = &sfceFirCoef4Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = sfceRect, .label = &sfceFirCoef5Slider.label, .component = &sfceFirCoef5Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = sfceRect, .label = &sfceFirCoef6Slider.label, .component = &sfceFirCoef6Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = sfceRect, .label = &sfceFirCoef7Slider.label, .component = &sfceFirCoef7Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    sfceRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = sfceRect, .label = &sfceMixSlider.label, .component = &sfceMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = sfceRect, .comp1 = &sfceDryBtn, .comp2 = &sfceHalfBtn, .comp3 = &sfceWetBtn });
}

void GuiFx::layoutFxOrder(juce::Rectangle<int> rect) {
    auto spr = rect.removeFromTop(FxGuiValue::Fx::SeparatorHeight);
    routeSeparator.setBounds(spr);

    layoutMain({ .mainRect = rect, .component = &showRouteBtn });

    for (int fxr = 0; fxr < NumEffects; fxr++) {
        routeFx[fxr].setVisible(isShowRoute);
        routeUp[fxr].setVisible(isShowRoute);
        routeDown[fxr].setVisible(isShowRoute);
    }

    fileSeparator.setVisible(isShowRoute);
    importFxOrderBtn.setVisible(isShowRoute);
    exportFxOrderBtn.setVisible(isShowRoute);
    importFxParamBtn.setVisible(isShowRoute);
    exportFxParamBtn.setVisible(isShowRoute);

    if (!isShowRoute) {
        return;
    }

    for (int fxr = 0; fxr < NumEffects; fxr++) {
        layoutMainFxOrder({ .rect = rect, .comp1 = &routeFx[fxr], .comp2 = &routeUp[fxr], .comp3 = &routeDown[fxr] });
    }

    auto spr2 = rect.removeFromTop(FxGuiValue::Fx::SeparatorHeight);
    fileSeparator.setBounds(spr2);

    layoutMainTwoComps({ .rect = rect, .comp1 = &importFxOrderBtn, .comp2 = &exportFxOrderBtn, .paddingBottom = 0 });
    rect.removeFromTop(4);
    layoutMainTwoComps({ .rect = rect, .comp1 = &importFxParamBtn, .comp2 = &exportFxParamBtn, .paddingBottom = 0 });
}

void GuiFx::updateFxOrder() {
    order = ctx.audioProcessor.getFxOrder();

    for (int i = 0; i < NumEffects; i++) {
        routeFx[i].setText(effectNames[order[i]], juce::sendNotification);
    }

    ctx.editor.resized();
}

void GuiFx::updateFilterEnabled() {
    bool bypassed = flBypassBtn.getToggleState();

    flSeparator.setEnabled(!bypassed);
    flTypeSelector.setEnabledWithLabel(!bypassed);
    flFreqSlider.setEnabledWithLabel(!bypassed);
    flQSlider.setEnabledWithLabel(!bypassed);
    flMixSlider.setEnabledWithLabel(!bypassed);
    flDryBtn.setEnabled(!bypassed);
    flHalfBtn.setEnabled(!bypassed);
    flWetBtn.setEnabled(!bypassed);
}
void GuiFx::updateTremoloEnabled() {
    bool bypassed = tBypassBtn.getToggleState();

    tSeparator.setEnabled(!bypassed);
    tRateSlider.setEnabledWithLabel(!bypassed);
    tDepthSlider.setEnabledWithLabel(!bypassed);
    tMixSlider.setEnabledWithLabel(!bypassed);
    tDryBtn.setEnabled(!bypassed);
    tHalfBtn.setEnabled(!bypassed);
    tWetBtn.setEnabled(!bypassed);
}
void GuiFx::updateVibratoEnabled() {
    bool bypassed = vBypassBtn.getToggleState();

    vSeparator.setEnabled(!bypassed);
    vRateSlider.setEnabledWithLabel(!bypassed);
    vDepthSlider.setEnabledWithLabel(!bypassed);
    vMixSlider.setEnabledWithLabel(!bypassed);
    vDryBtn.setEnabled(!bypassed);
    vHalfBtn.setEnabled(!bypassed);
    vWetBtn.setEnabled(!bypassed);
}
void GuiFx::updateMBCEnabled() {
    bool bypassed = mbcBypassBtn.getToggleState();

    mbcSeparator.setEnabled(!bypassed);
    mbcRateSlider.setEnabledWithLabel(!bypassed);
    mbcBitsSlider.setEnabledWithLabel(!bypassed);
    mbcMixSlider.setEnabledWithLabel(!bypassed);
    mbcDryBtn.setEnabled(!bypassed);
    mbcHalfBtn.setEnabled(!bypassed);
    mbcWetBtn.setEnabled(!bypassed);
}
void GuiFx::updateDelayEnabled() {
    bool bypassed = dBypassBtn.getToggleState();

    dSeparator.setEnabled(!bypassed);
    dTimeSlider.setEnabledWithLabel(!bypassed);
    dFbSlider.setEnabledWithLabel(!bypassed);
    dMixSlider.setEnabledWithLabel(!bypassed);
    dDryBtn.setEnabled(!bypassed);
    dHalfBtn.setEnabled(!bypassed);
    dWetBtn.setEnabled(!bypassed);
}
void GuiFx::updateReverbEnabled() {
    bool bypassed = rBypassBtn.getToggleState();

    rSeparator.setEnabled(!bypassed);
    rSizeSlider.setEnabledWithLabel(!bypassed);
    rDampSlider.setEnabledWithLabel(!bypassed);
    rMixSlider.setEnabledWithLabel(!bypassed);
    rDryBtn.setEnabled(!bypassed);
    rHalfBtn.setEnabled(!bypassed);
    rWetBtn.setEnabled(!bypassed);
}
void GuiFx::updateEq3bEnabled() {
    bool bypassed = eq3bBypassBtn.getToggleState();

    eq3bSeparator.setEnabled(!bypassed);
    eq3bLowGainDbSlider.setEnabledWithLabel(!bypassed);
    eq3bMidFreqSlider.setEnabledWithLabel(!bypassed);
    eq3bMidGainDbSlider.setEnabledWithLabel(!bypassed);
    eq3bHighGainDbSlider.setEnabledWithLabel(!bypassed);
    eq3bMixSlider.setEnabledWithLabel(!bypassed);
    eq3bDryBtn.setEnabled(!bypassed);
    eq3bHalfBtn.setEnabled(!bypassed);
    eq3bWetBtn.setEnabled(!bypassed);
}
void GuiFx::updateSfcEchoEnabled() {
    bool bypassed = sfceBypassBtn.getToggleState();

    sfceSeparator.setEnabled(!bypassed);
    sfceTimeSlider.setEnabledWithLabel(!bypassed);
    sfceFbSlider.setEnabledWithLabel(!bypassed);
    sfceFirCoef0Slider.setEnabledWithLabel(!bypassed);
    sfceFirCoef1Slider.setEnabledWithLabel(!bypassed);
    sfceFirCoef2Slider.setEnabledWithLabel(!bypassed);
    sfceFirCoef3Slider.setEnabledWithLabel(!bypassed);
    sfceFirCoef4Slider.setEnabledWithLabel(!bypassed);
    sfceFirCoef5Slider.setEnabledWithLabel(!bypassed);
    sfceFirCoef6Slider.setEnabledWithLabel(!bypassed);
    sfceFirCoef7Slider.setEnabledWithLabel(!bypassed);
    sfceMixSlider.setEnabledWithLabel(!bypassed);
    sfceDryBtn.setEnabled(!bypassed);
    sfceHalfBtn.setEnabled(!bypassed);
    sfceWetBtn.setEnabled(!bypassed);
}

// ==============================================================================
// Import / Export Logic
// ==============================================================================
void GuiFx::importFxOrder()
{
    juce::File defaultDir(ctx.audioProcessor.defaultFxOrderDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importFxOrderFile, defaultDir, Io::ExtensionGlob::fxOrder);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultFxOrderDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                if (lines.size() == 0) return;

                // 1行目のサンプル数を取得
                int fxCount = lines[0].trim().getIntValue();
                int readFxCount = std::min(fxCount, NumEffects);

                std::vector<int> newOrders;

                for (int i = 0; i < readFxCount; i++) {
                    newOrders.push_back(lines[i + 1].trim().getIntValue());
                }

                // 保存していたエフェクト数より少ない場合は新しい値で埋める
                if (readFxCount < NumEffects) {
                    for (int j = readFxCount; j < NumEffects; j++) {
                        newOrders.push_back(j);
                    }
                }

                ctx.audioProcessor.updateFxOrder(newOrders);

                updateFxOrder();
            }
        });
}

void GuiFx::exportFxOrder()
{
    juce::File defaultDir(ctx.audioProcessor.defaultFxOrderDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportFxOrderFile, defaultDir.getChildFile("default.fxo"), Io::ExtensionGlob::fxOrder);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultFxOrderDir = file.getParentDirectory().getFullPathName();

                // 1行目にサンプル数
                juce::String content = juce::String(NumEffects) + "\n";

                // 2行目以降に値を書き込む
                for (int v : order) {
                    content += juce::String(v) + "\n";
                }

                file.replaceWithText(content);
            }
        });
}

void GuiFx::importFxParam()
{
    juce::File defaultDir(ctx.audioProcessor.defaultFxParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importFxParamFile, defaultDir, Io::ExtensionGlob::fxParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultFxParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 1) return;

                int num = lines[0].getIntValue();

                if (size < 2) return;

                bypassToggle.setToggleState(lines[1].getIntValue() == 1, juce::sendNotification);

                if (num == 0 || size < 6) return;

                // Tremollo
                tBypassBtn.setToggleState(lines[2].getIntValue() == 1, juce::sendNotification);
                tRateSlider.setValue(lines[3].getFloatValue(), juce::sendNotification);
                tDepthSlider.setValue(lines[4].getFloatValue(), juce::sendNotification);
                tMixSlider.setValue(lines[5].getFloatValue(), juce::sendNotification);

                if (num == 1 || size < 10) return;

                // Vibrato
                vBypassBtn.setToggleState(lines[6].getIntValue() == 1, juce::sendNotification);
                vRateSlider.setValue(lines[7].getFloatValue(), juce::sendNotification);
                vDepthSlider.setValue(lines[8].getFloatValue(), juce::sendNotification);
                vMixSlider.setValue(lines[9].getFloatValue(), juce::sendNotification);

                if (num == 2 || size < 14) return;

                // Modern Bit Crusher
                mbcBypassBtn.setToggleState(lines[10].getIntValue() == 1, juce::sendNotification);
                mbcRateSlider.setValue(lines[11].getFloatValue(), juce::sendNotification);
                mbcBitsSlider.setValue(lines[12].getFloatValue(), juce::sendNotification);
                mbcMixSlider.setValue(lines[13].getFloatValue(), juce::sendNotification);

                if (num == 3 || size < 18) return;

                // Delay
                dBypassBtn.setToggleState(lines[14].getIntValue() == 1, juce::sendNotification);
                dTimeSlider.setValue(lines[15].getFloatValue(), juce::sendNotification);
                dFbSlider.setValue(lines[16].getFloatValue(), juce::sendNotification);
                dMixSlider.setValue(lines[17].getFloatValue(), juce::sendNotification);

                if (num == 4 || size < 22) return;

                // Reverb
                rBypassBtn.setToggleState(lines[18].getIntValue() == 1, juce::sendNotification);
                rSizeSlider.setValue(lines[19].getFloatValue(), juce::sendNotification);
                rDampSlider.setValue(lines[20].getFloatValue(), juce::sendNotification);
                rMixSlider.setValue(lines[21].getFloatValue(), juce::sendNotification);

                if (num == 5 || size < 27) return;

                // Filter
                flBypassBtn.setToggleState(lines[22].getIntValue() == 1, juce::sendNotification);
                flTypeSelector.setSelectedItemIndex(lines[23].getIntValue(), juce::sendNotification);
                flFreqSlider.setValue(lines[24].getFloatValue(), juce::sendNotification);
                flQSlider.setValue(lines[25].getFloatValue(), juce::sendNotification);
                flMixSlider.setValue(lines[26].getFloatValue(), juce::sendNotification);

                if (num == 6 || size < 33) return;

                // 3Band EQ
                eq3bBypassBtn.setToggleState(lines[27].getIntValue() == 1, juce::sendNotification);
                eq3bLowGainDbSlider.setValue(lines[28].getFloatValue(), juce::sendNotification);
                eq3bMidFreqSlider.setValue(lines[29].getFloatValue(), juce::sendNotification);
                eq3bMidGainDbSlider.setValue(lines[30].getFloatValue(), juce::sendNotification);
                eq3bHighGainDbSlider.setValue(lines[31].getFloatValue(), juce::sendNotification);
                eq3bMixSlider.setValue(lines[32].getFloatValue(), juce::sendNotification);

                if (num == 7 || size < 45) return;

                // SFC Echo
                sfceBypassBtn.setToggleState(lines[33].getIntValue() == 1, juce::sendNotification);
                sfceTimeSlider.setValue(lines[34].getFloatValue(), juce::sendNotification);
                sfceFbSlider.setValue(lines[35].getFloatValue(), juce::sendNotification);
                sfceFirCoef0Slider.setValue(lines[36].getFloatValue(), juce::sendNotification);
                sfceFirCoef1Slider.setValue(lines[37].getFloatValue(), juce::sendNotification);
                sfceFirCoef2Slider.setValue(lines[38].getFloatValue(), juce::sendNotification);
                sfceFirCoef3Slider.setValue(lines[39].getFloatValue(), juce::sendNotification);
                sfceFirCoef4Slider.setValue(lines[40].getFloatValue(), juce::sendNotification);
                sfceFirCoef5Slider.setValue(lines[41].getFloatValue(), juce::sendNotification);
                sfceFirCoef6Slider.setValue(lines[42].getFloatValue(), juce::sendNotification);
                sfceFirCoef7Slider.setValue(lines[43].getFloatValue(), juce::sendNotification);
                sfceMixSlider.setValue(lines[44].getFloatValue(), juce::sendNotification);
            }
        });
}

void GuiFx::exportFxParam()
{
    juce::File defaultDir(ctx.audioProcessor.defaultFxParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportFxParamFile, defaultDir.getChildFile("default.2fx"), Io::ExtensionGlob::fxParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultFxParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = juce::String(NumEffects) + "\n";

                // Master Bypass
                content += juce::String(bypassToggle.getToggleState() ? 1 : 0) + "\n";

                // Tremollo
                content += juce::String(tBypassBtn.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(tRateSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(tDepthSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(tMixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                // Vibrato
                content += juce::String(vBypassBtn.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(vRateSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(vDepthSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(vMixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                // Modern Bit Crusher
                content += juce::String(mbcBypassBtn.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(mbcRateSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(mbcBitsSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(mbcMixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                // Delay
                content += juce::String(dBypassBtn.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(dTimeSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(dFbSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(dMixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                // Reverb
                content += juce::String(rBypassBtn.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(rSizeSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(rDampSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(rMixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                // Filter
                content += juce::String(flBypassBtn.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(flTypeSelector.getSelectedItemIndex()) + "\n";
                content += juce::String(flFreqSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(flQSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(flMixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                // 3Band EQ
                content += juce::String(eq3bBypassBtn.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(eq3bLowGainDbSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(eq3bMidFreqSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(eq3bMidGainDbSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(eq3bHighGainDbSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(eq3bMixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                // SFC Echo
                content += juce::String(sfceBypassBtn.getToggleState() ? 1 : 0) + "\n";
                content += juce::String(sfceTimeSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(sfceFbSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(sfceFirCoef0Slider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(sfceFirCoef1Slider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(sfceFirCoef2Slider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(sfceFirCoef3Slider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(sfceFirCoef4Slider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(sfceFirCoef5Slider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(sfceFirCoef6Slider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(sfceFirCoef7Slider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(sfceMixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                file.replaceWithText(content);
            }
        });
}
