#include <vector>

#include "./Core.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Processor/Ssg/Keys.h"
#include "../../Processor/Ssg/Values.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiValues.h"
#include "./GuiText.h"
#include "../../Core/Gui/GuiStructs.h"

static std::vector<SelectItem> bdItems = {
    {.name = "1: 4-bit (16 steps)",  .value = 1 },
    {.name = "2: 5-bit (32 steps)",  .value = 2 },
    {.name = "3: 6-bit (64 steps)",  .value = 3 },
    {.name = "4: 8-bit (256 steps)", .value = 4 },
    {.name = "5: Raw",               .value = 5 },
    {.name = "6: 7-bit (OPLL/128 steps)", .value = 6 }
};

static std::vector<SelectItem> rateItems = {
    {.name = " 1: 96kHz",    .value = 1 },
    {.name = " 2: 55.5kHz",  .value = 2 },
    {.name = " 3: 49.7kHz",  .value = 3 },
    {.name = " 4: 48kHz",    .value = 4 },
    {.name = " 5: 44.1kHz",  .value = 5 },
    {.name = " 6: 22.05kHz", .value = 6 },
    {.name = " 7: 16kHz",    .value = 7 },
    {.name = " 8: 12kHz",    .value = 8 },
    {.name = " 9: 11kHz",    .value = 9 },
    {.name = "10: 8kHz",     .value = 10 },
    {.name = "11: 5.5kHz",   .value = 11 },
    {.name = "12: 4kHz",     .value = 12 },
    {.name = "13: 2kHz",     .value = 13 },
};

static std::vector<SelectItem> ssgEnvItems = {
    {.name = "0: Saw Down",                    .value = 1 },
    {.name = "1: Saw Down & Hold",             .value = 2 },
    {.name = "2: Triangle",                    .value = 3 },
    {.name = "3: Alternative Saw Down & Hold", .value = 4 },
    {.name = "4: Saw Up",                      .value = 5 },
    {.name = "5: Saw Up & Hold",               .value = 6 },
    {.name = "6: Triangle Invert",             .value = 7 },
    {.name = "7: Alternative Saw Up & Hold",   .value = 8 },
};

static std::vector<SelectItem> ssgPrItems = {
    {.name = "0: 1:1 (50%)",     .value = 1 },
    {.name = "1: 3:5 (37.5%)",   .value = 2 },
    {.name = "2: 5:11 (31.25%)", .value = 3 },
    {.name = "3: 1:3 (25%)",     .value = 4 },
    {.name = "4: 1:4 (20%)",     .value = 5 },
    {.name = "5: 3:13 (18.75%)", .value = 6 },
    {.name = "6: 1:7 (12.5%)",   .value = 7 },
    {.name = "7: 1:15 (6.25%)",  .value = 8 },
};

static std::vector<SelectItem> ssgWsItems = {
    {.name = "0: Pulse(Rect)",    .value = 1 },
    {.name = "1: Triangle / Saw", .value = 2 },
};

static std::vector<SelectItem> ssgDmItems = {
    {.name = "0: Preset Ratio",      .value = 1 },
    {.name = "1: Variable (Slider)", .value = 2 },
};

static std::vector<SelectItem> lfoPmShapeItems = {
    {.name = "0: Sine",                .value = 1 },
    {.name = "1: Saw Up",              .value = 2 },
    {.name = "2: Saw Down",            .value = 3 },
    {.name = "3: Square",              .value = 4 },
    {.name = "4: Triangle",            .value = 5 },
    {.name = "5: Sample & Hold",       .value = 6 },
    {.name = "6: Saw Down & One Shot", .value = 7 },
    {.name = "7: Triangle & One Shot", .value = 8 },
};

static std::vector<SelectItem> lfoAmShapeItems = {
    {.name = "0: Sine",                .value = 1 },
    {.name = "1: Saw Up",              .value = 2 },
    {.name = "2: Saw Down",            .value = 3 },
    {.name = "3: Square",              .value = 4 },
    {.name = "4: Triangle",            .value = 5 },
    {.name = "5: Sample & Hold",       .value = 6 },
    {.name = "6: Saw Down & One Shot", .value = 7 },
    {.name = "7: Triangle & One Shot", .value = 8 },
};

