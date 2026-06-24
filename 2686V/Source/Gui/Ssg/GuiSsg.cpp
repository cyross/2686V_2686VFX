#include <vector>

#include "./GuiSsg.h"

#include "../../Core/Processor/PluginProcessor.h"
#include "../../Core/Editor/PluginEditor.h"

#include "../../Processor/Ssg/ProcessorSsgKeys.h"
#include "../../Processor/Ssg/ProcessorSsgValues.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "./GuiSsgValues.h"
#include "./GuiSsgText.h"
#include "../../Core/Gui/GuiStructs.h"

// 1:4bit, 2:5bit, 3:6bit, 4:7bit, 5:8bit, 6:9bit, 7:10bit, 8:12bit, 9:16bit, 10:20bit, 11:24bit, 12:raw(32bit)
static std::vector<SelectItem> bdItems = {
    {.name = " 1:  4-bit (16 steps)",       .value = 1 },
    {.name = " 2:  5-bit (32 steps)",       .value = 2 },
    {.name = " 3:  6-bit (64 steps)",       .value = 3 },
    {.name = " 4:  7-bit (128 steps)",      .value = 4 },
    {.name = " 5:  8-bit (256 steps)",      .value = 5 },
    {.name = " 6:  9-bit (512 steps)",      .value = 6 },
    {.name = " 7: 10-bit (1024 steps)",     .value = 7 },
    {.name = " 8: 12-bit (4096 steps)",     .value = 8 },
    {.name = " 9: 16-bit (32768 steps)",    .value = 9 },
    {.name = "10: 20-bit (1048576 steps)",  .value = 10 },
    {.name = "11: 24-bit (16777216 steps)", .value = 11 },
    {.name = "12: Raw",                     .value = 12 }
};

