#include "../../Core/Editor/EditorGuiValues.h"
#include "../../Processor/Mod/ProcessorModKeys.h"
#include <algorithm>
#include "./GuiFx.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Fx/ProcessorFxKeys.h"
#include "../../Processor/Fx/ProcessorFxValues.h"
#include "../../Core/Const/ConstFileValues.h"
#include "../../Core/Const/ConstGlobal.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiFxValues.h"
#include "../../Core/Gui/GuiStructs.h"
#include "../../Core/Gui/GuiRefresh.h"
#include "../../Core/Io/ParamFile.h"

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

// QUALITY のビット一覧から、流れてくる音でも扱える頭の 12 個だけを採る。
//
// 借りてくる QualityPcm::qualityItems は別のファイルにある静的な変数で、
// どちらが先に作られるかは決まっていない。ここを静的な変数にすると、
// 相手がまだ空のうちに絞り込んでしまい、一覧が空になる。
// 最初に呼ばれたときに作れば、相手は必ずできあがっている。
std::vector<SelectItem>& GuiFx::getPcmBitItems() {
    static std::vector<SelectItem> items = [] {
        std::vector<SelectItem> list;

        for (const auto& item : QualityPcm::qualityItems)
        {
            if (item.value > FxPrValue::Pcm::Bit::max) continue;

            list.push_back(item);
        }

        return list;
    }();

    return items;
}