void GuiSsg::setup()
{
    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    const juce::String code = SsgPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, SsgGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(18.0f));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    qualityCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::visibleQuality, .invisibleTitle = SsgGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = *this, .id = code + SsgPrKey::bit, .title = SsgGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = *this, .id = code + SsgPrKey::rate, .title = SsgGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    fixComponent.setupComponent(*this, code, tabOrder, "-> 440Hz", 440);

    unisonComponent.setupComponent(*this, code, tabOrder);

    ampEnvComponent.setupComponent(*this, code, tabOrder);

    pitchEnvComponent.setupComponent(*this, code, tabOrder, SsgPrKey::pitchAdsr + SsgPrKey::bypass, SsgGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(*this, code, tabOrder, SsgPrKey::ssgSwEnv + SsgPrKey::bypass, SsgGuiText::SsgSwEnv::bypass);

    mulDetuneComponent.setupComponent(*this, code, tabOrder);

    hwEnvCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::visibleHwEnv, .invisibleTitle = SsgGuiText::Category::invisibleHwEnv, .enableChangeDetailVisible = true });

    envEnableButton.setup({ .parent = *this, .id = code + SsgPrKey::HwEnv::enable, .title = SsgGuiText::Ssg::HwEnv::enable, .isReset = true });
    envEnableButton.setWantsKeyboardFocus(true);
    envEnableButton.setExplicitFocusOrder(++tabOrder);

    shapeSelector.setup({ .parent = *this, .id = code + SsgPrKey::HwEnv::shape, .title = SsgGuiText::Ssg::HwEnv::shape, .items = ssgEnvItems, .isReset = true });
    shapeSelector.setWantsKeyboardFocus(true);
    shapeSelector.setExplicitFocusOrder(++tabOrder);

    periodSlider.setup({ .parent = *this, .id = code + SsgPrKey::HwEnv::period, .title = SsgGuiText::Ssg::HwEnv::speed, .isReset = true, .regType = RegisterType::SsgEnv });
    periodSlider.setWantsKeyboardFocus(true);
    periodSlider.setExplicitFocusOrder(++tabOrder);

    lfoCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::visibleLfo, .invisibleTitle = SsgGuiText::Category::invisibleLfo, .enableChangeDetailVisible = true });

    lfoPmFreqSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::pmFreq, .title = SsgGuiText::Lfo::pmSpeed, .isReset = true });
    lfoPmFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoPmFreqSlider.setWantsKeyboardFocus(true);
    lfoPmFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmFreqSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::amFreq, .title = SsgGuiText::Lfo::amSpeed, .isReset = true });
    lfoAmFreqSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoAmFreqSlider.setWantsKeyboardFocus(true);
    lfoAmFreqSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmSmRtSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::amSmoothRatio, .title = SsgGuiText::Lfo::amSmoothRatio, .isReset = true });
    lfoAmSmRtSlider.setWantsKeyboardFocus(true);
    lfoAmSmRtSlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelaySlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::syncDelay, .title = SsgGuiText::Lfo::syncDelay, .isReset = true });
    lfoSyncDelaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    lfoSyncDelaySlider.setWantsKeyboardFocus(true);
    lfoSyncDelaySlider.setExplicitFocusOrder(++tabOrder);

    lfoSyncDelayToZeroBtn.setup({ .parent = *this, .title = "Async", .isReset = false, .isResized = false });
    lfoSyncDelayToZeroBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToZeroBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToZeroBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(0.0f);
        };

    lfoSyncDelayToOneBtn.setup({ .parent = *this, .title = "Sync", .isReset = false, .isResized = false });
    lfoSyncDelayToOneBtn.setWantsKeyboardFocus(true);
    lfoSyncDelayToOneBtn.setExplicitFocusOrder(++tabOrder);
    lfoSyncDelayToOneBtn.onClick = [this] {
        lfoSyncDelaySlider.setValue(1.0f);
        };

    lfoPmToggle.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::pm, .title = SsgGuiText::Lfo::pmEn, .isReset = true });
    lfoPmToggle.setWantsKeyboardFocus(true);
    lfoPmToggle.setExplicitFocusOrder(++tabOrder);

    lfoPmShapeSelector.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::pmShape, .title = SsgGuiText::Lfo::pmShape, .items = lfoPmShapeItems, .isReset = true });
    lfoPmShapeSelector.setWantsKeyboardFocus(true);
    lfoPmShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoPmsSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::pms, .title = SsgGuiText::Lfo::pms, .isReset = true });
    lfoPmsSlider.setWantsKeyboardFocus(true);
    lfoPmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoPmdSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::pmd, .title = SsgGuiText::Lfo::pmd, .isReset = true });
    lfoPmdSlider.setWantsKeyboardFocus(true);
    lfoPmdSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmToggle.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::am, .title = SsgGuiText::Lfo::amEn, .isReset = true });
    lfoAmToggle.setWantsKeyboardFocus(true);
    lfoAmToggle.setExplicitFocusOrder(++tabOrder);

    lfoAmShapeSelector.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::amShape, .title = SsgGuiText::Lfo::amShape, .items = lfoAmShapeItems, .isReset = true });
    lfoAmShapeSelector.setWantsKeyboardFocus(true);
    lfoAmShapeSelector.setExplicitFocusOrder(++tabOrder);

    lfoAmsSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::ams, .title = SsgGuiText::Lfo::ams, .isReset = true });
    lfoAmsSlider.setWantsKeyboardFocus(true);
    lfoAmsSlider.setExplicitFocusOrder(++tabOrder);

    lfoAmdSlider.setup({ .parent = *this, .id = code + SsgPrKey::Lfo::amd, .title = SsgGuiText::Lfo::amd, .isReset = true });
    lfoAmdSlider.setWantsKeyboardFocus(true);
    lfoAmdSlider.setExplicitFocusOrder(++tabOrder);

    monoPolyCat.setupOtherCategory({ .parent = *this, .title = SsgGuiText::Category::visibleMonoMode, .invisibleTitle = SsgGuiText::Category::invisibleMonoMode, .enableChangeDetailVisible = true });

    monoModeToggle.setup({ .parent = *this, .id = SsgPrKey::monoMode, .title = SsgGuiText::monoPoly, .isReset = true });
    monoModeToggle.setWantsKeyboardFocus(true);
    monoModeToggle.setExplicitFocusOrder(++tabOrder);

	voiceGroup.setup(*this, SsgGuiText::Group::voice);

    shapeCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::shape });

    waveSelector.setup({ .parent = *this, .id = code + SsgPrKey::wveform, .title = SsgGuiText::Ssg::Voice::form, .items = ssgWsItems, .isReset = true, .isResized = true });
    waveSelector.setWantsKeyboardFocus(true);
    waveSelector.setExplicitFocusOrder(++tabOrder);

    toneCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::ssgTone });

    levelSlider.setup({ .parent = *this, .id = code + SsgPrKey::tone, .title = SsgGuiText::Ssg::Voice::tone, .isReset = true, .regType = RegisterType::SsgVol });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    noiseCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::ssgNoise });

    noiseSlider.setup({ .parent = *this, .id = code + SsgPrKey::noise, .title = SsgGuiText::Ssg::Voice::noise, .isReset = true, .regType = RegisterType::SsgVol });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = *this, .id = code + SsgPrKey::noiseFreq, .title = SsgGuiText::Ssg::Voice::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    noiseOnNoteButton.setup({ .parent = *this, .id = code + SsgPrKey::noiseOnNote, .title = SsgGuiText::Ssg::Voice::noiseOnNote, .isReset = true });
    noiseOnNoteButton.setWantsKeyboardFocus(true);
    noiseOnNoteButton.setExplicitFocusOrder(++tabOrder);

    mixCat.setupHwCategory({ .parent = *this, .title = SsgGuiText::Category::mix });

    // 初期状態反映
    mixSlider.setup({ .parent = *this, .id = code + SsgPrKey::mix , .title = SsgGuiText::Ssg::Voice::mix, .isReset = true });
    mixSlider.setWantsKeyboardFocus(true);
    mixSlider.setExplicitFocusOrder(++tabOrder);

    mixSetTone.setup({ .parent = *this, .title = SsgGuiText::Ssg::Voice::tone, .isReset = false, .isResized = false });
    mixSetTone.setWantsKeyboardFocus(true);
    mixSetTone.setExplicitFocusOrder(++tabOrder);
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };

    mixSetMix.setup({ .parent = *this, .title = SsgGuiText::Ssg::Voice::mix, .isReset = false, .isResized = false });
    mixSetMix.setWantsKeyboardFocus(true);
    mixSetMix.setExplicitFocusOrder(++tabOrder);
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };

    mixSetNoise.setup({ .parent = *this, .title = SsgGuiText::Ssg::Voice::noise, .isReset = false, .isResized = false });
    mixSetNoise.setWantsKeyboardFocus(true);
    mixSetNoise.setExplicitFocusOrder(++tabOrder);
    mixSetNoise.onClick = [this] { mixSlider.setValue(1.0, juce::sendNotification); };

    // Duty Controls Setup
    dutyGroup.setup(*this, SsgGuiText::Group::ssgDuty);

    dutyModeSelector.setup({ .parent = *this, .id = code + SsgPrKey::Duty::mode, .title = SsgGuiText::Ssg::Duty::mode, .items = ssgDmItems, .isReset = true, .isResized = true });
    dutyModeSelector.setWantsKeyboardFocus(true);
    dutyModeSelector.setExplicitFocusOrder(++tabOrder);

    dutyPresetSelector.setup({ .parent = *this, .id = code + SsgPrKey::Duty::preset, .title = SsgGuiText::Ssg::Duty::preset, .items = ssgPrItems, .isReset = true, .isResized = true });
    dutyPresetSelector.setWantsKeyboardFocus(true);
    dutyPresetSelector.setExplicitFocusOrder(++tabOrder);

    dutyVarSlider.setup({ .parent = *this, .id = code + SsgPrKey::Duty::var, .title = SsgGuiText::Ssg::Duty::var, .isReset = true });
    dutyVarSlider.setWantsKeyboardFocus(true);
    dutyVarSlider.setExplicitFocusOrder(++tabOrder);

    pulseInvCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::invert });

    dutyInvertButton.setup({ .parent = *this, .id = code + SsgPrKey::Duty::inv, .title = SsgGuiText::Ssg::Duty::invert, .isReset = true, .isResized = true });
    dutyInvertButton.setWantsKeyboardFocus(true);
    dutyInvertButton.setExplicitFocusOrder(++tabOrder);

    triGroup.setup(*this, SsgGuiText::Group::ssgTri);

    triKeyTrackButton.setup({ .parent = *this, .id = code + SsgPrKey::Tri::keyTrk, .title = SsgGuiText::Ssg::Tri::keyTrack, .isReset = true, .isResized = true });
    triKeyTrackButton.setWantsKeyboardFocus(true);
    triKeyTrackButton.setExplicitFocusOrder(++tabOrder);

    triFreqSlider.setup({ .parent = *this, .id = code + SsgPrKey::Tri::freq, .title = SsgGuiText::Ssg::Tri::manualFreq, .isReset = true });
    triFreqSlider.setWantsKeyboardFocus(true);
    triFreqSlider.setExplicitFocusOrder(++tabOrder);

    triPeakCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::peak });

    triPeakSlider.setup({ .parent = *this, .id = code + SsgPrKey::Tri::peak, .title = SsgGuiText::Ssg::Tri::peak, .isReset = true });
    triPeakSlider.setWantsKeyboardFocus(true);
    triPeakSlider.setExplicitFocusOrder(++tabOrder);

    triSetSawDown.setup({ .parent = *this, .title = SsgGuiText::Ssg::Tri::peakTo00, .isReset = false, .isResized = false });
    triSetSawDown.setWantsKeyboardFocus(true);
    triSetSawDown.setExplicitFocusOrder(++tabOrder);
    triSetSawDown.onClick = [this] { triPeakSlider.setValue(0.0, juce::sendNotification); };

    triSetTri.setup({ .parent = *this, .title = SsgGuiText::Ssg::Tri::peakTo05, .isReset = false, .isResized = false });
    triSetTri.setWantsKeyboardFocus(true);
    triSetTri.setExplicitFocusOrder(++tabOrder);
    triSetTri.onClick = [this] { triPeakSlider.setValue(0.5, juce::sendNotification); };

    triSetSawUp.setup({ .parent = *this, .title = SsgGuiText::Ssg::Tri::peakTo10, .isReset = false, .isResized = false });
    triSetSawUp.setWantsKeyboardFocus(true);
    triSetSawUp.setExplicitFocusOrder(++tabOrder);
    triSetSawUp.onClick = [this] { triPeakSlider.setValue(1.0, juce::sendNotification); };

    setupGraph();
    updateGraph();
}