// 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 33.08k, 7: 32k 8: 22.05k, 9: 16k, 10: 12k, 11: 11k 12: 8k 13: 5.5k 14: 4k 15: 2k
static std::vector<SelectItem> rateItems = {
    {.name = " 1: 96kHz",    .value = 1 },
    {.name = " 2: 55.5kHz",  .value = 2 },
    {.name = " 3: 49.7kHz",  .value = 3 },
    {.name = " 4: 48kHz",    .value = 4 },
    {.name = " 5: 44.1kHz",  .value = 5 },
    {.name = " 6: 33.08kHz", .value = 6 },
    {.name = " 7: 32kHz",    .value = 7 },
    {.name = " 8: 22.05kHz", .value = 8 },
    {.name = " 9: 16kHz",    .value = 9 },
    {.name = "10: 12kHz",    .value = 10 },
    {.name = "11: 11kHz",    .value = 11 },
    {.name = "12: 8kHz",     .value = 12 },
    {.name = "12: 5.5kHz",   .value = 13 },
    {.name = "13: 4kHz",     .value = 14 },
    {.name = "15: 2kHz",     .value = 15 },
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

void GuiSsg::setup()
{
    p_curveCore = ctx.audioProcessor.getCurveCore();
    p_guiCurve = ctx.editor.getCurveGui();

    const juce::String code = SsgPrKey::prefix;
    int tabOrder = 1;

    mainGroup.setup(*this, SsgGuiText::Group::mainGroup);

    presetNameLabel.setup({ .parent = *this, .title = "" });
    presetNameLabel.setText(ctx.audioProcessor.presetName, juce::NotificationType::dontSendNotification);
    presetNameLabel.setFont(juce::Font(juce::FontOptions(18.0f)));
    presetNameLabel.setColour(juce::Label::backgroundColourId, juce::Colours::darkblue.withAlpha(0.4f));

    addAndMakeVisible(presetNameSeparator);
    presetNameSeparator.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::grey });

    levelSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::level, .title = SsgGuiText::Ssg::level, .isReset = true });
    levelSlider.setWantsKeyboardFocus(true);
    levelSlider.setExplicitFocusOrder(++tabOrder);

    formCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Category::visibleForm, .invisibleTitle = SsgGuiText::Category::invisibleForm, .enableChangeDetailVisible = true });
    qualityCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Category::visibleQuality, .invisibleTitle = SsgGuiText::Category::invisibleQuality, .enableChangeDetailVisible = true });

    bitSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::bit, .title = SsgGuiText::bit, .items = bdItems, .isReset = true });
    bitSelector.setWantsKeyboardFocus(true);
    bitSelector.setExplicitFocusOrder(++tabOrder);

    rateSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::rate, .title = SsgGuiText::rate, .items = rateItems, .isReset = true });
    rateSelector.setWantsKeyboardFocus(true);
    rateSelector.setExplicitFocusOrder(++tabOrder);

    fixComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, "-> 440", 440);

    unisonComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    ampEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    pitchEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, SsgPrKey::pitchAdsr + SsgPrKey::bypass, SsgGuiText::PitchAdsr::bypass);

    ssgSwEnvComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder, SsgPrKey::ssgSwEnv + SsgPrKey::bypass, SsgGuiText::SsgSwEnv::bypass);

    mulDetuneComponent.setupComponent(mainGroup.contentCanvas, code, tabOrder);

    hwEnvCat.setupHwCategory({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Category::visibleHwEnv, .invisibleTitle = SsgGuiText::Category::invisibleHwEnv, .enableChangeDetailVisible = true });

    envEnableButton.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::HwEnv::enable, .title = SsgGuiText::Ssg::HwEnv::enable, .isReset = true });
    envEnableButton.setWantsKeyboardFocus(true);
    envEnableButton.setExplicitFocusOrder(++tabOrder);

    shapeSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::HwEnv::shape, .title = SsgGuiText::Ssg::HwEnv::shape, .items = ssgEnvItems, .isReset = true });
    shapeSelector.setWantsKeyboardFocus(true);
    shapeSelector.setExplicitFocusOrder(++tabOrder);

    periodSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::HwEnv::period, .title = SsgGuiText::Ssg::HwEnv::speed, .isReset = true, .regType = RegisterType::SsgEnv });
    periodSlider.setWantsKeyboardFocus(true);
    periodSlider.setExplicitFocusOrder(++tabOrder);

    lfo.setupComponent(
        mainGroup.contentCanvas,
        code,
        tabOrder
    );

    midiComponent.setupComponent(mainGroup.contentCanvas, tabOrder);

    utilityCat.setupOtherCategory({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Category::visibleUtil, .invisibleTitle = SsgGuiText::Category::invisibleUtil, .enableChangeDetailVisible = true });

    broadcastLevelButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::bcLevel });
    broadcastLevelButton.setWantsKeyboardFocus(true);
    broadcastLevelButton.setExplicitFocusOrder(++tabOrder);
    broadcastLevelButton.onClick = [this] {
        float level = levelSlider.getValue();

        ctx.editor.breadcastLevel(level);
        };

    mainGroup.contentCanvas.addAndMakeVisible(uSep001);
    uSep001.setup({ .lineThick = 2.0f, .lineColour = juce::Colours::white });

    importToneNoiseParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::toneNoiseFileImport, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false });
    importToneNoiseParamButton.setWantsKeyboardFocus(true);
    importToneNoiseParamButton.setExplicitFocusOrder(++tabOrder);
    importToneNoiseParamButton.onClick = [this] {
        importToneNoiseParam();
        };

    exportToneNoiseParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::toneNoiseFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportToneNoiseParamButton.setWantsKeyboardFocus(true);
    exportToneNoiseParamButton.setExplicitFocusOrder(++tabOrder);
    exportToneNoiseParamButton.onClick = [this] {
        exportToneNoiseParam();
        };

    importLfoParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::lfoFileImport, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false });
    importLfoParamButton.setWantsKeyboardFocus(true);
    importLfoParamButton.setExplicitFocusOrder(++tabOrder);
    importLfoParamButton.onClick = [this] {
        importLfoParam();
        };

    exportLfoParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::lfoFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportLfoParamButton.setWantsKeyboardFocus(true);
    exportLfoParamButton.setExplicitFocusOrder(++tabOrder);
    exportLfoParamButton.onClick = [this] {
        exportLfoParam();
        };

    importAmpEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::ampEnvFileImport, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false });
    importAmpEnvParamButton.setWantsKeyboardFocus(true);
    importAmpEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importAmpEnvParamButton.onClick = [this] {
        importAmpEnvParam();
        };

    exportAmpEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::ampEnvFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportAmpEnvParamButton.setWantsKeyboardFocus(true);
    exportAmpEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportAmpEnvParamButton.onClick = [this] {
        exportAmpEnvParam();
        };

    importPitchEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::pitchEnvFileImport, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false });
    importPitchEnvParamButton.setWantsKeyboardFocus(true);
    importPitchEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importPitchEnvParamButton.onClick = [this] {
        importPitchEnvParam();
        };

    exportPitchEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::pitchEnvFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportPitchEnvParamButton.setWantsKeyboardFocus(true);
    exportPitchEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportPitchEnvParamButton.onClick = [this] {
        exportPitchEnvParam();
        };

    importSsgSwEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::ssgSwEnvFileImport, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false });
    importSsgSwEnvParamButton.setWantsKeyboardFocus(true);
    importSsgSwEnvParamButton.setExplicitFocusOrder(++tabOrder);
    importSsgSwEnvParamButton.onClick = [this] {
        importSsgSwEnvParam();
        };

    exportSsgSwEnvParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::ssgSwEnvFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportSsgSwEnvParamButton.setWantsKeyboardFocus(true);
    exportSsgSwEnvParamButton.setExplicitFocusOrder(++tabOrder);
    exportSsgSwEnvParamButton.onClick = [this] {
        exportSsgSwEnvParam();
        };

    importDetuneParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::detuneFileImport, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false });
    importDetuneParamButton.setWantsKeyboardFocus(true);
    importDetuneParamButton.setExplicitFocusOrder(++tabOrder);
    importDetuneParamButton.onClick = [this] {
        importDetuneParam();
        };

    exportDetuneParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::detuneFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportDetuneParamButton.setWantsKeyboardFocus(true);
    exportDetuneParamButton.setExplicitFocusOrder(++tabOrder);
    exportDetuneParamButton.onClick = [this] {
        exportDetuneParam();
        };

    importUnisonParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::unisonFileImport, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false });
    importUnisonParamButton.setWantsKeyboardFocus(true);
    importUnisonParamButton.setExplicitFocusOrder(++tabOrder);
    importUnisonParamButton.onClick = [this] {
        importUnisonParam();
        };

    exportUnisonParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::unisonFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportUnisonParamButton.setWantsKeyboardFocus(true);
    exportUnisonParamButton.setExplicitFocusOrder(++tabOrder);
    exportUnisonParamButton.onClick = [this] {
        exportUnisonParam();
        };

    importQualityParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::qualityFileImport, .bgColor = juce::Colours::darkkhaki.darker(0.5f), .isReset = false, .isResized = false });
    importQualityParamButton.setWantsKeyboardFocus(true);
    importQualityParamButton.setExplicitFocusOrder(++tabOrder);
    importQualityParamButton.onClick = [this] {
        importQualityParam();
        };

    exportQualityParamButton.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Utility::qualityFileExport, .bgColor = juce::Colours::darkgreen, .isReset = false, .isResized = false });
    exportQualityParamButton.setWantsKeyboardFocus(true);
    exportQualityParamButton.setExplicitFocusOrder(++tabOrder);
    exportQualityParamButton.onClick = [this] {
        exportQualityParam();
        };

    waveSelector.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::wveform, .title = SsgGuiText::Ssg::Voice::form, .items = ssgWsItems, .isReset = true, .isResized = true });
    waveSelector.setWantsKeyboardFocus(true);
    waveSelector.setExplicitFocusOrder(++tabOrder);

    toneSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::tone, .title = SsgGuiText::Ssg::Voice::tone, .isReset = true, .regType = RegisterType::SsgVol });
    toneSlider.setWantsKeyboardFocus(true);
    toneSlider.setExplicitFocusOrder(++tabOrder);

    noiseSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::noise, .title = SsgGuiText::Ssg::Voice::noise, .isReset = true, .regType = RegisterType::SsgVol });
    noiseSlider.setWantsKeyboardFocus(true);
    noiseSlider.setExplicitFocusOrder(++tabOrder);

    noiseFreqSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::noiseFreq, .title = SsgGuiText::Ssg::Voice::noiseFreq, .isReset = true });
    noiseFreqSlider.setWantsKeyboardFocus(true);
    noiseFreqSlider.setExplicitFocusOrder(++tabOrder);

    noiseOnNoteButton.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::noiseOnNote, .title = SsgGuiText::Ssg::Voice::noiseOnNote, .isReset = true });
    noiseOnNoteButton.setWantsKeyboardFocus(true);
    noiseOnNoteButton.setExplicitFocusOrder(++tabOrder);

    // 初期状態反映
    mixSlider.setup({ .parent = mainGroup.contentCanvas, .id = code + SsgPrKey::mix , .title = SsgGuiText::Ssg::Voice::mix, .isReset = true });
    mixSlider.setWantsKeyboardFocus(true);
    mixSlider.setExplicitFocusOrder(++tabOrder);

    mixSetTone.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Ssg::Voice::tone, .isReset = false, .isResized = false });
    mixSetTone.setWantsKeyboardFocus(true);
    mixSetTone.setExplicitFocusOrder(++tabOrder);
    mixSetTone.onClick = [this] { mixSlider.setValue(0.0, juce::sendNotification); };

    mixSetMix.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Ssg::Voice::mix, .isReset = false, .isResized = false });
    mixSetMix.setWantsKeyboardFocus(true);
    mixSetMix.setExplicitFocusOrder(++tabOrder);
    mixSetMix.onClick = [this] { mixSlider.setValue(0.5, juce::sendNotification); };

    mixSetNoise.setup({ .parent = mainGroup.contentCanvas, .title = SsgGuiText::Ssg::Voice::noise, .isReset = false, .isResized = false });
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

    dutyFcButton.setup({ .parent = *this, .id = code + SsgPrKey::Duty::fc, .title = SsgGuiText::Ssg::Duty::fc, .isReset = true });
    dutyFcButton.setWantsKeyboardFocus(true);
    dutyFcButton.setExplicitFocusOrder(++tabOrder);

    dutyFcFlucSlider.setup({ .parent = *this, .id = code + SsgPrKey::Duty::fcFluc, .title = SsgGuiText::Ssg::Duty::fcFluc, .isReset = true });
    dutyFcFlucSlider.setWantsKeyboardFocus(true);
    dutyFcFlucSlider.setExplicitFocusOrder(++tabOrder);

    pulseInvCat.setupSwCategory({ .parent = *this, .title = SsgGuiText::Category::invert });

    dutyInvertButton.setup({ .parent = *this, .id = code + SsgPrKey::Duty::inv, .title = SsgGuiText::Ssg::Duty::invert, .isReset = true });
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
    auto mmRect = mainArea.reduced(SsgGuiValue::Group::Padding::width, SsgGuiValue::Group::Padding::height);
    mmRect.removeFromTop(SsgGuiValue::Group::TitlePaddingTop);

    layoutMainParamName({ .mainRect = mmRect, .label = &presetNameLabel });

    // 区切り線エリアを確保
    auto presetNameSeparatorArea = mmRect.removeFromTop(SsgGuiValue::MainGroup::Separator::height);
    presetNameSeparator.setBounds(presetNameSeparatorArea);

    // グラフ用の区画を確保
    layoutGraph(mmRect);
    updateGraph();

    // 固定ヘッダーを配置して残った「mmRect」を、Viewportの領域としてセットする
    // (mainArea の左上座標を引いて、グループ内での相対座標に変換しています)
    mainGroup.setViewportCustomBounds(mmRect.translated(-mainArea.getX(), -mainArea.getY()));

    // キャンバスの中身のレイアウトは常に Y=0 からスタートさせる
    juce::Rectangle<int> mRect(0, 0, mainGroup.viewport.getMaximumVisibleWidth(), 2000);

    layoutMain({ .mainRect = mRect, .label = &levelSlider.label, .component = &levelSlider });

    layoutFormCat(mRect);

    ampEnvComponent.layoutComponent(mRect);

    pitchEnvComponent.layoutComponent(mRect);

    mulDetuneComponent.layoutComponent(mRect);

    layoutHwEnvCat(mRect);

    ssgSwEnvComponent.layoutComponent(mRect);

    lfo.layoutComponent(mRect);

    fixComponent.layoutComponent(mRect);

    unisonComponent.layoutComponent(mRect);

    layoutQualityCat(mRect);

    midiComponent.layoutComponent(mRect);

    layoutUtilityCat(mRect);

    int usedHeight = 2000 - mRect.getHeight();

    // 下部の余白を足して、キャンバスの最終的な高さをセット
    mainGroup.setContentHeight(usedHeight + 20);

    auto paramArea = pageArea.removeFromLeft(SsgGuiValue::ParamGroup::width);

    // Wave Group
    float waveParam = *ctx.audioProcessor.apvts.getRawParameterValue(code + SsgPrKey::wveform);
    int waveMode = (waveParam > SsgPrValue::boolThread) ? 1 : 0;
    auto waveArea = paramArea.removeFromTop(140);

    if (waveMode == 0) // Pulse
    {
        dutyGroup.setVisible(true);
        triGroup.setVisible(false);

        triKeyTrackButton.setVisible(false);
        triPeakSlider.setVisibleWithLabel(false);
        triFreqSlider.setVisibleWithLabel(false);
        triSetSawDown.setVisible(false);
        triSetTri.setVisible(false);
        triSetSawUp.setVisible(false);

        dutyModeSelector.setVisibleWithLabel(true);
        dutyVarSlider.setVisibleWithLabel(true);
        dutyFcButton.setVisible(true);
        dutyFcFlucSlider.setVisibleWithLabel(true);
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

        layoutRow({ .rowRect = dRect, .component = &dutyFcButton });
        layoutRow({ .rowRect = dRect, .label = &dutyFcFlucSlider.label, .component = &dutyFcFlucSlider, });

        layoutRow({ .rowRect = dRect, .component = &dutyInvertButton, .paddingBottom = 0 });
    }
    else // Triangle
    {
        dutyGroup.setVisible(false);
        triGroup.setVisible(true);

        dutyModeSelector.setVisibleWithLabel(false);
        dutyInvertButton.setVisible(false);
        dutyPresetSelector.setVisibleWithLabel(false);
        dutyFcButton.setVisible(false);
        dutyFcFlucSlider.setVisibleWithLabel(false);
        dutyVarSlider.setVisibleWithLabel(false);

        triKeyTrackButton.setVisible(true);
        triFreqSlider.setVisibleWithLabel(true);
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

void GuiSsg::layoutFormCat(Rectangle<int>& rect) {
    layoutMainCategory({ .mainRect = rect, .component = &formCat });

    bool visible = formCat.isDetailVisible();

    waveSelector.setVisibleWithLabel(visible);
    toneSlider.setVisibleWithLabel(visible);
    noiseSlider.setVisibleWithLabel(visible);
    noiseFreqSlider.setVisibleWithLabel(visible);
    noiseOnNoteButton.setVisible(visible);
    mixSlider.setVisibleWithLabel(visible);
    mixSetTone.setVisible(visible);
    mixSetMix.setVisible(visible);
    mixSetNoise.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .label = &waveSelector.label, .component = &waveSelector, });
        layoutMain({ .mainRect = rect, .label = &toneSlider.label, .component = &toneSlider, });
        layoutMain({ .mainRect = rect, .label = &noiseSlider.label, .component = &noiseSlider });
        layoutMain({ .mainRect = rect, .label = &noiseFreqSlider.label, .component = &noiseFreqSlider });
        layoutMain({ .mainRect = rect, .component = &noiseOnNoteButton, });
        layoutMain({ .mainRect = rect, .label = &mixSlider.label, .component = &mixSlider });
        layoutMainThreeComps({ .rect = rect, .comp1 = &mixSetTone, .comp2 = &mixSetMix, .comp3 = &mixSetNoise, .paddingBottom = 0 });
    }
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