GuiFx::GuiFx(const GuiContext& context) :
    GuiBase(context),
    mainGroup(context),
    modSwitchGroup(context),
    envBypassToggle(context),
    lfoBypassToggle(context),
    pitchBypassToggle(context),
    shiftBypassToggle(context),
    modAmpEnvGroup(context),
    modSsgHwEnvGroup(context),
    modSsgSwEnvGroup(context),
    modSsgSwEnv11Group(context),
    modLfoGroup(context),
    modPitchEnvGroup(context),
    modSsgSwPEnv11Group(context),
    modWtModGroup(context),
    modMulDetuneGroup(context),
    modUnisonGroup(context),
    ampEnvComponent(context),
    ssgHwEnvComponent(context),
    ssgSwEnvComponent(context),
    ssgSwEnv11Component(context),
    lfoComponent(context),
    pitchEnvComponent(context),
    ssgSwPEnv11Component(context),
    wtModComponent(context),
    mulDetuneComponent(context),
    unisonComponent(context),
    wtModBaseFreqSlider(context),
    tremGroup(context),
    vibGroup(context),
    mbcGroup(context),
    delayGroup(context),
    reverbGroup(context),
    filterGroup(context),
    eq3bGroup(context),
    sfceGroup(context),
    pcmGroup(context),
    pcmBypassBtn(context),
    pcmSeparator(context),
    pcmBitSelector(context),
    pcmRateSelector(context),
    pcmInterpSelector(context),
    pcmMixSlider(context),
    pcmDryBtn(context),
    pcmHalfBtn(context),
    pcmWetBtn(context),
    bypassToggle(context),
    mainSeparator(context),
    resetBtn(context),
    routeSeparator(context),
    showRouteBtn(context),
    routeFx{ GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context), GuiLabel(context) },
    routeUp{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
    routeDown{ GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context), GuiTextButton(context) },
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

    // 出力へ掛ける変調。音源ではチャンネルごとに持っていたものを、
    // ここでは出力に対して 1 組だけ持つ。
    //
    // エンベロープは MIDI の押し離しで動くので、鍵盤を触らなければ
    // 素通しになる。LFO は押さなくても回るため、札を分けてある。
    // 使う・使わないの札は 1 つの枠へまとめる。色は他の効果と同じ青系統。
    modSwitchGroup.setup(*this, juce::String("") + "変調の使用");
    modSwitchGroup.setBackgroundColor(groupBgColour);

    envBypassToggle.setup({ .parent = *this,
        .id = ModPrKey::prefix + ModPrKey::Env::bypass,
        .title = juce::String("") + "エンベロープをバイパス",
        .isReset = true });
    envBypassToggle.setWantsKeyboardFocus(true);
    envBypassToggle.setExplicitFocusOrder(++tabOrder);

    lfoBypassToggle.setup({ .parent = *this,
        .id = ModPrKey::prefix + ModPrKey::Lfo::bypass,
        .title = juce::String("") + "LFO をバイパス",
        .isReset = true });
    lfoBypassToggle.setWantsKeyboardFocus(true);
    lfoBypassToggle.setExplicitFocusOrder(++tabOrder);

    modAmpEnvGroup.setup(*this, juce::String("") + "AMP ENV");
    modSsgHwEnvGroup.setup(*this, juce::String("") + "SSG HW AMP ENV");
    modSsgSwEnvGroup.setup(*this, juce::String("") + "SSG SW AMP ENV");
    modSsgSwEnv11Group.setup(*this, juce::String("") + "SSG SW AMP ENV[11]");
    modLfoGroup.setup(*this, juce::String("") + "LFO");

    // 変調の枠も、他の効果と同じ青系統に塗る。
    for (auto* group : {
        &modAmpEnvGroup, &modSsgHwEnvGroup, &modSsgSwEnvGroup, &modSsgSwEnv11Group, &modLfoGroup })
    {
        group->setBackgroundColor(groupBgColour);
    }

    ampEnvComponent.setupComponent(modAmpEnvGroup.contentCanvas, ModPrKey::prefix, tabOrder);

    // 見出しの色は、実機の機能か独自の機能かで塗り分けている。
    // ここは SSG チャンネルではなく出力へ借りて置くので、既定のままにする。
    ssgHwEnvComponent.setupComponent(modSsgHwEnvGroup.contentCanvas, ModPrKey::prefix, tabOrder);

    ssgSwEnvComponent.setupComponent(modSsgSwEnvGroup.contentCanvas, ModPrKey::prefix, tabOrder,
        CPK::ssgSwEnv + CPK::bypass, FxGuiText::Mod::SsgSwEnv::bypass);

    ssgSwEnv11Component.setupComponent(modSsgSwEnv11Group.contentCanvas, ModPrKey::prefix, tabOrder,
        CPK::ssgSwEnv11 + CPK::bypass, FxGuiText::Mod::SsgSwEnv11::bypass);

    lfoComponent.setupComponent(modLfoGroup.contentCanvas, ModPrKey::prefix, tabOrder);

    // 音程側。入ってきた音を溜めてから読み出す速さを変えるので、
    // 音量側とは別に入り切りできるようにしてある。
    pitchBypassToggle.setup({ .parent = *this,
        .id = ModPrKey::prefix + ModPrKey::Pitch::bypass,
        .title = juce::String("") + "ピッチ変調をバイパス",
        .isReset = true });
    pitchBypassToggle.setWantsKeyboardFocus(true);
    pitchBypassToggle.setExplicitFocusOrder(++tabOrder);

    modPitchEnvGroup.setup(*this, juce::String("") + "PITCH ENV");
    modSsgSwPEnv11Group.setup(*this, juce::String("") + "SSG SW PITCH ENV[11]");
    modWtModGroup.setup(*this, juce::String("") + "WT PITCH MOD");

    for (auto* group : { &modPitchEnvGroup, &modSsgSwPEnv11Group, &modWtModGroup })
    {
        group->setBackgroundColor(groupBgColour);
    }

    pitchEnvComponent.setupComponent(modPitchEnvGroup.contentCanvas, ModPrKey::prefix, tabOrder,
        CPK::pitchAdsr + CPK::bypass, FxGuiText::Mod::PitchEnv::bypass);

    ssgSwPEnv11Component.setupComponent(modSsgSwPEnv11Group.contentCanvas, ModPrKey::prefix, tabOrder,
        CPK::ssgSwPEnv11 + CPK::bypass, FxGuiText::Mod::SsgSwPEnv11::bypass);

    wtModComponent.setupComponent(modWtModGroup.contentCanvas, ModPrKey::prefix, tabOrder);

    wtModBaseFreqSlider.setup({ .parent = modWtModGroup.contentCanvas,
        .id = ModPrKey::prefix + ModPrKey::WtMod::baseFreq,
        .title = FxGuiText::Mod::WtMod::baseFreq,
        .isReset = true });
    wtModBaseFreqSlider.setWantsKeyboardFocus(true);
    wtModBaseFreqSlider.setExplicitFocusOrder(++tabOrder);

    // 音程を一定量ずらすもの。鍵盤を押さなくても掛かるので、
    // 押し離しで動くエンベロープとは別の札にしてある。
    shiftBypassToggle.setup({ .parent = *this,
        .id = ModPrKey::prefix + ModPrKey::Shift::bypass,
        .title = juce::String("") + "音程ずらしをバイパス",
        .isReset = true });
    shiftBypassToggle.setWantsKeyboardFocus(true);
    shiftBypassToggle.setExplicitFocusOrder(++tabOrder);

    modMulDetuneGroup.setup(*this, juce::String("") + "MUL・DET");
    modUnisonGroup.setup(*this, juce::String("") + "UNISON・HARMONY");

    for (auto* group : { &modMulDetuneGroup, &modUnisonGroup })
    {
        group->setBackgroundColor(groupBgColour);
    }

    mulDetuneComponent.setupComponent(modMulDetuneGroup.contentCanvas, ModPrKey::prefix, tabOrder);
    unisonComponent.setupComponent(modUnisonGroup.contentCanvas, ModPrKey::prefix, tabOrder);
    mainGroup.setBackgroundColor(groupBgColour);

	bypassToggle.setup({ .parent = *this, .id = code + FxPrKey::bypass, .title = FxGuiText::Fx::masterBypass, .isReset = true });
    bypassToggle.setWantsKeyboardFocus(true);
    bypassToggle.setExplicitFocusOrder(++tabOrder);

	mainSeparator.setupComponent(*this);

    resetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::reset, .textColor = juce::Colours::white, .bgColor = juce::Colours::grey });
    resetBtn.setWantsKeyboardFocus(true);
    resetBtn.setExplicitFocusOrder(++tabOrder);
    resetBtn.onClick = [&] { this->ctx.audioProcessor.initParams("FX_"); };

    routeSeparator.setupComponent(*this);

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

    fileSeparator.setupComponent(*this);

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
    eq3bGroup.setup(*this, FxGuiText::Group::fxEq3B);
    eq3bGroup.setBackgroundColor(groupBgColour);
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
	tremGroup.setup(*this, FxGuiText::Group::fxTremolo);
    tremGroup.setBackgroundColor(groupBgColour);
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
	vibGroup.setup(*this, FxGuiText::Group::fxVibrato);
    vibGroup.setBackgroundColor(groupBgColour);
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
	mbcGroup.setup(*this, FxGuiText::Group::fxMbc);
    mbcGroup.setBackgroundColor(groupBgColour);
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
	delayGroup.setup(*this, FxGuiText::Group::fxDelay);
    delayGroup.setBackgroundColor(groupBgColour);
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
	reverbGroup.setup(*this, FxGuiText::Group::fxReverb);
    reverbGroup.setBackgroundColor(groupBgColour);
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
    sfceGroup.setup(*this, FxGuiText::Group::sfcEcho);
    sfceGroup.setBackgroundColor(groupBgColour);
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

    // 2686V PCM Bit Crusher
    pcmGroup.setup(*this, FxGuiText::Group::fxPcm);
    pcmGroup.setBackgroundColor(groupBgColour);
    const juce::String pcmPrefix = code + FxPrKey::pcm;

    pcmBypassBtn.setup({ .parent = *this, .id = pcmPrefix + FxPrKey::bypass, .title = FxGuiText::Fx::bypass, .isReset = true });
    pcmBypassBtn.setWantsKeyboardFocus(true);
    pcmBypassBtn.setExplicitFocusOrder(++tabOrder);
    pcmBypassBtn.onStateChange = [this] {
        updatePcmEnabled();
    };

    pcmSeparator.setupComponent(*this);

    pcmBitSelector.setup({ .parent = *this, .id = pcmPrefix + FxPrKey::Pcm::bit, .title = FxGuiText::Fx::Pcm::bit, .items = getPcmBitItems(), .isReset = true });
    pcmBitSelector.setWantsKeyboardFocus(true);
    pcmBitSelector.setExplicitFocusOrder(++tabOrder);

    pcmRateSelector.setup({ .parent = *this, .id = pcmPrefix + FxPrKey::Pcm::rate, .title = FxGuiText::Fx::Pcm::rate, .items = QualityPcm::rateItems, .isReset = true });
    pcmRateSelector.setWantsKeyboardFocus(true);
    pcmRateSelector.setExplicitFocusOrder(++tabOrder);

    pcmInterpSelector.setup({ .parent = *this, .id = pcmPrefix + FxPrKey::Pcm::interp, .title = FxGuiText::Fx::Pcm::interp, .items = QualityPcm::interpItems, .isReset = true });
    pcmInterpSelector.setWantsKeyboardFocus(true);
    pcmInterpSelector.setExplicitFocusOrder(++tabOrder);

    pcmMixSlider.setup({ .parent = *this, .id = pcmPrefix + FxPrKey::mix, .title = FxGuiText::Fx::mix, .isReset = true });
    pcmMixSlider.setWantsKeyboardFocus(true);
    pcmMixSlider.setExplicitFocusOrder(++tabOrder);

    pcmDryBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::dry });
    pcmDryBtn.setWantsKeyboardFocus(true);
    pcmDryBtn.setExplicitFocusOrder(++tabOrder);
    pcmDryBtn.onClick = [&] { pcmMixSlider.setValue(0.0f); };

    pcmHalfBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::mix });
    pcmHalfBtn.setWantsKeyboardFocus(true);
    pcmHalfBtn.setExplicitFocusOrder(++tabOrder);
    pcmHalfBtn.onClick = [&] { pcmMixSlider.setValue(0.5f); };

    pcmWetBtn.setup({ .parent = *this, .title = FxGuiText::Fx::Mix::wet });
    pcmWetBtn.setWantsKeyboardFocus(true);
    pcmWetBtn.setExplicitFocusOrder(++tabOrder);
    pcmWetBtn.onClick = [&] { pcmMixSlider.setValue(1.0f); };

    // 変調の中身は最初から開いておく。FX タブでは 1 枠が小さく、
    // たたまれていると何が入っているのか分からないため。
    for (auto* group : {
        &modAmpEnvGroup, &modSsgHwEnvGroup, &modSsgSwEnvGroup, &modSsgSwEnv11Group, &modLfoGroup,
        &modPitchEnvGroup, &modSsgSwPEnv11Group, &modWtModGroup, &modMulDetuneGroup, &modUnisonGroup })
    {
        for (auto* child : group->contentCanvas.getChildren())
        {
            if (auto* cat = dynamic_cast<GuiCategoryLabel*>(child)) cat->setDetailVisible(true);
        }
    }

    updateFilterEnabled();
    updateTremoloEnabled();
    updateVibratoEnabled();
    updateMBCEnabled();
    updateDelayEnabled();
    updateReverbEnabled();
    updateEq3bEnabled();
    updateSfcEchoEnabled();
    updatePcmEnabled();
}