void GuiSsg::layout(juce::Rectangle<int> content)
{
    juce::String code = SsgPrKey::prefix;
    auto pageArea = content.withZeroOrigin();

    auto mainArea = pageArea.removeFromLeft(SsgGuiValue::MainGroup::width);

    mainGroup.setBounds(mainArea);
    auto mRect = mainArea.reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);
    mRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mRect.removeFromTop(SsgGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    // グラフ用の区画を確保
    layoutGraph(mRect);
    updateGraph();

    ampEnvComponent.layoutComponent(mRect);

    pitchEnvComponent.layoutComponent(mRect);

    mulDetuneComponent.layoutComponent(mRect);

    layoutHwEnvCat(mRect);

    ssgSwEnvComponent.layoutComponent(mRect);

    layoutLfoCat(mRect);

    fixComponent.layoutComponent(mRect);

    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    layoutMonoModeCat(mRect);

    auto paramArea = pageArea.removeFromLeft(SsgGuiValue::ParamGroup::width);

    // --- Voice Group ---
    auto voiceArea = paramArea.removeFromTop(230);

    voiceGroup.setBounds(voiceArea);
    auto vRect = voiceGroup.getBounds().reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);

    vRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

    layoutRowCategory({ .rowRect = vRect, .label = &shapeCat });
    layoutRow({ .rowRect = vRect, .label = &waveSelector.label, .component = &waveSelector, });
    layoutRowCategory({ .rowRect = vRect, .label = &toneCat });
    layoutRow({ .rowRect = vRect, .label = &levelSlider.label, .component = &levelSlider, });
    layoutRowCategory({ .rowRect = vRect, .label = &noiseCat });
    layoutRow({ .rowRect = vRect, .label = &noiseSlider.label, .component = &noiseSlider });
    layoutRow({ .rowRect = vRect, .label = &noiseFreqSlider.label, .component = &noiseFreqSlider });
    layoutRow({ .rowRect = vRect, .component = &noiseOnNoteButton, });
    layoutRowCategory({ .rowRect = vRect, .label = &mixCat });
    layoutRow({ .rowRect = vRect, .label = &mixSlider.label, .component = &mixSlider });
    layoutRowThreeComps({ .rect = vRect, .comp1 = &mixSetTone, .comp2 = &mixSetMix, .comp3 = &mixSetNoise, .paddingBottom = 0 });

    // Wave Group
    float waveParam = *ctx.audioProcessor.apvts.getRawParameterValue(code + SsgPrKey::wveform);
    int waveMode = (waveParam > SsgPrValue::boolThread) ? 1 : 0;
    auto waveArea = paramArea.removeFromTop(140);

    if (waveMode == 0) // Pulse
    {
        dutyGroup.setVisible(true);
        triGroup.setVisible(false);

        triKeyTrackButton.setVisible(false);
        triPeakCat.setVisible(false);
        triPeakSlider.setVisibleWithLabel(false);
        triFreqSlider.setVisibleWithLabel(false);
        triSetSawDown.setVisible(false);
        triSetTri.setVisible(false);
        triSetSawUp.setVisible(false);

        pulseInvCat.setVisible(true);
        dutyModeSelector.setVisibleWithLabel(true);
        dutyVarSlider.setValue(true);
        dutyVarSlider.label.setVisible(true);
        dutyInvertButton.setVisible(true);

        dutyGroup.setBounds(waveArea);
        auto dRect = dutyGroup.getBounds().reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);

        dRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

        layoutRow({ .rowRect = dRect, .label = &dutyModeSelector.label, .component = &dutyModeSelector });

        float dutyModeVal = *ctx.audioProcessor.apvts.getRawParameterValue(code + SsgPrKey::Duty::mode);
        if (dutyModeVal < 0.5f) {
            dutyPresetSelector.setVisibleWithLabel(true);
            dutyVarSlider.setVisibleWithLabel(false);
            layoutRow({ .rowRect = dRect, .label = &dutyPresetSelector.label, .component = &dutyPresetSelector, });
        }
        else {
            dutyPresetSelector.setVisibleWithLabel(false);
            dutyVarSlider.setVisibleWithLabel(true);
            layoutRow({ .rowRect = dRect, .label = &dutyVarSlider.label, .component = &dutyVarSlider, });
        }

        layoutRowCategory({ .rowRect = dRect, .label = &pulseInvCat });
        layoutRow({ .rowRect = dRect, .component = &dutyInvertButton, .paddingBottom = 0 });
    }
    else // Triangle
    {
        dutyGroup.setVisible(false);
        triGroup.setVisible(true);

        pulseInvCat.setVisible(false);
        dutyModeSelector.setVisibleWithLabel(false);
        dutyInvertButton.setVisible(false);
        dutyPresetSelector.setVisibleWithLabel(false);
        dutyVarSlider.setVisible(false);
        dutyVarSlider.label.setVisible(false);

        triKeyTrackButton.setVisible(true);
        triFreqSlider.setVisibleWithLabel(true);
        triPeakCat.setVisible(true);
        triPeakSlider.setVisibleWithLabel(true);
        triSetSawDown.setVisible(true);
        triSetTri.setVisible(true);
        triSetSawUp.setVisible(true);

        triGroup.setBounds(waveArea);
        auto tRect = triGroup.getBounds().reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);

        tRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

        layoutRow({ .rowRect = tRect, .component = &triKeyTrackButton });

        bool isKeyTrack = triKeyTrackButton.getToggleState();
        triFreqSlider.setVisibleWithLabel(!isKeyTrack);

        if (!isKeyTrack) {
            layoutRow({ .rowRect = tRect, .label = &triFreqSlider.label, .component = &triFreqSlider, });
        }
        else
        {
            tRect.removeFromTop(SsgGuiValue::Category::paddingTop);
        }

        layoutRowCategory({ .rowRect = tRect, .label = &triPeakCat });
        layoutRow({ .rowRect = tRect, .label = &triPeakSlider.label, .component = &triPeakSlider });
        layoutRowThreeComps({ .rect = tRect, .comp1 = &triSetSawDown, .comp2 = &triSetTri, .comp3 = &triSetSawUp, .paddingBottom = 0 });
    }
}