void GuiSsg::layoutUtilityCat(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &utilityCat });

    bool visible = utilityCat.isDetailVisible();

    broadcastLevelButton.setVisible(visible);
    uSep001.setVisible(visible);
    importToneNoiseParamButton.setVisible(visible);
    exportToneNoiseParamButton.setVisible(visible);
    importLfoParamButton.setVisible(visible);
    exportLfoParamButton.setVisible(visible);
    importAmpEnvParamButton.setVisible(visible);
    exportAmpEnvParamButton.setVisible(visible);
    importPitchEnvParamButton.setVisible(visible);
    exportPitchEnvParamButton.setVisible(visible);
    importSsgSwEnvParamButton.setVisible(visible);
    exportSsgSwEnvParamButton.setVisible(visible);
    importDetuneParamButton.setVisible(visible);
    exportDetuneParamButton.setVisible(visible);
    importUnisonParamButton.setVisible(visible);
    exportUnisonParamButton.setVisible(visible);
    importQualityParamButton.setVisible(visible);
    exportQualityParamButton.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &broadcastLevelButton });

        auto uSep001Area = rect.removeFromTop(4);
        uSep001.setBounds(uSep001Area);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importToneNoiseParamButton, .comp2 = &exportToneNoiseParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importLfoParamButton, .comp2 = &exportLfoParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importAmpEnvParamButton, .comp2 = &exportAmpEnvParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importPitchEnvParamButton, .comp2 = &exportPitchEnvParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importSsgSwEnvParamButton, .comp2 = &exportSsgSwEnvParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importDetuneParamButton, .comp2 = &exportDetuneParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importUnisonParamButton, .comp2 = &exportUnisonParamButton });

        rect.removeFromTop(4);

        layoutMainTwoComps({ .rect = rect, .comp1 = &importQualityParamButton, .comp2 = &exportQualityParamButton });
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

