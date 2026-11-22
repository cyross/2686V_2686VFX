#include "../../Core/Editor/EditorGuiValues.h"
#include <algorithm>
#include "./GuiFx.h"
#include "../../Effect/Fx/FxOrder.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Fx/ProcessorFxKeys.h"
#include "../../Processor/Fx/ProcessorFxValues.h"
#include "Shared/Core/Const/ConstFileValues.h"
#include "Shared/Core/Const/ConstGlobal.h"

#include "Shared/Core/Gui/GuiHelpers.h"
#include "./GuiFxValues.h"
#include "Shared/Core/Gui/GuiStructs.h"
#include "Shared/Core/Gui/GuiRefresh.h"
#include "Shared/Core/Io/ParamFile.h"
#include "../../Core/Gui/GuiPluginContext.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat fxOrderFormat{ "fxOrder", 1 };
	const Io::ParamFormat fxParamFormat{ "fxParam", 1 };
}

static std::vector<SelectItem> flTypeItems = {
    {.name = "LPF", .value = 1 },
    {.name = "HPF", .value = 2 },
    {.name = "BPF", .value = 3 }
};

GuiFx::GuiFx(const GuiContext& context) :
    GuiBase(context),
    fxGroup(context),
    mainCat(context),
    tremCat(context),
    vibCat(context),
    mbcCat(context),
    delayCat(context),
    reverbCat(context),
    filterCat(context),
    eq3bCat(context),
    sfceCat(context),
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
    order = pluginOf(ctx).getFxOrder();
}