void GuiSsg::updatePresetName(const juce::String& presetName)
{
    presetNameLabel.setText(presetName, juce::NotificationType::dontSendNotification);
}

void GuiSsg::initParams()
{
    this->ctx.audioProcessor.initParams("SSG_");
}

void GuiSsg::layoutQualityCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &qualityCat });

    bool visibleQuality = qualityCat.isDetailVisible();

    bitSelector.setVisibleWithLabel(visibleQuality);
    rateSelector.setVisibleWithLabel(visibleQuality);

    if (visibleQuality)
    {
        layoutMain({ .mainRect = rect, .label = &bitSelector.label, .component = &bitSelector });
        layoutMain({ .mainRect = rect, .label = &rateSelector.label, .component = &rateSelector, });
    }
}

void GuiSsg::layoutMonoModeCat(juce::Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &monoPolyCat });

    bool visible = monoPolyCat.isDetailVisible();

    monoModeToggle.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &monoModeToggle });
    }
}

void GuiSsg::layoutLfoCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &lfoCat });

    bool visible = lfoCat.isDetailVisible();

    lfoPmFreqSlider.setVisibleWithLabel(visible);
    lfoAmFreqSlider.setVisibleWithLabel(visible);
    lfoAmSmRtSlider.setVisibleWithLabel(visible);
    lfoSyncDelaySlider.setVisibleWithLabel(visible);
    lfoSyncDelayToZeroBtn.setVisible(visible);
    lfoSyncDelayToOneBtn.setVisible(visible);
    lfoPmToggle.setVisible(visible);
    lfoPmShapeSelector.setVisibleWithLabel(visible);
    lfoPmsSlider.setVisibleWithLabel(visible);
    lfoPmdSlider.setVisibleWithLabel(visible);
    lfoAmToggle.setVisible(visible);
    lfoAmShapeSelector.setVisibleWithLabel(visible);
    lfoAmsSlider.setVisibleWithLabel(visible);
    lfoAmdSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &lfoPmFreqSlider.label, .component = &lfoPmFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmFreqSlider.label, .component = &lfoAmFreqSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmSmRtSlider.label, .component = &lfoAmSmRtSlider });
        layoutMain({ .mainRect = rect, .label = &lfoSyncDelaySlider.label, .component = &lfoSyncDelaySlider });
        layoutMainTwoComps({ .rect = rect, .comp1 = &lfoSyncDelayToZeroBtn, .comp2 = &lfoSyncDelayToOneBtn });
        layoutMain({ .mainRect = rect, .component = &lfoPmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoPmShapeSelector.label, .component = &lfoPmShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoPmsSlider.label, .component = &lfoPmsSlider });
        layoutMain({ .mainRect = rect, .label = &lfoPmdSlider.label, .component = &lfoPmdSlider });
        layoutMain({ .mainRect = rect, .component = &lfoAmToggle });
        layoutMain({ .mainRect = rect, .label = &lfoAmShapeSelector.label, .component = &lfoAmShapeSelector });
        layoutMain({ .mainRect = rect, .label = &lfoAmsSlider.label, .component = &lfoAmsSlider });
        layoutMain({ .mainRect = rect, .label = &lfoAmdSlider.label, .component = &lfoAmdSlider });
    }
}