void GuiSsg::setLevel(float level) {
    levelSlider.setValue(level, juce::NotificationType::sendNotification);
}

void GuiSsg::importToneNoiseParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultToneNoiseParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importToneNoiseParamFile, defaultDir, Io::ExtensionGlob::ToneNoiseParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultToneNoiseParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 4) return;

                toneSlider.setValue(lines[0].getFloatValue(), juce::sendNotification);
                noiseSlider.setValue(lines[1].getFloatValue(), juce::sendNotification);
                noiseFreqSlider.setValue(lines[2].getFloatValue(), juce::sendNotification);
                mixSlider.setValue(lines[3].getFloatValue(), juce::sendNotification);
            }
        });
}

void GuiSsg::exportToneNoiseParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultToneNoiseParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportToneNoiseParamFile, defaultDir.getChildFile("default.toneNoise"), Io::ExtensionGlob::ToneNoiseParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultToneNoiseParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += juce::String(toneSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(noiseSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(noiseFreqSlider.getValue(), Global::floatDecimalPlaces) + "\n";
                content += juce::String(mixSlider.getValue(), Global::floatDecimalPlaces) + "\n";

                file.replaceWithText(content);
            }
        });
}

void GuiSsg::importLfoParam() {
    lfo.importParams();
}