void GuiFx::setup()
{
    const juce::String code = FxPrKey::prefix;
    int tabOrder = 1;
    const juce::Colour groupBgColour = GuiColor::FxTab::GroupBg;

    // 「エフェクト」の枠 (4.5.0)。部品は *this へ作ってから、最後に中身へ移す
    fxGroup.setup(*this, FxGuiText::Group::mainGroup);
    fxGroup.setBackgroundColor(groupBgColour);

    // メイン (全体の設定)
    mainCat.setupFxCategory({ .parent = *this, .title = FxGuiText::Group::mainCategory, .detailVisible = true, .enableChangeDetailVisible = true });

	bypassToggle.setup({ .parent = *this, .id = code + FxPrKey::bypass, .title = FxGuiText::Fx::masterBypass, .isReset = true });
    bypassToggle.setWantsKeyboardFocus(true);
    bypassToggle.setExplicitFocusOrder(++tabOrder);

	mainSeparator.setupComponent(*this);

    resetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::reset, .textColor = GuiColor::FxTab::ButtonText, .bgColor = GuiColor::FxTab::ResetBg });
    resetBtn.setWantsKeyboardFocus(true);
    resetBtn.setExplicitFocusOrder(++tabOrder);
    resetBtn.onClick = [&] { pluginOf(ctx).initParams("FX_"); };

    routeSeparator.setupComponent(*this);

    showRouteBtn.setup({ .parent = *this, .title = FxGuiText::Fx::showRoute, .textColor = GuiColor::FxTab::ButtonText, .bgColor = GuiColor::FxTab::RouteBg, .isReset = false });
    showRouteBtn.setWantsKeyboardFocus(true);
    showRouteBtn.setExplicitFocusOrder(++tabOrder);
    showRouteBtn.onClick = [this] {
        isShowRoute = !isShowRoute;

        editorOf(ctx).resized();
        };

    for (int fxr = 0; fxr < NumEffects; fxr++) {
        routeFx[fxr].setup({.parent = *this, .title = effectNames()[order[fxr]]});
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
                routeFx[i].setText(effectNames()[order[i]], juce::sendNotification);
            }

            pluginOf(ctx).updateFxOrder(order);

            editorOf(ctx).resized();
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
                routeFx[i].setText(effectNames()[order[i]], juce::sendNotification);
            }

            pluginOf(ctx).updateFxOrder(order);

            editorOf(ctx).resized();
            };
    }

    fileSeparator.setupComponent(*this);

    importFxOrderBtn.setup({ .parent = *this, .title = FxGuiText::Fx::orderFileImport, .textColor = GuiColor::FileButton::Text, .textOnColor = GuiColor::FileButton::TextOn, .bgColor = GuiColor::FxTab::OrderImportBg, .isReset = false, .isResized = false });
    importFxOrderBtn.setWantsKeyboardFocus(true);
    importFxOrderBtn.setExplicitFocusOrder(++tabOrder);
    importFxOrderBtn.onClick = [this] { importFxOrder(); };

    exportFxOrderBtn.setup({ .parent = *this, .title = FxGuiText::Fx::orderFileExport, .textColor = GuiColor::FileButton::Text, .textOnColor = GuiColor::FileButton::TextOn, .bgColor = GuiColor::FxTab::OrderExportBg, .isReset = false, .isResized = false});
    exportFxOrderBtn.setWantsKeyboardFocus(true);
    exportFxOrderBtn.setExplicitFocusOrder(++tabOrder);
    exportFxOrderBtn.onClick = [this] { exportFxOrder(); };

    importFxParamBtn.setup({ .parent = *this, .title = FxGuiText::Fx::paramFileImport, .textColor = GuiColor::FileButton::Text, .textOnColor = GuiColor::FileButton::TextOn, .bgColor = GuiColor::FxTab::ParamImportBg, .isReset = false, .isResized = false });
    importFxParamBtn.setWantsKeyboardFocus(true);
    importFxParamBtn.setExplicitFocusOrder(++tabOrder);
    importFxParamBtn.onClick = [this] { importFxParam(); };

    exportFxParamBtn.setup({ .parent = *this, .title = FxGuiText::Fx::paramFileExport, .textColor = GuiColor::FileButton::Text, .textOnColor = GuiColor::FileButton::TextOn, .bgColor = GuiColor::FxTab::ParamExportBg, .isReset = false, .isResized = false });
    exportFxParamBtn.setWantsKeyboardFocus(true);
    exportFxParamBtn.setExplicitFocusOrder(++tabOrder);
    exportFxParamBtn.onClick = [this] { exportFxParam(); };

    // Filter Group
    filterCat.setupFxCategory({ .parent = *this, .title = FxGuiText::Group::fxFilter, .detailVisible = true, .enableChangeDetailVisible = true });
    const juce::String filterPrefix = code + FxPrKey::fil;

    flBypassBtn.setup({ .parent = *this, .id = filterPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    flBypassBtn.setWantsKeyboardFocus(true);
    flBypassBtn.setExplicitFocusOrder(++tabOrder);
    flBypassBtn.onStateChange = [this] {
        updateFilterEnabled();
        };

    flSeparator.setupComponent(*this);

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
    eq3bCat.setupFxCategory({ .parent = *this, .title = FxGuiText::Group::fxEq3B, .detailVisible = true, .enableChangeDetailVisible = true });
    const juce::String eq3bPrefix = code + FxPrKey::eq3b;

    eq3bBypassBtn.setup({ .parent = *this, .id = eq3bPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    eq3bBypassBtn.setWantsKeyboardFocus(true);
    eq3bBypassBtn.setExplicitFocusOrder(++tabOrder);
    eq3bBypassBtn.onStateChange = [this] {
        updateEq3bEnabled();
        };

    eq3bSeparator.setupComponent(*this);

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
    tremCat.setupFxCategory({ .parent = *this, .title = FxGuiText::Group::fxTremolo, .detailVisible = true, .enableChangeDetailVisible = true });
    const juce::String trmPrefix = code + FxPrKey::trm;

    tBypassBtn.setup({ .parent = *this, .id = trmPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    tBypassBtn.setWantsKeyboardFocus(true);
    tBypassBtn.setExplicitFocusOrder(++tabOrder);
    tBypassBtn.onStateChange = [this] {
        updateTremoloEnabled();
        };

    tSeparator.setupComponent(*this);

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
    vibCat.setupFxCategory({ .parent = *this, .title = FxGuiText::Group::fxVibrato, .detailVisible = true, .enableChangeDetailVisible = true });
    const juce::String vibPrefix = code + FxPrKey::vib;

    vBypassBtn.setup({ .parent = *this, .id = vibPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    vBypassBtn.setWantsKeyboardFocus(true);
    vBypassBtn.setExplicitFocusOrder(++tabOrder);
    vBypassBtn.onStateChange = [this] {
        updateVibratoEnabled();
        };

    vSeparator.setupComponent(*this);

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
    mbcCat.setupFxCategory({ .parent = *this, .title = FxGuiText::Group::fxMbc, .detailVisible = true, .enableChangeDetailVisible = true });
    const juce::String mbcPrefix = code + FxPrKey::mbc;

    mbcBypassBtn.setup({ .parent = *this, .id = mbcPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    mbcBypassBtn.setWantsKeyboardFocus(true);
    mbcBypassBtn.setExplicitFocusOrder(++tabOrder);
    mbcBypassBtn.onStateChange = [this] {
        updateMBCEnabled();
        };

    mbcSeparator.setupComponent(*this);

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
    delayCat.setupFxCategory({ .parent = *this, .title = FxGuiText::Group::fxDelay, .detailVisible = true, .enableChangeDetailVisible = true });
    const juce::String dlyPrefix = code + FxPrKey::dly;

    dBypassBtn.setup({ .parent = *this, .id = dlyPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    dBypassBtn.setWantsKeyboardFocus(true);
    dBypassBtn.setExplicitFocusOrder(++tabOrder);
    dBypassBtn.onStateChange = [this] {
        updateDelayEnabled();
        };

    dSeparator.setupComponent(*this);

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
    reverbCat.setupFxCategory({ .parent = *this, .title = FxGuiText::Group::fxReverb, .detailVisible = true, .enableChangeDetailVisible = true });
    const juce::String rvbPrefix = code + FxPrKey::rvb;

    rBypassBtn.setup({ .parent = *this, .id = rvbPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    rBypassBtn.setWantsKeyboardFocus(true);
    rBypassBtn.setExplicitFocusOrder(++tabOrder);
    rBypassBtn.onStateChange = [this] {
        updateReverbEnabled();
        };

    rSeparator.setupComponent(*this);

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
    sfceCat.setupFxCategory({ .parent = *this, .title = FxGuiText::Group::sfcEcho, .detailVisible = true, .enableChangeDetailVisible = true });
    const juce::String sfcePrefix = code + FxPrKey::sfcEcho;

    sfceBypassBtn.setup({ .parent = *this, .id = sfcePrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    sfceBypassBtn.setWantsKeyboardFocus(true);
    sfceBypassBtn.setExplicitFocusOrder(++tabOrder);
    sfceBypassBtn.onStateChange = [this] {
        updateSfcEchoEnabled();
        };

    sfceSeparator.setupComponent(*this);

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

    moveToGroup();
}

void GuiFx::moveToGroup()
{
    // 枠そのもの以外を、枠の中身へ移す。見えているかどうかはそのまま (区分の板は
    // 隠れたまま移す)。並びも保つので、板は部品の後ろに残る
    juce::Array<juce::Component*> children;

    for (auto* child : getChildren()) {
        if (child != &fxGroup) children.add(child);
    }

    for (auto* child : children) {
        removeChildComponent(child);
        fxGroup.contentCanvas.addChildComponent(child);
    }
}

void GuiFx::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    // 「エフェクト」の枠 1 つを、ペインの幅いっぱい・下端まで取る (4.5.0)
    auto fxArea = pageArea.removeFromLeft(FxGuiValue::Fx::MainWidth);

    fxGroup.setBounds(fxArea);

    auto inner = fxArea.withZeroOrigin().reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    inner.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    fxGroup.setViewportCustomBounds(inner);

    juce::Rectangle<int> rect(0, 0, fxGroup.getContentWidth(), 20000);

    layoutMainCat(rect);

    // 効果の区分は、かける順番に並べる。順番を変えたらすぐ並べ直す
    for (const int type : order) layoutEffectCat(rect, type);

    fxGroup.setContentHeight(rect.getY() + categoryContentTrailingPadding);
}

void GuiFx::layoutMainCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &mainCat });

    const bool visible = mainCat.isDetailVisible();

    bypassToggle.setVisible(visible);
    mainSeparator.setVisible(visible);
    resetBtn.setVisible(visible);
    routeSeparator.setVisible(visible);
    showRouteBtn.setVisible(visible);

    if (!visible) {
        for (int fxr = 0; fxr < NumEffects; fxr++) {
            routeFx[fxr].setVisible(false);
            routeUp[fxr].setVisible(false);
            routeDown[fxr].setVisible(false);
        }

        fileSeparator.setVisible(false);
        importFxOrderBtn.setVisible(false);
        exportFxOrderBtn.setVisible(false);
        importFxParamBtn.setVisible(false);
        exportFxParamBtn.setVisible(false);

        return;
    }

    layoutMain({ .mainRect = rect, .component = &bypassToggle });

    mainSeparator.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .component = &resetBtn });

    layoutFxOrder(rect);

    rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}

void GuiFx::layoutEffectCat(juce::Rectangle<int>& rect, int type)
{
    switch (type) {
    case 0: {
        layoutMainCategory({ .mainRect = rect, .label = &filterCat });

        const bool visible = filterCat.isDetailVisible();

        flBypassBtn.setVisible(visible);
        flSeparator.setVisible(visible);
        flTypeSelector.setVisibleWithLabel(visible);
        flFreqSlider.setVisibleWithLabel(visible);
        flQSlider.setVisibleWithLabel(visible);
        flMixSlider.setVisibleWithLabel(visible);
        flDryBtn.setVisible(visible);
        flHalfBtn.setVisible(visible);
        flWetBtn.setVisible(visible);

        if (!visible) break;

        layoutRow({ .rowRect = rect, .component = &flBypassBtn });

        flSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .label = &flTypeSelector.label, .component = &flTypeSelector, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &flFreqSlider.label, .component = &flFreqSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &flQSlider.label, .component = &flQSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        rect.removeFromTop(FxGuiValue::Padding::space);
        layoutRow({ .rowRect = rect, .label = &flMixSlider.label, .component = &flMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRowThreeComps({ .rect = rect, .comp1 = &flDryBtn, .comp2 = &flHalfBtn, .comp3 = &flWetBtn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
        break;
    }
    case 1: {
        layoutMainCategory({ .mainRect = rect, .label = &eq3bCat });

        const bool visible = eq3bCat.isDetailVisible();

        eq3bBypassBtn.setVisible(visible);
        eq3bSeparator.setVisible(visible);
        eq3bLowGainDbSlider.setVisibleWithLabel(visible);
        eq3bMidFreqSlider.setVisibleWithLabel(visible);
        eq3bMidGainDbSlider.setVisibleWithLabel(visible);
        eq3bHighGainDbSlider.setVisibleWithLabel(visible);
        eq3bMixSlider.setVisibleWithLabel(visible);
        eq3bDryBtn.setVisible(visible);
        eq3bHalfBtn.setVisible(visible);
        eq3bWetBtn.setVisible(visible);

        if (!visible) break;

        layoutRow({ .rowRect = rect, .component = &eq3bBypassBtn });

        eq3bSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .label = &eq3bLowGainDbSlider.label, .component = &eq3bLowGainDbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &eq3bMidFreqSlider.label, .component = &eq3bMidFreqSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &eq3bMidGainDbSlider.label, .component = &eq3bMidGainDbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &eq3bHighGainDbSlider.label, .component = &eq3bHighGainDbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        rect.removeFromTop(FxGuiValue::Padding::space);
        layoutRow({ .rowRect = rect, .label = &eq3bMixSlider.label, .component = &eq3bMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRowThreeComps({ .rect = rect, .comp1 = &eq3bDryBtn, .comp2 = &eq3bHalfBtn, .comp3 = &eq3bWetBtn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
        break;
    }
    case 2: {
        layoutMainCategory({ .mainRect = rect, .label = &tremCat });

        const bool visible = tremCat.isDetailVisible();

        tBypassBtn.setVisible(visible);
        tSeparator.setVisible(visible);
        tRateSlider.setVisibleWithLabel(visible);
        tDepthSlider.setVisibleWithLabel(visible);
        tMixSlider.setVisibleWithLabel(visible);
        tDryBtn.setVisible(visible);
        tHalfBtn.setVisible(visible);
        tWetBtn.setVisible(visible);

        if (!visible) break;

        layoutRow({ .rowRect = rect, .component = &tBypassBtn });

        tSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .label = &tRateSlider.label, .component = &tRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &tDepthSlider.label, .component = &tDepthSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        rect.removeFromTop(FxGuiValue::Padding::space);
        layoutRow({ .rowRect = rect, .label = &tMixSlider.label, .component = &tMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRowThreeComps({ .rect = rect, .comp1 = &tDryBtn, .comp2 = &tHalfBtn, .comp3 = &tWetBtn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
        break;
    }
    case 3: {
        layoutMainCategory({ .mainRect = rect, .label = &vibCat });

        const bool visible = vibCat.isDetailVisible();

        vBypassBtn.setVisible(visible);
        vSeparator.setVisible(visible);
        vRateSlider.setVisibleWithLabel(visible);
        vDepthSlider.setVisibleWithLabel(visible);
        vMixSlider.setVisibleWithLabel(visible);
        vDryBtn.setVisible(visible);
        vHalfBtn.setVisible(visible);
        vWetBtn.setVisible(visible);

        if (!visible) break;

        layoutRow({ .rowRect = rect, .component = &vBypassBtn });

        vSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .label = &vRateSlider.label, .component = &vRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &vDepthSlider.label, .component = &vDepthSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        rect.removeFromTop(FxGuiValue::Padding::space);
        layoutRow({ .rowRect = rect, .label = &vMixSlider.label, .component = &vMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRowThreeComps({ .rect = rect, .comp1 = &vDryBtn, .comp2 = &vHalfBtn, .comp3 = &vWetBtn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
        break;
    }
    case 4: {
        layoutMainCategory({ .mainRect = rect, .label = &mbcCat });

        const bool visible = mbcCat.isDetailVisible();

        mbcBypassBtn.setVisible(visible);
        mbcSeparator.setVisible(visible);
        mbcBitsSlider.setVisibleWithLabel(visible);
        mbcRateSlider.setVisibleWithLabel(visible);
        mbcMixSlider.setVisibleWithLabel(visible);
        mbcDryBtn.setVisible(visible);
        mbcHalfBtn.setVisible(visible);
        mbcWetBtn.setVisible(visible);

        if (!visible) break;

        layoutRow({ .rowRect = rect, .component = &mbcBypassBtn });

        mbcSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .label = &mbcBitsSlider.label, .component = &mbcBitsSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &mbcRateSlider.label, .component = &mbcRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        rect.removeFromTop(FxGuiValue::Padding::space);
        layoutRow({ .rowRect = rect, .label = &mbcMixSlider.label, .component = &mbcMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRowThreeComps({ .rect = rect, .comp1 = &mbcDryBtn, .comp2 = &mbcHalfBtn, .comp3 = &mbcWetBtn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
        break;
    }
    case 5: {
        layoutMainCategory({ .mainRect = rect, .label = &delayCat });

        const bool visible = delayCat.isDetailVisible();

        dBypassBtn.setVisible(visible);
        dSeparator.setVisible(visible);
        dTimeSlider.setVisibleWithLabel(visible);
        dFbSlider.setVisibleWithLabel(visible);
        dMixSlider.setVisibleWithLabel(visible);
        dDryBtn.setVisible(visible);
        dHalfBtn.setVisible(visible);
        dWetBtn.setVisible(visible);

        if (!visible) break;

        layoutRow({ .rowRect = rect, .component = &dBypassBtn });

        dSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .label = &dTimeSlider.label, .component = &dTimeSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &dFbSlider.label, .component = &dFbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        rect.removeFromTop(FxGuiValue::Padding::space);
        layoutRow({ .rowRect = rect, .label = &dMixSlider.label, .component = &dMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRowThreeComps({ .rect = rect, .comp1 = &dDryBtn, .comp2 = &dHalfBtn, .comp3 = &dWetBtn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
        break;
    }
    case 6: {
        layoutMainCategory({ .mainRect = rect, .label = &reverbCat });

        const bool visible = reverbCat.isDetailVisible();

        rBypassBtn.setVisible(visible);
        rSeparator.setVisible(visible);
        rSizeSlider.setVisibleWithLabel(visible);
        rDampSlider.setVisibleWithLabel(visible);
        rMixSlider.setVisibleWithLabel(visible);
        rDryBtn.setVisible(visible);
        rHalfBtn.setVisible(visible);
        rWetBtn.setVisible(visible);

        if (!visible) break;

        layoutRow({ .rowRect = rect, .component = &rBypassBtn });

        rSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .label = &rSizeSlider.label, .component = &rSizeSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &rDampSlider.label, .component = &rDampSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        rect.removeFromTop(FxGuiValue::Padding::space);
        layoutRow({ .rowRect = rect, .label = &rMixSlider.label, .component = &rMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRowThreeComps({ .rect = rect, .comp1 = &rDryBtn, .comp2 = &rHalfBtn, .comp3 = &rWetBtn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
        break;
    }
    case 7: {
        layoutMainCategory({ .mainRect = rect, .label = &sfceCat });

        const bool visible = sfceCat.isDetailVisible();

        sfceBypassBtn.setVisible(visible);
        sfceSeparator.setVisible(visible);
        sfceTimeSlider.setVisibleWithLabel(visible);
        sfceFbSlider.setVisibleWithLabel(visible);
        sfceFirCoef0Slider.setVisibleWithLabel(visible);
        sfceFirCoef1Slider.setVisibleWithLabel(visible);
        sfceFirCoef2Slider.setVisibleWithLabel(visible);
        sfceFirCoef3Slider.setVisibleWithLabel(visible);
        sfceFirCoef4Slider.setVisibleWithLabel(visible);
        sfceFirCoef5Slider.setVisibleWithLabel(visible);
        sfceFirCoef6Slider.setVisibleWithLabel(visible);
        sfceFirCoef7Slider.setVisibleWithLabel(visible);
        sfceMixSlider.setVisibleWithLabel(visible);
        sfceDryBtn.setVisible(visible);
        sfceHalfBtn.setVisible(visible);
        sfceWetBtn.setVisible(visible);

        if (!visible) break;

        layoutRow({ .rowRect = rect, .component = &sfceBypassBtn });

        sfceSeparator.layoutComponent(rect);

        layoutRow({ .rowRect = rect, .label = &sfceTimeSlider.label, .component = &sfceTimeSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &sfceFbSlider.label, .component = &sfceFbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        rect.removeFromTop(FxGuiValue::Padding::space);
        layoutRow({ .rowRect = rect, .label = &sfceFirCoef0Slider.label, .component = &sfceFirCoef0Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &sfceFirCoef1Slider.label, .component = &sfceFirCoef1Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &sfceFirCoef2Slider.label, .component = &sfceFirCoef2Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &sfceFirCoef3Slider.label, .component = &sfceFirCoef3Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &sfceFirCoef4Slider.label, .component = &sfceFirCoef4Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &sfceFirCoef5Slider.label, .component = &sfceFirCoef5Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &sfceFirCoef6Slider.label, .component = &sfceFirCoef6Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRow({ .rowRect = rect, .label = &sfceFirCoef7Slider.label, .component = &sfceFirCoef7Slider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        rect.removeFromTop(FxGuiValue::Padding::space);
        layoutRow({ .rowRect = rect, .label = &sfceMixSlider.label, .component = &sfceMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth, .compWidth = FxGuiValue::Fx::AreaValueWidth });
        layoutRowThreeComps({ .rect = rect, .comp1 = &sfceDryBtn, .comp2 = &sfceHalfBtn, .comp3 = &sfceWetBtn });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
        break;
    }
    default:
        break;
    }
}

void GuiFx::layoutFxOrder(juce::Rectangle<int>& rect) {
    routeSeparator.layoutComponent(rect);

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

    fileSeparator.layoutComponent(rect);

    layoutMainTwoComps({ .rect = rect, .comp1 = &importFxOrderBtn, .comp2 = &exportFxOrderBtn, .paddingBottom = 0 });
    rect.removeFromTop(4);
    layoutMainTwoComps({ .rect = rect, .comp1 = &importFxParamBtn, .comp2 = &exportFxParamBtn, .paddingBottom = 0 });
}

void GuiFx::updateFxOrder() {
    order = pluginOf(ctx).getFxOrder();

    for (int i = 0; i < NumEffects; i++) {
        routeFx[i].setText(effectNames()[order[i]], juce::sendNotification);
    }

    editorOf(ctx).resized();
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
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    editorOf(ctx).openParamBrowser(pluginOf(ctx).defaultFxOrderDir,
        { EditorGuiText::ParamBrowser::kindFxOrder },
        [this](const juce::File& file) { applyFxOrderFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiFx::applyFxOrderFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    pluginOf(ctx).defaultFxOrderDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingFxOrder(lines, index);
        }

        Io::ParamWriter writer(fxOrderFormat);

        writeFxOrder(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, fxOrderFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    // 名前で読む。3.0.0 のはじめの形は番号だったので、
    // 名前として読めなければ番号として読み直す。
    auto storedNames = reader->getStringArray("order");

    std::vector<int> newOrders;

    for (const auto& name : storedNames) {
        int id = fxTypeFromName(name);

        // 数で書かれていたときはここへ来る
        if (id < 0 && name.containsOnly("0123456789")) id = name.getIntValue();

        newOrders.push_back(id);
    }

    // 範囲外・重複・取りこぼしのならしは 1 箇所にまとめてある。
    pluginOf(ctx).updateFxOrder(normalizeFxOrder(newOrders, NumEffects));

    updateFxOrder();
}

void GuiFx::exportFxOrder()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    editorOf(ctx).openParamBrowserToSave(pluginOf(ctx).defaultFxOrderDir,
        { EditorGuiText::ParamBrowser::kindFxOrder }, Io::Extension::fxOrder,
        [this](const juce::File& file) { writeFxOrderFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiFx::writeFxOrderFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    pluginOf(ctx).defaultFxOrderDir = file.getParentDirectory().getFullPathName();

    // 1行目にサンプル数
    Io::ParamWriter writer(fxOrderFormat);
    writeFxOrder(writer);

    writer.writeTo(file);
}

void GuiFx::importFxParam()
{
    // ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
    // 読めるのはこの区分だけなので、ほかは選べない。
    editorOf(ctx).openParamBrowser(pluginOf(ctx).defaultFxParamDir,
        { EditorGuiText::ParamBrowser::kindFxParam },
        [this](const juce::File& file) { applyFxParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiFx::applyFxParamFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;


    // 次回のダイアログ用にディレクトリを保存
    pluginOf(ctx).defaultFxParamDir = file.getParentDirectory().getFullPathName();

    // 3.0.0 より前のファイルは、当時の処理で読み込んでから
    // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
    // 読み込みは当時のものをそのまま使う。
    if (Io::isLegacyFile(file)) {
        juce::StringArray lines;

        file.readLines(lines);

        int index = 0;

        {
            // 読み終えてからまとめて描き直す
            GuiRefresh::Batch batch;

            setImportingFxParams(lines, index);
        }

        Io::ParamWriter writer(fxParamFormat);

        writeFxParams(writer);

        Io::writeConverted(file, writer);

        return;
    }

    auto reader = Io::ParamReader::open(file, fxParamFormat);

    if (!reader.has_value()) return;

    // 読み終えてからまとめて描き直す
    GuiRefresh::Batch batch;

    bypassToggle.setToggleState(reader->getBool("bypass", bypassToggle.getToggleState()), juce::sendNotification);

    {
        auto tremolo = reader->child("tremolo");

        tBypassBtn.setToggleState(tremolo.getBool("bypass", tBypassBtn.getToggleState()), juce::sendNotification);
        tRateSlider.setValue(tremolo.getFloat("rate", (float)tRateSlider.getValue()), juce::sendNotification);
        tDepthSlider.setValue(tremolo.getFloat("depth", (float)tDepthSlider.getValue()), juce::sendNotification);
        tMixSlider.setValue(tremolo.getFloat("mix", (float)tMixSlider.getValue()), juce::sendNotification);
    }

    {
        auto vibrato = reader->child("vibrato");

        vBypassBtn.setToggleState(vibrato.getBool("bypass", vBypassBtn.getToggleState()), juce::sendNotification);
        vRateSlider.setValue(vibrato.getFloat("rate", (float)vRateSlider.getValue()), juce::sendNotification);
        vDepthSlider.setValue(vibrato.getFloat("depth", (float)vDepthSlider.getValue()), juce::sendNotification);
        vMixSlider.setValue(vibrato.getFloat("mix", (float)vMixSlider.getValue()), juce::sendNotification);
    }

    {
        auto bitCrusher = reader->child("bitCrusher");

        mbcBypassBtn.setToggleState(bitCrusher.getBool("bypass", mbcBypassBtn.getToggleState()), juce::sendNotification);
        mbcRateSlider.setValue(bitCrusher.getFloat("rate", (float)mbcRateSlider.getValue()), juce::sendNotification);
        mbcBitsSlider.setValue(bitCrusher.getFloat("bits", (float)mbcBitsSlider.getValue()), juce::sendNotification);
        mbcMixSlider.setValue(bitCrusher.getFloat("mix", (float)mbcMixSlider.getValue()), juce::sendNotification);
    }

    {
        auto delay = reader->child("delay");

        dBypassBtn.setToggleState(delay.getBool("bypass", dBypassBtn.getToggleState()), juce::sendNotification);
        dTimeSlider.setValue(delay.getFloat("time", (float)dTimeSlider.getValue()), juce::sendNotification);
        dFbSlider.setValue(delay.getFloat("fb", (float)dFbSlider.getValue()), juce::sendNotification);
        dMixSlider.setValue(delay.getFloat("mix", (float)dMixSlider.getValue()), juce::sendNotification);
    }

    {
        auto reverb = reader->child("reverb");

        rBypassBtn.setToggleState(reverb.getBool("bypass", rBypassBtn.getToggleState()), juce::sendNotification);
        rSizeSlider.setValue(reverb.getFloat("size", (float)rSizeSlider.getValue()), juce::sendNotification);
        rDampSlider.setValue(reverb.getFloat("damp", (float)rDampSlider.getValue()), juce::sendNotification);
        rMixSlider.setValue(reverb.getFloat("mix", (float)rMixSlider.getValue()), juce::sendNotification);
    }

    {
        auto filter = reader->child("filter");

        flBypassBtn.setToggleState(filter.getBool("bypass", flBypassBtn.getToggleState()), juce::sendNotification);
        flTypeSelector.setSelectedItemIndex(filter.getInt("type", flTypeSelector.getSelectedItemIndex()), juce::sendNotification);
        flFreqSlider.setValue(filter.getFloat("freq", (float)flFreqSlider.getValue()), juce::sendNotification);
        flQSlider.setValue(filter.getFloat("q", (float)flQSlider.getValue()), juce::sendNotification);
        flMixSlider.setValue(filter.getFloat("mix", (float)flMixSlider.getValue()), juce::sendNotification);
    }

    {
        auto eq3band = reader->child("eq3band");

        eq3bBypassBtn.setToggleState(eq3band.getBool("bypass", eq3bBypassBtn.getToggleState()), juce::sendNotification);
        eq3bLowGainDbSlider.setValue(eq3band.getFloat("lowGainDb", (float)eq3bLowGainDbSlider.getValue()), juce::sendNotification);
        eq3bMidFreqSlider.setValue(eq3band.getFloat("midFreq", (float)eq3bMidFreqSlider.getValue()), juce::sendNotification);
        eq3bMidGainDbSlider.setValue(eq3band.getFloat("midGainDb", (float)eq3bMidGainDbSlider.getValue()), juce::sendNotification);
        eq3bHighGainDbSlider.setValue(eq3band.getFloat("highGainDb", (float)eq3bHighGainDbSlider.getValue()), juce::sendNotification);
        eq3bMixSlider.setValue(eq3band.getFloat("mix", (float)eq3bMixSlider.getValue()), juce::sendNotification);
    }

    {
        auto sfcEcho = reader->child("sfcEcho");

        sfceBypassBtn.setToggleState(sfcEcho.getBool("bypass", sfceBypassBtn.getToggleState()), juce::sendNotification);
        sfceTimeSlider.setValue(sfcEcho.getFloat("time", (float)sfceTimeSlider.getValue()), juce::sendNotification);
        sfceFbSlider.setValue(sfcEcho.getFloat("fb", (float)sfceFbSlider.getValue()), juce::sendNotification);
        sfceFirCoef0Slider.setValue(sfcEcho.getFloat("firCoef0", (float)sfceFirCoef0Slider.getValue()), juce::sendNotification);
        sfceFirCoef1Slider.setValue(sfcEcho.getFloat("firCoef1", (float)sfceFirCoef1Slider.getValue()), juce::sendNotification);
        sfceFirCoef2Slider.setValue(sfcEcho.getFloat("firCoef2", (float)sfceFirCoef2Slider.getValue()), juce::sendNotification);
        sfceFirCoef3Slider.setValue(sfcEcho.getFloat("firCoef3", (float)sfceFirCoef3Slider.getValue()), juce::sendNotification);
        sfceFirCoef4Slider.setValue(sfcEcho.getFloat("firCoef4", (float)sfceFirCoef4Slider.getValue()), juce::sendNotification);
        sfceFirCoef5Slider.setValue(sfcEcho.getFloat("firCoef5", (float)sfceFirCoef5Slider.getValue()), juce::sendNotification);
        sfceFirCoef6Slider.setValue(sfcEcho.getFloat("firCoef6", (float)sfceFirCoef6Slider.getValue()), juce::sendNotification);
        sfceFirCoef7Slider.setValue(sfcEcho.getFloat("firCoef7", (float)sfceFirCoef7Slider.getValue()), juce::sendNotification);
        sfceMixSlider.setValue(sfcEcho.getFloat("mix", (float)sfceMixSlider.getValue()), juce::sendNotification);
    }
}

void GuiFx::exportFxParam()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    editorOf(ctx).openParamBrowserToSave(pluginOf(ctx).defaultFxParamDir,
        { EditorGuiText::ParamBrowser::kindFxParam }, Io::Extension::fxParam,
        [this](const juce::File& file) { writeFxParamFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiFx::writeFxParamFile(const juce::File& file)
{
    if (file == juce::File{}) return;

    // 次回のダイアログ用にディレクトリを保存
    pluginOf(ctx).defaultFxParamDir = file.getParentDirectory().getFullPathName();

    Io::ParamWriter writer(fxParamFormat);
    writeFxParams(writer);

    writer.writeTo(file);
}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiFx::setImportingFxOrder(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていた
    int size = lines.size();

    juce::ignoreUnused(index, size);

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

    pluginOf(ctx).updateFxOrder(newOrders);

    updateFxOrder();

}

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiFx::writeFxOrder(Io::ParamWriter& writer) {
    // 名前で書く。番号だと、効果を足したプラグインとの間で位置がずれて
    // 別の効果として読まれてしまう。
    std::vector<juce::String> names;

    for (int id : order) names.push_back(fxTypeName(id));

    writer.setArray("order", names);


}

// 3.0.0 より前の形式を読む。移行のときに当時の読み手ごと書き換えて
// しまったので、履歴から戻したもの。
void GuiFx::setImportingFxParams(juce::StringArray& lines, int& index) {
    // 当時の処理は行数を size で見ていた
    int size = lines.size();

    juce::ignoreUnused(index, size);

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

// 書き出す中身。エクスポートと変換の両方から使う。
void GuiFx::writeFxParams(Io::ParamWriter& writer) {
    writer.set("bypass", bypassToggle.getToggleState());

    {
        auto tremolo = writer.child("tremolo");

        tremolo.set("bypass", tBypassBtn.getToggleState());
        tremolo.set("rate", (float)tRateSlider.getValue());
        tremolo.set("depth", (float)tDepthSlider.getValue());
        tremolo.set("mix", (float)tMixSlider.getValue());
    }

    {
        auto vibrato = writer.child("vibrato");

        vibrato.set("bypass", vBypassBtn.getToggleState());
        vibrato.set("rate", (float)vRateSlider.getValue());
        vibrato.set("depth", (float)vDepthSlider.getValue());
        vibrato.set("mix", (float)vMixSlider.getValue());
    }

    {
        auto bitCrusher = writer.child("bitCrusher");

        bitCrusher.set("bypass", mbcBypassBtn.getToggleState());
        bitCrusher.set("rate", (float)mbcRateSlider.getValue());
        bitCrusher.set("bits", (float)mbcBitsSlider.getValue());
        bitCrusher.set("mix", (float)mbcMixSlider.getValue());
    }

    {
        auto delay = writer.child("delay");

        delay.set("bypass", dBypassBtn.getToggleState());
        delay.set("time", (float)dTimeSlider.getValue());
        delay.set("fb", (float)dFbSlider.getValue());
        delay.set("mix", (float)dMixSlider.getValue());
    }

    {
        auto reverb = writer.child("reverb");

        reverb.set("bypass", rBypassBtn.getToggleState());
        reverb.set("size", (float)rSizeSlider.getValue());
        reverb.set("damp", (float)rDampSlider.getValue());
        reverb.set("mix", (float)rMixSlider.getValue());
    }

    {
        auto filter = writer.child("filter");

        filter.set("bypass", flBypassBtn.getToggleState());
        filter.set("type", flTypeSelector.getSelectedItemIndex());
        filter.set("freq", (float)flFreqSlider.getValue());
        filter.set("q", (float)flQSlider.getValue());
        filter.set("mix", (float)flMixSlider.getValue());
    }

    {
        auto eq3band = writer.child("eq3band");

        eq3band.set("bypass", eq3bBypassBtn.getToggleState());
        eq3band.set("lowGainDb", (float)eq3bLowGainDbSlider.getValue());
        eq3band.set("midFreq", (float)eq3bMidFreqSlider.getValue());
        eq3band.set("midGainDb", (float)eq3bMidGainDbSlider.getValue());
        eq3band.set("highGainDb", (float)eq3bHighGainDbSlider.getValue());
        eq3band.set("mix", (float)eq3bMixSlider.getValue());
    }

    {
        auto sfcEcho = writer.child("sfcEcho");

        sfcEcho.set("bypass", sfceBypassBtn.getToggleState());
        sfcEcho.set("time", (float)sfceTimeSlider.getValue());
        sfcEcho.set("fb", (float)sfceFbSlider.getValue());
        sfcEcho.set("firCoef0", (float)sfceFirCoef0Slider.getValue());
        sfcEcho.set("firCoef1", (float)sfceFirCoef1Slider.getValue());
        sfcEcho.set("firCoef2", (float)sfceFirCoef2Slider.getValue());
        sfcEcho.set("firCoef3", (float)sfceFirCoef3Slider.getValue());
        sfcEcho.set("firCoef4", (float)sfceFirCoef4Slider.getValue());
        sfcEcho.set("firCoef5", (float)sfceFirCoef5Slider.getValue());
        sfcEcho.set("firCoef6", (float)sfceFirCoef6Slider.getValue());
        sfcEcho.set("firCoef7", (float)sfceFirCoef7Slider.getValue());
        sfcEcho.set("mix", (float)sfceMixSlider.getValue());
    }


}