void GuiSsg::layoutHwEnvCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &hwEnvCat });

    bool visible = hwEnvCat.isDetailVisible();

    envEnableButton.setVisible(visible);
    shapeSelector.setVisibleWithLabel(visible);
    periodSlider.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &envEnableButton });
        layoutMain({ .mainRect = rect, .label = &shapeSelector.label, .component = &shapeSelector });
        layoutMain({ .mainRect = rect, .label = &periodSlider.label, .component = &periodSlider, .paddingBottom = 0 });
    }
}

void GuiSsg::setupGraph()
{
    addAndMakeVisible(&graph); // グラフを追加

    graphBtnAmp.setup({ .parent = *this, .title = "Amp", .isReset = false, .isResized = false });
    graphBtnAmp.setToggleState(true, juce::dontSendNotification); // デフォルトON
    graphBtnAmp.onClick = [this] { setGraphMode(GraphMode::Amp); };

    graphBtnPitch.setup({ .parent = *this, .title = "Pitch", .isReset = false, .isResized = false });
    graphBtnPitch.onClick = [this] { setGraphMode(GraphMode::Pitch); };

    graphBtnSsg.setup({ .parent = *this, .title = "SSG SW", .isReset = false, .isResized = false });
    graphBtnSsg.onClick = [this] { setGraphMode(GraphMode::SsgSw); };

    auto repaintGraph = [this]() { updateGraph(); };

    ampEnvComponent.setupGraph(repaintGraph);

    pitchEnvComponent.setupGraph(repaintGraph);

    ssgSwEnvComponent.setupGraph(repaintGraph);

    addAndMakeVisible(graphSeparator);
    graphSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });
}