void GuiFx::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    pageArea.removeFromLeft(FxGuiValue::Fx::SideMargin);
    pageArea.removeFromRight(FxGuiValue::Fx::SideMargin);

    // 一番左がメイン。順番の一覧が縦に伸びるので、丈をいっぱいに使う。
    auto mainArea = pageArea.removeFromLeft(FxGuiValue::Fx::ColWidth);

    pageArea.removeFromLeft(FxGuiValue::Fx::ColGap);

    // その右を、上が FX の段、下が変調の段。
    auto fxArea = pageArea.removeFromTop(
        FxGuiValue::Fx::AreaHeightRow1 + FxGuiValue::Fx::SectionGap + FxGuiValue::Fx::AreaHeightRow2);

    pageArea.removeFromTop(FxGuiValue::Fx::SectionGap);

    // FX の右の空きへ変調を置く。タブを増やさずに収める。
    {
        auto modArea = pageArea;

        // 6 列 2 段。1 枠目は使う・使わないの札をまとめたもので、
        // 残り 10 枠に各機能が入る。
        int rowHeight = (modArea.getHeight() - FxGuiValue::Fx::SectionGap) / FxGuiValue::Fx::ModRows;

        auto upperRow = modArea.removeFromTop(rowHeight);

        modArea.removeFromTop(FxGuiValue::Fx::SectionGap);

        auto lowerRow = modArea;

        // 使う・使わないの札。ここだけは中身が札だけなので、
        // スクロールを持たない普通の枠にしてある。
        auto switchArea = upperRow.removeFromLeft(FxGuiValue::Fx::ColWidth);

        upperRow.removeFromLeft(FxGuiValue::Fx::ColGap);

        modSwitchGroup.setBounds(switchArea);

        auto switchRect = switchArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

        switchRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

        layoutMain({ .mainRect = switchRect, .component = &envBypassToggle });
        layoutMain({ .mainRect = switchRect, .component = &lfoBypassToggle });
        layoutMain({ .mainRect = switchRect, .component = &pitchBypassToggle });
        layoutMain({ .mainRect = switchRect, .component = &shiftBypassToggle });

        // 列を 1 つ切り出して、中身を上から積む。積んだ高さをそのまま
        // キャンバスの高さにするので、はみ出したぶんはスクロールで届く。
        auto layoutModColumn = [&](juce::Rectangle<int>& row, GuiScrollGroup& group, auto&& layoutBody)
        {
            auto colArea = row.removeFromLeft(FxGuiValue::Fx::ColWidth);

            row.removeFromLeft(FxGuiValue::Fx::ColGap);

            group.setBounds(colArea);

            auto inner = colArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

            inner.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

            group.setViewportCustomBounds(inner.translated(-colArea.getX(), -colArea.getY()));

            juce::Rectangle<int> rect(0, 0, group.getContentWidth(), 4000);

            layoutBody(rect);

            group.setContentHeight(rect.getY() + categoryContentTrailingPadding);
        };

        layoutModColumn(upperRow, modAmpEnvGroup, [&](juce::Rectangle<int>& rect) { ampEnvComponent.layoutComponent(rect); });
        layoutModColumn(upperRow, modSsgHwEnvGroup, [&](juce::Rectangle<int>& rect) { ssgHwEnvComponent.layoutComponent(rect); });
        layoutModColumn(upperRow, modSsgSwEnvGroup, [&](juce::Rectangle<int>& rect) { ssgSwEnvComponent.layoutComponent(rect); });
        layoutModColumn(upperRow, modSsgSwEnv11Group, [&](juce::Rectangle<int>& rect) { ssgSwEnv11Component.layoutComponent(rect); });

        layoutModColumn(upperRow, modLfoGroup, [&](juce::Rectangle<int>& rect) { lfoComponent.layoutComponent(rect); });

        layoutModColumn(lowerRow, modPitchEnvGroup, [&](juce::Rectangle<int>& rect) { pitchEnvComponent.layoutComponent(rect); });
        layoutModColumn(lowerRow, modSsgSwPEnv11Group, [&](juce::Rectangle<int>& rect) { ssgSwPEnv11Component.layoutComponent(rect); });
        layoutModColumn(lowerRow, modWtModGroup, [&](juce::Rectangle<int>& rect)
        {
            wtModComponent.layoutComponent(rect);

            wtModBaseFreqSlider.setBounds(rect.removeFromTop(FxGuiValue::Fx::ModBaseFreqHeight));
        });

        layoutModColumn(lowerRow, modMulDetuneGroup, [&](juce::Rectangle<int>& rect) { mulDetuneComponent.layoutComponent(rect); });
        layoutModColumn(lowerRow, modUnisonGroup, [&](juce::Rectangle<int>& rect) { unisonComponent.layoutComponent(rect); });
    }

    mainGroup.setBounds(mainArea);

    auto mRect = mainArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    mRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutMain({ .mainRect = mRect, .component = &bypassToggle });

    mainSeparator.layoutComponent(mRect);

    layoutMain({ .mainRect = mRect, .component = &resetBtn });

    layoutFxOrder(mRect);

    // 1 段目は背の低いもの 6 つ、2 段目は背の高いもの 3 つ。
    auto row1 = fxArea.removeFromTop(FxGuiValue::Fx::AreaHeightRow1);

    fxArea.removeFromTop(FxGuiValue::Fx::SectionGap);

    auto row2 = fxArea;

    // Filter
    auto rect1 = row1.removeFromLeft(FxGuiValue::Fx::ColWidth);
    auto flArea = rect1.removeFromTop(FxGuiValue::Fx::HeightFilter);

    filterGroup.setBounds(flArea);

    auto flRect = flArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    flRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = flRect, .component = &flBypassBtn });

    flSeparator.layoutComponent(flRect);

    layoutRow({ .rowRect = flRect, .label = &flTypeSelector.label, .component = &flTypeSelector, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = flRect, .label = &flFreqSlider.label, .component = &flFreqSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = flRect, .label = &flQSlider.label, .component = &flQSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    flRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = flRect, .label = &flMixSlider.label, .component = &flMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = flRect, .comp1 = &flDryBtn, .comp2 = &flHalfBtn, .comp3 = &flWetBtn });

    // 3-Band EQ
    row1.removeFromLeft(FxGuiValue::Fx::ColGap);
    auto rect2 = row1.removeFromLeft(FxGuiValue::Fx::ColWidth);
    auto eq3bArea = rect2.removeFromTop(FxGuiValue::Fx::HeightEq3b);

    eq3bGroup.setBounds(eq3bArea);

    auto eq3bRect = eq3bArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    eq3bRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = eq3bRect, .component = &eq3bBypassBtn });

    eq3bSeparator.layoutComponent(eq3bRect);

    layoutRow({ .rowRect = eq3bRect, .label = &eq3bLowGainDbSlider.label, .component = &eq3bLowGainDbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = eq3bRect, .label = &eq3bMidFreqSlider.label, .component = &eq3bMidFreqSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = eq3bRect, .label = &eq3bMidGainDbSlider.label, .component = &eq3bMidGainDbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = eq3bRect, .label = &eq3bHighGainDbSlider.label, .component = &eq3bHighGainDbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    eq3bRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = eq3bRect, .label = &eq3bMixSlider.label, .component = &eq3bMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = eq3bRect, .comp1 = &eq3bDryBtn, .comp2 = &eq3bHalfBtn, .comp3 = &eq3bWetBtn });

    // Tremolo
    row1.removeFromLeft(FxGuiValue::Fx::ColGap);
    auto rect3 = row1.removeFromLeft(FxGuiValue::Fx::ColWidth);
    auto trmArea = rect3.removeFromTop(FxGuiValue::Fx::HeightTremoro);

    tremGroup.setBounds(trmArea);

    auto trmRect = trmArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    trmRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = trmRect, .component = &tBypassBtn });

    tSeparator.layoutComponent(trmRect);

    layoutRow({ .rowRect = trmRect, .label = &tRateSlider.label, .component = &tRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = trmRect, .label = &tDepthSlider.label, .component = &tDepthSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    trmRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = trmRect, .label = &tMixSlider.label, .component = &tMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = trmRect, .comp1 = &tDryBtn, .comp2 = &tHalfBtn, .comp3 = &tWetBtn });

    // Vibrato
    row1.removeFromLeft(FxGuiValue::Fx::ColGap);
    auto rect4 = row1.removeFromLeft(FxGuiValue::Fx::ColWidth);
    auto vibArea = rect4.removeFromTop(FxGuiValue::Fx::HeightVibrato);

    vibGroup.setBounds(vibArea);

    auto vibRect = vibArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    vibRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = vibRect, .component = &vBypassBtn });

    vSeparator.layoutComponent(vibRect);

    layoutRow({ .rowRect = vibRect, .label = &vRateSlider.label, .component = &vRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = vibRect, .label = &vDepthSlider.label, .component = &vDepthSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    vibRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = vibRect, .label = &vMixSlider.label, .component = &vMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = vibRect, .comp1 = &vDryBtn, .comp2 = &vHalfBtn, .comp3 = &vWetBtn });

    // Modern Bit Crusher
    row1.removeFromLeft(FxGuiValue::Fx::ColGap);
    auto rect5 = row1.removeFromLeft(FxGuiValue::Fx::ColWidth);
    auto mbcArea = rect5.removeFromTop(FxGuiValue::Fx::HeightMbc);

    mbcGroup.setBounds(mbcArea);

    auto mbcRect = mbcArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    mbcRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = mbcRect, .component = &mbcBypassBtn });

    mbcSeparator.layoutComponent(mbcRect);

    layoutRow({ .rowRect = mbcRect, .label = &mbcBitsSlider.label, .component = &mbcBitsSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = mbcRect, .label = &mbcRateSlider.label, .component = &mbcRateSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    mbcRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = mbcRect, .label = &mbcMixSlider.label, .component = &mbcMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = mbcRect, .comp1 = &mbcDryBtn, .comp2 = &mbcHalfBtn, .comp3 = &mbcWetBtn });

    // Delay
    row1.removeFromLeft(FxGuiValue::Fx::ColGap);
    auto rect6 = row1.removeFromLeft(FxGuiValue::Fx::ColWidth);
    auto dlyArea = rect6.removeFromTop(FxGuiValue::Fx::HeightDelay);

    delayGroup.setBounds(dlyArea);

    auto dlyRect = dlyArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    dlyRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = dlyRect, .component = &dBypassBtn });

    dSeparator.layoutComponent(dlyRect);

    layoutRow({ .rowRect = dlyRect, .label = &dTimeSlider.label, .component = &dTimeSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = dlyRect, .label = &dFbSlider.label, .component = &dFbSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    dlyRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = dlyRect, .label = &dMixSlider.label, .component = &dMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = dlyRect, .comp1 = &dDryBtn, .comp2 = &dHalfBtn, .comp3 = &dWetBtn });

    // Reverb
    row2.removeFromLeft(FxGuiValue::Fx::ColGap);
    auto rect7 = row2.removeFromLeft(FxGuiValue::Fx::ColWidth);
    auto rvbArea = rect7.removeFromTop(FxGuiValue::Fx::HeightReverb);

    reverbGroup.setBounds(rvbArea);

    auto rvbRect = rvbArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    rvbRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = rvbRect, .component = &rBypassBtn });

    rSeparator.layoutComponent(rvbRect);

    layoutRow({ .rowRect = rvbRect, .label = &rSizeSlider.label, .component = &rSizeSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = rvbRect, .label = &rDampSlider.label, .component = &rDampSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    rvbRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = rvbRect, .label = &rMixSlider.label, .component = &rMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = rvbRect, .comp1 = &rDryBtn, .comp2 = &rHalfBtn, .comp3 = &rWetBtn });

    // SfcEcho
    row2.removeFromLeft(FxGuiValue::Fx::ColGap);
    auto rect8 = row2.removeFromLeft(FxGuiValue::Fx::ColWidth);
    auto sfceArea = rect8.removeFromTop(FxGuiValue::Fx::HeightSfcEcho);

    sfceGroup.setBounds(sfceArea);

    auto sfceRect = sfceArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    sfceRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = sfceRect, .component = &sfceBypassBtn });

    sfceSeparator.layoutComponent(sfceRect);

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

    // 2686V PCM Bit Crusher
    // 9 個目なので、3 列目の一番下へ置く。
    row2.removeFromLeft(FxGuiValue::Fx::ColGap);
    auto rect9 = row2.removeFromLeft(FxGuiValue::Fx::ColWidth);
    auto pcmArea = rect9.removeFromTop(FxGuiValue::Fx::HeightPcm);

    pcmGroup.setBounds(pcmArea);

    auto pcmRect = pcmArea.reduced(FxGuiValue::Group::Padding::width, FxGuiValue::Group::Padding::height);

    pcmRect.removeFromTop(FxGuiValue::Group::TitlePaddingTop);

    layoutRow({ .rowRect = pcmRect, .component = &pcmBypassBtn });

    pcmSeparator.layoutComponent(pcmRect);

    layoutRow({ .rowRect = pcmRect, .label = &pcmBitSelector.label, .component = &pcmBitSelector, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = pcmRect, .label = &pcmRateSelector.label, .component = &pcmRateSelector, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRow({ .rowRect = pcmRect, .label = &pcmInterpSelector.label, .component = &pcmInterpSelector, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    pcmRect.removeFromTop(FxGuiValue::Padding::space);
    layoutRow({ .rowRect = pcmRect, .label = &pcmMixSlider.label, .component = &pcmMixSlider, .labelWidth = FxGuiValue::Fx::AreaLabelWidth });
    layoutRowThreeComps({ .rect = pcmRect, .comp1 = &pcmDryBtn, .comp2 = &pcmHalfBtn, .comp3 = &pcmWetBtn });
}

void GuiFx::layoutFxOrder(juce::Rectangle<int> rect) {
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
void GuiFx::updatePcmEnabled() {
    bool bypassed = pcmBypassBtn.getToggleState();

    pcmSeparator.setEnabled(!bypassed);
    pcmBitSelector.setEnabledWithLabel(!bypassed);
    pcmRateSelector.setEnabledWithLabel(!bypassed);
    pcmInterpSelector.setEnabledWithLabel(!bypassed);
    pcmMixSlider.setEnabledWithLabel(!bypassed);
    pcmDryBtn.setEnabled(!bypassed);
    pcmHalfBtn.setEnabled(!bypassed);
    pcmWetBtn.setEnabled(!bypassed);
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
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importFxOrderFile, defaultDir, Io::ExtensionGlob::fxOrder);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultFxOrderDir = file.getParentDirectory().getFullPathName();

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

                    // このプラグインに無い効果は読み飛ばす。
                    // 他のプラグインにしかないものが混ざっていても壊れない。
                    if (id < 0 || id >= NumEffects) continue;

                    // 同じものが二度出てきたら後のほうは捨てる
                    if (std::find(newOrders.begin(), newOrders.end(), id) != newOrders.end()) continue;

                    newOrders.push_back(id);
                }

                // 書かれていなかった効果を後ろへ足す。必ず全部そろえる。
                for (int i = 0; i < NumEffects; i++) {
                    if (std::find(newOrders.begin(), newOrders.end(), i) == newOrders.end()) {
                        newOrders.push_back(i);
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
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportFxOrderFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::fxOrder)), Io::saveGlob(Io::Extension::fxOrder));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultFxOrderDir = file.getParentDirectory().getFullPathName();

                // 1行目にサンプル数
                Io::ParamWriter writer(fxOrderFormat);
                writeFxOrder(writer);

                writer.writeTo(file);
            }
        });
}

void GuiFx::importFxParam()
{
    juce::File defaultDir(ctx.audioProcessor.defaultFxParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importFxParamFile, defaultDir, Io::ExtensionGlob::fxParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultFxParamDir = file.getParentDirectory().getFullPathName();

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
        });
}

void GuiFx::exportFxParam()
{
    juce::File defaultDir(ctx.audioProcessor.defaultFxParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportFxParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::fxParam)), Io::saveGlob(Io::Extension::fxParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultFxParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(fxParamFormat);
                writeFxParams(writer);

                writer.writeTo(file);
            }
        });
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

    ctx.audioProcessor.updateFxOrder(newOrders);

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

// FX のパラメータを初期値へ戻す。
//
// 接頭辞で選ぶので、FX_ で始まるものだけが対象になる。順番は
// パラメータではないので、ここでは戻らない。
void GuiFx::initParams()
{
    ctx.audioProcessor.initParams(FxPrKey::prefix + "_");
}