void GuiSsg::exportLfoParam() {
    lfo.exportParams();
}

void GuiSsg::importAmpEnvParam() {
    ampEnvComponent.importParams();
}

void GuiSsg::exportAmpEnvParam() {
    ampEnvComponent.exportParams();
}

void GuiSsg::importPitchEnvParam() {
    pitchEnvComponent.importParams();
}

void GuiSsg::exportPitchEnvParam() {
    pitchEnvComponent.exportParams();
}

void GuiSsg::importSsgSwEnvParam() {
    ssgSwEnvComponent.importParams();
}

void GuiSsg::exportSsgSwEnvParam() {
    ssgSwEnvComponent.exportParams();
}

void GuiSsg::importDetuneParam() {
    mulDetuneComponent.importParams();
}

void GuiSsg::exportDetuneParam() {
    mulDetuneComponent.exportParams();
}

void GuiSsg::importUnisonParam() {
    unisonComponent.importParams();
}

void GuiSsg::exportUnisonParam() {
    unisonComponent.exportParams();
}

void GuiSsg::importQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importQualityParamFile, defaultDir, Io::ExtensionGlob::QualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::StringArray lines;
                file.readLines(lines);

                int size = lines.size();

                if (size < 2) return;

                bitSelector.setSelectedItemIndex(lines[0].getIntValue(), juce::sendNotification);
                rateSelector.setSelectedItemIndex(lines[1].getIntValue(), juce::sendNotification);
            }
        });
}

void GuiSsg::exportQualityParam() {
    juce::File defaultDir(ctx.audioProcessor.defaultQualityParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportQualityParamFile, defaultDir.getChildFile("default.quality"), Io::ExtensionGlob::QualityParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultQualityParamDir = file.getParentDirectory().getFullPathName();

                juce::String content = "";

                content += juce::String(bitSelector.getSelectedItemIndex()) + "\n";
                content += juce::String(rateSelector.getSelectedItemIndex()) + "\n";

                file.replaceWithText(content);
            }
        });
}