void GuiSsg::setGraphMode(GraphMode mode)
{
    currentGraphMode = mode;

    // ラジオボタン的な排他制御
    graphBtnAmp.setToggleState(mode == GraphMode::Amp, juce::dontSendNotification);
    graphBtnPitch.setToggleState(mode == GraphMode::Pitch, juce::dontSendNotification);
    graphBtnSsg.setToggleState(mode == GraphMode::SsgSw, juce::dontSendNotification);

    // モードが変わったらグラフを描画し直す
    updateGraph();
}

void GuiSsg::layoutGraph(juce::Rectangle<int>& rect)
{
    auto mainArea = rect.removeFromTop(SsgGuiValue::MainGroup::Graph::height + SsgGuiValue::MainGroup::Separator::height);

    // 区切り線エリアを確保
    auto separatorArea = mainArea.removeFromBottom(SsgGuiValue::MainGroup::Separator::height);

    graphSeparator.setBounds(separatorArea);

    // そのうち下部20pxをボタンエリアにする
    auto btnArea = mainArea.removeFromBottom(SsgGuiValue::MainGroup::Graph::ButtonHeight);
    int btnWidth = btnArea.getWidth() / 3;

    graphBtnAmp.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnPitch.setBounds(btnArea.removeFromLeft(btnWidth));
    graphBtnSsg.setBounds(btnArea);

    // 残りをグラフエリアにする
    graph.setBounds(mainArea);
}

// グラフを再計算して描画
void GuiSsg::updateGraph()
{
    GraphMode mode = currentGraphMode;

    // カーブモードが有効かどうかを判定
    bool isCurveMode = p_guiCurve != nullptr && p_guiCurve->enable.getToggleState();

    // =============================================================
    // Pitch Env
    // =============================================================
    if (mode == GraphMode::Pitch) {
        pitchEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    // =============================================================
    // SSG SW Env
    // =============================================================
    else if (mode == GraphMode::SsgSw) {
        ssgSwEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
    // =============================================================
    // Amp Env
    // =============================================================
    else {
        ampEnvComponent.updateGraph(graph, p_curveCore, isCurveMode, 0);
    }
}
